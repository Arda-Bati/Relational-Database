//
//  CommandProcessor.cpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include <iostream>
#include "CommandProcessor.hpp"
#include "Tokenizer.hpp"
#include "Statement.hpp"

namespace ECE141 {
  
  CommandProcessor::CommandProcessor() {}
  
  CommandProcessor::~CommandProcessor() {}

  
  // USE: ---------------------------------------------
  void showError(StatusResult &aResult) {
    switch(aResult.code) {
      case ECE141::unknownCommand: std::cout << "Unknown command: "; break;
      case ECE141::userTerminated: break; //ignore this...
      default: std::cout << "Unknown error"; break;
    }
    std::cout << std::endl;
  }
  
  // USE: -----------------------------------------------------
  StatusResult CommandProcessor::interpret(const Statement &aStatement) {
    ECE141::Keywords currKwrd = aStatement.getType();
    switch(currKwrd) {
      case ECE141::Keywords::version_kw : std::cout << "ECEDB - version 1" << std::endl;
                                          return StatusResult(Errors::noError);
      case ECE141::Keywords::help_kw : std::cout << "version -- shows the current version of this application" << std::endl;
                                       std::cout << "help -- shows this list of commands" << std::endl;
                                       std::cout << "quit -- terminates the execution of this DB application" << std::endl;
                                       return StatusResult(Errors::noError);
      case ECE141::Keywords::quit_kw : return StatusResult(Errors::userTerminated);
    }
    return StatusResult();
  }
  
  // USE: retrieve a statement based on given text input...
  Statement* CommandProcessor::getStatement(Tokenizer &aTokenizer) {
    if(aTokenizer.size()) {
      ECE141::Keywords currKwrd = aTokenizer.current().keyword;
      aTokenizer.next();
      switch(currKwrd) {
        case ECE141::Keywords::version_kw : return new Statement(aTokenizer, currKwrd);
        case ECE141::Keywords::help_kw : return new Statement(aTokenizer, currKwrd);
        case ECE141::Keywords::quit_kw : return new Statement(aTokenizer, currKwrd);
      }
    }
    return nullptr;
  }

  // USE: -------------------------------------------------------
  StatusResult CommandProcessor::processInput(Tokenizer &aTokenizer, IInterpreter *another) {
    StatusResult theResult;
    
    while(theResult && aTokenizer.more()) {
      if(Statement *theStatement=getStatement(aTokenizer)) {
        theResult=interpret(*theStatement);
        delete theStatement;
      }
      else if(another) {
        if(Statement *theStatement=another->getStatement(aTokenizer)) { //check this
          theResult=another->interpret(*theStatement);
          delete theStatement;
        }
      }
      else return StatusResult{ECE141::unknownCommand};
      
      if(!theResult) showError(theResult);
    }
    return theResult;
  }

 
  
}
