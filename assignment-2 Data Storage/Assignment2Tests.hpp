//
//  Assignment2Tests.hpp
//  Database2
//
//  Created by rick gessner on 4/7/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef Assignment2Tests_h
#define Assignment2Tests_h

#include "Value.hpp"
#include "Row.hpp"
#include "Storage.hpp"
#include <cstdlib>
#include <ctime>

std::ostream& getOutput() {
  return std::cout;
}

void runConstructorTests(std::ostream &anOutput) {
  //STUDENT: add code here to test that construct your Value class for each type
  
  std::string theString("hello");
  ECE141::Value theValue1(theString);  //build varchar from std::string
  theValue1.debugDump(anOutput);
  
  ECE141::Value theValue2(theString.c_str());  //build varchar from char*
  theValue2.debugDump(anOutput);

  ECE141::Value theValue3(true);  //build boolean value
  theValue3.debugDump(anOutput);
  
  ECE141::Value theValue4(-1234); //build int value
  theValue4.debugDump(anOutput);
  
  ECE141::Value theValue5(2345);  //build unix_timestamp value
  theValue5.debugDump(anOutput);
  
  ECE141::Value theValue6(3.14);  //build float value
  theValue6.debugDump(anOutput);
  
}

void runAssignmentTests(std::ostream &anOutput) {
  //STUDENT: Implement test to validate assignment operators for all your types...
  ECE141::Value theValue(true); //start with boolean value...
  
  theValue=3.14;  //convert to float
  theValue.debugDump(anOutput);

  theValue=-123;  //convert to int
  theValue.debugDump(anOutput);

  std::string theString("hello");
  theValue=theString;  //convert to string
  theValue.debugDump(anOutput);

  theValue=34567;  //convert to unix_timestamp
  theValue.debugDump(anOutput);

  theValue=theString.c_str();  //convert to string (via char*)
  theValue.debugDump(anOutput);
  
  theValue=false;  //convert to bool
  theValue.debugDump(anOutput);

}

void runConversionTests(std::ostream &anOutput) {
  ECE141::Value theValue(true);
  ECE141::Value theValue2("hello");
  ECE141::Value theValue3(3.14);
  ECE141::Value theValue4(-1234);
  ECE141::Value theValue5(256);

  //test conversion to bool
  bool aBool;
  aBool = theValue;
  anOutput << aBool << std::endl;
  aBool = theValue2;
  anOutput << aBool << std::endl;
  aBool = theValue3;
  anOutput << aBool << std::endl;
  aBool = theValue4;
  anOutput << aBool << std::endl;
  aBool = theValue5;
  anOutput << aBool << std::endl;

  //test conversion to std::string
  std::string aString;
  aString = (std::string)theValue;
  anOutput << aString << std::endl;
  aString = (std::string)theValue2;
  anOutput << aString << std::endl;
  aString = (std::string)theValue3;
  anOutput << aString << std::endl;
  aString = (std::string)theValue4;
  anOutput << aString << std::endl;
  aString = (std::string)theValue5;
  anOutput << aString << std::endl;

  //test conversion to int
  int theInt;
  theInt = theValue;
  anOutput << theInt << std::endl;
  theInt = theValue2;
  anOutput << theInt << std::endl;
  theInt = theValue3;
  anOutput << theInt << std::endl;
  theInt = theValue4;
  anOutput << theInt << std::endl;
  theInt = theValue5;
  anOutput << theInt << std::endl;

  //test conversion to float
  float theFloat;
  theFloat = theValue;
  anOutput << theFloat << std::endl;
  theFloat = theValue2;
  anOutput << theFloat << std::endl;
  theFloat = theValue3;
  anOutput << theFloat << std::endl;
  theFloat = theValue4;
  anOutput << theFloat << std::endl;
  theFloat = theValue5;
  anOutput << theFloat << std::endl;

  //test conversion to timestamp
  uint32_t theUint;
  theUint = theValue;
  anOutput << theUint << std::endl;
  theUint = theValue2;
  anOutput << theUint << std::endl;
  theUint = theValue3;
  anOutput << theUint << std::endl;
  theUint = theValue4;
  anOutput << theUint << std::endl;
  theUint = theValue5;
  anOutput << theUint << std::endl;

  //test conversion to char*
  char* theString;
  theString = (char*)theValue;
  anOutput << theString << std::endl;
  theString = (char*)theValue2;
  anOutput << theString << std::endl;
  theString = (char*)theValue3;
  anOutput << theString << std::endl;
  theString = (char*)theValue4;
  anOutput << theString << std::endl;
  theString = (char*)theValue5;
  anOutput << theString << std::endl;
}

//This is called by testing code to make a Value object for testing...
ECE141::Value createValueObjectForType(ECE141::DataType aType) {
  //std::srand((unsigned int)std::time(nullptr));
  float theFloat=(std::rand() % 13) * 3.14;
  switch(aType) {
    case ECE141::DataType::int_type : return ECE141::Value((int)theFloat);
    case ECE141::DataType::float_type : return ECE141::Value(theFloat);
    case ECE141::DataType::bool_type : return ECE141::Value((bool)theFloat);
    case ECE141::DataType::timestamp_type : return ECE141::Value((uint32_t)theFloat);
    case ECE141::DataType::varchar_type : return ECE141::Value("hello there :)");
    default : return ECE141::Value();
  }
}

//Below we are creating a series of random values for your tests.
//1. N values are stored in the Row
//2. The Row is encoded in to a block
//3. The block is decoded back into a row
//4. The two rows are compared (should be identical)...

void runEncodeDecodeTests(std::ostream &anOutput) {

  //STUDENT: This will only work when:
  //   1. your Value class is working
  //   2. you implemented Block(const KeyValues &aList)  (in storage.cpp)
  //   3. you implemented Row(const Block &aBlock)  (in row.cpp)
  
  std::srand(std::time(NULL));//random # seed...
  
  ECE141::Row theRow;
  ECE141::KeyValues &theColumns=theRow.getColumns();
  
  //generate 20 values and stuff them into theRow...
  for(int theV=0;theV<50;theV++) {
    ECE141::DataType theType=ECE141::DataType::no_type;
    float theFloat=(std::rand() % 13) * 3.14;
    
    //randomly choose a value type...
    switch((std::rand() % 5)+1) {
      case 1 : theType = ECE141::DataType::int_type; break;
      case 2 : theType = ECE141::DataType::float_type; break;
      case 3 : theType = ECE141::DataType::bool_type; break;
      case 4 : theType = ECE141::DataType::timestamp_type; break;
      case 5 : theType = ECE141::DataType::varchar_type; break;
      default : break;
    }
    
      //now make a value of for type type...
    ECE141::Value theValue=createValueObjectForType(theType);
    
    char theField[20]{0};
    sprintf(theField, "field_%d", theV);
    theColumns[theField]=theValue;
  }
  
  ECE141::Block theBlock(theColumns); //Encode your key/values into the block...
  ECE141::Row theDecodedRow(theBlock);
  
  anOutput << "Encode/Decode test: ";
  if(theRow==theDecodedRow) {
    anOutput << "PASSED!" << std::endl;
  }
  else {anOutput << "FAILED!" << std::endl;}
  
}

void runAssignment2Tests() {
  std::ostream &theOutput=getOutput();
  runConstructorTests(theOutput);
  runConversionTests(theOutput);
  runAssignmentTests(theOutput);
  runEncodeDecodeTests(theOutput);
}


#endif /* Assignment2Tests_h */
