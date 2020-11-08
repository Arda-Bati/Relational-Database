//
//  Value.hpp
//  Database1
//
//  Created by rick gessner on 3/20/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef Value_hpp
#define Value_hpp

//#include <cstring>
#include <string>
#include <map>
#include <iostream>
#include "Errors.hpp"

namespace ECE141 {
  
  //NOTE: integer is a SIGNED value
  //NOTE: timestamp is an UNSIGNED value!!!!!
  
  enum class DataType {
    no_type='N', int_type='I', float_type='F', bool_type='B', timestamp_type='T', varchar_type='V',
    //our enums are associated w/chars to be readable in encoded storage blocks...
  };
  
  //USE: define a dynamic type that allows us to hold field data in rows...
  class Value {
    public:
      Value() : boolValue(false), valueType(DataType::no_type) {}
      Value(bool aValue) : boolValue(aValue), valueType(DataType::bool_type) {}
      Value(float aValue) : fltValue(aValue), valueType(DataType::float_type) {}
      Value(double aValue) : fltValue(aValue), valueType(DataType::float_type) {}
      Value(std::string aValue) : varchar(aValue), valueType(DataType::varchar_type) {}
      Value(uint32_t aValue) : timestamp(aValue), valueType(DataType::timestamp_type) {}
      Value(const char* aValue) : varchar(std::string(aValue)), valueType(DataType::varchar_type) {}
      Value(char* aValue) : varchar(std::string(aValue)), valueType(DataType::varchar_type) {} //not sure if we need both...
      Value(int aValue) : valueType(DataType::int_type) {
        if (aValue < 0) {
          intValue = aValue;
          valueType = DataType::int_type;
        }
        else {
          timestamp = aValue;
          valueType = DataType::timestamp_type;
        }
      }

      Value& operator=(Value& aValue);
      Value& operator=(int aValue);
      Value& operator=(float aValue);
      Value& operator=(double aValue);
      Value& operator=(std::string aValue);
      Value& operator=(char* aValue);
      Value& operator=(const char* aValue);
      Value& operator=(bool aValue);
      bool operator==(Value& aValue);

      operator bool();
      explicit operator std::string();
      operator int();
      operator float();
      operator double(); //not sure if we need this
      operator uint32_t();
      explicit operator char*();
      explicit operator const char*();

      std::string getString();
      
      //This class needs multiple ctors() for each type (bool, timest, int, float, varchar)...
      //This class needs operator=, for each basic type...
      //This class needs conversion operator for each basic type
      
      size_t        getSize();
      DataType      getType();
      
      StatusResult  become(DataType aType); //cause the value to assume given type
      
      void          debugDump(std::ostream &anOutput);  //describe (emit) this object onto given stream...

    protected:
    union
    {
      bool boolValue;
      int intValue;
      float fltValue;
      uint32_t timestamp;
    };
    std::string varchar; //should not be keeping this in union... so have to rewrite code for this

    DataType valueType;
  };
  
  //for holding a collection of key/values...
  using KeyValues = std::map<std::string, Value>;
  
}

#endif /* Value_h */
