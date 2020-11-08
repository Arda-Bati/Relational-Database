//
//  Attribute.hpp
//  Datatabase4
//
//  Created by rick gessner on 4/12/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef Attribute_hpp
#define Attribute_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include "keywords.hpp"
#include "Value.hpp"
#include "MemoryStream.hpp"

namespace ECE141 {
  
  struct PersistAttribute; //frwd...
  
  struct Property {
    Property(std::string aName, int aTableId=0) : name(aName), tableId(aTableId) {}
    std::string     name;
    bool            desc;
    int             tableId;
  };
  
  using PropertyList = std::vector<Property>;
  
  class Attribute {
  protected:
    std::string   name;
    DataType      type;
    uint32_t      size : 24;
    uint32_t      autoIncrement : 1;
    uint32_t      primary : 1;
    uint32_t      nullable: 1;
    
  public:
    
    static Keywords charToKeyword(char aChar);
    
    Attribute(DataType aType=DataType::no_type);
    
    Attribute(std::string aName, DataType aType,
              uint8_t aSize=0, bool autoIncr=false, bool primary=false);
    
    Attribute(PersistAttribute &aDef);
    
    Attribute(const Attribute &aCopy);
    
    operator PersistAttribute();
    
    ~Attribute();
    
    Attribute&    setName(std::string &aName);
    Attribute&    setType(DataType aType);
    Attribute&    setSize(int aSize);
    Attribute&    setAutoIncrement(bool anAuto);
    Attribute&    setPrimaryKey(bool anAuto);
    Attribute&    setNullable(bool aNullable);
    
    const std::string&  getName() const {return name;}
    DataType      getType() const {return type;}
    size_t        getSize() const;  //used for storage
   // size_t        getWidth() const;  //used for table display...
    
    //general purpose...
    
    bool          isCompatible(DataType aType) const;
    bool          isPrimaryKey() const {return primary;}
    bool          isNullable() const {return nullable;}
    bool          isAutoIncrement() const {return autoIncrement;}
    
    std::string   describeTabular();
    
    bool isValid(); //self validation...

    friend BufferReader& operator >> (BufferReader& istm, Attribute &aValue);
    friend BufferWriter& operator << (BufferWriter& ostm, const Attribute &aValue);

  };
  

}


#endif /* Attribute_hpp */
