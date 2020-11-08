//
//  DBStatement.hpp
//  Datatabase4
//
//  Created by rick gessner on 4/13/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef DBStatement_h
#define DBStatement_h

#include "Statement.hpp"

namespace ECE141 {
  
  //base class for reuse...
  class DBStatement: public Statement {
  public:
    DBStatement(DBManager &aManager, Keywords aStatementType=Keywords::unknown_kw)
    : Statement(aStatementType), name(""), manager(aManager) {}
    
    virtual StatusResult parse(Tokenizer &aTokenizer) {return StatusResult{};}
    virtual StatusResult run(std::ostream &aStream) {return StatusResult{noError};}
    
    //--
    std::string name;
    DBManager   &manager; //cache in case we need to call it...
  };
  
  //USE: declare a create DB command...
  class CreateDBStatement: public DBStatement {
  public:
    CreateDBStatement(DBManager &aManager) : DBStatement(aManager, Keywords::create_kw)  {}
    
    StatusResult parse(Tokenizer &aTokenizer) {
      if(3<=aTokenizer.remaining()) {
        Token &theFirstToken = aTokenizer.peek(1); //should be 'database_kw'...
        if(Keywords::database_kw==theFirstToken.keyword) {
          Token &theNextToken = aTokenizer.peek(2);
          if(TokenType::identifier==theNextToken.type) {
            aTokenizer.next(3); //skip CREATE DATABASE {name}...
            name=theNextToken.data;
            return StatusResult{noError}; //no error
          }
        }
        return StatusResult{Errors::unknownCommand};
      }
      return StatusResult{syntaxError};
    }
    
    //USE: called to execute a CreateDB statement...
    StatusResult run(std::ostream &aStream) const {
      return manager.createDatabase(name);
    }
    
  };
  
  //-----------------------------------------------------
  
  //USE: declare a describe database <name>...
  class DescribeDBStatement : public DBStatement {
  public:
    DescribeDBStatement(DBManager &aManager) : DBStatement(aManager, Keywords::show_kw) {}
    
    StatusResult parse(Tokenizer &aTokenizer) {
      if(3<=aTokenizer.remaining()) { 
        Token &theFirstToken = aTokenizer.peek(1); //should be 'database_kw'...
        if(Keywords::database_kw==theFirstToken.keyword) {
          Token &theNextToken = aTokenizer.peek(2);
          if(TokenType::identifier==theNextToken.type) {
            aTokenizer.next(3); //skip DESCRIBE DATABASE {name}...
            name=theNextToken.data;
            return StatusResult{noError}; //no error
          }
        }
        return StatusResult{Errors::unknownCommand};
      }
      return StatusResult{syntaxError};
    }
    
    StatusResult run(std::ostream &aStream) const {
      return manager.describeDatabase(name);
    }
    
  };
  
  //------------------------------------------------------------
  
  //USE: declare a drop DB command...
  class DropDBStatement: public DBStatement {
  public:
    DropDBStatement(DBManager &aManager) : DBStatement(aManager, Keywords::drop_kw) {}
    
    //-----------------------------------------------------------
    
    StatusResult parse(Tokenizer &aTokenizer) {
      if(3<=aTokenizer.remaining()) {
        Token &theFirstToken = aTokenizer.peek(1); //should be 'datbase_kw'...
        if(Keywords::database_kw==theFirstToken.keyword) {
          Token &theNextToken = aTokenizer.peek(2);
          if(TokenType::identifier==theNextToken.type) {
            aTokenizer.next(3); //skip USE DATABASE {name}...
            name=theNextToken.data;
            return StatusResult{noError}; //no error
          }
        }
        return StatusResult{Errors::unknownCommand};
      }
      return StatusResult{syntaxError};
    }
    
    StatusResult run(std::ostream &aStream) const {
      return manager.dropDatabase(name);
    }
    
  };
  
  //-----------------------------------------------------
  
  //USE: declare a Show databases command...
  class ShowDBStatement : public DBStatement {
  public:
    ShowDBStatement(DBManager &aManager) : DBStatement(aManager, Keywords::show_kw) {}
    
    //-----------------------------------------------------------
    StatusResult parse(Tokenizer &aTokenizer) {
      if(2<=aTokenizer.remaining()) {
        Token &theToken = aTokenizer.peek(1); //should be 'databases_kw'...
        if(Keywords::databases_kw==theToken.keyword) {
          aTokenizer.next(2); //skip SHOW DATABASEs {name}...
          name=theToken.data;
          return StatusResult{noError}; //no error
        }
        return StatusResult{Errors::unknownCommand};
      }
      return StatusResult{syntaxError};
    }
    
    StatusResult run(std::ostream &aStream) const {
      return manager.showDatabases();
    }
    
  };
  
  //-----------------------------------------------------
  
  //USE: declare a USE DB command...
  class UseDBStatement: public DBStatement {
  public:
    UseDBStatement(DBManager &aManager) : DBStatement(aManager, Keywords::use_kw) {}
    
    StatusResult parse(Tokenizer &aTokenizer) {
      if(3<=aTokenizer.remaining()) {
        Token &theFirstToken = aTokenizer.peek(1); //should be 'use_kw'...
        if(Keywords::database_kw==theFirstToken.keyword) {
          Token &theNextToken = aTokenizer.peek(2);
          if(TokenType::identifier==theNextToken.type) {
            aTokenizer.next(3); //skip USE DATABASE {name}...
            name=theNextToken.data;
            return StatusResult{noError};
          }
        }
        return StatusResult{Errors::unknownCommand};
      }
      return StatusResult{syntaxError};
    }
    
    StatusResult run(std::ostream &aStream) const {
      return manager.useDatabase(name);
    }
  };
  
}

#endif /* DBStatement_h */

