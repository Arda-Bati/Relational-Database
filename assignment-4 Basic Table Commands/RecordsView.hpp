//
//  RecordsView.hpp
//  Datatabase4
//
//  Created by rick gessner on 4/26/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef RecordsView_h
#define RecordsView_h

#include <iostream>
#include <iomanip>
#include "View.hpp"
#include "Entity.hpp"
#include "Storage.hpp"

namespace ECE141 {

  // USE: This view is used to show a record set in tabular format...
  class RecordsView : public View {
  public:
    
    RecordsView(const Entity &anEntity, RowCollection &aCollection)
      : collection{aCollection}, entity(anEntity) {}
    
    // USE: this is the main show() for the view, where it presents all the rows...
    bool show(std::ostream &anOutput) {
      //STUDENT: Implement this, to show each row (from aCollection) like this example:
      //

      // > select * from Names
      // +-----------+----------------------+
      // | id        | first_name           |
      // +-----------+----------------------+
      // | 1         | rickg                |
      // +-----------+----------------------+
      // | 2         | ramsin               |
      // +-----------+----------------------+
      
      return true; //or false ,if your view fails...
    }
    
  protected:
    RowCollection  &collection;
    const Entity   &entity;
  };

}

#endif /* RecordsView_h */
