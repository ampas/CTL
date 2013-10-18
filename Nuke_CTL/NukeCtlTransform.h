//
//  NukeCtlTransform.h
//  IIF
//
//  Created by Joseph Goldstone on 10/14/13.
//  Copyright (c) 2013 Joseph Goldstone. All rights reserved.
//

#ifndef __IIF__NukeCtlTransform__
#define __IIF__NukeCtlTransform__

#include <CtlSimdInterpreter.h>
#include <CtlFunctionCall.h>

#include <vector>
#include <string>

#include "NukeCtlChanArgMap.h"


namespace NukeCtl
{
  class TransformFriend;
  
  class Transform {
    friend class TransformFriend;
  public:
    Transform(DD::Image::Row& row,
              const std::string &modulePath,
              const std::string &transformPath);
    void
    execute();
  private:
    static
    const std::vector<std::string>
    parseModulePath(const std::string &mp);
    
    static
    void
    verifyModuleName(const std::string &n);
    
    static
    Ctl::FunctionCallPtr
    topLevelFunctionInTransform(Ctl::SimdInterpreter& interpreter, const std::string &transformPath);
   
    const std::vector<std::string>  modulePathComponents_;
    Ctl::SimdInterpreter      interpreter_;
    Ctl::FunctionCallPtr      functionCall_;
    NukeCtl::ChanArgMap       inputArgMap_;
    NukeCtl::ChanArgMap       outputArgMap_;

    std::string           transformPath_; // Forensics
    
  };
}

#endif /* defined(__IIF__NukeCtlTransform__) */
