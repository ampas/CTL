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
//	A convenience interface between OpenEXR files and the
//	Color Transformation Language (CTL) interpreter.
//
//-----------------------------------------------------------------------------

#include <ImfCtlApplyTransforms.h>
#include <ImfCtlCopyFunctionArg.h>
#include <ImfHeader.h>
#include <ImfFrameBuffer.h>
#include <CtlInterpreter.h>
#include <IlmThreadPool.h>
#include <Iex.h>
#include <mutex>

using namespace std;
using namespace Iex;
using namespace Imath;
using namespace IlmThread;
using namespace Imf;
using namespace Ctl;

#if 0
    #include <iostream>
    #define debug(x) (cout << x << endl)
#else
    #define debug(x)
#endif

#if 0
    #include <iostream>
    #define debug1(x) (cout << x << endl)
#else
    #define debug1(x)
#endif


namespace ImfCtl {
namespace {


typedef vector <FunctionCallPtr> FunctionList;


void
callFunctions
    (const FunctionList &funcs,
     const Box2i &transformWindow,
     size_t firstSample,
     size_t numSamples,
     const Header &envHeader,
     const Header &inHeader,
     const FrameBuffer &inFb,
     Header &outHeader,
     const FrameBuffer &outFb)
{
    debug ("callFunctions (..., "
	   "firstSample = " << firstSample << ", "
	   "numSamples = " << numSamples << ", ...)");

    //
    // For each function, provide input argument values,
    // call the function, and return output argument values
    // to the caller.
    //

    FunctionCallPtr previousFunc = 0;

    for (size_t i = 0; i < funcs.size(); ++i)
    {
	FunctionCallPtr func = funcs[i];
	debug ("\tfunction " << func->name());

	//
	// Provide input argument values.
	//

	for (size_t j = 0; j < func->numInputArgs(); ++j)
	{
	    FunctionArgPtr arg = func->inputArg (j);
	    debug ("\t\tinput arg " << arg->name());

	    if (arg->isVarying())
	    {
		//
		// Varying input argument -- try to find a value in
		// - the previous function's output arguments
		// - the slices in inFb
		// - the argument's default value
		//

		if (previousFunc)
		{
		    FunctionArgPtr previousArg =
			previousFunc->findOutputArg (arg->name());

		    if (!previousArg)
		    {
			previousArg =
			    previousFunc->findOutputArg (arg->name() + "Out");
		    }

		    if (previousArg)
		    {
			debug ("\t\t\tusing previous output arg");

			copyFunctionArg (numSamples, previousArg, arg);
			continue;
		    }
		}

		FrameBuffer::ConstIterator inSlice =
		    inFb.find (arg->name().c_str());

		if (inSlice != inFb.end())
		{
		    debug ("\t\t\tusing input frame buffer");

		    copyFunctionArg (transformWindow,
				     firstSample,
				     numSamples,
				     inSlice.slice(),
				     arg);
		    continue;
		}

		if (arg->hasDefaultValue())
		{
		    debug ("\t\t\tusing default value");

		    arg->setDefaultValue ();
		    continue;
		}

		THROW (ArgExc,
		       "Cannot find value for varying input "
		       "argument " << arg->name() << " of "
		       "CTL function " << func->name() << ".");
	    }
	    else
	    {
		//
		// Uniform input argument -- try to find a value in
		// - the previous function's output arguments
		// - the attributes in inHeader
		// - the attributes in envHeader
		// - the argument's default value
		//

		if (previousFunc)
		{
		    FunctionArgPtr previousArg =
			previousFunc->findOutputArg (arg->name());

		    if (!previousArg)
		    {
			previousArg =
			    previousFunc->findOutputArg (arg->name() + "Out");
		    }

		    if (previousArg)
		    {
			debug ("\t\t\tusing previous output arg");

			copyFunctionArg (1, previousArg, arg);
			continue;
		    }
		}

		Header::ConstIterator inAttr =
		    inHeader.find (arg->name().c_str());

		if (inAttr != inHeader.end())
		{
		    debug ("\t\t\tusing input header");

		    copyFunctionArg (inAttr.attribute(), arg);
		    continue;
		}

		Header::ConstIterator envAttr =
		    envHeader.find (arg->name().c_str());

		if (envAttr != envHeader.end())
		{
		    debug ("\t\t\tusing environment header");

		    copyFunctionArg (envAttr.attribute(), arg);
		    continue;
		}

		if (arg->hasDefaultValue())
		{
		    debug ("\t\t\tusing default value");

		    arg->setDefaultValue ();
		    continue;
		}

		THROW (ArgExc,
		       "Cannot find value for uniform input "
		       "argument " << arg->name() << " of "
		       "CTL function " << func->name() << ".");
	    }
	}

	//
	// Call the function
	//

	func->callFunction (numSamples);

	//
	// Return output argument values to the caller.
	//

	for (size_t j = 0; j < func->numOutputArgs(); ++j)
	{
	    FunctionArgPtr arg = func->outputArg (j);
	    debug ("\t\toutput arg " << arg->name());

	    if (arg->isVarying())
	    {
		//
		// Varying output value
		//
		
		FrameBuffer::ConstIterator outSlice =
		    outFb.find (arg->name().c_str());

		if (outSlice == outFb.end() &&
		    arg->name().size() > 3 &&
		    arg->name().substr (arg->name().size() - 3) == "Out")
		{
		    outSlice = outFb.find
			(arg->name().substr(0, arg->name().size() - 3).c_str());
		}

		if (outSlice != outFb.end())
		{
		    debug ("\t\t\tcopying to output frame buffer\n");

		    copyFunctionArg (transformWindow,
				     firstSample,
				     numSamples,
				     arg,
				     outSlice.slice());
		    continue;
		}

	    }
	    else
	    {
		//
		// Uniform output value
		//

		Header::Iterator outAttr =
		    outHeader.find (arg->name().c_str());

		if (outAttr == outHeader.end() &&
		    arg->name().size() > 3 &&
		    arg->name().substr (arg->name().size() - 3) == "Out")
		{
		    outAttr = outHeader.find
			(arg->name().substr(0, arg->name().size() - 3).c_str());
		}

		if (outAttr != outHeader.end())
		{
		    debug ("\t\t\tcopying to output header\n");

		    copyFunctionArg (arg, outAttr.attribute());
		    continue;
		}
	    }
	}

	previousFunc = func;
    }
}


class CallFunctionsTask: public Task
{
  public:

