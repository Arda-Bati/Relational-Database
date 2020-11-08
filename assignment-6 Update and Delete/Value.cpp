//
//  Value.cpp
//  Database1
//
//  Created by rick gessner on 4/8/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include "Value.hpp"

namespace ECE141 {

  Value& Value::operator=(Value& aValue) {
    //become(aValue.valueType);
    valueType = aValue.valueType;
    switch (valueType) {
      case ECE141::DataType::bool_type: boolValue = aValue.boolValue; break;
      case ECE141::DataType::int_type: intValue = aValue.intValue; break;
      case ECE141::DataType::float_type: fltValue = aValue.fltValue; break;
      case ECE141::DataType::varchar_type: varchar = aValue.varchar; break;
      case ECE141::DataType::timestamp_type: timestamp = aValue.timestamp; break;
      case ECE141::DataType::no_type: boolValue = false; //check this...
    }
    if (valueType != ECE141::DataType::varchar_type) {varchar = "";}
    return *this;
  }

  Value& Value::operator=(int aValue) {
    if (aValue > 0) {
      intValue = aValue;
      valueType = DataType::int_type;
    }
    else {
      timestamp = aValue;
      valueType = DataType::timestamp_type;
    }
    return *this;
  }

  Value& Value::operator=(double aValue) {
    fltValue = aValue;
    valueType = DataType::float_type;
    return *this;
  }

  Value& Value::operator=(float aValue) {
    fltValue = aValue;
    valueType = DataType::float_type;
    return *this;
  }

  Value& Value::operator=(std::string aValue) {
    varchar = aValue;
    valueType = DataType::varchar_type;
    return *this;
  }

  Value& Value::operator=(char* aValue) {
    varchar = std::string(aValue);
    valueType = DataType::varchar_type;
    return *this;
  }

  Value& Value::operator=(const char* aValue) {
    varchar = std::string(aValue);
    valueType = DataType::varchar_type;
    return *this;
  }

  Value& Value::operator=(bool aValue) {
    boolValue = aValue;
    valueType = DataType::bool_type;
    return *this;   
  }

  bool Value::operator==(Value& aValue) {
    if (valueType == aValue.getType()) {
      switch(valueType) {
        case ECE141::DataType::bool_type : return boolValue == aValue.boolValue;
        case ECE141::DataType::int_type : return intValue == aValue.intValue;
        case ECE141::DataType::float_type: return fltValue == aValue.fltValue;
        case ECE141::DataType::varchar_type : return varchar == aValue.varchar;
        case ECE141::DataType::timestamp_type : return timestamp == aValue.timestamp;
        case ECE141::DataType::no_type : return true;
      }
    }
    return false;
  }

  bool Value::operator<=(Value& aValue) {
    if (valueType == aValue.getType()) {
      switch(valueType) {
        case ECE141::DataType::bool_type : return boolValue <= aValue.boolValue;
        case ECE141::DataType::int_type : return intValue <= aValue.intValue;
        case ECE141::DataType::float_type: return fltValue <= aValue.fltValue;
        case ECE141::DataType::varchar_type : return varchar <= aValue.varchar;
        case ECE141::DataType::timestamp_type : return timestamp <= aValue.timestamp;
        case ECE141::DataType::no_type : return false;
      }
    }
    return false;
  }

  bool Value::operator>=(Value& aValue) {
    if (valueType == aValue.getType()) {
      switch(valueType) {
        case ECE141::DataType::bool_type : return boolValue >= aValue.boolValue;
        case ECE141::DataType::int_type : return intValue >= aValue.intValue;
        case ECE141::DataType::float_type: return fltValue >= aValue.fltValue;
        case ECE141::DataType::varchar_type : return varchar >= aValue.varchar;
        case ECE141::DataType::timestamp_type : return timestamp >= aValue.timestamp;
        case ECE141::DataType::no_type : return false;
      }
    }
    return false;
  }

  bool Value::operator>(Value& aValue) {
    if (valueType == aValue.getType()) {
      switch(valueType) {
        case ECE141::DataType::bool_type : return boolValue > aValue.boolValue;
        case ECE141::DataType::int_type : return intValue > aValue.intValue;
        case ECE141::DataType::float_type: return fltValue > aValue.fltValue;
        case ECE141::DataType::varchar_type : return varchar > aValue.varchar;
        case ECE141::DataType::timestamp_type : return timestamp > aValue.timestamp;
        case ECE141::DataType::no_type : return false;
      }
    }
    return false;
  }

  bool Value::operator<(Value& aValue) {
    if (valueType == aValue.getType()) {
      switch(valueType) {
        case ECE141::DataType::bool_type : return boolValue < aValue.boolValue;
        case ECE141::DataType::int_type : return intValue < aValue.intValue;
        case ECE141::DataType::float_type: return fltValue < aValue.fltValue;
        case ECE141::DataType::varchar_type : return varchar < aValue.varchar;
        case ECE141::DataType::timestamp_type : return timestamp < aValue.timestamp;
        case ECE141::DataType::no_type : return false;
      }
    }
    return false;
  }

  Value::operator bool() {
    switch(valueType) {
      case ECE141::DataType::bool_type : return boolValue;
      case ECE141::DataType::int_type : return bool(intValue);
      case ECE141::DataType::float_type: return (fltValue ? true : false); //can you just return fltValue then??
      case ECE141::DataType::varchar_type : return !varchar.empty();
      case ECE141::DataType::timestamp_type : return (timestamp ? true : false);
      default : return false;
    }
  }
  Value::operator int() {
    switch(valueType) {
      case ECE141::DataType::bool_type : return (int)boolValue;
      case ECE141::DataType::int_type : return intValue;
      case ECE141::DataType::float_type: return (int)fltValue;
      case ECE141::DataType::varchar_type : try {int temp = std::stoi(varchar); return temp;}
                                            catch (const std::exception& e) {return 0;}
      case ECE141::DataType::timestamp_type : return (int)timestamp;
      default : return 0;
    }
  }

