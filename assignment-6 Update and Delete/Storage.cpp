//
//  Storage.cpp
//  ECEDatabase
//
//  Created by rick gessner on 4/1/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include "Storage.hpp"
#include "Row.hpp"
#include <iostream>
#include <sstream>

#define _CRT_SECURE_NO_WARNINGS 1
#define _CRT_SECURE_NO_DEPRECATE 1 //turn off annoying ms warnings...

namespace ECE141 {
  

  //Return path to the folder where you want to store your DB's...
  const char* Storage::getDefaultStoragePath() {
    return "/tmp";
  }
  
  //path to the folder where you want to store your DB's...
  std::string Storage::getDatabasePath(const std::string &aDBName) {
    std::ostringstream theStream;
    theStream << Storage::getDefaultStoragePath() << "/" << aDBName << ".db";
    return theStream.str();
  }
  
  // USE: ctor ---------------------------------------
  Storage::Storage(const std::string aName, CreateNewStorage) : toc('T'), name(aName) {
    StatusResult theResult=makeEmpty(); //create file, and you can init the contents...
    if(!theResult) throw std::runtime_error("Can't create file");
  }
  
  // USE: ctor ---------------------------------------
  Storage::Storage(const std::string aName, OpenExistingStorage) : toc('T'), name(aName) {
    std::string thePath = Storage::getDatabasePath(aName);
    stream.open (thePath.c_str(), std::fstream::binary | std::fstream::in | std::fstream::out);
    if(stream) {
      stream.seekg(stream.tellg(), std::ios::beg); //C++ trick to sync stream buffers...
      if(!loadTOC()) {
        throw std::runtime_error("Can't read file");
      }
    }
    else throw std::runtime_error("Can't open file");
  }

  // USE: dtor ---------------------------------------
  Storage::~Storage() {
    writeBlock(0,toc);
    stream.close();
  }
  
  // USE: retrieve toc (block=0) from storage --------------------------------------
  StatusResult Storage::loadTOC() {
    
    //STUDENT: It's TOTALLY up to you where you put your TOC (if you do at all)
    //         By default, we put this at block 0
    
    stream.seekg(stream.tellg(), std::ios::beg); //C++ trick to sync stream buffers...
    return Storage::readBlock(0, toc);
  }
  
  // USE: Do WHATEVER you want to do to manage your file here. Maybe, create a TOC block?
  StatusResult Storage::setupEmptyStorage() {    
    //STUDENT: It's TOTALLY up to you how you want to manage storage file internals...
    //         Our default approach to keep the list of entities (TOC) in block 0...
    return addBlock(toc);
  }
  
  // USE: create a new storage file ---------------------------------------
  StatusResult Storage::makeEmpty() {
    std::string thePath = Storage::getDatabasePath(name);
    stream.clear(); // Clear Flag, then create file...
    stream.open(thePath.c_str(), std::fstream::in | std::fstream::out | std::fstream::trunc);
    stream.close();
    stream.open(thePath.c_str(), std::fstream::binary | std::fstream::in | std::fstream::out);
    if(stream.is_open()) {
      return setupEmptyStorage();
    }
    return StatusResult{writeError};
  }
  
  // USE: validate we're open and ready ---------------------------------------
  bool Storage::isReady() const {
    return stream.is_open();
  }
  
  // USE: count blocks in file ---------------------------------------
  uint32_t Storage::getTotalBlockCount() {
    stream.seekg(stream.tellg(), std::ios::beg); //force read mode; dumb c++ issue...
    stream.seekg(0, std::ios::end);
    int thePos = (int)stream.tellg();
    return thePos / sizeof(Block);
  }
  
  //Linear scan (ugh) for free blocks; if we don't fine one, append a new one...
  //return statusresult (the blocknum is returned as proxied value)
  StatusResult Storage::findFreeBlockNum() {
    uint32_t      theCount=getTotalBlockCount();
    StatusResult  theResult{noError};
    Block         theBlock;

    stream.seekg(stream.tellg(), std::ios::beg); //force read mode; dumb c++ issue...
    for(uint32_t theIndex=0;theIndex<theCount;theIndex++) {
      theResult=readBlock(theIndex, theBlock);
      if(!theResult) return theResult;
      if(kFreeBlockType==theBlock.header.type) {
        return StatusResult{noError,theIndex};
      }
    }
    
    //ok, we didn't find one, so add a new one, and return that blocknum
    return StatusResult{noError, theCount};
  }
    
  // USE: read data a given block # ---------------------------------------
  StatusResult Storage::readBlock(int aBlockNumber, Block &aBlock, std::size_t aBlockSize) {
    stream.seekg(aBlockNumber * sizeof(Block));
    if(stream.read ((char*)&aBlock, aBlockSize)) {
      return StatusResult{noError};
    }
    return StatusResult(readError);
  }
  
