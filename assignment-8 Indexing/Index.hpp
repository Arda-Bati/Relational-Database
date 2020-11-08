#ifndef Index_h
#define Index_h

#include <map>
#include "Errors.hpp"
#include "Value.hpp"
#include "Storage.hpp"

namespace ECE141 {

  class Storage;
  class SelectRowBlockVisitor;

  class Index {
    public:
    
      Index(const std::string &aField, uint32_t aTableId, DataType aKeyType)
        : field(aField), tableId(aTableId), keyType(aKeyType), dirty(false) {}

      Index&   addKeyValue(const Value &aKey, uint32_t aValue);   
      Index&   removeKeyValue(const Value &aKey);
      Index&   addBlockNum(uint32_t theBlockNum); //potentially change later
      Index&   decodeFromBlock(Block& aBlock); //could be used to add indices to map when making the Index
      //Index&   markDirty(bool aState);
      bool     isDirty(); //return whether or not index has been updated...
      bool     contains(const Value &aKey);
      uint32_t getValue(const Value &aKey);

      StatusResult writeToStorage(Storage& aStorage, uint32_t startBlockNum); //so index should know all of its blockNums...
      StatusResult dropIndex(Storage &aStorage);
      StatusResult each(Storage& aStorage, StorageCallback aCallback);
      //StatusResult each(Storage& aStorage, SelectRowBlockVisitor& theSRBV);
      
      using IndexType = std::map<Value, uint32_t>; //c++ typedef...

    protected:    
      IndexType   index;
      std::string field;    //name of the field being indexed
      DataType    keyType;  //what Value datatype are we using for this field?
      uint32_t    tableId;
      bool        dirty;
      std::vector<uint32_t> blockNums;
    };

}

#endif /*Index_h*/