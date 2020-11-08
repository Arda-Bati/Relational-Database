//
//  MemoryStream.c
//  Database3
//
//  Created by rick gessner on 4/15/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include "MemoryStream.hpp"
#include "Value.hpp"

namespace ECE141 {
  
  BufferReader::BufferReader() { close();}
  
  BufferReader::BufferReader(char* aBuffer, size_t aSize, size_t aMax) : buffer(aBuffer) {
    size=aSize; //#bytes written into buffer...
    max=aMax;   //actual buffer len
    pos=0;
    memcpy(buffer, aBuffer, aSize);
  }
  
  void BufferReader::close() {
    memset(buffer,0,max);
    pos=size=max=0;
  }
  
  bool BufferReader::eof() const {
    return pos >= size;
  }
  
  char* BufferReader::getBuffer() {return (char*)&buffer[0];}
  
  size_t BufferReader::tellg() {return pos;}
  
  bool BufferReader::seekg (size_t aPos) {
    if(pos<size)
      pos = (int)aPos;
    else return false;
    return true;
  }
  
  bool BufferReader::seekg (size_t offset, SeekOrientation aWay) {
    if(SeekOrientation::front==aWay && offset < size)
      pos = (int)offset;
    else if(SeekOrientation::current==aWay && (pos + offset) < size)
      pos += offset;
    else if(SeekOrientation::back==aWay && (size + offset) < size)
      pos = (int)(size + offset);
    else return false;
    return true;
  }
  
  void BufferReader::read(char* aPtr, size_t aSize) {
    if(eof())
      throw std::runtime_error("EOF");
    
    if((pos + aSize) > size)
      throw std::runtime_error("EOF");
    
    std::memcpy(reinterpret_cast<void*>(aPtr), &buffer[pos], aSize);
    aPtr[aSize]=0; //just in case...
    pos += aSize;
  }
  
  void BufferReader::read(std::string& str, const unsigned int aSize) {
    if (eof())
      throw std::runtime_error("EOF");
    
    if ((pos + aSize) > size)
      throw std::runtime_error("EOF");
    
    str.assign(&buffer[pos], aSize);
    
    pos += str.size();
  }
  
  BufferReader& operator >> (BufferReader& istm, bool &aBool) {
    istm.read<bool>(aBool);
    return istm;
  }
  
  BufferReader& operator >> (BufferReader& istm, char &aChar) {
    istm.read<char>(aChar);
    return istm;
  }
  
  BufferReader& operator >> (BufferReader& istm, uint8_t &aByte) {
    istm.read<uint8_t>(aByte);
    return istm;
  }
  
  BufferReader& operator >> (BufferReader& istm, uint16_t &anInt) {
    istm.read<uint16_t>(anInt);
    return istm;
  }

  BufferReader& operator >> (BufferReader& istm, uint32_t &anInt) {
    istm.read<uint32_t>(anInt);
    return istm;
  }

  BufferReader& operator >> (BufferReader& istm, int &anInt) {
    istm.read<int>(anInt);
    return istm;
  }

  BufferReader& operator >> (BufferReader& istm, float &aFloat) {
    istm.read<float>(aFloat);
    return istm;
  }
  
  BufferReader& operator >> (BufferReader& istm, char *aBuffer) {
    uint8_t theSize{0};
    istm.read<uint8_t>(theSize);
    istm.read(aBuffer,(int)theSize);
    return istm;
  }
  
  BufferReader& operator >> (BufferReader& istm, std::string& aString) {
    aString.clear();
    
    uint8_t theSize = 0;
    istm.read(theSize);
    
    if(theSize<=0)
      return istm;
    
    istm.read(aString, theSize);
    return istm;
  }

  
  //--------------------------------------------------
  
  BufferWriter::BufferWriter(char* aBuffer, size_t aMax) : max(aMax), buffer(aBuffer) {
    close(); //clear buffer, and reset...
  }
  
  BufferWriter&  BufferWriter::close() {
    memset(buffer,0,max);
    pos=0;
    return *this;
  }
  
  BufferWriter& BufferWriter::writeCString(const char *aBuffer, size_t aLength) {
    write<uint8_t>((uint8_t)aLength);
    if(aLength) writeRaw((void*)aBuffer, aLength);
    return *this;
  }
  
  BufferWriter& operator << (BufferWriter& ostm, const bool aBool) {
    ostm.write<bool>(aBool);
    return ostm;
  }
  
  BufferWriter& operator << (BufferWriter& ostm, const char aChar) {
    ostm.write<char>(aChar);
    return ostm;
  }
  
  BufferWriter& operator << (BufferWriter& ostm, const uint8_t aByte) {
    ostm.write<uint8_t>(aByte);
    return ostm;
  }
  
  BufferWriter& operator << (BufferWriter& ostm, const uint16_t anInt) {
    ostm.write<uint16_t>(anInt);
    return ostm;
  }
  
  BufferWriter& operator << (BufferWriter& ostm, const uint32_t anInt) {
    ostm.write<uint32_t>(anInt);
    return ostm;
  }

  BufferWriter& operator << (BufferWriter& ostm, const int anInt) {
    ostm.write<int>(anInt);
    return ostm;
  }
  
  BufferWriter& operator << (BufferWriter& ostm, float aFloat) {
    ostm.write<float>(aFloat);
    return ostm;
  }
  
  BufferWriter& operator << (BufferWriter& ostm, const char *aBuffer) {
    ostm.writeCString(aBuffer, aBuffer ? strlen(aBuffer) : 0);
    return ostm;
  }
  
  BufferWriter& operator << (BufferWriter& ostm, const std::string &aString) {
    ostm.writeCString(aString.c_str(), aString.length());
    return ostm;
  }

}
