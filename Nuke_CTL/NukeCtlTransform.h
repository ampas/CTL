//
//  NukeCtlTransform.h
//  IIF
//
//  Created by Joseph Goldstone on 10/14/13.
//  Copyright (c) 2013 Joseph Goldstone. All rights reserved.
//

#ifndef __IIF__NukeCtlTransform__
#define __IIF__NukeCtlTransform__

#include <CtlRcPtr.h>
#include <CtlSimdInterpreter.h>
#include <CtlFunctionCall.h>

#include "NukeCtlChanArgMap.h"

#include <vector>
#include <string>

namespace NukeCtl
{
  class TransformFriend;
  
  class RcSimdInterpreter : public Ctl::SimdInterpreter, public Ctl::RcObject
  {
  };
  
  typedef Ctl::RcPtr<RcSimdInterpreter> SimdInterpreterPtr;
  
  class Transform {
    friend class TransformFriend;
  public:
    Transform(const std::string &modulePath,
              const std::string &transformPath);

    Transform(const Transform &transform);
    
    Transform&
    operator=(const Transform &rhs);
    
    void
    execute(const DD::Image::Row &in, int l, int r, DD::Image::Row &out);
  private:
    static
    const std::vector<std::string>
    parseModulePath(const std::string &modulePath);
    
    static
    void
    verifyModuleName(const std::string &moduleName);
    
    static
    bool
    matchesCTLCannotFindFunctionExceptionText(const std::exception& e, const std::string &functionName);
    
    static
    bool
    matchesCTLCannotFindModuleExceptionText(const std::exception& e);
    
    static
    const std::string
    missingModuleFromException(const std::exception& e);
    
    static
    Ctl::FunctionCallPtr
    topLevelFunctionInTransform(SimdInterpreterPtr i, const std::string& p);
    
    void
    loadArgMap();
    
    std::vector<std::string>  modulePathComponents_;
    
    // Keeping this in the heap makes it possible to share it between the Transform
    // and TransformFriend (which gets used in unit testing).
     SimdInterpreterPtr        interpreter_;
    
    // On the other hand, you do NOT want to try anything comparable for sharing
    // functional calls with Ctl::FunctionCallPtr. That sort of thing will work in
    // unit testing, which is single-threaded, but Nuke has multiple threads sharing
    // from a single Nuke Op.
    
    // transformPath_ is not used at runtime but is handy for forensics
    std::string               transformPath_;

  };
}

#endif /* defined(__IIF__NukeCtlTransform__) */
