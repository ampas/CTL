//
//  NukeCtlUtils.cpp
//  IIF
//
//  Created by Joseph Goldstone on 10/14/13.
//  Copyright (c) 2013 Joseph Goldstone. All rights reserved.
//

#include "NukeCtlUtils.h"

#include <sstream>
using namespace std;

#include "Iex.h"
using namespace Iex;

namespace NukeCtl {

  // These two are from http://stackoverflow.com/questions/236129/how-to-split-a-string-in-c
  vector<string>&
  split(const string &s, char delim, vector<string> &elems)
  {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
      elems.push_back(item);
    }
    // special-case (not in the StackOverflow version)
    // For some reason I can't find this C++11 predicate from <algorithms>
    //   if (all_of(elems.begin(), elems.end(), [](string s){ return s.size() == 0; }))
    // Probably all for the best; I can't assume someone else compiling this would have C++11.
    bool all_empty = true;
    for (vector<string>::const_iterator i = elems.begin(); i < elems.end(); ++i)
    {
      if (i->size() != 0)
      {
        all_empty = false;
        break;
      }
    }
    if (all_empty)
    {
      elems.clear();
    }
    return elems;
  }
  
  vector<string>
  split(const string &s, char delim)
  {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
  }
  
  const string
  trimLeft(const string &s, const string &debris)
  {
    // Beats me why passing an empty string as the first arg causes an exception, but the comparable trimRight case does not
    return s.size() == 0 ? s : s.substr(s.find_first_not_of(debris), string::npos);
  }
  
  const string
  trimRight(const string &s, const string &debris)
  {
    return s.substr(0, s.find_last_not_of(debris) + 1);
  }
  
  string
  filenameFromPathname(const string &path)
  {
    string strippedPath(trimRight(trimLeft(path, "\t "), "\t "));
    // special-case the root
    if (strippedPath == "/")
    {
      return strippedPath;
    }
    // directories are components too
    strippedPath = trimRight(strippedPath, "/");
    vector<string> pathComponents(split(path, '/'));
    if (pathComponents.size() == 0)
    {
      THROW(ArgExc, "Expected at least one component of path '" << path << "', but saw none");
    }
    return pathComponents[pathComponents.size() -1];
  }
  
  string
  modulenameFromFilename(const string &filename)
  {
    // blank filename is an error
    string strippedFilename(trimRight(trimLeft(filename, "\t "), "\t "));
    if (strippedFilename == "")
    {
      THROW(ArgExc, "Expected non-blank filename");
    }
    vector<string> filenameComponents(split(strippedFilename, '.'));
    return filenameComponents[0];
  }
  
}