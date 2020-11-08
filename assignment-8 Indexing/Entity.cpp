//
//  Schema.cpp
//  ECEDatabase
//
//  Created by rick gessner on 4/19/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include "Entity.hpp"
#include "Row.hpp"
#include "Tokenizer.hpp"
#include "Storage.hpp"
#include "Index.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <streambuf>
#include <sys/stat.h>
#include "Helpers.hpp"
#include "MemoryStream.hpp"
#include "View.hpp"
#include "Filters.hpp"

const int gMultiplier = 37;

namespace ECE141 {
  
  //utility...
  
  uint32_t Entity::hashString(const char *str) {
    uint32_t h{0};
    unsigned char *p;
    for (p = (unsigned char*)str; *p != '\0'; p++)
      h = gMultiplier * h + *p;
    return h;
  }

  uint32_t Entity::getHash() {
    return hash;
  }
 
  //----------------------------------------------------
  
  Entity::Entity(std::string aName)
    : name(aName), attributes(), dirty(false), count(0), blockNum(0), autoincr(0) {
    hash = hashString(aName.c_str());
  }
  
  Entity::Entity(const Entity &aCopy) :
    name(aCopy.name), attributes(aCopy.attributes), dirty(aCopy.dirty), autoincr(aCopy.autoincr), count(aCopy.count), 
    blockNum(aCopy.blockNum), hash(aCopy.hash), indices(aCopy.indices)  { //check this
  }
  
  Entity::Entity(Block &aBlock, uint32_t aBlockNum) {
    //STUDENT: construct an entity (decode) from the given block...
      blockNum = aBlockNum;
      //Not sure about this yet
      Attribute theAttribute;
      uint32_t theIndexBN;
      BufferReader buffR(aBlock.data, kPayloadSize, kPayloadSize);
      buffR >> autoincr;
      buffR >> hash;

      //so still need to load all of the indices... do we want to store the block nums for them
      //right after the entity... if it's stored as a 0, that means we don't have an Index for this
      //attribute

      buffR >> theAttribute;
      while (0 < theAttribute.getName().size()) {
        attributes.push_back(theAttribute);
        if (theAttribute.isAutoIncrement()) {
          buffR >> theIndexBN;
          indexBlockNums.insert({theAttribute.getName(), theIndexBN});
        }
        buffR >> theAttribute;
      }

      dumpDebug();

    // STUDENT: Encode your entity data into the given block...
  }
    
  Entity::Entity() {}

  Entity::~Entity() {
    for (auto index : indices) {
      delete index.second;
    }
  }

  // USE: ask the entity for name of primary key (may not have one...)
  std::string Entity::getPrimaryKey() {
    for(auto &theAttr : attributes) {
      if(theAttr.isAutoIncrement())
        return theAttr.getName();
    } 
    return "";
  }
  
  // USE: ask entity for next auto-incr value  (for a new record)....
  uint32_t Entity::getNextAutoIncrementValue() {
    dirty=true; //so we resave when DB is released...
    return ++autoincr;
  }

  void Entity::each(attribute_callback aCallback) const {
    for(auto theAttr : attributes) {
      aCallback(theAttr);
    }
  }


  Entity& Entity::markDirty(bool aState) {
    dirty=aState;
    return *this;
  }

  Entity& Entity::addAttribute(std::string aName, DataType aType, uint8_t aSize, bool autoIncr) {
    if (autoIncr) {
      Index *theIndex = new Index(aName, hash, aType);
      indices.insert({aName, theIndex});
    }
    attributes.push_back(Attribute(aName, aType, aSize, autoIncr)); //CHANGED
    return *this;
  }
  
  Entity& Entity::addAttribute(const Attribute &anAttribute) {
    if (anAttribute.isAutoIncrement()) {
      Index *theIndex = new Index(anAttribute.getName(), hash, anAttribute.getType());
      indices.insert({anAttribute.getName(), theIndex});
    }
    attributes.push_back(anAttribute);
    //make index object which uses primary key as it's field...
    //so for now can assume that field will just be auto_inc (int)
    return *this;
  }
  
