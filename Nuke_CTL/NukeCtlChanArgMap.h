//
//  NukeCtlChanArgMap.h
//  NukeCtl
//
//  Created by Joseph Goldstone on 10/15/13.
//  Copyright (c) 2013 ARRI Inc. All rights reserved.
//

#ifndef __NukeCtl__NukeCtlChanArgMap__
#define __NukeCtl__NukeCtlChanArgMap__

#include "DDImage/Channel.h"
#include "DDImage/Row.h"
#include "CtlFunctionCall.h"
#include <string>
#include <map>

namespace NukeCtl
{
  class ChanArgMap
  {
  public:
    ChanArgMap()
    {
    }
    
    void
    load(DD::Image::Row& row, Ctl::FunctionCallPtr& fn);
    
    void
    bind(DD::Image::Channel, const std::string &functionArgName);
    
    void
    copyChanToArg(DD::Image::Channel channel, int x, int r);
  private:
    std::map<DD::Image::Channel, Ctl::FunctionArgPtr> chanToFunctionArg_;
    std::map<Ctl::FunctionArgPtr, DD::Image::Channel> functionArgToChan_;
  };
}

#endif /* defined(__NukeCtl__NukeCtlChanArgMap__) */
