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

#if !defined(CTLRENDER_TRANSFORM_INCLUDE)
#define CTLRENDER_TRANSFORM_INCLUDE

#include <list>

#include "NukeInputParameters.h"
#include "DDImage/Row.h"
#include "DDImage/PixelIop.h"
#include <CtlRcPtr.h>
#include <CtlFunctionCall.h>
#include <CtlSimdInterpreter.h>
#include <CtlStdType.h>
#include <exception>
#include <Iex.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

using namespace DD::Image;
// structure to capture a CTL parameter.
// A parameter consists of a name and up to 4 floating point values.
// The count member holds the number of floating point values for this parameter

struct ctl_parameter_t
{
	ctl_parameter_t()
	{
		name = 0;
		count = 0;
		memset(value, 0, sizeof(value));
	}

	const char *name;
	uint8_t count;
	float value[4];
};

typedef std::list<ctl_parameter_t> CTLParameters;

// structure to capture the CTL filename plus parameters for the CTL
struct ctl_operation_t
{
	ctl_operation_t()
	{
		filename = 0;
	}

	const char *filename;
	CTLParameters local;
};

typedef std::list<ctl_operation_t> CTLOperations;

/*********************************************************/
/*                                                       */
/*                Class CTLResult                        */
/*                                                       */
/*********************************************************/

class CTLResult: public Ctl::RcObject {

public:
	CTLResult(): Ctl::RcObject() {
		external = false;
	}
	virtual ~CTLResult() {
	}
	
	Ctl::TypeStoragePtr data;
	bool external;
	std::string alt_name;
};

typedef Ctl::RcPtr<CTLResult> CTLResultPtr;
typedef std::list<CTLResultPtr> CTLResults;

void NukeTransform(const Row&, int, int, int, ChannelMask, Row&, const char*, std::vector<std::string>, std::vector<std::vector<float> >, std::vector<int>);
CTLResultPtr mkresult(const char*, const char*, const half*, size_t, size_t);
void mkimage(half*, const CTLResults&, size_t);
void run_ctl_transform(const ctl_operation_t&, CTLResults*, size_t);
void set_ctl_function_arguments_from_ctl_results(Ctl::FunctionArgPtr*, const CTLResults&, size_t, size_t);
void set_ctl_results_from_ctl_function_arguments(CTLResults*, const Ctl::FunctionArgPtr&, size_t, size_t, size_t);
void add_parameter_value_to_ctl_results(CTLResults*, std::string, std::vector<float>, int);

#endif