  Value::operator uint32_t() {
    switch(valueType) {
      case ECE141::DataType::bool_type : return (uint32_t)boolValue;
      case ECE141::DataType::int_type : return (intValue < 0) ? 0 : (uint32_t)intValue;
      case ECE141::DataType::float_type: return (fltValue < 0) ? 0 : (uint32_t)fltValue;
      case ECE141::DataType::varchar_type : try {int temp = std::stoi(varchar); return temp < 0 ? 0 : (uint32_t)temp;}
                                            catch (const std::exception& e) {return 0;}
      case ECE141::DataType::timestamp_type : return timestamp;
      default : return 0;
    }
  }
  Value::operator double() {
    switch(valueType) {
      case ECE141::DataType::bool_type : return (double)boolValue;
      case ECE141::DataType::int_type : return (double)intValue;
      case ECE141::DataType::float_type: return (double)fltValue;
      case ECE141::DataType::varchar_type : try {float temp = std::stof(varchar); return (double)temp;}
                                            catch (const std::exception& e) {return 0;}
      case ECE141::DataType::timestamp_type : return (double)timestamp;
      default : return 0;
    }
  }
  Value::operator float() {
    switch(valueType) {
      case ECE141::DataType::bool_type : return (float)boolValue;
      case ECE141::DataType::int_type : return (float)intValue;
      case ECE141::DataType::float_type: return fltValue;
      case ECE141::DataType::varchar_type : try {return std::stof(varchar);}
                                            catch (const std::exception& e) {return 0;}
      case ECE141::DataType::timestamp_type : return (float)timestamp;
      default : return 0;
    }
  }

  Value::operator std::string() {
    switch(valueType) {
      //case ECE141::DataType::timestamp_type : return (int)timestamp;
      default : return getString();
    }
  }

  Value::operator char*() {
    switch(valueType) {
      //case ECE141::DataType::timestamp_type : return (int)timestamp;
      default : return const_cast<char*>(getString().c_str());
    }
  }

  Value::operator const char*() {
    switch(valueType) {
      //case ECE141::DataType::timestamp_type : return (int)timestamp;
      default : return getString().c_str();
    }
  }

  //so can use this for transforming string values to proper types...
  //this can just be used in encoding/decoding methods... though if written properly
  //could also potentially use for conversion operators...
  StatusResult  Value::become(DataType aType) {
    valueType = aType;
    switch (valueType) {
      case ECE141::DataType::bool_type      : if (varchar == "true") {boolValue = true;}
                                              else {boolValue = false;} break;

      case ECE141::DataType::int_type       : try {intValue = std::stoi(varchar);} //do we always want a negative int value??
                                              catch (const std::exception& e) {intValue = 0;} break;

      case ECE141::DataType::float_type     : try {fltValue = std::stof(varchar);}
                                              catch (const std::exception& e) {fltValue = 0;} break;

      case ECE141::DataType::varchar_type   : break;

      case ECE141::DataType::timestamp_type : try {
                                                int temp = std::stoi(varchar);
                                                if (temp >= 0) {timestamp = (uint32_t)temp;} 
                                                else {timestamp = 0;}break;
                                              }
                                              catch (const std::exception& e) {timestamp = 0;} break;

      case ECE141::DataType::no_type        : boolValue = false; //may change this later...
    }
    if (valueType != ECE141::DataType::varchar_type) {varchar = "";}
    return StatusResult(Errors::noError);
  }

  DataType Value::getType() {
    return valueType;
  }

  std::string Value::getString() {
    switch (valueType) {
      case ECE141::DataType::bool_type : return (boolValue) ? "true" : "false";
      case ECE141::DataType::int_type : return std::to_string(intValue);
      case ECE141::DataType::float_type: return std::to_string(fltValue);
      case ECE141::DataType::varchar_type : return varchar;
      case ECE141::DataType::timestamp_type : return std::to_string(timestamp);
      default : return ""; //check this...
    }
  }

  size_t Value::getSize() {
    switch (valueType) {
      case ECE141::DataType::bool_type : return sizeof(boolValue);
      case ECE141::DataType::int_type : return sizeof(intValue);
      case ECE141::DataType::float_type: return sizeof(fltValue);
      case ECE141::DataType::varchar_type : return sizeof(varchar);
      case ECE141::DataType::timestamp_type : return sizeof(timestamp);
      case ECE141::DataType::no_type : return 0; //check this...
    }
  return 0;
  }
  
  void Value::debugDump(std::ostream &anOutput) {
    std::string theType;
    switch (valueType) {
      case ECE141::DataType::bool_type : theType = "bool, value: ";
                                         if (boolValue) {
                                            theType += "true";
                                         }
                                         else {theType += "false";} break;
      case ECE141::DataType::int_type : theType = "int, value: " + std::to_string(intValue); break;
      case ECE141::DataType::float_type : theType = "float, value: " + std::to_string(fltValue); break;
      case ECE141::DataType::timestamp_type : theType = "uint32_t, value: " + std::to_string(timestamp); break;
      case ECE141::DataType::varchar_type : theType = "varchar, value: " + varchar; break;
      case ECE141::DataType::no_type : theType = "no type";
    }
    anOutput << theType << std::endl;
  } 
}