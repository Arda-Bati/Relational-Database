//
//  SQLInterpreter.hpp
//  Datatabase4
//
//  Created by rick gessner on 4/13/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef SQLInterpreter_hpp
#define SQLInterpreter_hpp

#include <stdio.h>
#include "CommandProcessor.hpp"
#include "Row.hpp"

namespace ECE141 {
  
  class Entity; //frwd...
  class Filters;
  
  class SQLInterpreter : public CommandProcessor {
  public:
    
    SQLInterpreter(IInterpreter *aNext=nullptr);
    virtual ~SQLInterpreter();
    
    StatusResult          createTable(Entity *anEntity); //low level DB call...
    StatusResult          dropTable(const std::string &aName); //low level DB call...
    StatusResult          showTables();
    
    StatusResult          insertRow(const Row &aRow, const std::string &aTableName); //low level DB call...
    StatusResult          deleteRows(const std::string &aTableName, const Filters &aFilters); //low level
    StatusResult          selectRows(const std::string &aTableName, const Filters &aFilters); //low level

    StatusResult          describeTable(const std::string &aName, std::ostream &anOutput);

  protected:
    
    virtual Statement*    getStatement(Tokenizer &aTokenizer);
    virtual StatusResult  interpret(const Statement &aStatement);
  };

}

#endif /* SQLInterpreter_hpp */
