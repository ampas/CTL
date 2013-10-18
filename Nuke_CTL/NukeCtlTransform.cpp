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
    } catch (const exception &e) {
      // This is grotesque but there is no CTL exception specific to this problem, so...
      const string functionNotFoundMarker("Cannot find CTL function");
      if (string(e.what()).find_first_of(functionNotFoundMarker) != string::npos)
      {
        string moduleName(modulenameFromFilename(transformPath));
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
  
  Transform::Transform(Row& row,
                       const string &modulePath,
                       const string &transformPath)
  : modulePathComponents_(parseModulePath(modulePath))
  {
    interpreter_.setUserModulePath(modulePathComponents_, modulePathComponents_.size() > 0);
    interpreter_.loadFile(transformPath);
    functionCall_ = topLevelFunctionInTransform(interpreter_, transformPath);
    // build input arg map
    // build output arg map
  }
  
  void
  Transform::execute()
  {
    // If transform path is blank or bad, just copy the data and return
    // Otherwise:
    //   copy the row input data into the function arg data area as per the input arg map
    //   run the interpreter
    //   copy the function arg data into the row output data as per the output arg map
  }
  
}
