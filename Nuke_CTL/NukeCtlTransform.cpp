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
  Transform::parseModulePath(const string &mp)
  {
    vector<string> modulePathComponents;
    try {
      modulePathComponents = split(mp,':');
    } catch (const ArgExc &e) {
      THROW(ArgExc, "Cannot parse module path '" << mp << "': " << e.what());
    }
    return modulePathComponents;
  }
  
  void
  Transform::verifyModuleName(const string &n)
  {
    // components of path need not exist, they just need to be syntactically legal. At least, that's as far as Ctl::Interpreter goes.
    // path should be legal, as far as the interpreter allows; that means no /, :. ; or \ characters.
    if (n.find_first_of("/:;\\") != string::npos)
    {
      THROW(ArgExc, "Module path component `" << n << "' contains invalid characters (one of /, :, ; or \\");
    }
  }
  
  FunctionCallPtr
  Transform::topLevelFunctionInTransform(SimdInterpreter& interpreter, const string &transformPath)
  {
    FunctionCallPtr functionCall;
    try {
      functionCall = interpreter.newFunctionCall("main");
    } catch (const ArgExc &e) {
      // This is grotesque but there is no CTL exception specific to this problem, so...
      const string functionNotFoundMarker("Cannot find CTL function");
      if (string(e.what()).find_first_of(functionNotFoundMarker) != string::npos)
      {
        string moduleName(modulenameFromFilename(filenameFromPathname(transformPath)));
        try {
          functionCall = interpreter.newFunctionCall(moduleName);
        } catch (const exception &e) {
          if (string(e.what()).find_first_of(functionNotFoundMarker) != string::npos)
          {
            THROW(ArgExc, "CTL file at '" << transformPath << "' has neither a main function nor one named '" << moduleName << "'");
          }
        }
      }
    }
    return functionCall;
  }
  
  void
  Transform::checkTopLevelFunctionReturnsVoid()
  {
    if (functionCall_->returnValue()->type().cast<Ctl::VoidType>().refcount() == 0)
    {
      THROW(ArgExc, "top-level function of CTL file at '" << transformPath_
            << "' returns other than void type");
    }
  }
  
  Transform::Transform(const string &modulePath,
                       const string &transformPath)
  : modulePathComponents_(parseModulePath(modulePath)),
    transformPath_(transformPath)
  {
    // be diligent about not having bad parameters or state crash all of Nuke.
    // TODO: Transform ctor should have separate 'wrapper' try/catch blocks around its various steps.
    interpreter_.setUserModulePath(modulePathComponents_, modulePathComponents_.size()  > 0);
    interpreter_.loadFile(transformPath);
    functionCall_ = topLevelFunctionInTransform(interpreter_, transformPath);
    checkTopLevelFunctionReturnsVoid();
  }
  
  Transform::Transform(const Transform& t)
  : modulePathComponents_(t.modulePathComponents_),
    transformPath_(t.transformPath_)
  {
  }
  
  void
  Transform::loadArgMap(const DD::Image::Row& in)
  {
    argMap_.load(in, functionCall_);
  }
  
  void
  Transform::execute(const Row& in, int l, int r, Row& out)
  {
    for (int x = l; x < r;)
    {
      int chunkSize = max(x - r, static_cast<int>(interpreter_.maxSamples()));
      argMap_.copyChanDataToArgData(in, x, x + chunkSize);
      functionCall_->callFunction(chunkSize);
      argMap_.copyArgDataToChanData(out, x, x + chunkSize);
    }
  }
  
}