    CallFunctionsTask
	(TaskGroup *group,
	 Interpreter &interpreter,
	 const StringList &transformNames,
	 const Box2i &transformWindow,
	 size_t taskSamplesBegin,
	 size_t taskSamplesEnd,
	 const Header &envHeader,
	 const Header &inHeader,
	 const FrameBuffer &inFb,
	 Header &outHeader,
	 const FrameBuffer &outFb,
	 std::mutex &exceptionMutex,
	 string &exceptionWhat);

    virtual void	execute();

  private:

    Interpreter &	_interpreter;
    const StringList &	_transformNames;
    const Box2i &	_transformWindow;
    size_t		_taskSamplesBegin;
    size_t		_taskSamplesEnd;
    const Header &	_envHeader;
    const Header &	_inHeader;
    const FrameBuffer &	_inFb;
    Header &		_outHeader;
    const FrameBuffer &	_outFb;
    std::mutex &		_exceptionMutex;
    string &		_exceptionWhat;
};


CallFunctionsTask::CallFunctionsTask
    (TaskGroup *group,
     Interpreter &interpreter,
     const StringList &transformNames,
     const Box2i &transformWindow,
     size_t taskSamplesBegin,
     size_t taskSamplesEnd,
     const Header &envHeader,
     const Header &inHeader,
     const FrameBuffer &inFb,
     Header &outHeader,
     const FrameBuffer &outFb,
     std::mutex &exceptionMutex,
     string &exceptionWhat)
:
    Task (group),
    _interpreter (interpreter),
    _transformNames (transformNames),
    _transformWindow (transformWindow),
    _taskSamplesBegin (taskSamplesBegin),
    _taskSamplesEnd (taskSamplesEnd),
    _envHeader (envHeader),
    _inHeader (inHeader),
    _inFb (inFb),
    _outHeader (outHeader),
    _outFb (outFb),
    _exceptionMutex (exceptionMutex),
    _exceptionWhat (exceptionWhat)
{
    // empty
}


void
CallFunctionsTask::execute()
{
    debug1 ("CallFunctionsTask::execute()");

    try
    {
	//
	// Get function call objects for all transform functions
	// that we want to call.
	//

	FunctionList funcs;

	for (size_t i = 0; i < _transformNames.size(); ++i)
	    funcs.push_back (_interpreter.newFunctionCall (_transformNames[i]));

	//
	// Repeatedly call the transform functions, breaking the
	// varying data into packets of at most maxSamples samples.
	//

	size_t maxSamples = _interpreter.maxSamples();
	size_t begin = _taskSamplesBegin;
	size_t end = _taskSamplesEnd;

	debug1 ("\tbegin = " << begin << ", end = " << end);
	
	while (begin < end)
	{
	    size_t numSamples = min (end - begin, maxSamples);

	    callFunctions (funcs, _transformWindow,
			   begin, numSamples,
			   _envHeader, _inHeader, _inFb,
			   _outHeader, _outFb);

	    begin += numSamples;
	}
    }
    catch (const std::exception &exc)
    {
	std::lock_guard<std::mutex> lock(_exceptionMutex);
	_exceptionWhat = exc.what();
    }
    catch (...)
    {
	std::lock_guard<std::mutex> lock(_exceptionMutex);
	_exceptionWhat = "unrecognized exception";
    }
}

} // namespace


void
applyTransforms
    (Interpreter &interpreter,
     const StringList &transformNames,
     const Box2i &transformWindow,
     const Header &envHeader,
     const Header &inHeader,
     const FrameBuffer &inFb,
     Header &outHeader,
     const FrameBuffer &outFb,
     int numThreads)
{
    //
    // Load the CTL modules that we expect to contain the 
    // functions we want to call.
    //

    for (size_t i = 0; i < transformNames.size(); ++i)
	interpreter.loadModule (transformNames[i]);

    //
    // Determine how many samples we will process.
    //

    size_t totalSamples = (static_cast<size_t>(transformWindow.max.x) - static_cast<size_t>(transformWindow.min.x + 1)) *
			  (static_cast<size_t>(transformWindow.max.y) - static_cast<size_t>(transformWindow.min.y + 1));

    if (totalSamples <= 0)
	return;

    //
    // Create tasks to be processed by the thread pool.
    // The pixels in the transformWindow are are split into
    // chunks of approximately equal size.  Each task will
    // process one of of those chunks, breaking it down
    // further into packets that are small enough for the
    // interpreter.
    //
    // If a task catches an exception, it locks the exceptionMutex,
    // below, stores the exception's what() string in exceptionWhat,
    // and releases the mutex.
    //

    std::mutex exceptionMutex;
    string exceptionWhat;

    {
	TaskGroup taskGroup;

	numThreads = max (numThreads, 1);

	for (int i = 0; i < numThreads; ++i)
	{
	    size_t taskSamplesBegin = totalSamples * i / numThreads;
	    size_t taskSamplesEnd = totalSamples * (i + 1) / numThreads;
	    
	    ThreadPool::addGlobalTask
		(new CallFunctionsTask (&taskGroup,
					interpreter,
		                        transformNames,
					transformWindow,
					taskSamplesBegin,
					taskSamplesEnd,
					envHeader,
					inHeader,
					inFb,
					outHeader,
					outFb,
					exceptionMutex,
					exceptionWhat));
	}

	//
	// finish all tasks
	//
    }

    //
    // If any of the tasks encountered an exception, re-throw
    // the exception so that it can be handled by the caller
    // of applyTransforms().
    //

    if (exceptionWhat.size() > 0)
	throw Iex::LogicExc (exceptionWhat);
}

} // namespace ImfCtl
