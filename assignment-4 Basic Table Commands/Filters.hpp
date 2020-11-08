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
#include "Attribute.hpp"

namespace ECE141 {
  
  class Row;
  class Value;
  class Tokenizer;
  class Entity;
  
  struct Expression {
    Property    key;
    Value       value;
    Operators   op;
    uint32_t    entityId;
    
    Expression(Property aKey, Value aValue, Operators anOp)
    : key(aKey), value(aValue), op(anOp), entityId(0) {}
    
    Expression(std::string aKey, Value aValue, Operators anOp )
    : key(Property(aKey)), value(aValue), op(anOp), entityId(0) {}
    
    bool operator()(const Value &aValue) const;
    
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
