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
    chanToInArg_.clear();
    outArgToChan_.clear();
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
      chanToInArg_[c] = arg;
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
      outArgToChan_[arg] = findChannel(j->second.c_str());
    }
  }
  
  void
  ChanArgMap::copyInputRowToArgData(const DD::Image::Row &in, int x0, int x1)
  {
    for (map<Channel, FunctionArgPtr>::const_iterator i = chanToInArg_.begin(); i != chanToInArg_.end(); ++i)
    {
      Channel channel = i->first;
      FunctionArgPtr arg = i->second;
      size_t src_stride = sizeof(float);
      size_t dst_offset = 0;
      size_t count = (x1 - x0) + 1;
      arg->set(in[channel] + x0, src_stride, dst_offset, count);
    }
  }
  
  void
  ChanArgMap::copyArgDataToOutputRow(int x0, int x1, DD::Image::Row &out)
  {
    for (map<FunctionArgPtr, Channel>::const_iterator i = outArgToChan_.begin(); i != outArgToChan_.end(); ++i)
    {
      FunctionArgPtr arg = i->first;
      Channel channel = i->second;
      size_t dst_stride = sizeof(float);
      size_t src_offset = 0;
      size_t count = (x1 - x0) + 1;
      arg->get(out.writable(channel) + x0, dst_stride, src_offset, count);
    }
  }

}
