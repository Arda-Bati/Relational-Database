//
//  SQLStatement.hpp
//  Datatabase4
//
//  Created by rick gessner on 4/13/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef SQLStatement_hpp
#define SQLStatement_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include "Statement.hpp"
#include "Attribute.hpp"
#include "Row.hpp"
#include "Filters.hpp"
#include "Entity.hpp"

namespace ECE141 {

  class SQLInterpreter;
  
  //----------------------------------

  class SQLStatement: public Statement {
  public:
            SQLStatement(SQLInterpreter &anInterpreter, Keywords aStatementType=Keywords::unknown_kw);
    virtual StatusResult  parse(Tokenizer &aTokenizer);
    
    StatusResult          parseValueList(StringList &aList, Tokenizer &aTokenizer);
    StatusResult          parseIdentifierList(StringList &aList, Tokenizer &aTokenizer);
    StatusResult          parseIdValueList(KeyValues &aList, Tokenizer &aTokenizer);

    virtual StatusResult  run(std::ostream &anOutput);
    
    std::string     name;
    SQLInterpreter  &interpreter;
  };

  //-----------------------------------------------------
  
  class CreateTableStatement: public SQLStatement {
  public:
    
    CreateTableStatement(SQLInterpreter &anInterpreter);
    
    StatusResult parseAttribute(Attribute &anAttribute, Tokenizer &aTokenizer);
    StatusResult parseAttributes(Tokenizer &aTokenizer);
    StatusResult parseAttributeOptions(Attribute &anAttribute, Tokenizer &aTokenizer);
    StatusResult parse(Tokenizer &aTokenizer);
    
    StatusResult run(std::ostream &aStream) const;
    
    std::vector<Attribute>  attributes;
  };

  //----------------------------------
  
  class DescribeStatement: public SQLStatement {
  public:
    
    DescribeStatement(SQLInterpreter &anInterpreter);
    
    StatusResult parse(Tokenizer &aTokenizer);
    StatusResult run(std::ostream &aStream) const;    
  };

  //----------------------------------
  
  class DeleteStatement: public SQLStatement, public Filtered {
  public:
    
    DeleteStatement(SQLInterpreter &anInterpreter);
    
    StatusResult parse(Tokenizer &aTokenizer);
    StatusResult run(std::ostream &aStream) const;
  };

  //----------------------------------
  
  class DropTableStatement: public SQLStatement {
  public:
    
    DropTableStatement(SQLInterpreter &anInterpreter);
    
    StatusResult parse(Tokenizer &aTokenizer);
    StatusResult run(std::ostream &aStream) const;
    
    std::vector<std::string>  fields;
    std::vector<Row>          rows; //one for each record they're inserting...
  };

    //----------------------------------
  

  class InsertStatement: public SQLStatement {
  public:
    
    InsertStatement(SQLInterpreter &anInterpreter);
    
    StatusResult      makeRowsFromValueLists(Tokenizer &aTokenizer, Entity &anEntity, StringList &aFields);
    StatusResult      parse(Tokenizer &aTokenizer);
    StatusResult      run(std::ostream &aStream) const;
    
    std::vector<Row>  rows; //one for each record they're inserting...
  };

  //----------------------------------
  
  class ShowTablesStatement: public SQLStatement {
  public:
    
    ShowTablesStatement(SQLInterpreter &anInterpreter);
    
    StatusResult parse(Tokenizer &aTokenizer);
    StatusResult run(std::ostream &aStream) const;    
  };
  
  //----------------------------------

  class SelectStatement: public SQLStatement, public Filtered {
  public:
    
    SelectStatement(SQLInterpreter &anInterpreter);
    
    StatusResult parseFields(Attribute &anAttribute, Tokenizer &aTokenizer);
    StatusResult parseValues(Tokenizer &aTokenizer);
    StatusResult parseOrderBy(Tokenizer &aTokenizer);
    StatusResult parseGroupBy(Tokenizer &aTokenizer);

    StatusResult parse(Tokenizer &aTokenizer);
    StatusResult run(std::ostream &aStream) const;
    
    bool                      selectAll; //for select * from...
    std::vector<std::string>  fields;
    StringList                groupBy;
    PropertyList              orderBy;    
  };

  //----------------------------------
  
  class UpdateStatement: public SQLStatement, public Filtered {
  public:
    
    UpdateStatement(SQLInterpreter &anInterpreter);
    
    StatusResult parseFields(Tokenizer &aTokenizer);
    StatusResult parseValueLists(Tokenizer &aTokenizer, Entity &anEntity);
    
    StatusResult parse(Tokenizer &aTokenizer);
    StatusResult run(std::ostream &aStream) const;
    
    KeyValues                 keyValues;
    std::vector<Row>          rows; //one for each record they're inserting...
  };
  
}

#endif /* SQLStatement_hpp */