  // USE: low-level write data to given block (assumes seekp is set) ------------------------
  StatusResult write(Block &aBlock, std::fstream &aStream) {
    if(aStream.write ((char*)&aBlock, sizeof(Block))) {
      aStream.flush();
      return StatusResult{};
    }
    return StatusResult{writeError};
  }
  
  // USE: write data a given block (after seek) ---------------------------------------
  StatusResult Storage::writeBlock(int aBlockNumber, Block &aBlock) {
    stream.seekg(stream.tellg(), std::ios::beg); //attemp to sync stream buffers...
    stream.seekp(aBlockNumber * sizeof(Block));
    return write(aBlock, stream);
  }
  
  // USE: save block (in free block, or appended bloc ---------------------
  StatusResult Storage::addBlock(Block &aBlock) {
    StatusResult theResult=findFreeBlockNum();
    if(theResult) {
      return writeBlock(theResult.value, aBlock);
    }
    
    stream.seekg(stream.tellg(), std::ios::beg); //attemp to sync stream buffers...
    stream.seekp(0, std::ios::end); //append...
        
    return write(aBlock, stream);
  }
  
  // USE: mark block as free ---------------------------------------
  StatusResult Storage::releaseBlock(int aBlockNumber) {
    Block theBlock;
    memset(&theBlock,0,sizeof(theBlock)); //make an empty block, mark as [F]ree
    theBlock.header.type='F';
    return writeBlock(aBlockNumber,theBlock);
  }
  
  // USE: add entity to TOC, and add block to storage --------------------------
  StatusResult Storage::addEntity(const std::string &aName, Block &aBlock) {
    StatusResult theResult = findFreeBlockNum();
    if(theResult) {
      PersistEntity theEntity;
      
      size_t theLength=aName.length();
#if defined(__APPLE__) || defined(__linux__)
      std::strncpy(theEntity.name, aName.c_str(), theLength);
#else
      std::strncpy(theEntity.name, aName.c_str(), theLength);
      //strncpy_s(theEntity.name, kStoredIdentifierLen, aName.c_str(), theLength);
#endif
      theEntity.name[theLength]=0;
      
      theEntity.blocknum=theResult.value;
      memcpy(&toc.entities.items[toc.entities.header.count++], &theEntity, sizeof(PersistEntity));
      theResult=writeBlock(theEntity.blocknum, aBlock);
      theResult.value=theEntity.blocknum; //return this to user...
    }

    return theResult;
  }
  
  //Find name in TOC.entities, return index, or -1 if not found.
  int findIndexOfEntityInTOC(const PersistEntities &aList, const std::string &aName) {
    for(int i=0;i<aList.header.count;i++) {
      if(aName==aList.items[i].name) {
        return i;
      }
    }
    return -1;
  }
  
  //** USE: check toc for named entity; return ptr to PE, or nullptr if not found...
  PersistEntity* Storage::findEntityInTOC(const std::string &aName) {
    int theIndex = findIndexOfEntityInTOC(toc.entities, aName);
    return theIndex >=0 ? &toc.entities.items[theIndex] : nullptr;
  }

  //** USE: remove entity from TOC and mark entity-block free --------
  StatusResult Storage::dropEntity(const std::string &aName) {

    //Find entity with this name in TOC and remove it from the list.
    //Decrement the count in the entities header.
    
    int theIndex = findIndexOfEntityInTOC(toc.entities, aName);
    if(theIndex>=0) {
        //if theindex < toc.entites.count, overwrite entity at theIndex, with last Entity
      PersistEntity *theLastPE=&toc.entities.items[toc.entities.header.count-1];
      PersistEntity *thePE=&toc.entities.items[theIndex];
      releaseBlock(thePE->blocknum); //mark block as free...
      memset(thePE,0,sizeof(PersistEntity)); //clear it out for human readability
      if(thePE<theLastPE) {
        memcpy(thePE, theLastPE, sizeof(PersistEntity)); //overwrite index with nth...
      }
      toc.entities.header.count--;
      return StatusResult{noError};
    }
    return StatusResult{notImplemented};
  }

  // USE: call this to iterate blocks for your own purpose. You'll get a callback with each block
  StatusResult Storage::eachBlock(StorageCallback aCallback) {
    uint32_t      theCount=getTotalBlockCount();
    Block         theBlock;
    StatusResult  theResult{noError};
    
    for(uint32_t theIndex=0;theIndex<theCount;theIndex++) {
      theResult=readBlock(theIndex, theBlock);
      if(theResult) {
        theResult = aCallback(*this, theBlock, theIndex);
        if(!theResult) break;
      }
      else break;
    }
    return theResult;
  }


}

