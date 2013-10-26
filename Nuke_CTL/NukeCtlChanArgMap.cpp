//
//  NukeCtlChanArgMap.cpp
//  NukeCtl
//
//  Created by Joseph Goldstone on 10/15/13.
//  Copyright (c) 2013 ARRI Inc. All rights reserved.
//

#include "NukeCtlChanArgMap.h"
#include "Iex.h"

using namespace Ctl;
using namespace DD::Image;
using namespace std;
using namespace Iex;

namespace NukeCtl
{
  void
  ChanArgMap::load(const Row& in, FunctionCallPtr fn)
  {
    foreach(c, in.writable_channels())
    {
      const char* chanName = getName(c);
      map<string, string>::const_iterator i = chanNameToArgName_.find(chanName);
      if (i == chanNameToArgName_.end())
      {
        THROW(LogicExc, "Could not find expected CTL function arg name "
              "for Nuke channel named '" << chanName << "'");
      }
      FunctionArgPtr arg = fn->findInputArg(i->second);
      chanToArgData_[c] = arg->data();
    }
    for (size_t i=0, n=fn->numOutputArgs(); i < n; ++i)
    {
      FunctionArgPtr arg = fn->inputArg(i);
      map<string, string>::const_iterator j = argNameToChanName_.find(arg->name());
      if (j == argNameToChanName_.end())
      {
        THROW(LogicExc, "Could not find expected Nuke channel name for CTL "
              "function output arg name '" << arg->name() << "'");
      }
      argDataToChan_[arg->data()] = findChannel(j->second.c_str());
    }
  }

  void
  ChanArgMap::copyChanDataToArgData(const DD::Image::Row& r, int x0, int x1)
  {
    for (map<Channel, char*>::const_iterator i = chanToArgData_.begin(); i != chanToArgData_.end(); ++i)
    {
      for (int j = x0, k = 0; j < x1; ++j, ++k)
      {
        (i->second)[k*sizeof(half)] = static_cast<half>(r[i->first][j]);
      }
    }
  }
  
  void
  ChanArgMap::copyArgDataToChanData(DD::Image::Row& r, int x0, int x1)
  {
    for (map<char*, Channel>::const_iterator i = argDataToChan_.begin(); i != argDataToChan_.end(); ++i)
    {
      for (int j = x0, k = 0; j < x1; ++j, ++k)
      {
        r.writable(i->second)[j] = (i->first)[k*sizeof(half)];
      }
    }
  }

}
