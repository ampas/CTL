///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2009 Academy of Motion Picture Arts and Sciences
// ("A.M.P.A.S."). Portions contributed by others as indicated.
// All rights reserved.
// 
// A world-wide, royalty-free, non-exclusive right to distribute, copy,
// modify, create derivatives, and use, in source and binary forms, is
// hereby granted, subject to acceptance of this license. Performance of
// any of the aforementioned acts indicates acceptance to be bound by the
// following terms and conditions:
// 
//   * Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the Disclaimer of Warranty.
// 
//   * Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the Disclaimer of Warranty
//     in the documentation and/or other materials provided with the
//     distribution.
// 
//   * Nothing in this license shall be deemed to grant any rights to
//     trademarks, copyrights, patents, trade secrets or any other
//     intellectual property of A.M.P.A.S. or any contributors, except
//     as expressly stated herein, and neither the name of A.M.P.A.S.
//     nor of any other contributors to this software, may be used to
//     endorse or promote products derived from this software without
//     specific prior written permission of A.M.P.A.S. or contributor,
//     as appropriate.
// 
// This license shall be governed by the laws of the State of California,
// and subject to the jurisdiction of the courts therein.
// 
// Disclaimer of Warranty: THIS SOFTWARE IS PROVIDED BY A.M.P.A.S. AND
// CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
// BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT ARE DISCLAIMED. IN NO
// EVENT SHALL A.M.P.A.S., ANY CONTRIBUTORS OR DISTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
// IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
// IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////

#include "NukeTransform.h"

using namespace DD::Image;

void set_ctl_function_arguments_from_ctl_results(Ctl::FunctionArgPtr *arg, const CTLResults &ctl_results, size_t offset, size_t count);
void set_ctl_results_from_ctl_function_arguments(CTLResults *ctl_results, const Ctl::FunctionArgPtr &arg, size_t offset, size_t count, size_t total);

// Performs Ctl transformation on Nuke scanline
void NukeTransform(const Row& in, int y, int x, int r, ChannelMask channels, Row& out, const char* filename, std::vector<std::string> paramName, std::vector<std::vector<float> > paramValues, std::vector<int> paramSize) {

	int numChan = channels.size();
	int i = 0, j;
	size_t width = r - x;
	half image_buffer[numChan*width];
	int fExists = strcasecmp("", filename);
	
	// if file hasn't been selected copy input channels to output
	if (fExists) {
		
		// format input data to image buffer
	    foreach(n, channels) {
    
    		const float* inptr = in[n] + x;
    		float* outptr = out.writable(n) + x;

    		for (j = 0; j < width; j++) {   
    			      		        	
          		image_buffer[i+numChan*j] = *inptr++;
        	}
        	i++;
    	}  	
    } else {	
    	
    	// copy input channels to output and return
    	foreach(n, channels) {
    
    		const float* inptr = in[n] + x;
    		float* outptr = out.writable(n) + x;

    		for (j = 0; j < width; j++) {   
    	      		
          		*outptr++ = *inptr++;
        	}
        	i++;
    	} 
    	return; 	
    }	
	
    CTLOperations ctl_operations;
    CTLParameters global_ctl_parameters;
    ctl_operation_t new_ctl_operation;
    CTLOperations::const_iterator operations_iter;
    CTLResults ctl_results;
    	
    // can add ability to have multiple ctl operations here
    new_ctl_operation.local.clear();
    new_ctl_operation.filename = filename;

    ctl_operations.push_back(new_ctl_operation);
    
    // push channels into image buffer
    
    if (numChan > 0) {
    	ctl_results.push_back(mkresult("rIn", "c00In", image_buffer, 0, width));
    }
    if (numChan > 1) {
    	ctl_results.push_back(mkresult("gIn", "c01In", image_buffer, 1, width));
    }
    if (numChan > 2) {
    	ctl_results.push_back(mkresult("bIn", "c02In", image_buffer, 2, width));
    }
    if (numChan > 3) {
    	ctl_results.push_back(mkresult("aIn", "c03In", image_buffer, 3, width));
    }
    
    // if more than RGBA channels
    char name[16];
    for (i = 4; i < numChan; i++) {
    	memset(name, 0, sizeof(name));
    	snprintf(name, sizeof(name) - 1, "c%02dIn", i);
    	ctl_results.push_back(mkresult(name, NULL, image_buffer, i, width));
    }
    	
    // run for each ctl transformation, this will be 1 unless plugin is modified to handle multiple files
    for (operations_iter = ctl_operations.begin(); operations_iter != ctl_operations.end(); operations_iter++) {
    	// add user parameters ctl results
    	for (i = 0; i < paramSize.size(); i++) {
    		add_parameter_value_to_ctl_results(&ctl_results, paramName[i], paramValues[i], paramSize[i]);
    	}
    	// performs ctl operation on data
     	run_ctl_transform(*operations_iter, &ctl_results, width);
    }
    
    // load results back into image buffer
    mkimage(image_buffer, ctl_results, width);
    
    i = 0;
    // format image buffer data to output channel
    foreach(n, channels) {
    
    	float* outptr = out.writable(n) + x;
    	int pos = 0;

    	for (j = 0; j < width; j++) {
          	*outptr++ = image_buffer[i+numChan*j];      		
        }
        i++;
    }
    
}

