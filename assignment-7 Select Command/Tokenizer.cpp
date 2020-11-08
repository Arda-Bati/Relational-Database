//
//  Tokenizer.cpp
//  Database3
//
//  Created by rick gessner on 3/19/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include "Tokenizer.hpp"
#include <exception>

namespace ECE141 {
  
  Tokenizer::Tokenizer(std::istream &anInput) : input(anInput), index(0)  {}
  
  //convert input given in constructor to collection of tokens...
  StatusResult Tokenizer::tokenize() {
    StatusResult theResult;
    return theResult;
  }
  
  Token& Tokenizer::tokenAt(int anOffset) {
    if(anOffset<size()) return tokens[anOffset];
    throw std::range_error("bad offset");
  }
  
  bool Tokenizer::next(int anOffset) {
    if(index+anOffset<=size()) {
      index+=anOffset;
      return true;
    }
    return false;
  }
  
  Token& Tokenizer::current() {
    return tokens[index];
  }
  
  Token& Tokenizer::peek(int anOffset) {
    return tokenAt(index+anOffset);
  }
  
}
