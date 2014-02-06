//
//  NukeCtlUtils.h
//  IIF
//
//  Created by Joseph Goldstone on 10/14/13.
//  Copyright (c) 2013 Joseph Goldstone. All rights reserved.
//

#ifndef __IIF__NukeCtlUtils__
#define __IIF__NukeCtlUtils__

#include <vector>
#include <string>

namespace NukeCtl {
  
  // These two are from http://stackoverflow.com/questions/236129/how-to-split-a-string-in-c
  std::vector<std::string>&
  split(const std::string &s,
        char delim,
        std::vector<std::string> &elems);
  
  std::vector<std::string>
  split(const std::string &s, char delim);
  
  const std::string
  trimLeft(const std::string &s, const std::string &debris);
  
  const std::string
  trimRight(const std::string &s, const std::string &debris);
  
  std::string
  filenameFromPathname(const std::string &path);
  
  std::string
  modulenameFromFilename(const std::string &filename);
  
}

#endif /* defined(__IIF__NukeCtlUtils__) */

