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


//-----------------------------------------------------------------------------
//
//	class Interpreter
//
//-----------------------------------------------------------------------------

#include <CtlInterpreter.h>
#include <CtlModule.h>
#include <CtlModuleSet.h>
#include <CtlLContext.h>
#include <CtlSymbolTable.h>
#include <CtlParser.h>
#include <CtlExc.h>
#include <Iex.h>
#include <fstream>
#include <algorithm>
#include <cassert>
#include <string.h>
#include <memory>
#include <mutex>

#ifdef WIN32
    #include <io.h>
    #include <stdio.h>
    #include <stdlib.h>
#else
    #include <unistd.h>
#endif

using namespace std;
using namespace Iex;

#if 0
    #include <iostream>
    #define debug(x) (cout << x << endl)
#else
    #define debug(x)
#endif

namespace Ctl {
namespace {


struct ModulePathsData
{
    std::mutex           mutex;
    vector<string>  paths;
};


ModulePathsData&
modulePathsInternal()
{
    static ModulePathsData mpd;
    static bool firstTime = true;


    std::lock_guard<std::mutex> lock (mpd.mutex);
    // on the first attempt, load all paths from the environment variable
    if(firstTime)
    {
        firstTime = false;
        vector<string>& modPaths = mpd.paths;

        string path;

        const char *env = getenv ("CTL_MODULE_PATH");
        if (env)
            path = env;
        
        if (path == "")
			#if defined (WIN32) || defined (WIN64)
			path = "."; // default windows install location?
			#else
            path = ".:/usr/local/lib/CTL:/usr/local/" PACKAGE
			       "-" VERSION "/lib/CTL";
			#endif

        size_t pos = 0;
        while (pos < path.size())
        {
	    #if defined (WIN32) || defined (WIN64)

		size_t end = path.find (';', pos);

	    #else

		size_t end = path.find (':', pos);

	    #endif
            
            if (end == string::npos)
                end = path.size();
            
            string pathItem = path.substr (pos, end - pos);

            if(find(modPaths.begin(), modPaths.end(), pathItem ) 
               == modPaths.end())
                modPaths.push_back(pathItem);

            pos = end + 1;
        }
    }
    return mpd;
}

} // namespace


struct Interpreter::Data
{
    SymbolTable		symtab;
    ModuleSet		moduleSet;
    std::mutex		mutex;
};


Interpreter::Interpreter (): _data (new Data)
{
    set_module_path = false;
}


Interpreter::~Interpreter ()
{
    delete _data;
}


SymbolTable &
Interpreter::symtab ()
{
    return _data->symtab;
}


vector<string>
Interpreter::modulePaths()
{
    vector<string> retPaths;
    {
        ModulePathsData &mpd = modulePathsInternal();
        std::lock_guard<std::mutex> lock(mpd.mutex);
        retPaths = mpd.paths;
    }
    return retPaths;
}

string
Interpreter::findModule (const string& moduleName)
{
    //
    // Find the file that contains the source code for a given module
    // by searching a colon-separated list of directories provided by
    // the CTL_MODULE_PATH environment variable.
    //
    // Note: for security reasons, we do not want to give CTL modules
    // any information about the host machine's file system layout.
    // Therefore we disallow any characters in module names that could
    // be used to build absolute or relative file paths.
    //

    if (moduleName.find_first_of ("/:;\\") != string::npos)
    {
	THROW (ArgExc, "CTL module name \"" << moduleName << "\" is invalid. "
		       "Module names cannot contain '/', ':', ';' or '\\' "
		       "characters.");
    }


    {
        ModulePathsData &mpd = modulePathsInternal();
        std::lock_guard<std::mutex> lock(mpd.mutex);
        
        // Users can define a separate module path besides the environment variable
        // per instance on ctl running.
        
        if (set_module_path) 
        {
            for(vector<string>::iterator it = user_module_paths.begin();
            it != user_module_paths.end();
            it++)
            {
                string fileName = *it + '/' + moduleName + ".ctl";
            
                #ifdef WIN32            
                if (!_access (fileName.c_str(), 0)) {
#else
                if (!access (fileName.c_str(), F_OK)) {
#endif
                    return fileName;
                }
            }
            
            THROW (ArgExc, "Cannot find CTL module \"" << moduleName << "\".");
            return ""; // return to prevent a warning with some compilers.
        }
        

        for(vector<string>::iterator it = mpd.paths.begin();
            it != mpd.paths.end();
            it++)
        {
            string fileName = *it  + '/' + moduleName + ".ctl";

#ifdef WIN32            
            if (!_access (fileName.c_str(), 0))
#else
            if (!access (fileName.c_str(), F_OK))
#endif
                return fileName;
        }
    }

    THROW (ArgExc, "Cannot find CTL module \"" << moduleName << "\".");
    return ""; // return to prevent a warning with some compilers.
}

void
Interpreter::setModulePaths(const vector<string>& newModPaths)
{

    ModulePathsData &mpd = modulePathsInternal();
    std::lock_guard<std::mutex> lock(mpd.mutex);
    mpd.paths = newModPaths;
}

void
Interpreter::setUserModulePath(const vector<string> path, const bool set)
{
    set_module_path = set;
    user_module_paths = path;
}

void
Interpreter::loadModule (const string &moduleName, const string &fileName, const string &moduleSource)
{
    debug ("Interpreter::loadModule (moduleName = " << moduleName << ")");

    std::lock_guard<std::mutex> lock(_data->mutex);
    loadModuleRecursive (moduleName, fileName, moduleSource);
}

void Interpreter::_loadModule(const std::string &moduleName,
                              const std::string &fileName,
                              const std::string &moduleSource) {                              
    // 
    // set up the source code string for parsing.
    // 

    std::unique_ptr<istream> input;
        
    if (!moduleSource.empty())
    {
        debug ("\tloading from source");
        
        stringstream *tmp_strm = new stringstream;
        (*tmp_strm) << moduleSource;
        input.reset( tmp_strm );
    }
    else
    {
        //
        // Using the module search path, locate the file that contains the
        // source code for the module.  Open the file.
        //
        ifstream *tmp_strm = new ifstream;
        tmp_strm->open(fileName.c_str());
        
        if (!(*tmp_strm))
        {
	        THROW_ERRNO ("Cannot load CTL module \"" << moduleName << "\". "
		         "Opening file \"" << fileName << "\" for reading "
		         "failed (%T).");
        }

        debug ("\tloading from file \"" << fileName << "\"");
        input.reset( tmp_strm );
    }
    
    assert(input.get());

    Module *module = 0;
    LContext *lcontext = 0;

    try
    {
	//
	// Create a Module, an Lcontext and a Parser
	//

	module = newModule (moduleName, fileName);	
	_data->moduleSet.addModule (module);
	lcontext = newLContext (*input, module, _data->symtab);
	Parser parser (*lcontext, *this);

	//
	// Parse the source code and generate executable code
	// for the module
	//

	debug ("\tparsing input");
	SyntaxNodePtr syntaxTree = parser.parseInput ();

	if (syntaxTree && lcontext->numErrors() == 0)
	{
	    debug ("\tgenerating code");
	    syntaxTree->generateCode (*lcontext);
	}

	if (lcontext->numErrors() > 0)
	{
	    lcontext->printDeclaredErrors();
	    THROW (LoadModuleExc,
		   "Failed to load CTL module \"" << moduleName << "\".");
	}

	//
	// Run the module's initialization code
	//

	debug ("\trunning module initialization code");
	module->runInitCode();

	//
	// Cleanup: the LContext and the module's local symbols
	// are no longer needed, but we keep the global symbols.
	//

	debug ("\tcleanup");
	delete lcontext;
	_data->symtab.deleteAllLocalSymbols (module);
    }
    catch (...)
    {
	//
	// Something went wrong while loading the module, clean up
	//

	if(lcontext) delete lcontext;
	_data->symtab.deleteAllSymbols (module);
	_data->moduleSet.removeModule (moduleName);
	throw;
    }
}

void Interpreter::loadFile(const std::string &fileName,
                           const std::string &_moduleName) {
    std::lock_guard<std::mutex> lock(_data->mutex);
	std::string moduleName;
	char random[32];

	if(_moduleName.size()==0) {
		// This might have unintended consequences...
		memset(random, 0, sizeof(random));
		long r = static_cast<long>(rand()) << (sizeof(int)*8) | rand();
		snprintf(random, sizeof(random)-1, "module.%08x",
		         (unsigned int)(time(NULL)+r));
		moduleName=random;
	} else {
		moduleName=_moduleName;
	}	

    _loadModule(moduleName, fileName);
}

void
Interpreter::loadModuleRecursive (const string &moduleName, const string &fileName, const string &moduleSource)
{
    debug ("Interpreter::loadModuleRecursive "
	   "(moduleName = " << moduleName << ")");

    if (moduleIsLoadedInternal (moduleName))
    {
	debug ("\talready loaded");
	return;
    }
    
	string realFileName;
	if(fileName.empty() && !moduleName.empty())
		realFileName = findModule (moduleName);
	else
		realFileName = fileName;


	_loadModule(moduleName, realFileName, moduleSource);
}

bool
Interpreter::moduleIsLoaded (const std::string &moduleName) const
{
    std::lock_guard<std::mutex> lock(_data->mutex);
    return moduleIsLoadedInternal (moduleName);
}


bool
Interpreter::moduleIsLoadedInternal (const std::string &moduleName) const
{
    return _data->moduleSet.containsModule (moduleName);
}



FunctionCallPtr
Interpreter::newFunctionCall (const std::string &functionName)
{
    std::lock_guard<std::mutex> lock(_data->mutex);
    
    //
    // Calling a CTL function with variable-size array arguments
    // from C++ is not supported.
    //

    const SymbolInfoPtr info = symtab().lookupSymbol (functionName);

    if (!info)
	THROW (ArgExc, "Cannot find CTL function " << functionName << ".");

    if (!info->isFunction())
	THROW (TypeExc, "CTL object " << functionName << " is not a function "
			"(it is of type " << info->type()->asString() << ").");

    const FunctionTypePtr fType = info->type();
    const ParamVector &parameters = fType->parameters();

    for (int i = parameters.size() - 1; i >= 0; --i)
    {
	const Param &param = parameters[i];
        ArrayTypePtr aType = param.type.cast<ArrayType>();

        if(aType)
        {
            SizeVector sizes;
            aType->sizes (sizes);
	    
            for (unsigned int j = 0; j < sizes.size(); j++)
            {
                if (sizes[j] == 0)
                    THROW (ArgExc, "CTL function " << functionName << " "
				   "has a variable-size array "
				   "argument, " << param.name << ", and can "
				   "only be called by another CTL function.");
            }
        }
    }
    
    return newFunctionCallInternal (info, functionName);
}

} // namespace Ctl
