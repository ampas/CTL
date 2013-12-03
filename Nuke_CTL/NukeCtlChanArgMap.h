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
    ChanArgMap();
    
    ChanArgMap(const ChanArgMap& c);
    
    ChanArgMap&
    operator=(const ChanArgMap& rhs);
    
    void
    load(Ctl::FunctionCallPtr fn);
    
    void
    copyInputRowToArgData(const DD::Image::Row&, int x0, int x1);
    
    void
    copyArgDataToOutputRow(int x0, int x1, DD::Image::Row&);
    
  private:
    std::map<std::string, std::string> argNameToChanName_;
    
    std::map<DD::Image::Channel, char*> chanToInArgData_;
    std::map<char*, DD::Image::Channel> outArgDataToChan_;
  };
}

#endif /* defined(__NukeCtl__NukeCtlChanArgMap__) */
