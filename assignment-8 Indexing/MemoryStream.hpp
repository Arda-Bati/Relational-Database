//
//  MemoryStream.h
//  Database3
//
//  Created by rick gessner on 4/15/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef MemoryStream_hpp
#define MemoryStream_hpp

#include <stdio.h>
#include <stdint.h>
#include <cstdio>
#include <string>
#include <cstring>
#include <stdexcept>

namespace ECE141 {
  
  class Value; //forward decl...
  
  enum class SeekOrientation {front, current, back};
  
  //Use: this class can read binary data from a fixed-size memory buffer...
  
  class BufferReader {
  protected:
    char*   buffer;
    size_t  size; //actual size
    size_t  max;  //max size...
    int     pos;  //read pos
    
  public:
    BufferReader();
    BufferReader(char* aBuffer, size_t aSize, size_t aMax);
    
    void close();
    bool eof() const;
    
    char* getBuffer();
    
    size_t tellg(); //current read pos...
    bool seekg (size_t aPos);
    bool seekg (size_t aPos, SeekOrientation);
    
    template<typename T>
    void read(T& t) {
      if(eof()) throw std::runtime_error("EOF");
      
      if((pos + sizeof(T)) > size)
        throw std::runtime_error("EOF");
      
      std::memcpy(reinterpret_cast<void*>(&t), &buffer[pos], sizeof(T));
      pos += sizeof(T);
    }
    
    void read(char* aPtr, size_t aSize);
    void read(std::string& str, const unsigned int aSize);
  };
  
  
  BufferReader& operator >> (BufferReader& istm, char &aValue);
  BufferReader& operator >> (BufferReader& istm, bool &aValue);
  BufferReader& operator >> (BufferReader& istm, uint32_t  &aValue);
  BufferReader& operator >> (BufferReader& istm, uint16_t  &aValue);
  BufferReader& operator >> (BufferReader& istm, float &aValue);
  BufferReader& operator >> (BufferReader& istm, char *aBuffer);
  BufferReader& operator >> (BufferReader& istm, uint8_t &aValue);
  BufferReader& operator >> (BufferReader& istm, std::string& aValue);
  
  //------------------------------------------------------------------------
  //Use: this class can write binary data to a fixed-size memory buffer...
  class BufferWriter {
  protected:
    char*   buffer;
    size_t  max;
    size_t  pos; //curr write offset...
    
  public:
    BufferWriter(char* aBuffer, size_t aMax);
    
    BufferWriter& close();
    
    char* getBuffer() {return buffer;}
    size_t tellp() {return pos;}
    
    template<typename T>
    BufferWriter&  write(const T& aValue) {
      size_t theSize=sizeof(T);
      std::memcpy(reinterpret_cast<void*>(&buffer[pos]), reinterpret_cast<const void*>(&aValue), theSize);
      pos+=theSize;
      return *this;
    }
    
    BufferWriter&  writeCString(const char *aValue, size_t aLength);
    
    BufferWriter&  writeRaw(void *aBuffer, size_t aSize) {
      std::memcpy(reinterpret_cast<void*>(&buffer[pos]), aBuffer, aSize);
      pos+=aSize;
      return *this;
    }
    
  };
  
  BufferWriter& operator << (BufferWriter& ostm, const bool  aValue);
  BufferWriter& operator << (BufferWriter& ostm, const char  aValue);
  BufferWriter& operator << (BufferWriter& ostm, const uint8_t aValue);
  BufferWriter& operator << (BufferWriter& ostm, const uint16_t aValue);
  BufferWriter& operator << (BufferWriter& ostm, const uint32_t aValue);
  BufferWriter& operator << (BufferWriter& ostm, const float aValue);
  BufferWriter& operator << (BufferWriter& ostm, const char* aValue);
  BufferWriter& operator << (BufferWriter& ostm, const std::string &aValue);

}

#endif /* MemoryStream_h */
