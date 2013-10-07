///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 Academy of Motion Picture Arts and Sciences
// ("A.M.P.A.S."). Portions contributed by others as indicated.
// All rights reserved.
//
// A worldwide, royalty-free, non-exclusive right to copy, modify, create
// derivatives, and use, in source and binary forms, is hereby granted,
// subject to acceptance of this license. Performance of any of the
// aforementioned acts indicates acceptance to be bound by the following
// terms and conditions:
//
//  * Copies of source code, in whole or in part, must retain the
//    above copyright notice, this list of conditions and the
//    Disclaimer of Warranty.
//
//  * Use in binary form must retain the above copyright notice,
//    this list of conditions and the Disclaimer of Warranty in the
//    documentation and/or other materials provided with the distribution.
//
//  * Nothing in this license shall be deemed to grant any rights to
//    trademarks, copyrights, patents, trade secrets or any other
//    intellectual property of A.M.P.A.S. or any contributors, except
//    as expressly stated herein.
//
//  * Neither the name "A.M.P.A.S." nor the name of any other
//    contributors to this software may be used to endorse or promote
//    products derivative of or based on this software without express
//    prior written permission of A.M.P.A.S. or the contributors, as
//    appropriate.
//
// This license shall be construed pursuant to the laws of the State of
// California, and any disputes related thereto shall be subject to the
// jurisdiction of the courts therein.
//
// Disclaimer of Warranty: THIS SOFTWARE IS PROVIDED BY A.M.P.A.S. AND
// CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
// BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT ARE DISCLAIMED. IN NO
// EVENT SHALL A.M.P.A.S., OR ANY CONTRIBUTORS OR DISTRIBUTORS, BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, RESITUTIONARY,
// OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//
// WITHOUT LIMITING THE GENERALITY OF THE FOREGOING, THE ACADEMY
// SPECIFICALLY DISCLAIMS ANY REPRESENTATIONS OR WARRANTIES WHATSOEVER
// RELATED TO PATENT OR OTHER INTELLECTUAL PROPERTY RIGHTS IN THE ACADEMY
// COLOR ENCODING SYSTEM, OR APPLICATIONS THEREOF, HELD BY PARTIES OTHER
// THAN A.M.P.A.S., WHETHER DISCLOSED OR UNDISCLOSED.
///////////////////////////////////////////////////////////////////////////

#include "NukeInputParameters.h"

// Checks if a string is not a keyword
// Used to check is an input parameter is
// specified by the user
bool isKeyword(std::string key) {
  
	if (key == "rIn")
		return true;
	if (key == "gIn")
		return true;
	if (key == "bIn")
		return true;
	if (key == "aIn")
		return true;
  
	if (key == "rOut")
		return true;
	if (key == "gOut")
		return true;
	if (key == "bOut")
		return true;
	if (key == "aOut")
		return true;
  
	return false;
}

// Checks for user parameters and returns a string with a the parameters and a default value of 0
// This is used as a way for users to add and modify custom input parameters
std::string inputParameters(const char* filename, std::vector<std::string> *paramName, std::vector<std::vector<float> > *paramValues, std::vector<int> *paramSize) {
  Ctl::SimdInterpreter interpreter;
  Ctl::FunctionCallPtr fn;
  Ctl::FunctionArgPtr arg;
  std::string paramStr = "";
  char *name = NULL;
  char *module;
  char *slash;
  char *dot;
  size_t i, j;
  size_t arSize;
	std::vector<float> tempValues;
  
  try {
    
    // Set module as the filename without the path or extension
    // This will be used as the entrance function in case main is not present
    // CTL allows either "main" or a function named at the filename as
    // the starting function
    int len = strlen(filename) + 1;
    name = (char*)alloca(len);
    memset(name, 0, len);
    strcpy(name, filename);
    
    // get filename without path
    slash = strrchr(name, '/');
    if (slash == NULL) {
      module = name;
    }
    else {
      module = slash + 1;
    }
    
    // remove extension from module
    dot = strrchr(module, '.');
    if (dot != NULL) {
      *dot = 0;
    }
    
    try {
      interpreter.loadFile(filename);
      // Try calling main
      try {
        fn = interpreter.newFunctionCall(std::string("main"));
      }
      catch (const Iex::ArgExc &e) {
        return "";
      }
      
      // If main doesn't exists, call the module name
      if (fn.refcount() == 0) {
        fn = interpreter.newFunctionCall(std::string(module));
      }
    }
    catch (...) {
      return "";
    }
    if (fn->returnValue()->type().cast<Ctl::VoidType>().refcount() == 0) {
      return "";
    }
    
    // Loop through input arguments. If an argument is found without a default
    // argument, then add it to the parameter string. We exclude arguments with
    // default arguments as they will not be modifiable to the user.
    // Zero is set at the default value.
    paramName->clear();
    paramValues->clear();
    paramSize->clear();
    
    // for each argument get a user parameter and its array size
    for (i = 0; i < fn->numInputArgs(); i++) {
      arg = fn->inputArg(i);
      // filter out default values and keywords
      if (!isKeyword(arg->name()) && !arg->hasDefaultValue()) {
        paramName->push_back(arg->name());
        tempValues.clear();
        // If there is an array
        if (arg->type().cast<Ctl::ArrayType>()) {
          arSize = arg->type().cast<Ctl::ArrayType>()->size();
          for (j = 0; j < arSize; j++) {
            tempValues.push_back(0);
          }
          
          paramSize->push_back(arSize);
        }
        else {
          tempValues.push_back(0);
          paramSize->push_back(1);
        }
        paramValues->push_back(tempValues);
      }
    }
    // for each parameter output its name and a 0 for each element
    for (i = 0; i < paramName->size(); i++) {
      paramStr += ((*paramName)[i] + " =");
      // for arrays
      for (j = 0; j < (*paramSize)[i]; j++) {
        paramStr += " 0";
      }
      paramStr += "\n";
    }
  }
  catch (...) {
    return "";
  }
	
  return paramStr;
}
