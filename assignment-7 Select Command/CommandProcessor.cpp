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
      case Keywords::version_kw: std::cout << "Version 0.7" << std::endl; break;
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
  
  // USE: parse tokens in tokenizer and create assocated statement;
  //      If you can't do it, ask your next (chain-of-resp) to try...
  StatusResult CommandProcessor::processInput(Tokenizer &aTokenizer) {
    if(Statement *theStatement=getStatement(aTokenizer)) {
      StatusResult theResult = interpret(*theStatement);
      delete theStatement;
      return theResult;
    }
    else if(next) {
      return next->processInput(aTokenizer);
    }
    return StatusResult{ECE141::unknownCommand};
  }
  
  // USE: retrieve active db (if available) to subystem...
  Database* CommandProcessor::getActiveDatabase() {
    return next ? next->getActiveDatabase() : nullptr;
  }

}
