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

#ifndef INCLUDED_IMF_CTL_APPLY_TRANSFORMS_H
#define INCLUDED_IMF_CTL_APPLY_TRANSFORMS_H

//-----------------------------------------------------------------------------
//
//	A convenience interface between OpenEXR files and the
//	Color Transformation Language (CTL) interpreter.
//
//	Function applyTransforms() applies a series of CTL functions to
//	the pixels in an OpenEXR frame buffer, and places the results in
//	another OpenEXR frame buffer.
//
//	applyTransforms() has the following parameters:
//
//	    interpreter		The instance of the CTL interpreter that will
//	    			execute the color transformation functions.
//
//	    transformNames	A list of function names.  The corresponding
//	    			CTL functions will be called in the order in
//				which their names appear in the list.
//
//	    transformWindow	The region in the input and output frame
//				buffers that contains the pixel data that
//				will be read and written by the CTL functions.
//
//	    envHeader		An OpenEXR header that contains information
//	    			about "the environment", for example, display
//				primary chromaticities and white point.
//
//	    inHeader		An OpenEXR header that describes the pixels
//	    			in the input frame buffer.  This is typically
//				the header of the image file from where the
//				pixels originated.
//
//	    inFb		The input frame buffer; contains the pixels
//	    			that are to be processed.
//
//	    outHeader		An OpenEXR header that holds the values of
//	    			non-varying output parameters of the CTL
//				functions (see below).
//
//	    outFb		The output frame buffer; holds the pixels
//	    			output by the CTL functions.
//
//	    numThreads		Number of threads that will be used to
//				execute the CTL functions (see below).
//
//	applyTransforms() first loads the CTL modules that contain the
//	functions listed in transformNames.  Each function is assumed to
//	live in a module with the same name as the function.
//
//	applyTransforms() then calls each of the CTL functions listed in
//	transformNames.  The values for the function's input parameters come
//	from the output arguments of the previous function, from envHeader,
//      from inHeader or from inFb.  The values of some of the function's
//	output parameters are stored in outHeader or outFb.
//
//	CTL function parameters:
//
//	Before a CTL function is called, it must be supplied with values for
//	its input parameters.  For each parameter, with name n, the value is
//	found as follows:
//
//	    if n is varying
//
//		if the previous function has an output parameter with name n
//		    use previous function's output parameter
//
//		else if the previous function has an output parameter
//              whose name is n concatenated with "Out"
//		    use previous function's output parameter
//
//		else if inFb contains a slice with name n
//		    use inFb slice
//
//		else if input parameter n has a default value
//		    use default value
//
//		else
//		    error (throw Iex::ArgExc)
//
//	    else (n is uniform)
//
//		if the previous function has an output parameter with name n
//		    use previous function's output parameter
//
//		else if the previous function has an output parameter
//              whose name is n concatenated with "Out"
//		    use previous function's output parameter
//
//		else if inHeader contains an attribute with name n
//		    use inHeader attribute
//
//		else if envHeader contains an attribute with name n
//		    use envHeader attribute
//
//		else if input parameter n has a default value
//		    use default value
//
//		else
//		    error (throw Iex::ArgExc)
//
//	In all cases, the type of the value used must match the type of the
//	input parameter.  A type mismatch is an error; applyTransforms()
//	throws an Iex::TypeExc.
//
//	After the CTL function returns, the value of each output parameters,
//	with name n, may be copied into outHeader or outFb, in addition to
//	potentially being used as an input to the next CTL function:
//
//	    if n is varying
//
//		if outFb contains a slice with name n
//		    copy the value into the outFb slice
//
//		else if n ends in "Out" and outFb contains a slice
//		whose name is n, except without the trailing "Out"
//		    copy the value into the outFb slice
//
//	    else (n is uniform)
//     
//		if outHeader contains an attribute with name n
//		    copy the value into the outHeader attribute
//     
//		else if n ends in "Out" and outHeader contains an attribute
//		whose name is n, except without the trailing "Out"
//		    copy the value into the outHeader attribute
//
//	The type of the output parameter must match the type of the frame
//	buffer slice or header attribute.  A type mismatch is an error;
//	applyTransforms() throws an Iex::TypeExc.
//
//	applyTransforms() does not add attributes to outHeader or slices
//	to the outFb.  Only existing attributes or slices are used.
//
//	Multi-Threading:
//
//	applyTransforms() uses threads to execute multiple instances of
//	the CTL functions in parallel.  applyTransforms() does this by
//	submitting tasks to the same thread pool that the IlmImf library
//	uses for OpenEXR file I/O.
//
//	The numThreads argument to applyTransforms() lets application
//	code specify how many threads should be kept busy running CTL
//	code.  The default behavior is to try to occupy all threads in
//	the thread pool.
//
//-----------------------------------------------------------------------------

#include <string>
#include <vector>
#include <ImathBox.h>
#include <ImfThreading.h>
#include <ImfHeader.h>
#include <ImfFrameBuffer.h>

namespace Ctl
{
    class Interpreter;
}

namespace ImfCtl
{
    typedef std::vector <std::string> StringList;

    void
    applyTransforms
	(Ctl::Interpreter &interpreter,
	 const StringList &transformNames,
	 const Imath::Box2i &transformWindow,
	 const Imf::Header &envHeader,
	 const Imf::Header &inHeader,
	 const Imf::FrameBuffer &inFb,
	 Imf::Header &outHeader,
	 const Imf::FrameBuffer &outFb,
	 int numThreads = Imf::globalThreadCount());
}

#endif
