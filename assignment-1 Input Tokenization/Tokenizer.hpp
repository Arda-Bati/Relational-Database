//
//  Tokenizer.hpp
//  Database
//
//  Created by rick gessner on 3/19/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef Tokenizer_hpp
#define Tokenizer_hpp

#include <stdio.h>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include "keywords.hpp"


namespace ECE141 {
  
  using callback = bool(char aChar);
  
  //Delineates different types of tokens our system will encounter. You may need to add more...
  enum class TokenType {
    comma=1, colon, semicolon, number, operators, apostrophe,
    lparen, rparen, lbracket, rbracket, lbrace, rbrace,
    slash, star, equal, plus, minus, keyword,
    identifier, function, string, unknown
  };
  
  //Holds a given token, created when input (command line or script) is tokenized...
  struct Token {
    Token(TokenType aType=TokenType::unknown,std::string aString="",Keywords aKW=Keywords::unknown_kw) :
    type(aType), keyword(aKW), data(aString) {}
    
    TokenType   type;
    Keywords    keyword;
    std::string data;
  };
  
  
  // USE: This class performs tokenizing for the sytsem --------------------
  
  class Tokenizer {
  public:
    Tokenizer(std::istream &anInputStream);
    
    virtual StatusResult  tokenize();
    virtual Token&        tokenAt(int anOffset);
    size_t                size() {return tokens.size();}
    
    virtual void          restart() {index=0;}
    virtual bool          more() {return index<size();}
    virtual bool          next(int anOffset=1);
    virtual Token&        peek(int anOffset=1);
    virtual Token&        current();
    
  protected:
    bool isNumeric(char theChar);
    bool isOperator(char theChar);
    bool isPunctuation(char theChar);
    bool isAlpha(char theChar);

    std::vector<Token>    tokens;
    std::istream          &input;
    int                   index;
  };
  
}

#endif /* Tokenizer_hpp */
