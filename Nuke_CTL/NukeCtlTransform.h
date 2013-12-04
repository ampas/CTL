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

#include <vector>
#include <string>

#include "NukeCtlChanArgMap.h"


namespace NukeCtl
{
  class TransformFriend;
  
  // Keeping this in the heap makes it possible to share it between the Transform
  // and TransformFriend (which gets used in unit testing).
  class RcSimdInterpreter : public Ctl::SimdInterpreter, public Ctl::RcObject
  {
  };
  
  typedef Ctl::RcPtr<RcSimdInterpreter> SimdInterpreterPtr;
  
  class Transform {
    friend class TransformFriend;
  public:
    Transform(const std::string &modulePath,
              const std::string &transformPath);

    Transform(const Transform& transform);
    
    Transform&
    operator=(const Transform& rhs);
    
    void
    loadArgMap();
    
    void
    execute(const DD::Image::Row& in, int l, int r, DD::Image::Row& out);
  private:
    static
    const std::vector<std::string>
    parseModulePath(const std::string &modulePath);
    
    static
    void
    verifyModuleName(const std::string &moduleName);
    
    static
    Ctl::FunctionCallPtr
    topLevelFunctionInTransform(SimdInterpreterPtr interpreter, const std::string &transformPath);
    
    void
    checkTopLevelFunctionReturnsVoid();
   
    std::vector<std::string>  modulePathComponents_;
    SimdInterpreterPtr        interpreter_;
    Ctl::FunctionCallPtr      functionCall_;
    NukeCtl::ChanArgMap       argMap_;

    // Forensics
    std::string               transformPath_;
    
  };
}

#endif /* defined(__IIF__NukeCtlTransform__) */
