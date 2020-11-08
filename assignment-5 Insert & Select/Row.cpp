//
//  Row.cpp
//  ECEDatabase
//
//  Created by rick gessner on 4/29/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include <sstream>
#include "Row.hpp"
#include "Storage.hpp"
#include <algorithm>
#include "MemoryStream.hpp"

namespace ECE141 {
  
  Row::Row(int32_t aBlockNumber) : blockNumber(aBlockNumber), columns() {
  }
  
  Row::Row(const Row &aCopy) : blockNumber(aCopy.blockNumber), columns(aCopy.columns) {
  }
  
  Row::Row(const Block &aBlock) {
    BufferReader buffR(const_cast<char*>(aBlock.data), kPayloadSize, kPayloadSize);
    uint8_t theSize;
    buffR >> theSize;
    std::string key;
    std::string aValue;
    char valType;

    for (uint32_t i = 0; i < theSize; i++) {
      buffR >> key >> valType >> aValue;
      ECE141::Value theValue(aValue);
      theValue.become((ECE141::DataType)valType);
      columns.insert({key, theValue});
    }
    
    /*std::string theData = std::string(aBlock.data);
    std::stringstream theStream(theData);
    std::string temp;
    int count = 0;
    int theType = 3; //so 0 = key, 1 = type, 2 = value
    std::string key;
    std::string valType;
    
    while (getline(theStream, temp, '"')) {
      if (!(count % theType)) {
        key = temp;
        //std::cout << "Key: " << key << std::endl;
      }

      else if ((count % theType) == 1) {
        valType = temp;
        //std::cout << "valType: " << valType << std::endl;
      }

      else {
        //std::cout << "value: " << temp << std::endl;
        ECE141::Value theValue(temp);
        theValue.become((ECE141::DataType)valType[0]);
        //theValue.debugDump(std::cout);
        //columns[key] = theValue;
        columns.insert({key, theValue});
      }
      count++;
    }*/
  }

  Row::Row(KeyValues &aKeyValueList) : blockNumber(0), columns(aKeyValueList) {    
  }
  
  Row::~Row() {
    //STUDENT: implement this too
    blockNumber = NULL;
    //columns.erase(); //what else?
  }

  // USE: add a new field+value into the row...
  Row& Row::addColumn(const std::string &aString, const Value &aValue) {
    columns[aString]=const_cast<Value&>(aValue);
    return *this;
  }

  Row& Row::operator=(const Row &aRow){
    blockNumber = aRow.blockNumber;
    columns = aRow.columns;
    return *this;
  } 

  bool Row::operator==(const Row &aCopy) {
    if (columns.size() != (const_cast<Row&>(aCopy)).getColumns().size()) {return false;}
    
    std::map<std::string,ECE141::Value>::iterator it=columns.begin();
    auto it2=columns.begin();

    while (it != columns.end() && it2 != (const_cast<Row&>(aCopy)).getColumns().end()) {
      if (it->first != it2 -> first) {
        return false;
      }
      else if (!(it->second == it2 -> second)) {
        return false;
      }
      it++; it2++;
    }
    return true;
  }

  //---------------------------------------------------
  
  RowCollection::RowCollection() : rows() {}
  
  RowCollection::RowCollection(const RowCollection &aCopy) : rows(aCopy.rows) {}
  
  RowCollection::~RowCollection() {
    for(auto theRow : rows) {
      delete theRow;
    }
  }
  
  RowCollection& RowCollection::add(Row* aRow) {
    rows.push_back(aRow);
    return *this;
  }
  
  
}
