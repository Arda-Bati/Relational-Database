//
//  Statement.cpp
//  Database
//
//  Created by rick gessner on 3/20/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include "Statement.hpp"
#include "Tokenizer.hpp"

namespace ECE141 {
  
  Statement::Statement(Tokenizer &aTokenizer, Keywords aStatementType)
    : tokenizer(aTokenizer), stmtType(aStatementType)  {
  }
  
  Statement::Statement(const Statement &aCopy) : tokenizer(aCopy.tokenizer) {
    stmtType=aCopy.stmtType;
  }
  
  Statement::~Statement() {
  }
  
  StatusResult Statement::run() {
    //return commandable.runStatement(*this);
    return StatusResult();
  }
  
}
