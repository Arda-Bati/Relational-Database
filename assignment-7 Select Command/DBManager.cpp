//
//  DBManager.cpp
//  Database3
//
//  Created by rick gessner on 3/25/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include <iostream>
#include <string>
#include "DBManager.hpp"
#include "Tokenizer.hpp"
#include "Storage.hpp"
#include "Statement.hpp"
#include "keywords.hpp"
#include "Database.hpp"
#include "FolderReader.hpp"
#include "DBStatement.hpp"

namespace ECE141 {

  
  class FolderView : public View {
  public:
    FolderView(const FolderReader &aReader, const char *anExtension="db")
      : reader(aReader), extension(anExtension), stream(nullptr) {}
    
    virtual ~FolderView() {}
    
    //STUDENT: add operator() here to receive files from a Folder Reader...
    bool operator()(const std::string &aString) {
      (*stream) << aString << std::endl;
      return true;
    }
    
    //called for each db file in your storage folder...
    virtual bool show(std::ostream &aStream) {
      stream = &aStream;
      aStream << "Showing databases: \n";
      reader.each(*this, "db");
      return true;
    }
    
    const FolderReader  &reader;
    const char          *extension;
    std::ostream        *stream;
  };


  //===================================================================
  
  
  DBManager::DBManager(IInterpreter *aNext) : CommandProcessor(aNext) {
    activeDB=nullptr;
  }
  
  DBManager::~DBManager() {
    releaseDB();
  }
  
  DBManager& DBManager::releaseDB() {
    if(activeDB) {
      delete activeDB;
      activeDB=nullptr;
    }
    return *this;
  }
  
  // USE: act as statement factory for DB-commands...
  Statement* DBManager::getStatement(Tokenizer &aTokenizer) {
    Statement *theResult=nullptr;
    if(aTokenizer.size()) {
      Token theToken = aTokenizer.current();
      switch(theToken.keyword) {
        case Keywords::create_kw    : theResult = new CreateDBStatement(*this); break;
        case Keywords::drop_kw      : theResult = new DropDBStatement(*this); break;
        case Keywords::show_kw      : theResult = new ShowDBStatement(*this); break;
        case Keywords::describe_kw  : theResult = new DescribeDBStatement(*this); break;
        case Keywords::use_kw       : theResult = new UseDBStatement(*this); break;
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
  
  // USE: call this to find (and open) db (storage) file with given name in known location...
  Database* DBManager::loadDatabase(const std::string &aName) const {
    Database *theResult=nullptr;
    if(FolderReader *theReader= FolderReader::factory(Storage::getDefaultStoragePath())) {
      if(theReader->exists(Storage::getDatabasePath(aName))) {
        theResult= new Database(aName, OpenExistingStorage{});
      }
      delete theReader;
    }
    return theResult;
  }
  
  // USE: called when it's time to create an actual DB file...
  StatusResult DBManager::createDatabase(const std::string &aName) {
    
    activeDB=releaseDB().loadDatabase(aName); //try to load DB with given name...
    if(!activeDB) {
      
      //if you're here, we can CREATE a db because the given name isn't used...
      
      try {
        
        //STUDENT:  create a new 'activeDB' database by constructing
        //a new Database object, use CreateNewStorage{} tag...
        
        activeDB = new Database(aName, CreateNewStorage{}); //SET THIS TO NEW DB you created...
        
        //STUDENT: Write a message to std::out indicating the task has been completed...
        std::cout << "created db " << aName << std::endl;
        
        //if it all goes well, return StatusResult{noError},
        //otherwise return appropriate error code...
        
        return StatusResult{noError};
      }
      catch(...) {
        
      }
    }
    return StatusResult{noError};
  }
  
  // USE: call this to perform the dropping of a database (remove the file)...
  StatusResult DBManager::dropDatabase(const std::string &aName) {
    std::string thePath = Storage::getDatabasePath(aName);

    FolderReader *theReader = FolderReader::factory(Storage::getDefaultStoragePath());
    if (theReader->exists(thePath)) {
      std::remove(thePath.c_str());
      delete theReader;
      std::cout << "Database " << aName << " deleted" << std::endl;
      return StatusResult{noError};
    }

    delete theReader;
    return StatusResult{unknownDatabase};
  }
  
  // USE: call this to have the DB dump all storage blocks in descriptive format... (DEBUG)
  StatusResult DBManager::describeDatabase(const std::string &aName) const {    
    if(Database *theDB=loadDatabase(aName)) {
      theDB->describe(std::cout);
      delete theDB;
      return StatusResult{noError};
    }
    return StatusResult{ECE141::unknownDatabase};
  }
  
  // USE: call this to have system display list of DB files from known storage location...
  StatusResult DBManager::showDatabases() const {
    FolderReader *theReader= FolderReader::factory(Storage::getDefaultStoragePath());
    FolderView *theView = new FolderView(*theReader);
    theView->show(std::cout);
    delete theView;
    delete theReader;

    return StatusResult{noError};
  }
  
  // USE: call this to cause a DB object to be loaded into memory for use...
  StatusResult DBManager::useDatabase(const std::string &aName) {
    Database *theDatabase = loadDatabase(aName);
    if (nullptr!=theDatabase) {
      releaseDB();
      activeDB = theDatabase;

      std::cout << "Using " << aName << " database" << std::endl;
      return StatusResult{ECE141::noError};
    }

    //STUDENT: your tasks:
    //  1. Try to load the database object with given name (see: loadDatabase())...
    //  2. If it worked, show a success msg, and return noError status, otherwise, return error status
    //  3. save loaded database into the activeDB data member!

    return StatusResult{ECE141::unknownDatabase};
  }
  
  // USE: router and observer for db-commands...
  StatusResult DBManager::interpret(const Statement &aStatement) {
    return aStatement.run(std::cout);
  }
  
  
}
