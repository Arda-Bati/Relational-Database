//
//  Helpers.hpp
//  Database3
//
//  Created by rick gessner on 4/14/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef Helpers_h
#define Helpers_h

#include "keywords.hpp"
#include "Value.hpp"

namespace ECE141 {
  
  static std::map<std::string, Operators> gOperators = {
    std::make_pair("=", Operators::equal_op),
    std::make_pair("!=", Operators::notequal_op),
    std::make_pair("not", Operators::notequal_op),
    std::make_pair("<", Operators::lt_op),
    std::make_pair("<=", Operators::lte_op),
    std::make_pair(">", Operators::gt_op),
    std::make_pair(">=", Operators::gte_op),
    std::make_pair("and", Operators::and_op),
    std::make_pair("or", Operators::or_op),
    std::make_pair("between", Operators::between_op)
  };
  
  
  class Helpers {
  public:
    
    static Keywords getKeywordId(const std::string aKeyword) {
      auto theIter = gDictionary.find(aKeyword);
      if (theIter != gDictionary.end()) {
        return theIter->second;
      }
      return Keywords::unknown_kw;
    }
    
    //convert from char to keyword...
    static Keywords charToKeyword(char aChar) {
      switch(toupper(aChar)) {
        case 'I': return Keywords::integer_kw;
        case 'T': return Keywords::datetime_kw;
        case 'B': return Keywords::boolean_kw;
        case 'F': return Keywords::float_kw;
        case 'V': return Keywords::varchar_kw;
        default:  return Keywords::unknown_kw;
      }
    }
    
    //USE: ----------
    static DataType getDataTypeForKeyword(Keywords aKeyword) {
      switch(aKeyword) {
        case Keywords::boolean_kw:  return DataType::bool_type;
        case Keywords::datetime_kw: return DataType::timestamp_type;
        case Keywords::float_kw:    return DataType::float_type;
        case Keywords::integer_kw:  return DataType::int_type;
        case Keywords::varchar_kw:  return DataType::varchar_type;
        default: return DataType::no_type;
      }
    }
    
    static const char* keywordToString(Keywords aType) {
      switch(aType) {
        case Keywords::integer_kw:  return "integer";
        case Keywords::float_kw:    return "float";
        case Keywords::boolean_kw:  return "bool";
        case Keywords::datetime_kw: return "datetime";
        case Keywords::varchar_kw:  return "varchar";
        default:                    return "unknown";
      }
    }
    
    static const char* dataTypeToString(DataType aType) {
      switch(aType) {
        case DataType::int_type:        return "integer";
        case DataType::float_type:      return "float";
        case DataType::bool_type:       return "bool";
        case DataType::timestamp_type:  return "datetime";
        case DataType::varchar_type:    return "varchar";
        case DataType::no_type:
        default:                return "unknown";
      }
    }
    
    // USE: ---validate that given keyword is a datatype...
    static bool isDatatype(Keywords aKeyword) {
      switch(aKeyword) {
        case Keywords::char_kw:
        case Keywords::datetime_kw:
        case Keywords::float_kw:
        case Keywords::integer_kw:
        case Keywords::varchar_kw:
          return true;
        default: return false;
      }
    }
    
    static Operators toOperator(std::string aString) {
      auto theIter = gOperators.find(aString);
      if (theIter != gOperators.end()) {
        return theIter->second;
      }
      return Operators::unknown_op;
    }
    
    static int getFunctionId(const std::string anIdentifier) {
      auto theIter = gFunctions.find(anIdentifier);
      if (theIter != gFunctions.end()) {
        return theIter->second;
      }
      return 0;
    }
    
    static bool isNumericKeyword(Keywords aKeyword) {
      static Keywords theTypes[]={Keywords::decimal_kw, Keywords::double_kw, Keywords::float_kw, Keywords::integer_kw};
      for(auto k : theTypes) {
        if(aKeyword==k) return true;
      }
      return false;
    }
    
    
  };
}
#endif /* Helpers_h */
