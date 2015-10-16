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

#include "dpx_file.hh"

#include <dpx.hh>
#include <fstream>

bool dpx_read(const char *name, float scale, ctl::dpx::fb<float> *pixels,
              format_t *format) {
	std::ifstream file;
	ctl::dpx dpxheader;

	file.open(name);

	if(!ctl::dpx::check_magic(&file)) {
		return 0;
	}
	
	dpxheader.read(&file);	
	dpxheader.read(&file, 0, pixels, scale);

	format->src_bps=dpxheader.elements[0].bits_per_sample;
	pixels->swizzle(dpxheader.elements[0].descriptor, FALSE);

	return 1;
}

std::map<const char *, uint32_t> dpx_read_details_for_adx_int(const char *name,
                                                              float scale,
                                                              ctl::dpx::fb<float> *pixels,
                                                              format_t *format) {
	std::ifstream file;
	ctl::dpx dpxheader;
	std::map<const char *, uint32_t> M;

	file.open(name);
	
	dpxheader.read(&file);	
	dpxheader.read(&file, 0, pixels, scale);

	M["18"] = static_cast<uint32_t>(dpxheader.number_of_elements);
	M["21.1"] = static_cast<uint32_t>(dpxheader.elements[0].data_sign);
	M["21.2"] = static_cast<uint32_t>(dpxheader.elements[0].ref_low_data_code);
	M["21.4"] = static_cast<uint32_t>(dpxheader.elements[0].ref_high_data_code);
	M["21.6"] = static_cast<uint32_t>(dpxheader.elements[0].descriptor);
	M["21.7"] = static_cast<uint32_t>(dpxheader.elements[0].transfer_characteristic);
	M["21.8"] = static_cast<uint32_t>(dpxheader.elements[0].colorimetric_characteristic);
	M["21.9"] = static_cast<uint32_t>(dpxheader.elements[0].bits_per_sample);
	M["21.10"] = static_cast<uint32_t>(dpxheader.elements[0].packing);

	if (static_cast<uint32_t>(dpxheader.number_of_elements) > 1){
		M["22.1"] = static_cast<uint32_t>(dpxheader.elements[1].data_sign);
		M["21.2"] = static_cast<uint32_t>(dpxheader.elements[1].ref_low_data_code);
		M["21.4"] = static_cast<uint32_t>(dpxheader.elements[1].ref_high_data_code);
		M["22.6"] = static_cast<uint32_t>(dpxheader.elements[1].descriptor);
		M["22.7"] = static_cast<uint32_t>(dpxheader.elements[1].transfer_characteristic);
		M["22.8"] = static_cast<uint32_t>(dpxheader.elements[1].colorimetric_characteristic);
		M["22.9"] = static_cast<uint32_t>(dpxheader.elements[1].bits_per_sample);
		M["22.10"] = static_cast<uint32_t>(dpxheader.elements[1].packing);
	}

	M["60"] = static_cast<uint32_t>(dpxheader.interlace);
	M["61"] = static_cast<uint32_t>(dpxheader.field_number);
	M["62"] = static_cast<uint32_t>(dpxheader.video_standard);

	return M;
}

std::map<const char *, float32_t> dpx_read_details_for_adx_float(const char *name,
                                                                 float scale,
                                                                 ctl::dpx::fb<float> *pixels,
                                                                 format_t *format) {
	std::ifstream file;
	ctl::dpx dpxheader;
	std::map<const char *, float32_t> M;

	file.open(name);
	
	dpxheader.read(&file);	
	dpxheader.read(&file, 0, pixels, scale);

	M["21.3"] = static_cast<float32_t>(dpxheader.elements[0].ref_low_quantity);
	M["21.5"] = static_cast<float32_t>(dpxheader.elements[0].ref_high_quantity);

	if (static_cast<uint32_t>(dpxheader.number_of_elements) > 1){
		M["22.3"] = static_cast<float32_t>(dpxheader.elements[1].ref_low_quantity);
		M["22.5"] = static_cast<float32_t>(dpxheader.elements[1].ref_high_quantity);
	}

	M["64"] = static_cast<float32_t>(dpxheader.horizontal_sampling_rate);
	M["65"] = static_cast<float32_t>(dpxheader.vertical_sampling_rate);
	M["66"] = static_cast<float32_t>(dpxheader.temporal_sampling_rate);
	M["67"] = static_cast<float32_t>(dpxheader.time_offset_sync_to_first_pixel);
	M["68"] = static_cast<float32_t>(dpxheader.gamma);
	M["69"] = static_cast<float32_t>(dpxheader.black_level_code);
	M["70"] = static_cast<float32_t>(dpxheader.black_gain);
	M["71"] = static_cast<float32_t>(dpxheader.breakpoint);
	M["72"] = static_cast<float32_t>(dpxheader.white_level_code);
	M["73"] = static_cast<float32_t>(dpxheader.integration_time);

	return M;
}

void dpx_write(const char *name, float scale, const ctl::dpx::fb<float> &pixels,
               format_t *format) {
	std::ofstream file;
	ctl::dpx dpxheader;

	file.open(name);

	dpxheader.elements[0].data_sign=0;
	dpxheader.elements[0].bits_per_sample=format->bps;

	// std::cout << " I am here dpx" << std::endl;
	// std::cout << format->bps << std::endl;
	// std::cout << dpxheader.elements[0].bits_per_sample << std::endl;

	dpxheader.write(&file, 0, pixels, scale);
	dpxheader.write(&file);	
}

