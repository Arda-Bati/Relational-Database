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
#include "Entity.hpp"
#include "Filters.hpp"
#include "Row.hpp"

namespace ECE141 {

  bool describeBlock(const Block &aBlock, uint32_t aBlockNum) {
    
    return true; 
  }

  class SelectRowBlockVisitor: public BlockVisitor {
  public:
    SelectRowBlockVisitor(const Entity &anEntity, const Filters &aFilters, RowCollection &aRowCollection, const StringList* aFields)
    : BlockVisitor(anEntity, aFilters), rowCollection(aRowCollection), fields(aFields) {}
    StatusResult operator()(Storage &aStorage, const Block &aBlock, uint32_t aBlockNum) {
      if('D'==aBlock.header.type) {
        uint32_t hash = const_cast<Entity&>(entity).getHash();
        if(hash==aBlock.header.extra) {
          Row *theRow = new Row(aBlock /* /*block_num?*/); //Hang on to the block number?
          //KeyValues &aKeyValueList = const_cast<Row&>(theRow).getColumns(); //need const_cast?
          KeyValues &aKeyValueList = theRow->getColumns();
          //std::cout << "Before matches" << std::endl;
          if (filters.matches(aKeyValueList)) {
            if(nullptr == fields){ //fields.empty()
              //std::cout << "fields.empty()" << fields.empty() << std::endl;
              rowCollection.add(theRow);  //block_num here?
            }
            else{
              //std::cout << "Doesn't match" << std::endl;
              KeyValues aKVList;
              for (auto field : *fields) {
                if (aKeyValueList.count(field)) {
                  aKVList.insert({field, aKeyValueList[field]});
                }
                else {
                  try {
                    Attribute& anAttribute = const_cast<Entity&>(entity).getAttribute(field);
                    Value aValue = const_cast<Entity&>(entity).getDefaultValue(anAttribute);
                    aKVList.insert({field, aValue});
                  }
                  catch(...) {
                    return StatusResult{invalidAttribute};
                    delete theRow;
                  }
                  //
                }
              }
              theRow = new Row(aKVList);
              rowCollection.add(theRow); 
            }
          } 
        }
      }
      return StatusResult{noError};
    }
    //const Entity            &entity; Don't need these guys
    //const Filters           &filters; Dammit
    RowCollection&            rowCollection;
    const StringList*         fields;

    const RowCollection& getCollection() {
        return rowCollection;
    }
  };

  
  //==================================================================
  
  // USE: This view is used to do a debug dump of db/storage container...
  class DescribeDatabaseView : public View {
  public:
    DescribeDatabaseView(Storage &aStorage) : storage{aStorage}, stream(nullptr) {}
    
    StatusResult operator()(Storage &storage, const Block &aBlock, uint32_t aBlockNum) {
      (*stream) << aBlockNum << ". block " << aBlock.header.type << "\n";
      return StatusResult{noError};
      //return true;
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
    StatusResult theResult{unknownTable};
    uint32_t autoInc;
    if(Entity *theEntity=getEntity(aTableName)) {
      KeyValues &aList = const_cast<Row&>(aRow).getColumns();
      //make sure that attribute is in entity, and that data types match
      if (theEntity->validate(aList)) {
        // Now we are sure this is valid row
        // We will insert this row so incrementing theEntity's internal autoincrement variable
        autoInc = theEntity->getNextAutoIncrementValue();
        // Checking if theEntity has a primaryKey. 
        // If it has one, let's assume it is auto incremented (for now...)
        std::string attrName = theEntity->getPrimaryKey();
        if("" != attrName){
          // Finding this attribute in the aList and updating its value to the current autoincrement value
          auto it = aList.find(attrName); 
          if (it != aList.end()) {
              it->second = (int)autoInc;
          }
          else { //user likely not going to enter id when inserting row, as we need to make it for them... therefore it's not going to be in aList
            aList.insert({attrName, (int)autoInc});
          }
          // Else, we don't have to do anything (considering the primary key & auto increment assumption made above)
        }
        
        Block theBlock(aList);
        //use extra value of block header to store hash of entity
        theBlock.header.extra = theEntity->getHash();
        theBlock.header.id = autoInc;//returns a uint_32...
        theResult = storage.findFreeBlockNum();
        uint32_t blocknum = theResult.value;
        theResult = storage.writeBlock(blocknum, theBlock);
      }

      /*if(theAttr.isAutoIncrement()){ //IF the attribute is autoIncrement, and we are creating a new row
        // Ignoring the user (if they want to change this field) and updating the Primary Key on our own
        // Not right! Though otherwise we have to deal with many edge cases!
        try{
          aKVList[theName] += 1;
        }
        catch{
          std::cout << "Wrong auto incrementing field type." << std::endl;
        }
      }*/
    }
    return theResult;
  }
  

  using StorageCallback = std::function<StatusResult(Storage &aStorage, const Block &aBlock, uint32_t aBlockNum)>;


  // USE: select a set of rows that match given filters (or all rows if filters are empty)...
  StatusResult Database::selectRows(RowCollection &aCollection, const Entity &anEntity,
                                    const Filters &aFilters, const StringList *aFields) {
    //STUDENT: Get blocks for this entity from storage, decode into rows, and add to collection
    //         NOTE:  aFields (if not empty) tells you which fields to load per row;
    //                otherwise load all fields (*)
    SelectRowBlockVisitor blockV(anEntity, aFilters, aCollection, aFields);
    StatusResult theResult = storage.eachBlock(blockV);
    aCollection = blockV.getCollection();

    return theResult;
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

