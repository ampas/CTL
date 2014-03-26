//
//  NukeCtlTransform.cpp
//  IIF
//
//  Created by Joseph Goldstone on 10/14/13.
//  Copyright (c) 2013 Joseph Goldstone. All rights reserved.
//

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif

#include "NukeCtlTransform.h"
#include "NukeCtlUtils.h"

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include "Iex.h"

#include <algorithm>

using namespace DD::Image;
using namespace NukeCtl;
using namespace Iex;
using namespace Ctl;
using namespace std;

namespace NukeCtl
{
  const vector<string>
  Transform::parseModulePath(const string &modulePath)
  {
    vector<string> modulePathComponents;
    try {
      modulePathComponents = split(modulePath,':');
    } catch (const ArgExc &e) {
      THROW(ArgExc, "Cannot parse module path '" << modulePath << "': " << e.what());
    }
    return modulePathComponents;
  }
  
  void
  Transform::verifyModuleName(const string &moduleName)
  {
    // components of path need not exist, they just need to be syntactically legal. At least, that's as far as Ctl::Interpreter goes.
    // path should be legal, as far as the interpreter allows; that means no /, :. ; or \ characters.
    if (moduleName.find_first_of("/:;\\") != string::npos)
    {
      THROW(ArgExc, "Module path component `" << moduleName << "' contains invalid characters (one of /, :, ; or \\");
    }
  }
  
  bool
  Transform::matchesCTLCannotFindFunctionExceptionText(const exception& e, const string &functionName)
  {
    ostringstream s;
    s << "Cannot find CTL function " << functionName << ".";
    string pattern(s.str());
    return pattern == e.what();
  }
  
  bool
  Transform::matchesCTLCannotFindModuleExceptionText(const exception& e)
  {
    string exceptionText(e.what());
    string::size_type i = exceptionText.find_first_of("Cannot find CTL module \"");
    string::size_type j = exceptionText.find_last_of("\"");
    return j != string::npos && i < j;
  }
  
  const string
  Transform::missingModuleFromException(const exception& e)
  {
    if (! matchesCTLCannotFindModuleExceptionText(e))
    {
      THROW(LogicExc, "Attempt to extract missing module name from an exception not concerned with missing modules");
    }
    string exceptionText(e.what());
    string::size_type i = exceptionText.find_first_not_of("Cannot find CTL module \"");
    string::size_type j = exceptionText.find("\".");
    return exceptionText.substr(i, j-i);
  }
  
  const string
  Transform::topLevelFunctionNameInTransform()
  {
    FunctionCallPtr functionCall;
    try {
      functionCall = interpreter_->newFunctionCall("main");
    }
    catch (const ArgExc &e)
    {
      // There is no CTL exception specific to this problem, so we use secret knowledge (i.e. we peek at the source)
      // to see exactly what the CTL interpreter would do if the module cannot be found. And what it does is throw
      // ArgExc with the what() string having the form "Cannot find CTL function <foo>."
      if (matchesCTLCannotFindFunctionExceptionText(e, "main"))
      {
        string moduleName(modulenameFromFilename(filenameFromPathname(transformPath_)));
        try {
          functionCall = interpreter_->newFunctionCall(moduleName);
          return moduleName;
        } catch (const exception &e) {
          if (matchesCTLCannotFindFunctionExceptionText(e, moduleName))
          {
            THROW(ArgExc, "CTL file at '" << transformPath_ << "' has neither a main function nor one named '" << moduleName << "'");
          } else if (matchesCTLCannotFindModuleExceptionText(e))
          {
            string missingModule(missingModuleFromException(e));
            THROW(ArgExc, "Module '" << missingModule << "' not in the module path; referenced by " << moduleName << " function in CTL file '" << transformPath_ << "'");
          }
          else
          {
            THROW(ArgExc, "Error searching for function 'main' and function '" << moduleName << "' in CTL file '" << transformPath_ << "': " << e.what());
          }
        }
      }
      else if (matchesCTLCannotFindModuleExceptionText(e))
      {
        string missingModule(missingModuleFromException(e));
        THROW(ArgExc, "Module '" << missingModule << "' not in the module path; referenced by main function in CTL file '" << transformPath_ << "'");
      }
      else
      {
        THROW(ArgExc, "Error searching for function 'main' in CTL file '" << transformPath_ << "': " << e.what());
      }
    }
    return "main";
  }

  
  Transform::Transform(const string &modulePath,
                       const string &transformPath)
  : modulePathComponents_(parseModulePath(modulePath)),
    interpreter_(RcPtr<RcSimdInterpreter>(new RcSimdInterpreter)),
    transformPath_(transformPath)
  {
    // be diligent about not having bad parameters or state crash all of Nuke.
    try {
      interpreter_->setUserModulePath(modulePathComponents_, modulePathComponents_.size() > 0);
    }
    catch (const BaseExc& e)
    {
      THROW(ArgExc, "error setting CTL module path `" << modulePath << "': " << e.what());
    }
    try {
      interpreter_->loadFile(transformPath_);
      try {
        functionName_ = topLevelFunctionNameInTransform();
      }
      catch (const BaseExc& e)
      {
        THROW(ArgExc, "error finding top-level function name in transform at path 1" << transformPath_ << "': " << e.what());
      }
    } catch (const BaseExc& e) {
      THROW(ArgExc, "error loading CTL transform from path `" << transformPath_ << "': " << e.what());
    }
  }
  
  void
  Transform::execute(const Row &in, int l, int r, Row &out)
  {
    // Although it doubtless looks tempting to create the function call and argument map
    // just once, at transform ctor time, and avoid the expense on each call to this
    // execute() function...you can't. As per page 17 of the CTL manual (24/07/2007 edition)
    // function call objects are not thread-safe. Interpreters (or at least the reference
    // SIMD interpreter) ARE thread-safe, so it's cool to stash an interpreter as CtlTransform
    // member variable and share it...but stay away from FunctionCallPtr member variables in
    // CtlTransform objects, and since they point into such objects, from ArgMap member
    // variables as well.
    
    try {
      FunctionCallPtr fn(interpreter_->newFunctionCall(functionName_));
      if (fn->returnValue()->type().cast<Ctl::VoidType>().refcount() == 0)
      {
        THROW(ArgExc, "top-level function of CTL file at '" << transformPath_
              << "' returns other than void type");
      }
      try {
        ChanArgMap argMap(fn);
        for (int x = l; x < r;)
        {
          int maxSamples = static_cast<int>(interpreter_->maxSamples());
          int chunkSize = min(r - x, maxSamples);
          argMap.copyInputRowToArgData(in, x, x + chunkSize);
          fn->callFunction(chunkSize);
          argMap.copyArgDataToOutputRow(x, x + chunkSize, out);
          x += chunkSize;
        }
      }
      catch (const BaseExc& e)
      {
        THROW(LogicExc, "could not construct argument map for CTL transform loaded from path `" << transformPath_ << ": " << e.what());
      }
    }
    catch (const BaseExc& e)
    {
      THROW(ArgExc, "error finding top-level function in CTL transform loaded from path `" << transformPath_ << ": " << e.what());
    }
  }
  
}
