//
//  NukeCtlTransform.cpp
//  IIF
//
//  Created by Joseph Goldstone on 10/14/13.
//  Copyright (c) 2013 Joseph Goldstone. All rights reserved.
//

#include "NukeCtlTransform.h"
#include "NukeCtlUtils.h"

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
  
  FunctionCallPtr
  Transform::topLevelFunctionInTransform()
  {
    FunctionCallPtr functionCall;
    try {
      functionCall = interpreter_->newFunctionCall("main");
    } catch (const ArgExc &e) {
      // This is grotesque but there is no CTL exception specific to this problem, so...
      const string functionNotFoundMarker("Cannot find CTL function");
      if (string(e.what()).find_first_of(functionNotFoundMarker) != string::npos)
      {
        string moduleName(modulenameFromFilename(filenameFromPathname(transformPath_)));
        try {
          functionCall = interpreter_->newFunctionCall(moduleName);
        } catch (const exception &e) {
          if (string(e.what()).find_first_of(functionNotFoundMarker) != string::npos)
          {
            THROW(ArgExc, "CTL file at '" << transformPath_ << "' has neither a main function nor one named '" << moduleName << "'");
          }
        }
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
    interpreter_->setUserModulePath(modulePathComponents_, modulePathComponents_.size()  > 0);
    interpreter_->loadFile(transformPath);
  }
  
  Transform::Transform(const Transform& transform)
  : modulePathComponents_(transform.modulePathComponents_),
    interpreter_(transform.interpreter_),
    transformPath_(transform.transformPath_)
  {
  }
  
  Transform&
  Transform::operator=(const Transform& rhs)
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
  Transform::execute(const Row& in, int l, int r, Row& out)
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
