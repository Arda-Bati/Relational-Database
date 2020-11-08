//
//  Storage.hpp
//  Database3
//
//  Created by rick gessner on 3/24/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef Storage_hpp
#define Storage_hpp

#include <stdint.h>
#include <cstring>
#include <string>
#include <sstream>
#include <fstream>
#include "Errors.hpp"
#include <functional>
#include "Row.hpp"

namespace ECE141 {
  
  class Row;
  
  const char kDataBlockType   = 'D';
  const char kEntityBlockType = 'E';
  const char kFreeBlockType   = 'F';
  const char kIndexBlockType  = 'I';

  struct BlockHeader {
    
    BlockHeader() : type('D') {reserved=0; extra=0; id=0;}
    
    BlockHeader(char aType) : type(aType)  {reserved=0; extra=0; id=0;}
    
    BlockHeader(const BlockHeader &aCopy) {
      type=aCopy.type;
      reserved=aCopy.reserved;
      extra=aCopy.extra;
      id=aCopy.id;
    }
    
    char      type;     //block type {[D]ata, [F]ree... }
    char      reserved; //maybe later?
    uint32_t  id;       //use this anyway you like
    uint32_t  extra;    //use this anyway you like
    
  };
  
  const size_t kBlockSize = 1024; //completely arbitary, use whatever...
  const size_t kPayloadSize = kBlockSize-sizeof(BlockHeader); //990 by default...
  
  //======= These are for persisting entities (tables) to storage...
  
  const size_t kStoredIdentifierLen = 15;
  
  struct PersistEntity { //a single entity
    PersistEntity() {name[0]=0; blocknum=0;};
    PersistEntity(const PersistEntity &aCopy) {
      strcpy(name, aCopy.name);
      blocknum=aCopy.blocknum;
    }
    
    char      name[kStoredIdentifierLen+1];
    uint32_t  blocknum;
  };
  
  struct PersistEntitiesHeader {
    uint8_t           count; //how many attributes...
    uint8_t           reserved; //not used yet
  };
  
  const size_t kEntitiesPerBlock =
    (kPayloadSize-sizeof(PersistEntitiesHeader)) / sizeof(PersistEntity); //49-ish

  struct PersistEntities {
    PersistEntitiesHeader header;
    PersistEntity         items[kEntitiesPerBlock];
  };

  //======= This is for tracking free-blocks (NOT CURRENTLY USED) -- if you plan to do so...
  
  const size_t kFreeItemsPerBlock = kPayloadSize / sizeof(bool);
  struct FreeMap {
    bool items[kFreeItemsPerBlock];
  };
  
  //======== Define single block in the storage file... ==============
  
  struct Block {
    
    Block(char aType='D') : header(aType) {
      memset(data,0,sizeof(data));
    }
    
    Block(const Block &aCopy) : header(aCopy.header) {}
    
    Block(BlockHeader &aHeader) : header(aHeader), data() {}
    
    Block(const KeyValues &aKVList) {
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
    
    //we use attributes[0] as table name...
    BlockHeader   header;
    union {
      char              data[kPayloadSize];
      PersistEntities   entities;
     //FreeMap           available;  //totally optional...
    };
  };
  
  //============================================================
  
  struct CreateNewStorage {};
  struct OpenExistingStorage {};

  using StorageCallback = std::function<bool(const Block &aBlock, uint32_t aBlockNum)>;
  
  // USE: Our main class for managing storage...
  class Storage {
  public:
    
    Storage(const std::string aName, CreateNewStorage);
    Storage(const std::string aName, OpenExistingStorage);
    //Storage(const Storage &aStorage);
    
    ~Storage();
    
    static const char* getDefaultStoragePath();
    static std::string getDatabasePath(const std::string &aDBName); //build a path to db with given name...
    
    StatusResult    loadTOC();  //get it from where you placed it in the storage file...
    StatusResult    makeEmpty(); //This works by default..
    StatusResult    setupEmptyStorage();
    
    StatusResult    readBlock(int aBlockNumber, Block &aBlock, std::size_t aBlockSize=kBlockSize);
    StatusResult    writeBlock(int aBlockNumber, Block &aBlock);
    
    StatusResult    addBlock(Block &aBlock);
    StatusResult    releaseBlock(int aBlockNumber); //mark the block as free...
    
    StatusResult    addEntity(const std::string &aName, Block &aBlock);
    StatusResult    dropEntity(const std::string &aName);
    
    PersistEntity*  findEntityInTOC(const std::string &aName); //return NULL if not found...

    Storage&        eachBlock(StorageCallback aCallback);
    
  protected:
    Block           toc;
    std::fstream    stream;
    std::string     name;    
    
    bool            isReady() const;
    StatusResult    findFreeBlockNum();
    uint32_t        getTotalBlockCount();
    
    friend class Database;
  };
  
}

#endif /* Storage_hpp */
