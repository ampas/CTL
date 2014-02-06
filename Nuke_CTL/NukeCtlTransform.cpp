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
  
  FunctionCallPtr
  Transform::topLevelFunctionInTransform()
  {
    FunctionCallPtr functionCall;
    try {
      functionCall = interpreter_->newFunctionCall("main");
    } catch (const ArgExc &e) {
      // There is no CTL exception specific to this problem, so we use secret knowledge (i.e. we peek at the source)
      // to see exactly what the CTL interpreter would do if the module cannot be found. And what it does is throw
      // ArgExc with the what() string having the form "Cannot find CTL function <foo>."
      if (matchesCTLCannotFindFunctionExceptionText(e, "main"))
      {
        string moduleName(modulenameFromFilename(filenameFromPathname(transformPath_)));
        try {
          functionCall = interpreter_->newFunctionCall(moduleName);
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
    return functionCall;
  }
  
  Transform::Transform(const string &modulePath,
                       const string &transformPath)
  : modulePathComponents_(parseModulePath(modulePath)),
    interpreter_(RcPtr<RcSimdInterpreter>(new RcSimdInterpreter)),
    transformPath_(transformPath)
  {
    // be diligent about not having bad parameters or state crash all of Nuke.
    // TODO: Transform ctor should have separate 'wrapper' try/catch blocks around its various steps.
    interpreter_->setUserModulePath(modulePathComponents_, modulePathComponents_.size() > 0);
    interpreter_->loadFile(transformPath);
  }
  
  Transform::Transform(const Transform &transform)
  : modulePathComponents_(transform.modulePathComponents_),
    interpreter_(transform.interpreter_),
    transformPath_(transform.transformPath_)
  {
  }
  
  Transform&
  Transform::operator=(const Transform &rhs)
  {
    if (this != &rhs)
    {
      modulePathComponents_ = rhs.modulePathComponents_;
      interpreter_          = rhs.interpreter_;
      transformPath_        = rhs.transformPath_;
    }
    return *this;
  }

  void
  Transform::execute(const Row &in, int l, int r, Row &out)
  {
    FunctionCallPtr functionCall = topLevelFunctionInTransform();
    if (functionCall->returnValue()->type().cast<Ctl::VoidType>().refcount() == 0)
    {
      THROW(ArgExc, "top-level function of CTL file at '" << transformPath_
            << "' returns other than void type");
    }
    ChanArgMap argMap(functionCall);
    for (int x = l; x < r;)
    {
      int chunkSize = min(r - x, static_cast<int>(interpreter_->maxSamples()));
      argMap.copyInputRowToArgData(in, x, x + chunkSize);
      functionCall->callFunction(chunkSize);
      argMap.copyArgDataToOutputRow(x, x + chunkSize, out);
      x += chunkSize;
    }
  }
  
}
