//
//  CommandProcessor.cpp
//  Database3
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include <iostream>
#include "CommandProcessor.hpp"
#include "Statement.hpp"
#include "Tokenizer.hpp"

namespace ECE141 {
  
  CommandProcessor::CommandProcessor(IInterpreter *aNext) : next(aNext) {}
  
  CommandProcessor::~CommandProcessor() {}
  
  // USE: -----------------------------------------------------
  StatusResult CommandProcessor::interpret(const Statement &aStatement) {
    switch(aStatement.getType()) {
      case Keywords::quit_kw:
        std::cout << "DB::141 is shutting down." << std::endl;
        return StatusResult(ECE141::userTerminated);
        break;
      case Keywords::version_kw: std::cout << "Version 0.1" << std::endl; break;
      case Keywords::help_kw: std::cout << "Show help here..." << std::endl; break;
      default: break;
    }
    return StatusResult();
  }
  
  // USE: retrieve a statement based on given text input...
  Statement* CommandProcessor::getStatement(Tokenizer &aTokenizer) {
    
    if(aTokenizer.more()) {
      Token &theToken = aTokenizer.current();
      switch(theToken.keyword) {
        case Keywords::help_kw    :
        case Keywords::version_kw :
        case Keywords::quit_kw    :
          aTokenizer.next(); //skip ahead...
          return new Statement(theToken.keyword);
          
        default: break;
      }
    }
    return nullptr;
  }
  
  // USE: -------------------------------------------------------
  StatusResult CommandProcessor::processInput(Tokenizer &aTokenizer) {
    StatusResult theResult{noError};
    
    while(theResult && aTokenizer.more()) {
      if(Statement *theStatement=getStatement(aTokenizer)) {
        theResult=interpret(*theStatement);
        delete theStatement;
      }
      else if(next) {
        return next->processInput(aTokenizer);
      }
      else return StatusResult{ECE141::unknownCommand};
      
    }
    return theResult;
  }
  
  // USE: retrieve active db (if available) to subystem...
  Database* CommandProcessor::getActiveDatabase() {
    return next ? next->getActiveDatabase() : nullptr;
  }

}
