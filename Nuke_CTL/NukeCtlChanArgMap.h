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
#include "CtlRcPtr.h"
#include "CtlStdType.h"

#include <string>
#include <map>

namespace NukeCtl
{
  class ChanArgMapFriend;
  
  class ChanArgMap
  {
    friend class ChanArgMapFriend;
  public:
    ChanArgMap()
    {
      chanNameToArgName_["r"] = "rIn";
      chanNameToArgName_["g"] = "gIn";
      chanNameToArgName_["b"] = "bIn";
      chanNameToArgName_["a"] = "aIn";
      argNameToChanName_["rIn"] = "r";
      argNameToChanName_["gIn"] = "g";
      argNameToChanName_["bIn"] = "b";
      argNameToChanName_["aIn"] = "a";
    }
    
    // Only handles the easy ones for now: in[Chan_Red] -> rIn and rIn -> out[Chan_Red], etc.
    void
    load(const DD::Image::Row& row, Ctl::FunctionCallPtr fn);
    
    void
    copyChanDataToArgData(const DD::Image::Row&, int x0, int x1);
    
    void
    copyArgDataToChanData(DD::Image::Row&, int x0, int x1);
    
  private:
    std::map<std::string, std::string> chanNameToArgName_;
    std::map<std::string, std::string> argNameToChanName_;
    
    std::map<DD::Image::Channel, char*> chanToArgData_;
    std::map<char*, DD::Image::Channel> argDataToChan_;
  };
}

#endif /* defined(__NukeCtl__NukeCtlChanArgMap__) */
