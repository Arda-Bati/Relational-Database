//
//  Database.cpp
//  Database1
//
//  Created by rick gessner on 4/12/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include "Database.hpp"
#include "View.hpp"
#include "BlockVisitor.hpp"

namespace ECE141 {
  
  bool describeBlock(const Block &aBlock, uint32_t aBlockNum) {
    
    return true;
  }
  
  //==================================================================
  
  // USE: This view is used to do a debug dump of db/storage container...
  class DescribeDatabaseView : public View {
  public:
    DescribeDatabaseView(Storage &aStorage) : storage{aStorage}, stream(nullptr) {}
    
    bool operator()(const Block &aBlock, uint32_t aBlockNum) {
      (*stream) << aBlockNum << ". block " << aBlock.header.type << "\n";
      return true;
    }
    
    bool show(std::ostream &anOutput) {
      stream = &anOutput;
      storage.eachBlock(*this);
      return true;
    }
    
  protected:
    Storage       &storage;
    std::ostream  *stream;
  };
  
  //==================================================================
  
  // USE: This view is used to show list of entities in TOC....
  class ShowTablesView : public View {
  public:
    ShowTablesView(Block &aTOC) : toc{aTOC} {}
    
    bool show(std::ostream &anOutput) {
      for(int i=0;i<toc.entities.header.count;i++) {
        anOutput << toc.entities.items[i].name << std::endl;
      }
      return true;
    }
    
  protected:
    Block         &toc;
  };
  
  //==================================================================
  
  Database::Database(const std::string aName, CreateNewStorage)
  : name(aName), storage(aName, CreateNewStorage{}) {
    //we've created storage, but haven't loaded any entities yet...
  }
  
  Database::Database(const std::string aName, OpenExistingStorage)
  : name(aName), storage(aName, OpenExistingStorage{}) {
    //we've opened storage, but haven't loaded any entities yet...
  }
  
  Database::~Database() {
    saveEntities();
  }
  
  // USE: a child object needs a named entity for processing...
  Entity* Database::getEntity(const std::string &aName) {
    
    //STUDENT: implement this method to retrieve entity from storage...
    if(entities.count(aName)) {
      return entities[aName];
    }
    else {
      //check unloaded schema from storage...
      PersistEntity *thePE = storage.findEntityInTOC(aName);
      if(thePE && thePE->blocknum>0) {
        Block theBlock;
        StatusResult theResult=storage.readBlock(thePE->blocknum, theBlock, sizeof(theBlock));
        if(theResult) {
          Entity *theEntity=new Entity(theBlock,thePE->blocknum);
          entities[aName]=theEntity;
          return theEntity;
        }
      }
    }
    return nullptr;
  }
  
  // USE:   Add a new table (entity);
  // NOTE:  The DB assumes ownership of the entity object...
  StatusResult Database::createTable(Entity *anEntity) {
    std::string &theName=anEntity->getName();
    entities[theName]=anEntity; //Entity name added to TOC
    
    Block theBlock;
    theBlock=(*anEntity); //convert from entity...
    StatusResult theResult= storage.addEntity(theName, theBlock);
    Entity *theEntity=entities[theName];
    theEntity->blockNum=theResult.value; //hang on to the blocknum...
    
    return theResult;
  }
  
  // USE: called to actually delete rows from storage for table (via entity) with matching filters...
  StatusResult Database::deleteRows(const Entity &anEntity, const Filters &aFilters) {
    // STUDENT: impelement this
    return StatusResult{noError};
  }
  
  // USE: Call this to dump the db for debug purposes...
  StatusResult Database::describe(std::ostream &anOutput) {
    if(View *theView = new DescribeDatabaseView(storage)) {
      theView->show(anOutput);  //STUDENT you need to implement the view...
      delete theView;
    }
    return StatusResult{noError};
  }
  
  // USE: call this to actually remove a table from the DB...
  StatusResult Database::dropTable(const std::string &aName) {

    //STUDENT: Implement this method:
    //         1. find the Entity in storage TOC
    //         2  if found, ask storage to drop it
    //         3. if you cache entities on DB, remove it there too...
    
    PersistEntity* dump = storage.findEntityInTOC(aName);
    if (dump){
      storage.dropEntity(aName);

      //TODO
      //We should now erease each block belonging to the this table.
      if (entities.find(aName) != entities.end()) {
        delete entities[aName];
        entities.erase(aName); //removing this entity from the cache
      }
      return StatusResult{noError};
    }
    return StatusResult{unknownTable};
  }
  
  // USE: call this to add a row into the given database...
  StatusResult Database::insertRow(const Row &aRow, const std::string &aTableName) {
    if(Entity *theEntity=getEntity(aTableName)) {
      //STUDENT: Add code here to store the row:
      //   1. encode row into a block
      //   2. add block to storage
      //   3. maybe in the future add it to an index
      //   4. get the next auto_incr value from entity (if you need it for id field)...
    }
    return StatusResult{unknownTable};
  }
  
  // USE: select a set of rows that match given filters (or all rows if filters are empty)...
  StatusResult Database::selectRows(RowCollection &aCollection, const Entity &anEntity,
                                    const Filters &aFilters, const StringList *aFields) {
    //STUDENT: Get blocks for this entity from storage, decode into rows, and add to collection
    //         NOTE:  aFields (if not empty) tells you which fields to load per row;
    //                otherwise load all fields (*)
    return StatusResult{noError};
  }
  
  //USE: resave entities that were in memory and changed...
  StatusResult Database::saveEntities() {
    StatusResult theResult{noError};
    for (auto thePair : entities) {
      if(thePair.second->isDirty()) {
        Block theBlock=(*thePair.second);
        theResult=storage.writeBlock(thePair.second->blockNum, theBlock);
      }
    }
    return theResult;
  }
  
  //USE: show the list of tables in this db...
  StatusResult Database::showTables(std::ostream &anOutput) {
    
    //STUDENT: create a ShowTablesView object, and call the show() method...
    //         That view is declared at the top of this file.

    if(View *theView = new ShowTablesView(storage.toc)) {
      theView->show(anOutput);  //STUDENT you need to implement the view...
      delete theView;
    }
    
    return StatusResult{noError};
  }
  
  // USE: called to update records already in db (future assignment)...
  StatusResult Database::updateRows(KeyValues &aList, const Entity &anEntity, const Filters &aFilters) {
    //STUDENT: Implement this...
    return StatusResult{noError};
  }
  
  // USE: asks the DB if a table exists in storage...
  bool Database::tableExists(const std::string aName) {
    //STUDENT: implement this if you need it...
    return false;
  }
  
}

