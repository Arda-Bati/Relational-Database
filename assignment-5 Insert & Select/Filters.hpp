//
//  Filters.hpp
//  Datatabase4
//
//  Created by rick gessner on 4/25/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef Filters_hpp
#define Filters_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include "Errors.hpp"
#include "Value.hpp"
#include "Tokenizer.hpp"

namespace ECE141 {
  
  class Row;
  class Value;
  class Tokenizer;
  class Entity;
  
  struct Operand {
    Operand() {}
    Operand(std::string &aName, TokenType aType, Value &aValue, uint32_t anId=0)
      : name(aName), type(aType), entityId(anId), value(aValue) {}
    TokenType   type; //so we know if it's a field, a number, or a string...
    std::string name; //for attr.
    Value       value;
    uint32_t    entityId;
  };
  
  struct Expression {
    Operand     lhs;
    Operand     rhs;
    Operators   op;
    
    Expression(Operand &aLHSOperand, Operators anOp, Operand &aRHSOperand)
      : lhs(aLHSOperand), op(anOp), rhs(aRHSOperand) {}
    
    bool operator()(KeyValues &aList);
    
  };
  
  using Expressions = std::vector<Expression*>;

  class Filters {
  public:
    
    Filters();
    Filters(const Filters &aFilters);
    ~Filters();
    
    size_t        getCount() const;
    bool          matches(KeyValues &aList) const;
    Filters&      add(Expression *anExpression);
    
    friend class Tokenizer;
    
  protected:
    Expressions  expressions;
  };
 
  
  //----------------------------------------
  // mixin to statements with "where" clause...
  //----------------------------------------
  
  class Filtered {
  public:
    
    Filtered();
    ~Filtered();
    
    StatusResult parseFilters(Tokenizer &aTokenizer, Entity &anEntity);
    StatusResult parseExpression(Tokenizer &aTokenizer, Entity &anEntity);

    Filters filters;
  };
  
}

#endif /* Filters_hpp */