  Attribute& Entity::getAttribute(const std::string &aName)  {
    for(auto theIter = attributes.begin(); theIter != attributes.end(); theIter++) {
      if(theIter->getName()==aName) {
        return *theIter;
      }
    }
    throw std::runtime_error("Attribute not found");
  }
  
  Value Entity::getDefaultValue(const Attribute &anAttribute) {
    Value theResult;
    theResult.become(anAttribute.getType());
    return theResult;
  }

  //Encode (pack) this entity into a storage block...
  Entity::operator Block() {
    Block theBlock{kEntityBlockType};

    //STUDENT: Decode your entity into a  block, and return it...
      //Not sure about this yet
      BufferWriter buffW(theBlock.data, kPayloadSize);
      buffW << autoincr;
      buffW << hash;
      for(auto theAttribute : attributes) {
        buffW << theAttribute;
        if (theAttribute.isAutoIncrement()) {
          buffW << indexBlockNums[theAttribute.getName()];
        }
      }
      

    return theBlock;
  }

  //** USE:
  StatusResult Entity::validate(const KeyValues &aList) {
    for (auto theKV : aList) {
      try {
        Attribute theAttribute = getAttribute(theKV.first);
        if (theAttribute.getType() != theKV.second.getType()) {
          return StatusResult{invalidAttribute};
        }
      }
      catch(...) {
        return StatusResult{unknownAttribute};
      }
    }
    return StatusResult{noError};
  }
    
  // USE: write given row into given block (using entity as guide...)
  StatusResult Entity::encodeData(KeyValues &aDataList, Block &aBlock) {
    StatusResult theResult{noError};
    
    return StatusResult{noError};
  }

  bool inGivenFields(const std::string &aFieldName, const StringList &aFields) {
    auto theEnd = aFields.end();
    return std::find(aFields.begin(), theEnd, aFieldName) != theEnd;
  }

  // USE: read data from block into given keyvalue list (for row?)...
  StatusResult Entity::decodeData(const Block &aBlock, KeyValues &aValuesList,
                                  const StringList *aFields) const {
    StatusResult theResult{noError};
    
    // STUDENT: Decode your entity from the given block...
    
    return theResult;
  }

  StatusResult Entity::loadIndices(Storage &aStorage) {
    //so when here... all of index blockNums should be in indexBlockNums map
    //now we just need to load them from storage
    uint32_t theBlockNum;
    Block theBlock{kIndexBlockType};
    for (auto thePair : indexBlockNums) {
      Index *theIndex = new Index(thePair.first, hash, getAttribute(thePair.first).getType()); //this is gross
      theBlockNum = thePair.second;
      while (theBlockNum) {//so each index block should first be storing the next block num
        aStorage.readBlock(theBlockNum, theBlock);
        theIndex->addBlockNum(theBlockNum); //not sure if we need this
        theIndex->decodeFromBlock(theBlock);
        theBlockNum = theBlock.header.next;
      }
      indices.insert({thePair.first, theIndex});
      //so after this... should have all index values
    }

    return StatusResult{noError};
  }

  StatusResult Entity::writeIndices(Storage &aStorage) {
    //so in this case, we're going to be loading the indices to
    //storage for the first time... so we need to find free block
    // store it to map.. and then you can write everything in next while loop
    StatusResult theResult;

    for (auto thePair : indices) {
      if (thePair.second->isDirty()) {
        if (!indexBlockNums[thePair.first]) {
          theResult = aStorage.findFreeBlockNum();
          indexBlockNums[thePair.first] = theResult.value;
          thePair.second->addBlockNum(theResult.value);
        }
        thePair.second->writeToStorage(aStorage, indexBlockNums[thePair.first]);
      }
    }
    return StatusResult{noError};
  }

  StatusResult Entity::dropIndices(Storage &aStorage) {
    for (auto thePair : indices) {
      thePair.second->dropIndex(aStorage);
    }
    return StatusResult{noError};
  }

  Index* Entity::getIndex(const std::string &aFieldName) {
    if (indices.count(aFieldName)) {
      return indices[aFieldName];
    }
    return nullptr;
  }

  void Entity::dumpDebug() {
    //
  }
  

}