void run_ctl_transform(const ctl_operation_t &ctl_operation, CTLResults *ctl_results, size_t count) {
    Ctl::SimdInterpreter interpreter;
    Ctl::FunctionCallPtr fn;
    Ctl::FunctionArgPtr arg;
    char *name = NULL;
    char *module;
    char *slash;
    char *dot;
    CTLResults new_ctl_results;
    
    try {
    
    	// copy filename to name
    	int len = strlen(ctl_operation.filename) + 1;
    	name = (char*)alloca(len);
    	memset(name, 0, len);
    	strcpy(name, ctl_operation.filename);
    	
    	// set module name to filename without path
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
    		interpreter.loadFile(ctl_operation.filename);

			// enter at main
    		fn = interpreter.newFunctionCall(std::string("main"));
    		
    		// if main doesn't exist use filename as entrance
    		if (fn.refcount() == 0) {	
    			fn = interpreter.newFunctionCall(std::string(module));
    		}
    	}
    	catch (...) {
    		THROW(Iex::ArgExc, "Problem loading CTL module into interpreter");
    	}
    	
    	// must return void
    	if (fn->returnValue()->type().cast<Ctl::VoidType>().refcount() == 0) {
    		THROW(Iex::ArgExc, "CTL main (or <module_name>) function must return a 'void'");
    		return;
    	}
    	
    	size_t offset = 0;
    	bool found_aOut = false;
    	while (offset < count) {
    		size_t pass = interpreter.maxSamples();
    		size_t i;
    	
    		if (pass > (count - offset)) {
    			pass = count- offset;
    		}
    	
    		// set input arguments
    		for (i = 0; i < fn->numInputArgs(); i++) {
    			arg = fn->inputArg(i);
    			set_ctl_function_arguments_from_ctl_results(&arg, *ctl_results, offset, pass);
    		}
    	
    		// do work
    		fn->callFunction(pass);
    	
    		// get results from output arguments
    		for (i = 0; i < fn->numOutputArgs(); i++) {
    			arg = fn->outputArg(i);
    			if (arg->name() == "aOut") {
    				found_aOut = true;
    			}    		
    			set_ctl_results_from_ctl_function_arguments(&new_ctl_results, arg, offset, pass, count);
    		}
    		
    		// out alpha channel must exist
    		if (!found_aOut) {
       			THROW(Iex::InputExc, "Must specify output alpha channel aOut");
    			return; 		
    		}
    		
    		offset += pass;
 		}
 		*ctl_results = new_ctl_results;   
    }
    catch (const Iex::InputExc &e) {
    	throw e;
    }
    catch (const Iex::ArgExc &e) {
    	throw e;
    }
    catch (...) {
    	THROW(Iex::ArgExc, "Problem running transformation");
    	throw;
    }

}

// load data from buffer into ctl results for a given channel
CTLResultPtr mkresult(const char *name, const char *alt_name, const half *fb, size_t offset, size_t width) {
	CTLResultPtr new_result = CTLResultPtr(new CTLResult());
	
	new_result->data = Ctl::DataArgPtr(new Ctl::DataArg(name, Ctl::DataTypePtr(new Ctl::StdHalfType()), width));
	
	if (alt_name != NULL) {
		new_result->alt_name = alt_name;
	}
	
	new_result->data->set(fb + offset, sizeof(half) * 4, 0, width);
	
	return new_result;
}

