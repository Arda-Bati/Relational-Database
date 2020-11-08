//
//  Database.cpp
//  Database3
//
//  Created by rick gessner on 4/12/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include "Database.hpp"
#include "View.hpp"

//#include "Entity.hpp" --- assignment 4...


namespace ECE141 {
  
  bool describeBlock(const Block &aBlock, uint32_t aBlockNum) {
    
    return true;
  }

  //==================================================================
  //** USE: This view is used to do a debug dump of db/storage container...
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
    
  //USE: asks the DB if a table exists in storage...
  bool Database::tableExists(const std::string aName) {
    return false;
  }
  
 StatusResult Database::saveEntities() {
    StatusResult theResult{noError};
/*  COMING SOON...
    for (auto thePair : entities) {
      if(thePair.second->isDirty()) {
        Block theBlock=(*thePair.second);
        theResult=storage.writeBlock(thePair.second->blockNum, theBlock);
      }
    }
*/
    return theResult;
  }
  
  //USE: show the list of tables in this db...
  StatusResult Database::showTables(std::ostream &anOutput) {
    for(int i=0;i<storage.toc.entities.header.count;i++) {
      anOutput << storage.toc.entities.items[i].name << std::endl;
    }
    return StatusResult{noError};
  }

  // ** USE: Call this to dump the db for debug purposes...
  StatusResult Database::describe(std::ostream &anOutput) {
    if (View *theView = new DescribeDatabaseView(storage)){
    theView->show(anOutput);
    delete theView;
    return StatusResult{noError};
  }
    
    return StatusResult{unknownType};
  }
  
/*
 
  //COMING SOON -- This will be in assignment 4...
 
 
  //USE: a child object needs a named entity for processing...
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

  // ** USE: add a new table (entity);
  //   NOTE:  The DB assumes ownership of the entity object...
  StatusResult Database::createTable(Entity *anEntity) {
    std::string &theName=anEntity->getName();
    entities[theName]=anEntity;
    
    Block theBlock;
    theBlock=(*anEntity); //convert from entity...
    StatusResult theResult= storage.addEntity(theName, theBlock);
    Entity *theEntity=entities[theName];
    theEntity->blockNum=theResult.value;
    return theResult;
  }
 
  // ** USE: call this to actually remove a table from the DB...
  StatusResult Database::dropTable(const std::string &aName) {
    if(PersistEntity *thePE=storage.findEntityInTOC(aName)){

      //1. remove from db.entities...
      entities.erase(aName); //erase from entities (may not be there)...
      
      //2. remove from storage...
      StatusResult theResult=storage.dropEntity(aName);
      return theResult;
    }
    return StatusResult{unknownTable};
  }

  // ** USE: call this to add a row into the given database...
  StatusResult Database::insertRow(const Row &aRow, const std::string &aTableName) {
    if(Entity *theEntity=getEntity(aTableName)) {
      
      //STUDENT: Add code here to store the row:
      //   1. encode row into a block
      //   2. add block to storage
      //   3. maybe in the future add it to an index
      //   Question: where does the Auto-incrementing ID for key
      
      return StatusResult{noError};
    }
    return StatusResult{unknownTable};
  }
 
*/
  
}

