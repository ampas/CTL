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
    ChanArgMap(Ctl::FunctionCallPtr fn);
    
    void
    copyInputRowToArgData(const DD::Image::Row &in, int x0, int x1);
    
    void
    copyArgDataToOutputRow(int x0, int x1, DD::Image::Row &out);
    
  private:
    // Not implemented, and this guarantees the compiler will not improvise one for us.
    ChanArgMap(const ChanArgMap &c);
    
    // Not implemented, and this guarantees the compiler will not improvise one for us.
    ChanArgMap&
    operator=(const ChanArgMap &rhs);
    
    std::map<std::string, std::string> argNameToChanName_;
    
    typedef std::map<DD::Image::Channel, Ctl::FunctionArgPtr> ChannelToFunctionArgPtrMap;

    // Without this being specified as a third arg to std::map below, outArgToChan_ will never have more than one member.
    struct FunctionArgPtrLess:
    public std::binary_function<const Ctl::FunctionArgPtr, const Ctl::FunctionArgPtr, bool> {
        bool operator() (const Ctl::FunctionArgPtr pa1, const Ctl::FunctionArgPtr pa2) const
      {
        return pa1->name() < pa2->name();
      }
    };
    typedef std::map<Ctl::FunctionArgPtr, DD::Image::Channel, FunctionArgPtrLess> FunctionArgPtrToChannelMap;
    
    ChannelToFunctionArgPtrMap chanToInArg_;
    FunctionArgPtrToChannelMap outArgToChan_;
  };
}

#endif /* defined(__NukeCtl__NukeCtlChanArgMap__) */
