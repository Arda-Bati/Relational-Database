//
//  Tokenizer.cpp
//  Database1
//
//  Created by rick gessner on 3/19/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include "Tokenizer.hpp"
#include <exception>
#include <algorithm>

namespace ECE141 {
  
  Tokenizer::Tokenizer(std::istream &anInput) : input(anInput), index(0)  {}
  
  //convert input given in constructor to collection of tokens...
  StatusResult Tokenizer::tokenize() {
    StatusResult theResult;

    Token aToken;
    char theChar;

    while (input >> theChar) {
      theChar = tolower(theChar);
      std::string theToken;

      //check for operator
      if (isOperator(theChar)) {
        theToken += theChar;

        //check to see if operator includes two chars
        if (isOperator(input.peek())) {
          theToken += input.peek();
          if (gOperators[theToken]) {
            aToken = Token(TokenType::operators, theToken, ECE141::Keywords::unknown_kw);
            input >> theChar;
          }
          else { aToken = Token(TokenType::operators, theToken.substr(0,1), ECE141::Keywords::unknown_kw); }
        }

        else { aToken = Token(TokenType::operators,theToken, ECE141::Keywords::unknown_kw); }
        tokens.push_back(aToken);
      }

      //check for number
      else if (isNumeric(theChar)) {
        theToken += theChar;

        while (isNumeric(input.peek())) {
          input >> theChar;
          theToken += theChar;
        }
        aToken = Token(TokenType::number, theToken, ECE141::Keywords::unknown_kw);
        tokens.push_back(aToken);
      }

      //check for identifiers... want to ignore quotes
      else if (theChar == '"') {
        while ('"' != input.peek()) {
          //if user forgets to add last quote... throw syntax error
          if (input.eof()) {
            theResult = StatusResult(Errors::syntaxError, 0);
            return theResult;
          }

          input.get(theChar); //use get so that whitespace included in identifier
          theToken += tolower(theChar); //not sure if we want to make identifiers lowercase too...
        }
        input >> theChar; //remove extra quote

        if (theToken.size()) {
          aToken = Token(TokenType::identifier, theToken, ECE141::Keywords::unknown_kw);
          tokens.push_back(aToken);
        }
      }

      else if (isPunctuation(theChar)) {
        //can process token immediately
        aToken = Token((TokenType) gPunctuation[theChar], std::string (1,theChar), ECE141::Keywords::unknown_kw);
        tokens.push_back(aToken);
      }

      //check for alpha here
      else if (isAlpha(theChar)){
        theToken += theChar;
        while (isAlpha(tolower(input.peek())) || isNumeric(input.peek())) {
          input >> theChar;
          theToken += tolower(theChar);
        }

        //now classify token as either operator, keyword, or identifier
        if (gOperators[theToken]) {
          aToken = Token(TokenType::operators, theToken, ECE141::Keywords::unknown_kw);
        }
        else if (gDictionary.find(theToken) != gDictionary.end()) {
          aToken = Token(TokenType::keyword, theToken, gDictionary[theToken]);
        }
        else {
          aToken = Token(TokenType::identifier, theToken, ECE141::Keywords::unknown_kw);
        }
        tokens.push_back(aToken);
      }

      //syntax error
      else {
        theResult = StatusResult(Errors::syntaxError, 0);
        return theResult;
      }
    }

    theResult = StatusResult(Errors::noError, 0);
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
  
  //check to see if char in operator table... only considering operators between 1-2 chars
  bool Tokenizer::isOperator(char theChar) {

    if (theChar == '>' || theChar == '<' || theChar == '=') {
      return true;
    }
    return false;
  }

  //check to see if character is number
  bool Tokenizer::isNumeric(char theChar) {
    if ((theChar >= '0' && theChar <= '9') || theChar == '.') { //not quite sure about dot, maybe do final check of this in main loop
      return true;
    }
    return false;
  }

  //check if operator is some sort of punctuation
  bool Tokenizer::isPunctuation(char theChar) {
    if (gPunctuation[theChar]) { //again, need to test this...
      return true;
    }
    return false;
  }

  bool Tokenizer::isAlpha(char theChar) {
    if ((theChar >= 'a' && theChar <= 'z') || theChar == '_') {
      return true;
    }
    return false;
  }
}
