//
//  RGTokenizer.hpp
//  Database3
//
//  Created by rick gessner on 3/24/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef RGTokenizer_hpp
#define RGTokenizer_hpp

#include <cstring>
#include <algorithm>
#include "Tokenizer.hpp"
#include "Value.hpp"
#include "Helpers.hpp"

namespace ECE141 {


  class RGTokenizer : public Tokenizer {
  public:
    
                RGTokenizer(std::istream &anInputStream);
    virtual StatusResult  tokenize();
    Token&                tokenAt(int anOffset);

    void            dump();
    
    std::string     readWhile(callback aCallback);
    std::string     readUntil(char aTerminal, bool addTerminal=false);
    std::string     readUntil(callback aCallback, bool addTerminal=false);
    size_t          remaining() {return index<size() ? size()-index :0;}
    
  };

}

#endif /* RGTokenizer_hpp */
