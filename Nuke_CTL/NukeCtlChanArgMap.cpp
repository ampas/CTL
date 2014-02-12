 //
//  NukeCtlChanArgMap.cpp
//  NukeCtl
//
//  Created by Joseph Goldstone on 10/15/13.
//  Copyright (c) 2013 ARRI Inc. All rights reserved.
//

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif

#include "NukeCtlChanArgMap.h"
#include "Iex.h"

#ifdef __clang__
#pragma clang diagnostic pop
#endif

using namespace Ctl;
using namespace DD::Image;
using namespace std;
using namespace Iex;

namespace NukeCtl
{
  ChanArgMap::ChanArgMap(FunctionCallPtr fn)
  {
    argNameToChanName_["rIn"] = "r";
    argNameToChanName_["gIn"] = "g";
    argNameToChanName_["bIn"] = "b";
    argNameToChanName_["aIn"] = "a";
    argNameToChanName_["rOut"] = "r";
    argNameToChanName_["gOut"] = "g";
    argNameToChanName_["bOut"] = "b";
    argNameToChanName_["aOut"] = "a";
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
      if (! arg)
      {
        THROW(LogicExc, "CTL function claims " << n << " output arguments but cannot get info for output arg " << i);
      }
      if (! arg->isVarying())
      {
        THROW(LogicExc, "CTL function output argument " << arg->name() << " is not a varying parameter");
      }
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
  ChanArgMap::copyInputRowToArgData(const DD::Image::Row &in, int x0, int x1)
  {
    for (map<Channel, char*>::const_iterator i = chanToInArgData_.begin(); i != chanToInArgData_.end(); ++i)
    {
      for (int j = x0, k = 0; j < x1; ++j, ++k)
      {
        reinterpret_cast<half*>(i->second)[k] = in[i->first][j];
      }
    }
  }
  
  void
  ChanArgMap::copyArgDataToOutputRow(int x0, int x1, DD::Image::Row &out)
  {
    for (map<char*, Channel>::const_iterator i = outArgDataToChan_.begin(); i != outArgDataToChan_.end(); ++i)
    {
      for (int j = x0, k = 0; j < x1; ++j, ++k)
      {
        out.writable(i->second)[j] = reinterpret_cast<half*>(i->first)[k];
      }
    }
  }

}
