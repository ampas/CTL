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
    Transform(const std::string &modulePath,
              const std::string &transformPath);

    Transform(const Transform& t);
    
    void
    loadArgMap(const DD::Image::Row& row);
    
    void
    execute(const DD::Image::Row& row, int x, int r, DD::Image::Row& out);
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
    
    void
    checkTopLevelFunctionReturnsVoid();
   
    char*
    inputArgBuffer(Ctl::FunctionCallPtr fn, const std::string& argName) const;
    
    char*
    outputArgBuffer(Ctl::FunctionCallPtr fn, const std::string& argName) const;
    
    const std::vector<std::string>  modulePathComponents_;
    Ctl::SimdInterpreter            interpreter_;
    Ctl::FunctionCallPtr            functionCall_;
    NukeCtl::ChanArgMap             argMap_;

    std::string                     transformPath_; // Forensics
    
  };
}

#endif /* defined(__IIF__NukeCtlTransform__) */
