//
//  Storage.cpp
//  ECEDatabase
//
//  Created by rick gessner on 4/1/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include <sstream>
#include <iostream>
#include "Storage.hpp"

namespace ECE141 {
  
  Block::Block(const KeyValues &aKVList) {
    //STUDENT: Implement this!...
    //char[] data...
    //
    std::stringstream theStream;
    for (auto aKVPair : aKVList) {
      theStream << aKVPair.first << "\"" << (char)aKVPair.second.getType()
                << "\"" << aKVPair.second.getString() << "\"";
      /*theStream << "{key:" << aKVPair.first << ", type:" << (char)aKVPair.second.getType()
                << ",value:\"" << aKVPair.second.getString() << "\"}";*/
      //so want to add {}, ',', and quotes along values
    }
    strcpy(data, theStream.str().c_str()); //do we need strcpy??
    //std::cout << data << std::endl;
  }
  
  //path to the folder where you want to store your DB's...
  const char* Storage::getDefaultStoragePath() {
    return "/tmp";
  }
  
  // USE: ctor ---------------------------------------
  Storage::Storage(const std::string aName) : toc('T',0), name(aName) {
    char thePath[128];
    sprintf(thePath,"%s/%s.%s", getDefaultStoragePath(), aName.c_str(),"db");
    stream.open (thePath, std::fstream::binary | std::fstream::in | std::fstream::out);
    
    toc.header.count=0; //init for new db...
    memset(toc.data,0,sizeof(toc.data));
  }
  
  // USE: dtor ---------------------------------------
  Storage::~Storage() {
    toc.header.dirty=false;
    writeBlock(0,toc);
    stream.close();
  }
  
  // USE: retrieve toc (block=0) from storage --------------------------------------
  StatusResult Storage::loadTOC() {
    stream.seekg(stream.tellg(), std::ios::beg); //C++ trick to sync stream buffers...
    return Storage::readBlock(0, toc);
  }
  
  // USE: create a new storage file ---------------------------------------
  StatusResult Storage::makeEmpty() {
    char thePath[128];
    sprintf(thePath,"%s/%s.%s", getDefaultStoragePath(), name.c_str(),"db");
    stream.clear(); // Clear Flag, then create file...
    stream.open(thePath, std::fstream::in | std::fstream::out | std::fstream::trunc);
    stream.close();
    stream.open(thePath, std::fstream::binary | std::fstream::in | std::fstream::out);
    return addBlock(toc);
  }
  
  // USE: validate we're open and ready ---------------------------------------
  bool Storage::isReady() {
    return stream.is_open();
  }
  
  // USE: count blocks in file ---------------------------------------
  uint32_t Storage::getTotalBlockCount() {
    stream.seekg(stream.tellg(), std::ios::beg); //force read mode; dumb c++ issue...
    stream.seekg(0, std::ios::end);
    int thePos = (int)stream.tellg();
    return thePos / kBlockSize;
  }
  
  // USE: scan free blocks. if not found, add a new block --------------------------------
  uint32_t Storage::findFreeBlockId() {
    if(toc.header.id) {
      //we have a free block...
    }
    return 0;
  }
  
  // USE: read data a given block # ---------------------------------------
  StatusResult Storage::readBlock(int aBlockNumber, Block &aBlock, std::size_t aBlockSize) {
    stream.seekg(aBlockNumber * kBlockSize);
    if(stream.read ((char*)&aBlock, aBlockSize)) {
      return StatusResult{};
    }
    return StatusResult(ECE141::readError);
  }
  
  // USE: low-level write data to given block (assumes seekp is set) ------------------------
  StatusResult write(Block &aBlock, std::fstream &aStream) {
    if(aStream.write ((char*)&aBlock, kBlockSize)) {
      aStream.flush();
      return StatusResult{};
    }
    return StatusResult{ECE141::writeError};
  }
  
  // USE: write data a given block (after seek) ---------------------------------------
  StatusResult Storage::writeBlock(int aBlockNumber, Block &aBlock) {
    stream.seekg(stream.tellg(), std::ios::beg); //attemp to sync stream buffers...
    stream.seekp(aBlockNumber * kBlockSize);
    return write(aBlock, stream);
  }
  
  // USE: save block (in free block, or appended bloc ---------------------
  StatusResult Storage::addBlock(Block &aBlock) {
    if(int theBlockNum = findFreeBlockId()) {
      return writeBlock(theBlockNum, aBlock);
    }
    
    stream.seekg(stream.tellg(), std::ios::beg); //attemp to sync stream buffers...
    stream.seekp(0, std::ios::end); //append...
    
    toc.header.dirty=true; //TEST ONLY
    
    return write(aBlock, stream);
  }
  
  // USE: mark block as free ---------------------------------------
  StatusResult Storage::releaseBlock(int aBlockNumber) {
    return StatusResult{ECE141::notImplemented};
  }
  
  // USE: add schema to TOC, and store properties in block --------------------------
  StatusResult Storage::addSchema(const std::string &aName, Block &aBlock) {
    return StatusResult{ECE141::storageFull};
  }
  
  // USE: remove schema from TOC and mark schema-block free -------------
  StatusResult Storage::dropSchema(const std::string &aName) {
    return StatusResult{ECE141::noError};
  }
  
}