// take data from ctl results and copy to image buffer
void mkimage(half *image_buffer, const CTLResults &ctl_results, size_t width) {
	enum have_channel_e {
		have_x = 0,
		have_y = 1,
		have_z = 2,
		have_r = 3,
		have_g = 4,
		have_b = 5,
		have_a = 6,
		have_xout = 8,
		have_yout = 9,
		have_zout = 10,
		have_rout = 11,
		have_gout = 12,
		have_bout = 13,
		have_aout = 14,
		mask_x = 1 << have_x,
		mask_y = 1 << have_y,
		mask_z = 1 << have_z,
		mask_r = 1 << have_r,
		mask_g = 1 << have_g,
		mask_b = 1 << have_b,
		mask_a = 1 << have_a,
		mask_xout = 1 << have_xout,
		mask_yout = 1 << have_yout,
		mask_zout = 1 << have_zout,
		mask_rout = 1 << have_rout,
		mask_gout = 1 << have_gout,
		mask_bout = 1 << have_bout,
		mask_aout = 1 << have_aout,
		have_none = 33,
	};
	
	CTLResults::const_iterator results_iter;
	CTLResultPtr channels[16];
	int channels_mask;
	have_channel_e channel;
	const char* channel_name;
	uint8_t on_channel;
	uint8_t channel_count;
	uint8_t c;
	CTLResultPtr ctl_result;
	
	int tests[] =
	{
			(51 << 24) | (mask_rout | mask_gout | mask_bout | mask_aout), (50 << 24)
			| (mask_rout | mask_gout | mask_bout), (158 << 24)
			| (mask_xout | mask_yout | mask_zout | mask_aout), (157 << 24)
			| (mask_xout | mask_yout | mask_zout), (159 << 24)
			| (mask_yout | mask_aout), (6 << 24) | (mask_yout), (162 << 24)
			| (mask_gout | mask_aout), (2 << 24) | (mask_gout), (161 << 24)
			| (mask_bout | mask_aout), (3 << 24) | (mask_bout), (160 << 24)
			| (mask_rout | mask_aout), (1 << 24) | (mask_rout), (4 << 24)
			| (mask_aout),

	        (51 << 24) | (mask_r | mask_g | mask_b | mask_a), (50 << 24)
			| (mask_r | mask_g | mask_b), (158 << 24)
			| (mask_x | mask_y | mask_z | mask_a), (159 << 24)
			| (mask_y | mask_a), (6 << 24) | (mask_y), (162 << 24)
			| (mask_g | mask_a), (2 << 24) | (mask_g), (161 << 24)
			| (mask_b | mask_a), (3 << 24) | (mask_b), (160 << 24)
			| (mask_r | mask_a), (1 << 24) | (mask_r), (4 << 24) | (mask_a),

	        0
	};
	
	channels_mask = 0;
	for (results_iter = ctl_results.begin(); results_iter != ctl_results.end(); results_iter++) {
		ctl_result = *results_iter;
		if (!ctl_result->data->isVarying() && ctl_result->data->elements() != width) {
			continue;
		}
		channel = have_none;
		channel_name = ctl_result->data->name().c_str();
		
		// use bitwise operations to find channel
		if (!strcasecmp("aOut", channel_name)) {
			channel = have_aout;
		} else if (!strcasecmp("rOut", channel_name)) {
		 	channel = have_rout;
		} else if (!strcasecmp("gOut", channel_name)) {
		 	channel = have_gout;
		} else if (!strcasecmp("bOut", channel_name)) {
		 	channel = have_bout;
		} 
		
		if (channel == have_none) {
			continue;
		}
		
		if (ctl_result->data->type().cast<Ctl::HalfType>().refcount() == 0 && ctl_result->data->type().cast<Ctl::FloatType>().refcount() == 0) {
			THROW(Iex::ArgExc, "CTL script not providing half or float as the output data type.");	
		}
		channels[channel] = ctl_result;
		channels_mask = channels_mask | (1 << channel);
	}		
	
	for (c = 0; tests[c] != 0; c++) {
		if ((channels_mask & tests[c] & 0x00ffffff) == (tests[c] & 0x00ffffff)) {
			channels_mask = tests[c];
			break;
		}
	}
	
	if (tests[c] == 0) {
		THROW(Iex::ArgExc, "Unable to determine what channels from the CTL script output should be saved.");
	}
	
	// counts the number of channels
	channel_count = 0;
	for (c = 0; c < 24; c++) {
		if (channels_mask & (1 << c)) {
			channel_count++;
		}
	}
	
	// copy data for found channel into image buffer
	on_channel = 0;
	for (c = 0;  c < 24; c++) {
		if (channels_mask & (1 << c)) {
			channels[c]->data->get(image_buffer + on_channel, sizeof(half) * channel_count, 0, width);
			on_channel++;
		}
	}
}

// set input arguments
void set_ctl_function_arguments_from_ctl_results(Ctl::FunctionArgPtr *arg, const CTLResults &ctl_results, size_t offset, size_t count) {
	CTLResults::const_iterator results_iter;
	Ctl::TypeStoragePtr src;
	Ctl::FunctionArgPtr dst;
	
	dst = *arg;
	
	// break when argument name found
	for (results_iter = ctl_results.begin(); results_iter != ctl_results.end(); results_iter++) {
		if ((*results_iter)->data->name() == dst->name() || (*results_iter)->alt_name == dst->name()) {
			break;
		}
	}
	
	// if name not found, must have a default value
	if (results_iter == ctl_results.end()) {
		if (dst->hasDefaultValue()) {
			dst->setDefaultValue();
		}
		else {
		    THROW(Iex::ArgExc, "CTL parameter '" << dst->name() << "' not specified and does not have a default value.");
			throw(std::exception());
		}
		return;
	}
	
	// copy data to argument
	src = (*results_iter)->data;
	if (!dst->isVarying()) {
		if (offset == 0) {
			dst->copy(src, 0, 0, 1);
		}
	}
	else {
		if (!src->isVarying()) {
			for (size_t i = 0; i < dst->elements(); i++) {
				dst->copy(src, 0, i, 1);
			}
		}
		else {
			dst->copy(src, offset, 0, count);
		}
	}
}

