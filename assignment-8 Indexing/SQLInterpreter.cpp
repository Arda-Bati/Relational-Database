//
//  SQLInterpreter.cpp
//  Datatabase4
//
//  Created by rick gessner on 4/13/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include "SQLInterpreter.hpp"
#include "RGTokenizer.hpp"
#include "Storage.hpp"
#include "Statement.hpp"
#include "keywords.hpp"
#include "Database.hpp"
#include "Attribute.hpp"
#include "SQLStatement.hpp"
#include "Entity.hpp"
#include "RecordsView.hpp"
#include "EntityDescriptionView.hpp"

namespace ECE141 {
  
  
  SQLInterpreter::SQLInterpreter(IInterpreter *aNext) : CommandProcessor(aNext) {}
  SQLInterpreter::~SQLInterpreter() {}
    
  // USE: -----------------------------------------------------
  StatusResult SQLInterpreter::createTable(Entity *anEntity) {
    if(Database *theDatabase=getActiveDatabase()) {
      if(anEntity) {
        StatusResult theResult = theDatabase->createTable(anEntity);
        if(theResult) {
		  //std::cout << "table created\n" << std::endl;;
          return theResult;
        }
        return StatusResult{invalidCommand};
      }
    }
    return StatusResult{unknownDatabase};
  }
  
  // USE: called when user requests rows to be deleted....
  StatusResult SQLInterpreter::deleteRows(const std::string &aName, const Filters &aFilters) {
    StatusResult result = StatusResult{writeError};
    if(Database *theDatabase=getActiveDatabase()) {
      return theDatabase->deleteRows(aName, aFilters);
    }
    return result;
  }

  StatusResult SQLInterpreter::describeTable(const std::string &aName, std::ostream &anOutput) {
    if(Database *theDatabase=getActiveDatabase()) {
      if(Entity *theEntity=theDatabase->getEntity(aName)) {
        View *theView = new EntityDescriptionView(*theEntity);
        theView->show(anOutput);
        delete theView;
        return StatusResult{noError};
      }
      return StatusResult{unknownTable};
    }
    return StatusResult{unknownDatabase};
  }

  // USE: -----------------------------------------------------
  StatusResult SQLInterpreter::dropTable(const std::string &aName) {
    if(Database *theDatabase=getActiveDatabase()) {
      StatusResult theResult = theDatabase->dropTable(aName);
      if(theResult) {
        std::cout << "tabled dropped\n";
        return theResult;
      }
    }
    return StatusResult{unknownDatabase};
  }
  
  // USE: -----------------------------------------------------
  StatusResult SQLInterpreter::insertRow(const Row &aRow, const std::string &aTableName) {
    if(Database *theDatabase=getActiveDatabase()) {
      return theDatabase->insertRow(aRow, aTableName);
    }
    return StatusResult{unknownDatabase};
  }
  
  // USE: called when user requests rows to be selected....
  StatusResult SQLInterpreter::selectRows(const std::string &aName, const Filters &aFilters,
                                          const PropertyList &anOrderBy) {
    if(Database *theDatabase=getActiveDatabase()) {
      if(Entity *theEntity=theDatabase->getEntity(aName)) {
        RowCollection theCollection;
        StatusResult theResult=theDatabase->selectRows(theCollection, *theEntity, aFilters);
        if(anOrderBy.size()) {
          theCollection.reorder(anOrderBy, *theEntity);
        }
        if(View *theView=new RecordsView(*theEntity, theCollection )) {
          theView->show(std::cout);
          delete theView;
        }
        return theResult;
      }
    }
    return StatusResult{unknownDatabase};
  }
  
  StatusResult SQLInterpreter::updateRows(const std::string &aName, const KeyValues &aKeyValues,
                                          const Filters &aFilters) {
    StatusResult theResult{unknownDatabase};
    if(Database *theDatabase=getActiveDatabase()) {
      if(Entity *theEntity=theDatabase->getEntity(aName)) {        
        RowCollection theCollection;
        if(theResult=theDatabase->selectRows(theCollection, *theEntity, aFilters)) {
          RowList& theRows=theCollection.getRows();
          for(auto *theRow : theRows) {
            theResult=theDatabase->updateRow(*theRow, aKeyValues, *theEntity);
            if(!theResult) break;
          }
        }
      }
    }
    return theResult;
  }

  // USE: -----------------------------------------------------
  StatusResult SQLInterpreter::showTables() {
    if(Database *theDatabase=getActiveDatabase()) {
      return theDatabase->showTables(std::cout);
    }
    return StatusResult{unknownDatabase};
  }
  
  //=========================================
  
  // USE: called when a SQLStatement is parsed and runnable ------
  StatusResult SQLInterpreter::interpret(const Statement &aStatement) {
    return aStatement.run(std::cout);
  }
  
  // USE: retrieve a statement based on given text input...
  Statement* SQLInterpreter::getStatement(Tokenizer &aTokenizer) {
    Statement *theResult=nullptr;
    if(aTokenizer.remaining()) {
      Token theToken = aTokenizer.current();
      switch(theToken.keyword) {
        case Keywords::create_kw  : theResult = new CreateTableStatement(*this); break;
        case Keywords::drop_kw    : theResult = new DropTableStatement(*this); break;
        case Keywords::insert_kw  : theResult = new InsertStatement(*this); break;
        case Keywords::delete_kw  : theResult = new DeleteStatement(*this); break;
        case Keywords::update_kw  : theResult = new UpdateStatement(*this); break;
        case Keywords::select_kw  : theResult = new SelectStatement(*this); break;
        case Keywords::describe_kw: theResult = new DescribeStatement(*this); break;
        case Keywords::show_kw:     theResult = new ShowTablesStatement(*this); break;
        default: break;
      }
      if(theResult) {
        StatusResult theError = theResult->parse(aTokenizer);
        if(!theError) {
          delete theResult;
          theResult=nullptr;
        }
      }
    }
    return theResult;
  }
  
  
}
