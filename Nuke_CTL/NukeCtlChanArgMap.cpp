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
  ChanArgMap::ChanArgMap()
  {
    argNameToChanName_["rIn"] = "r";
    argNameToChanName_["gIn"] = "g";
    argNameToChanName_["bIn"] = "b";
    argNameToChanName_["aIn"] = "a";
    argNameToChanName_["rOut"] = "r";
    argNameToChanName_["gOut"] = "g";
    argNameToChanName_["bOut"] = "b";
    argNameToChanName_["aOut"] = "a";
  }
  
  ChanArgMap::ChanArgMap(const ChanArgMap& c)
  : argNameToChanName_(c.argNameToChanName_),
  chanToInArgData_(c.chanToInArgData_),
  outArgDataToChan_(c.outArgDataToChan_)
  {
  }
  
  ChanArgMap&
  ChanArgMap::operator=(const ChanArgMap& rhs)
  {
    if (this != &rhs)
    {
      argNameToChanName_ = rhs.argNameToChanName_;
      chanToInArgData_   = rhs.chanToInArgData_;
      outArgDataToChan_  = rhs.outArgDataToChan_;
    }
    return *this;
  }

  void
  ChanArgMap::load(const Row& in, int x, int r, FunctionCallPtr fn)
  {
    // We might be called multiple times if, say, execute() noticed that x and r had changed since a prior row was processed.
    chanToInArgData_.clear();
    outArgDataToChan_.clear();
    for (size_t i = 0, n = fn->numInputArgs(); i < n; ++i)
    {
      FunctionArgPtr arg = fn->inputArg(i);
      if (! arg)
      {
        THROW(LogicExc, "CTL function claims " << n << " arguments but cannot get info for arg " << i);
      }
      if (! arg->isVarying())
      {
        THROW(LogicExc, "CTL function parameter " << arg->name() << " is not a varying parameter");
      }
      map<string, string>::const_iterator j = argNameToChanName_.find(arg->name());
      if (j == argNameToChanName_.end())
      {
        THROW(LogicExc, "CTL function parameter " << arg->name() << " has no mapping to a Nuke channel");
      }
      Channel c = findChannel(j->second.c_str());
      // TODO: Find out what findChannel returns if you ask for something it doesn't have, and throw an appropriate error.
      chanToInArgData_[c] = arg->data();
    }
    for (size_t i = 0, n = fn->numOutputArgs(); i < n; ++i)
    {
      FunctionArgPtr arg = fn->outputArg(i);
      map<string, string>::const_iterator j = argNameToChanName_.find(arg->name());
      if (j == argNameToChanName_.end())
      {
        THROW(LogicExc, "Could not find expected Nuke channel name for CTL "
              "function output arg name '" << arg->name() << "'");
      }
      outArgDataToChan_[arg->data()] = findChannel(j->second.c_str());
    }
  }

  void
  ChanArgMap::copyInputRowToArgData(const DD::Image::Row& r, int x0, int x1)
  {
    for (map<Channel, char*>::const_iterator i = chanToInArgData_.begin(); i != chanToInArgData_.end(); ++i)
    {
      for (int j = x0, k = 0; j < x1; ++j, ++k)
      {
        reinterpret_cast<half*>(i->second)[k] = r[i->first][j];
      }
    }
  }
  
  void
  ChanArgMap::copyArgDataToOutputRow(int x0, int x1, DD::Image::Row& r)
  {
    for (map<char*, Channel>::const_iterator i = outArgDataToChan_.begin(); i != outArgDataToChan_.end(); ++i)
    {
      for (int j = x0, k = 0; j < x1; ++j, ++k)
      {
        r.writable(i->second)[j] = reinterpret_cast<half*>(i->first)[k];
      }
    }
  }

}
