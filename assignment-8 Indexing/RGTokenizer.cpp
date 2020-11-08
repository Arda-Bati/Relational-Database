//
//  RGTokenizer.cpp
//  Database1
//
//  Created by rick gessner on 3/24/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include "RGTokenizer.hpp"

namespace ECE141 {
  
  RGTokenizer::RGTokenizer(std::istream &anInput) : Tokenizer(anInput)  {
  }
  
  bool isWhitespace(char aChar) {
    static const char* theWS = " \t\r\n\b";
    return strchr(theWS,aChar);
  }
  
  bool isNumber(char aChar) {
    return isdigit(aChar) || '.'==aChar;
  }
  
  bool isAlphaNum(char aChar) {
    return isalnum(aChar) || '_'==aChar;
  }
  
  bool isQuote(char aChar) {
    return quote==aChar;
  }
  
  bool isOperator(char aChar) {
    return strchr("+-/*%=>!<", aChar);
  }
  
  bool isSign(char aChar) {
    return strchr("+-", aChar);
  }
  
  bool isPunctuation(char aChar) {
    return strchr("()[]{}:,", aChar);
  }
  
  std::string RGTokenizer::readWhile(callback aCallback) {
    std::string theResult;
    while (!input.eof() && (*aCallback)(input.peek())) {
      char theChar=input.get();
      theResult+=theChar;
    }
    return theResult;
  }
  
  std::string RGTokenizer::readUntil(callback aCallback, bool addTerminal) {
    std::string theResult;
    while (!input.eof() && !(*aCallback)(input.peek())) {
      theResult+=input.get();
    }
    if(addTerminal) {
      if(!input.eof()) {
        theResult+=input.get();
      }
    }
    return theResult;
  }
  
  std::string RGTokenizer::readUntil(char aTerminalChar, bool addTerminal) {
    std::string theResult;
    while (!input.eof()) {
      char theChar=input.get();
      if(theChar!=aTerminalChar) {
        theResult+=theChar;
      }
      else break;
    }
    if(addTerminal) {
      if(!input.eof()) {
        theResult+=input.get();
      }
    }
    return theResult;
  }
  
  //run on input provided in constructor; produce vector of tokens...
  StatusResult RGTokenizer::tokenize() {
    
    StatusResult theResult;

    while(!input.eof()) {
      char  theChar = input.peek();
      if(isPunctuation(theChar)) {
        std::string temp;
        TokenType theType = TokenType::unknown;
        temp.push_back(input.get());
        switch(theChar) {
          case comma          : theType=TokenType::comma; break;
          case colon          : theType=TokenType::colon; break;
          case left_paren     : theType=TokenType::lparen; break;
          case right_paren    : theType=TokenType::rparen; break;
          case left_bracket   : theType=TokenType::lbracket; break;
          case right_bracket  : theType=TokenType::rbracket; break;
          case left_brace     : theType=TokenType::lbrace; break;
          case right_brace    : theType=TokenType::rbrace; break;
        }
        tokens.push_back(Token(theType,temp));
      }
      else if(isOperator(theChar)) {
        std::string temp;
        temp.push_back(input.get());
        tokens.push_back(Token(TokenType::operators,temp));
      }
      else if(isNumber(theChar)) {
        std::string theString=readWhile(isNumber);
        tokens.push_back(Token(TokenType::number,theString));
      }
      else if(isQuote(theChar) || (apostrophe==theChar)) {
        input.get(); //skip first quote...
        std::string theString = readUntil(theChar);
        tokens.push_back(Token(TokenType::identifier,theString));        
      }
      else if(semicolon==theChar) {
        tokens.push_back(Token(TokenType::semicolon,std::string(";")));
        input.get(); //skip the char...
      }
      else if(isAlphaNum(theChar)) {
        std::string theString = readWhile(isAlphaNum);
        std::string temp(theString);
        std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
        
        Keywords theKeyword = Helpers::getKeywordId(temp);
        if(Keywords::unknown_kw!=theKeyword) {
          tokens.push_back(Token(TokenType::keyword, temp, theKeyword));
        }
        //else if(getFunctionId(temp)) {
        //  tokens.push_back(Token(TokenType::function,temp));
        //}
        else {
          tokens.push_back(Token(TokenType::identifier,theString));
        }
      }
      readWhile(isWhitespace);
    }

    return theResult;
  }

  // USE: ----------------------------------------------
  void RGTokenizer::dump() {
    for (auto &theToken : tokens) {
      std::cout << "type ";
      switch(theToken.type) {
        case TokenType::comma:
        case TokenType::lparen:
        case TokenType::rparen:
        case TokenType::slash:
          std::cout << "punct " << theToken.data << std::endl;
          break;
          
        case TokenType::operators:
        case TokenType::star:
        case TokenType::equal:
        case TokenType::plus:
        case TokenType::minus:
          std::cout << "operator " << theToken.data << std::endl;
          break;
          
        case TokenType::number:
          std::cout << "number " << theToken.data << std::endl;
          break;
          
        case TokenType::string:
          std::cout << "string " << theToken.data << std::endl;
          break;
          
        case TokenType::identifier:
          std::cout << "identifier " << theToken.data << std::endl;
          break;
          
        case TokenType::keyword:
          std::cout << "keyword " << theToken.data << std::endl;
          break;
          
        default:
          break;
      }
    }
  }
  
}


