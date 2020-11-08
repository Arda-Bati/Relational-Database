//
//  Storage.hpp
//  Database1
//
//  Created by rick gessner on 3/24/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef Storage_hpp
#define Storage_hpp

#include <cstring>
#include <stdint.h>
#include <string>
#include <fstream>
#include "Errors.hpp"
#include "Row.hpp"

namespace ECE141 {
  
  static const int kBlockSize = 1024; //completely arbitary...
  
  class Row;
  
  struct BlockHeader {
    
    BlockHeader() : type('D'), dirty(false), id(0) {}
    
    BlockHeader(char aType, uint32_t anId) : type(aType), dirty(false), id(anId) {}
    
    BlockHeader(const BlockHeader &aCopy) {
      type=aCopy.type;
      dirty=aCopy.dirty;
      id=aCopy.id;
    }
    
    char      type;   //block type {[D]ata, [F]ree... } -- as char for 
    bool      dirty;  //currently unused...
    uint32_t  id;     //used as expected
    uint32_t  count;
  };
  
  //----- represents 1 block (binary segment) in the storage file... -----------------
  
  struct Block {
    
    Block(char aType='D', uint32_t id=0) : header(aType, id) {
      memset(data,0,sizeof(data));
    }
    
    Block(BlockHeader &aHeader) : header(aHeader), data() {}
    Block(const KeyValues &aKVList);  //encode data into the block...
    
    BlockHeader   header;


    //not part of block header... OKAYY??
    union {
      char            data[kBlockSize-sizeof(BlockHeader)];
      //SchemaDef     later...
      //AttributeDef  later... (attributes[0] will hold table name...
    };
  };
  
  //The following class models file storage of our encoding DB data-blocks...
  class Storage {
  public:
    
    Storage(const std::string aName);
    ~Storage();
    
    static const char* getDefaultStoragePath();
    
    StatusResult    loadTOC();
    StatusResult    makeEmpty();
    
    StatusResult    readBlock(int aBlockNumber, Block &aBlock, std::size_t aBlockSize=kBlockSize);
    StatusResult    writeBlock(int aBlockNumber, Block &aBlock);
    
    StatusResult    addBlock(Block &aBlock);
    StatusResult    releaseBlock(int aBlockNumber); //mark the block as free...
    
    StatusResult    addSchema(const std::string &aName, Block &aBlock);
    StatusResult    dropSchema(const std::string &aName);
    
  protected:
    Block           toc;
    std::fstream    stream;
    std::string     name;
    
    bool            isReady();
    uint32_t        findFreeBlockId();
    uint32_t        getTotalBlockCount();
    
  };
  
}

#endif /* Storage_hpp */