// set ctl results from output arguments 
void set_ctl_results_from_ctl_function_arguments(CTLResults *ctl_results, const Ctl::FunctionArgPtr &arg, size_t offset, size_t count, size_t total) {
	CTLResults::iterator results_iter;
	CTLResultPtr ctl_result;
	
	// break when output argument name found
	for (results_iter = ctl_results->begin(); results_iter != ctl_results->end(); results_iter++) {
		if ((*results_iter)->data->name() == arg->name()) {
			break;
		}
	}
	
	if (!arg->isVarying()) {
		if (offset != 0) {
			return;
		}
		total = 1;
		count = 1;
	}
	
	
	if (results_iter != ctl_results->end()) {
		ctl_result = *results_iter;
		
		std::string inputName;
		if (arg->name() == "rOut") {
			inputName = "rIn";
		}
		else if (arg->name() == "gOut") {
			inputName = "gIn";
		}
		else if (arg->name() == "bOut") {
			inputName = "bIn";
		}
		else if (arg->name() == "aOut") {
			inputName = "aIn";
		}
		
		if (!inputName.empty()) {
			for (results_iter = ctl_results->begin(); results_iter != ctl_results->end(); results_iter++) {
				if ((*results_iter)->data->name() == inputName) {
					break;
				}
			}
			
			if (results_iter != ctl_results->end()) {
				CTLResultPtr ctl_results_output_to_input;
				ctl_results_output_to_input = *results_iter;
				ctl_results_output_to_input->data->copy(arg, 0, offset, count);
			}
		}
	}
	else {
		ctl_result = CTLResultPtr(new CTLResult());
		ctl_result->data = new Ctl::DataArg(arg->name(), arg->type(), total);
		ctl_results->push_back(ctl_result);
		
		std::string inputName;
		if (arg->name() == "rOut") {
			inputName = "rIn";
		}
		else if (arg->name() == "gOut") {
			inputName = "gIn";
		}
		else if (arg->name() == "bOut") {
			inputName = "bIn";
		}
		else if (arg->name() == "aOut") {
			inputName = "aIn";
		}
		
		if (!inputName.empty()) {
			CTLResultPtr ctl_result_input;
				
			ctl_result_input = CTLResultPtr(new CTLResult());
			ctl_result_input->data = new Ctl::DataArg(inputName, arg->type(), total);
			ctl_results->push_back(ctl_result_input);
			ctl_result_input->data->copy(arg, 0, offset, count);
		}
	}
	
	// copy data from arguments
	ctl_result->data->copy(arg, 0, offset, count);

}

//
void add_parameter_value_to_ctl_results(CTLResults *ctl_results, std::string paramName, std::vector<float> paramValues, int arSize) {

	CTLResults::iterator results_iter;
	CTLResultPtr ctl_result;
	Ctl::DataTypePtr type;
	size_t i;
	
	// Break when parameter found
	for (results_iter = ctl_results->begin(); results_iter != ctl_results->end(); results_iter++) {
		if ((*results_iter)->data->name() == paramName.c_str()) {
			break;
		}
	}
	
	if (results_iter != ctl_results->end()) {
		ctl_result = *results_iter;

		if (!ctl_result->external)	{
			return;
		}	
	}
	else {
		// create new CTL result for user parameter
		ctl_result = CTLResultPtr(new CTLResult);
		ctl_result->external = true;
	}
	
	// If there is a single parameter value then we set the data type to a float
	// and copy the parameter value to the data portion of the ctl result
	if (arSize == 1) {	
		ctl_result->data = new Ctl::DataArg(paramName.c_str(), new Ctl::StdFloatType(), 1);
		ctl_result->data->set(&(paramValues[0]));
	}
	else {
		// If there are multiple parameter values we set the type as a float array and
		// copy and parameter value to the array
		type = new Ctl::StdArrayType(new Ctl::StdFloatType(), arSize);
		ctl_result->data = new Ctl::DataArg(paramName.c_str(), type, 1);
		for (i = 0; i < arSize; i++) {
			ctl_result->data->set(&(paramValues[i]), 0, 0, 1, "%d", i);
		}
	}
	
	ctl_results->push_back(ctl_result);
	
}




