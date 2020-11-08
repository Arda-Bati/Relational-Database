//
//  Filters.cpp
//  Datatabase4
//
//  Created by rick gessner on 4/25/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include "Filters.hpp"
#include <string>
#include "Value.hpp"
#include "keywords.hpp"
#include "Tokenizer.hpp"
#include "Helpers.hpp"
#include "Entity.hpp"


namespace ECE141 {
  
  bool Expression::operator()(const Value &aValue) const {
    return const_cast<Value&>(aValue)==const_cast<Value&>(value);
  }
  
  //--------------------------------------------------------------
  
  Filters::Filters()  {
  }
  
  //TBD
  Filters::Filters(const Filters &aCopy) {
  }
  
  Filters::~Filters() {
  }
  
  size_t Filters::getCount() const {return expressions.size();}
  
  Filters& Filters::add(Expression *anExpression) {
    expressions.push_back(anExpression);
    return *this;
  }
    
  //compare expressions to row; if all match, then return true
  bool Filters::matches(KeyValues &aList) const {
    for(auto &theExpr : expressions) {
      auto &theName = theExpr->key.name;
      try {
        if(aList.count(theName)) {
          auto &theValue=aList[theName];
          if(!(*theExpr)(theValue)) {
            return false;
          }
        }
      }
      catch(...) {return false;}
    }
    return true;
  }
 
  //=======================================================
  // This class helps sqlstatements with parsing...
  //=======================================================
  
  Filtered::Filtered() {}
  Filtered::~Filtered() {}
  
  //already got 'where', so go get key/value pairs until reach keyword or EOF...
  StatusResult Filtered::parseFilters(Tokenizer &aTokenizer, Entity &anEntity) {
    
    StatusResult  theResult{noError};
    while(theResult && aTokenizer.more()) {
      Token &theToken = aTokenizer.current(); //should be 'identifier'...
      switch(theToken.type) {
        case TokenType::identifier:
          theResult=parseExpression(aTokenizer, anEntity);
          break;
          
        case TokenType::keyword:
          return theResult; //done with expr...
          
        default:
          if(!filters.getCount()) {
            theResult=StatusResult{identifierExpected};
          }
          break;
      }
    }    
    return theResult;
  }
  
  //get ID, OP, VALUE...
  StatusResult Filtered::parseExpression(Tokenizer &aTokenizer, Entity &anEntity) {
    Token &theToken = aTokenizer.current(); //should be 'identifier'...
    if(TokenType::identifier==theToken.type) {
      std::string theName=theToken.data;
      aTokenizer.next();
      theToken = aTokenizer.current(); //should be 'operator'...
      if(TokenType::operators==theToken.type && "="==theToken.data) {
        aTokenizer.next();
        Operators theOp = Helpers::toOperator(theToken.data);
        theToken = aTokenizer.current(); //should be 'operator'...
        
        try {
          auto &theAttr=anEntity.getAttribute(theName);
          Value theValue(theToken.data); //assume a string...
          if(DataType::int_type==theAttr.getType()) {
            theValue=std::stoi(theToken.data);
          }
          Expression *theExpr=new Expression(theName, theValue, theOp);
          filters.add(theExpr);
          aTokenizer.next();
          return StatusResult{noError};
        }
        catch(...) {}        
      }
    }
    return StatusResult{syntaxError};
  }
  
  
}

