//
//  Database.hpp
//  Database3
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef Database_hpp
#define Database_hpp

#include <stdio.h>
#include <cstring>
#include <string>
#include <iostream>
#include <map>
#include "CommandProcessor.hpp"
#include "Storage.hpp"
#include "Row.hpp"

namespace ECE141 {
  
  class Parser;
  class Statement;
  class Entity;
  

  class Database  {
  public:
    
    Database(const std::string aPath, CreateNewStorage);
    Database(const std::string aPath, OpenExistingStorage);

    //Database(const Database &aDatabase);
    
    ~Database();
    
    bool            tableExists(const std::string aName);

    StatusResult    saveEntities();
    StatusResult    showTables(std::ostream &anOutput);

    std::string&    getName() {return name;}
    Entity*         getEntity(const std::string &aName);

    StatusResult    createTable(Entity *anEntity);
    StatusResult    dropTable(const std::string &aName);

    StatusResult    describe(std::ostream &anOutput); //dump the DB (storage) for debug purpose

    //Table related...

/*

    coming soon...

    StatusResult    insertRow(Row &aRow, const std::string &aTableName);
    RowCollection*  selectRows(const PropertyList &aFields, const FilterList &aFilters, const std::string &aTableName);
    StatusResult    updateRows(const KeyValues &aKVList, const FilterList &aFilters, const std::string &aTableName);
    StatusResul     deleteRow(const FilterList &aFilters, const std::string &aTableName);
 
    ...others...
 
 */

    
  protected:
    std::string                     name;
    Storage                         storage;
    std::map<std::string, Entity*>  entities;
  };
  
}

#endif /* Database_hpp */
