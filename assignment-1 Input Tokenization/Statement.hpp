//
//  Statement.hpp
//  Database
//
//  Created by rick gessner on 3/20/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef Statement_hpp
#define Statement_hpp

#include "keywords.hpp"

namespace ECE141 {
  
  class Tokenizer;
  
  class Statement {
  public:
                            Statement(Tokenizer &aTokenizer, Keywords aStatementType=Keywords::unknown_kw);
                            Statement(const Statement &aCopy);
    
    virtual                 ~Statement();
    
    Keywords                getType() const {return stmtType;}
    virtual   const char*   getStatementName() const {return "statement";}
    virtual   StatusResult  run();
    
  protected:
    Keywords   stmtType;
    Tokenizer  &tokenizer;
    
  };
  
}

#endif /* Statement_hpp */
