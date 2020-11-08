//
//  folder reader
//  Database3
//
//  Created by rick gessner on 4/12/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef FolderReader_hpp
#define FolderReader_hpp

#include <vector>
#include <string>
#include <functional>
#include "View.hpp"

namespace ECE141 {
  
  using folder_callback = std::function<bool(const std::string &aName)>;
  
  enum class Platform {mac, windows, unix};
  
  class FolderReader {
  public:

    static  Platform      getDefaultPlatform() {return Platform::windows;}
    static  FolderReader* factory(const char *aPath);

    
                          FolderReader(const char *aPath) : extension{nullptr} {}
    virtual               ~FolderReader() {}
    
    virtual bool          exists(const std::string &aName);
    
    virtual void          each(folder_callback aCallback, const char *anExtension="") const =0;
    
    const char *extension; //usually 'db', cache if given...
  };
    
};

#endif /* FolderReader_hpp */
