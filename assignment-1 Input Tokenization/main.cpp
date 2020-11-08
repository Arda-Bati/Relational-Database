//
//  main.cpp
//  Database1
//
//  Created by rick gessner on 3/17/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include "CommandProcessor.hpp"
#include "Tokenizer.hpp"
#include <iostream>
#include <sstream>

int main(int argc, const char * argv[]) {
  ECE141::CommandProcessor  theProcessor;
  ECE141::StatusResult      theResult{};
  
  if(argc>1) {
    //in a future assignment, you'll add code to process a script file. Ignore for now...
  }
  else {
    
    std::string theUserInput;
    bool running=true;
    
    std::cout << std::endl << "> ";
    while (running && std::getline(std::cin, theUserInput)) {
      if(theUserInput.length()) {
        std::stringstream theStream(theUserInput);
        ECE141::Tokenizer theTokenizer(theStream);
        theTokenizer.tokenize();
        theResult=theProcessor.processInput(theTokenizer);
      }
      if(ECE141::userTerminated==theResult.code)
        running=false;
      else std::cout << std::endl << "> ";
    }
  }
  
  return 0;
}
