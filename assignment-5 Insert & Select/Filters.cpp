//
//  Filters.cpp
//  Datatabase4
//
//  Created by rick gessner on 4/25/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include "Filters.hpp"
#include <string>
#include "keywords.hpp"
#include "Helpers.hpp"
#include "Entity.hpp"


namespace ECE141 {
  
  bool Expression::operator()(KeyValues &aList) {
    //if(aValue.getType()!=value.getType()) {}
    
    Value theLHS = lhs.value;
    Value theRHS = rhs.value;

    if(TokenType::identifier==lhs.type) {
      theLHS=aList[lhs.name];
    }

    if(TokenType::identifier==rhs.type) {
      theRHS=aList[rhs.name];
    }
    
    switch(op) {
      case Operators::equal_op: return theLHS==theRHS; break;
      case Operators::notequal_op: return !(theLHS==theRHS); break;
      case Operators::lt_op: return theLHS<theRHS; break;
      case Operators::lte_op: return !(theRHS<theLHS); break;
      case Operators::gt_op: return theRHS<theLHS; break;
      case Operators::gte_op: return !(theLHS<theRHS); break;

      case Operators::between_op:
      case Operators::or_op:
      case Operators::nor_op:
      case Operators::and_op:
      case Operators::not_op:
      case Operators::unknown_op:
      default: break;
    }

    return false;
  }
  
  //--------------------------------------------------------------
  
  Filters::Filters()  {
  }
  
  //TBD
  Filters::Filters(const Filters &aCopy) {
  }
  
  Filters::~Filters() {
    for (auto theExpr : expressions) {
      delete theExpr;
    }
  }
  
  size_t Filters::getCount() const {return expressions.size();}
  
  
  Filters& Filters::add(Expression *anExpression) {
    expressions.push_back(anExpression);
    return *this;
  }
    
  //compare expressions to row; if all match, then return true
  bool Filters::matches(KeyValues &aList) const {
    for(auto &theExpr : expressions) {
      if(!(*theExpr)(aList)) {
        return false;
      }
    }
    return true;
  }
 
  //=======================================================
  // This class helps sqlstatements with parsing...
  //=======================================================
  
  Filtered::Filtered() {}
  Filtered::~Filtered() {}

  
  bool isValidOperandType(TokenType aType) {
    switch(aType) {
      case TokenType::identifier:
      case TokenType::number:
      case TokenType::string: return true;
      default: break;
    }
    return false;
  }
  //where operand is field, number, string...
  StatusResult parseOperand(Tokenizer &aTokenizer, Entity &anEntity, Operand &anOperand) {
    StatusResult theResult{noError};
    Token &theToken = aTokenizer.current();
    if(TokenType::identifier==theToken.type) {
      anOperand.type=TokenType::identifier;
      try {
        Attribute &theAttr=anEntity.getAttribute(theToken.data);
        anOperand.name=theAttr.getName(); //hang on to field name...
        anOperand.entityId=anEntity.getHash();
      }
      catch(...) {
        anOperand.type=TokenType::string;
      }
    }
    else if(TokenType::number==theToken.type) {
      anOperand.type=TokenType::number;
      anOperand.value=theToken.data; //not sure the full type yet...
    }
    else theResult.code=syntaxError;
    return theResult;
  }
  
  //after parsing expression, let's get operands in sync...
  bool validateOperands(Operand &aLHS, Operand &aRHS, Entity &anEntity) {
    if(TokenType::identifier==aLHS.type) { //most common case...
      Attribute &theAttr=anEntity.getAttribute(aLHS.name);
      aRHS.value.become(theAttr.getType());
      return true;
    }
    else if(TokenType::identifier==aRHS.type) {
      Attribute &theAttr=anEntity.getAttribute(aRHS.name);
      aLHS.value.become(theAttr.getType());
      return true;
    }
    return false;
  }
  
  //get Operand, OP, Operand  (where operand is field, number, string)...
  StatusResult Filtered::parseExpression(Tokenizer &aTokenizer, Entity &anEntity) {
    StatusResult theResult{syntaxError};
    Token &theToken = aTokenizer.current(); //should be 'identifier'...
    
    Operand theLHS;
    if(isValidOperandType(theToken.type)) {
      theResult=parseOperand(aTokenizer, anEntity, theLHS);
      if(theResult) {
        aTokenizer.next(); //skip LHS...
        theToken = aTokenizer.current(); //should be 'operator'...
        Operators theOp=Helpers::toOperator(theToken.data);
        if(TokenType::operators==theToken.type && Operators::unknown_op!=theOp) {
          aTokenizer.next();
          theToken = aTokenizer.current(); //should be 'operator'...
          Operand theRHS;
          if(isValidOperandType(theToken.type)) {
            theResult=parseOperand(aTokenizer, anEntity, theRHS);
            if(theResult) {
              aTokenizer.next(); //skip RHS...
              if(validateOperands(theLHS,theRHS, anEntity)) {
                filters.add(new Expression(theLHS, theOp, theRHS));
              }
              else theResult.code=syntaxError;
            }
          }
        }
      }
    }

    return theResult;
  }
  

  //already got 'where', so go get key/value pairs until reach keyword or EOF...
  StatusResult Filtered::parseFilters(Tokenizer &aTokenizer, Entity &anEntity) {
    
    StatusResult  theResult{noError};
    while(theResult && aTokenizer.more()) {
      Token &theToken = aTokenizer.current(); //should be 'identifier'...
      if(TokenType::semicolon==theToken.type) {
        if(filters.getCount()) return theResult;
      }
      if(isValidOperandType(theToken.type)) {
        theResult=parseExpression(aTokenizer, anEntity);
      }
      else theResult.code=syntaxError;
    }
    return theResult;
  }
  
}

