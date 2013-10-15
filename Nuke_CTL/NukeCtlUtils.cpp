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

namespace NukeCtl {
  vector<string>&
  split(const string &s, char delim, vector<string> &elems)
  {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
      elems.push_back(item);
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
}