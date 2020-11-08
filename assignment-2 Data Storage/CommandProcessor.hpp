//
//  CommandProcessor.hpp
//  Database
//
//  Created by rick gessner on 3/17/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef CommandProcessor_hpp
#define CommandProcessor_hpp

#include <stdio.h>
#include <string>

#include "keywords.hpp"

namespace ECE141 {
  
  class Statement;
  class Tokenizer;
  
  class Command {
  public:
    Command(Keywords aKeyword) : keyword(aKeyword) {}
    
    Keywords getKeyword() const {return keyword;}
    
  protected:
    Keywords keyword;
  };
  
  //--------------------------------------------------
  
  class IInterpreter {
  public:
    virtual StatusResult  processInput(Tokenizer &aTokenizer, IInterpreter *another=nullptr)=0;
    virtual Statement*    getStatement(Tokenizer &aTokenizer)=0;
    virtual StatusResult  interpret(const Statement &aStatement)=0;    
  };
  
  //--------------------------------------------------
  
  class CommandProcessor : public IInterpreter {
  public:
    
    CommandProcessor();
    virtual ~CommandProcessor();
    
    virtual StatusResult  processInput(Tokenizer &aTokenizer, IInterpreter *another=nullptr);
    
  protected:
    
    virtual Statement*    getStatement(Tokenizer &aTokenizer);
    virtual StatusResult  interpret(const Statement &aStatement);
    
  };
}

#endif /* CommandProcessor_hpp */
