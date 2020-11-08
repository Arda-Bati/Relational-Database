//
//  EntityDescriptionView.cpp
//  Datatabase4
//
//  Created by rick gessner on 4/22/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include "EntityDescriptionView.hpp"
#include "Helpers.hpp"
#include "Entity.hpp"
#include "Attribute.hpp"
#include <iomanip>

//** USE: This view describes an entity (fields), suitable for console display...

namespace ECE141 {

  EntityDescriptionView::EntityDescriptionView(const Entity &anEntity)
   : entity{anEntity}, stream(nullptr) {}
  

  bool EntityDescriptionView::show(std::ostream &anOutput) {
    stream=&anOutput; //for use in callback...
    
    const char* theSep   ="+-------------------+--------------+------+------+----------------+------------------+";
    const char* theTitle ="| Field             | Type         | Null | Key  | Default        | Extra            |";
    
    anOutput << theSep << std::endl;
    anOutput << theTitle << std::endl;
    anOutput << theSep << std::endl;

    AttributeList theAttrs = entity.getAttributes();
    for(auto theAttribute : theAttrs) {
        anOutput << theAttribute.describeTabular() << std::endl;
    }
    
    //STUDENT: Add your code here to output the list of fields, properly formatted...
    //         Feel free to completely change this class if you have a better approach...

    anOutput << theSep << std::endl;

    return true;
  }
  
}
