//
//  Tokenizer.hpp
//  Database3
//
//  Created by rick gessner on 3/19/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef Tokenizer_hpp
#define Tokenizer_hpp

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "keywords.hpp"

namespace ECE141 {
  
  using callback = bool(char aChar);
  
  enum class TokenType {
    comma=1, colon, semicolon, number, operators, apostrophe,
    lparen, rparen, lbracket, rbracket, lbrace, rbrace,
    slash, star, equal, plus, minus, keyword,
    identifier, function, string, unknown
  };
  
  struct Token {
    Token(TokenType aType=TokenType::unknown, std::string aString="",
          Keywords aKW=Keywords::unknown_kw) : type(aType), keyword(aKW), data(aString) {}
    
    Token& operator=(const Token &aCopy) {
      type=aCopy.type;
      keyword=aCopy.keyword;
      data=aCopy.data;
      return *this;
    }
    
    TokenType   type;
    Keywords    keyword;
    std::string data;
  };
 
  class Tokenizer {
  public:
    Tokenizer(std::istream &anInputStream);
    
    virtual StatusResult  tokenize();
    virtual Token&        tokenAt(int anOffset);
    size_t                size() {return tokens.size();}
    size_t                remaining() {return index<size() ? size()-index :0;}

    virtual void          restart() {index=0;}
    virtual bool          more() {return index<size();}
    virtual bool          next(int anOffset=1);
    virtual Token&        peek(int anOffset=1);

    virtual Token&        current();

  protected:
    
    std::vector<Token>    tokens;
    std::istream          &input;
    int                   index;
  };
  
}

#endif /* Tokenizer_hpp */

