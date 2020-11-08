//
//  DBManager.hpp
//  Database3
//
//  Created by rick gessner on 3/25/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef DBManager_hpp
#define DBManager_hpp

#include <stdio.h>
#include <string>
#include "CommandProcessor.hpp"

namespace ECE141 {
  
  class Statement;
  class Database; //define this later...
  
  class DBManager : public CommandProcessor {
  public:
    
                          DBManager(IInterpreter *aNext=nullptr);
    virtual               ~DBManager();
    
    virtual Statement*    getStatement(Tokenizer &aTokenizer);
    virtual StatusResult  interpret(const Statement &aStatement);

    //db related commands...
    Database*             getActiveDatabase() {return activeDB;}
    
    StatusResult          useDatabase(const std::string &aName);
    StatusResult          createDatabase(const std::string &aName);
    StatusResult          dropDatabase(const std::string &aName);
    StatusResult          showDatabases() const;
    StatusResult          describeDatabase(const std::string &aName) const;

  protected:
    
    Database              *activeDB;
    
    Database*             loadDatabase(const std::string &aName) const; 
    DBManager&            releaseDB();
  };
  
}
#endif /* DBManager_hpp */
