//
//  FolderReader.cpp
//  Database3
//
//  Created by rick gessner on 4/12/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include <stdio.h>
#include "FolderReader.hpp"


#ifdef __APPLE__
  #include <dirent.h>
#elif __linux__
  #include <dirent.h>
#else
  #include "dirent_win.hpp"
#endif


namespace ECE141 {

  
  // USE: check to see if filename has a '.' extension...
  bool hasExtension(const std::string &aFilename, const char *anExtension) {
    if(anExtension) {
      std::string::size_type theIndex=aFilename.rfind('.');
      if(theIndex != std::string::npos) {
        std::string theExt = aFilename.substr(theIndex+1);
        return theExt==anExtension;
      }
      return false;
    }
    return false;
  }
  
  //=============================================

#ifdef __APPLE__
  class MacFolderReader : public FolderReader {
  public:
    
    MacFolderReader(const char *aPath) : FolderReader(aPath) {
      dir=opendir(aPath); //mac/unix OS call to open a folder for reading...
    }
    
    void each(folder_callback aCallback, const char *anExtension) const {
      if (dir) {

        //now iterate over all the files in the directory...
        struct dirent *theEntry;
        while ((theEntry = readdir (dir)) != NULL) {
          std::string theFile(theEntry->d_name);
          if(nullptr==anExtension || hasExtension(theFile, anExtension)) {
            aCallback(theFile);
          }
        }
        closedir (dir);
      }
    }
    
    DIR *dir;
    
  };
  
#elif __linux__
  
  class UnixFolderReader : public FolderReader {
  public:
    UnixFolderReader(const char *aPath) : FolderReader(aPath) {
      dir=opendir(aPath);
    } //should just work; not tested...

    ~UnixFolderReader() {
      if (dir) {
        closedir (dir);
      }
    }

    void each(folder_callback aCallback, const char *anExtension) const {
      if (dir) {

        //now iterate over all the files in the directory...
        struct dirent *theEntry;
        while ((theEntry = readdir (dir)) != NULL) {
          std::string theFile(theEntry->d_name);
          if(nullptr==anExtension || hasExtension(theFile, anExtension)) {
            aCallback(theFile);
          }
        }
        //closedir (dir);
      }
    }
    
    DIR *dir;
  };
  
#else   //WINDOZE...
  
  //NOTE: We included a custom version of dirent_win.h, so we can use
  //      the same calls to get files as we do on other platforms...
  ///     YOU MAY USE ANOTHER APPROACH if this doesn't work...
  
class WindowsFolderReader : public FolderReader {
  public:
    WindowsFolderReader(const char *aPath) : FolderReader(aPath), path(aPath) {}
    
    void each(folder_callback aCallback, const char* anExtension) const {
      //WINDOWS USERS -- I'm guessing at this code...Please VERIFY....
      struct dirent** theFiles;
      
      if (int n = scandir(path.c_str(), &theFiles, NULL, alphasort)) {
        struct dirent* theEntry;
        for (int i = 0; i < n; i++) {
          theEntry = theFiles[i];
          switch (theEntry->d_type) {
            case DT_LNK: break;
            case DT_DIR: break;
            case DT_REG:
            default:
            {
              std::string theFile(theEntry->d_name);
              if (nullptr == anExtension || hasExtension(theFile, anExtension)) {
                aCallback(theFile);
              }
            }
          }
        }
      }
    } //each
    
    std::string path;

  };  //WindowsFolderReader...
#endif
  
  //------------------------------------------------
  
  FolderReader* FolderReader::factory(const char *aPath) {
    switch(getDefaultPlatform()) {
#ifdef __APPLE__
      case Platform::mac:     return new MacFolderReader(aPath);
#elif __linux__
      case Platform::unix:    return new UnixFolderReader(aPath);
#else
      case Platform::windows: return new WindowsFolderReader(aPath);
#endif
      default:break;
    }
    return nullptr;
  }
  
  // USE: generic version of this. Override if your platform does something different...
  bool FolderReader::exists(const std::string &aFilePath) {
    if (FILE *file = fopen(aFilePath.c_str(), "r")) {
      fclose(file);
      return true;
    }
    return false;
  }

};
