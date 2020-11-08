//
//  BlockVisitor_h
//  Datatabase4
//
//  Created by rick gessner on 4/25/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef BlockVisitor_h
#define BlockVisitor_h

#include "Storage.hpp"
#include "Entity.hpp"
#include "Filters.hpp"
#include "Row.hpp"

namespace ECE141 {
  
  class BlockVisitor {
  public:
    BlockVisitor(const Entity &anEntity, const Filters &aFilters)
      : entity(anEntity), filters(aFilters) {}

    StatusResult operator()(Storage &aStorage, const Block &aBlock, uint32_t aBlockNum) {
      if('D'==aBlock.header.type) {
        if(0==aBlock.header.extra) {
        }
      }
      return StatusResult{noError};
    }

    const Entity      &entity;
    const Filters     &filters;
  };
    
}

#endif /* BlockVisitor_h */
