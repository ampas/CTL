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

#include "adx_file.hh"

#include <adx.hh>
#include <fstream>
#include <Iex.h>

template <class T>
ctl::dpx::fb<T>* prepareADX(const char *inputFile,
                            float input_scale,
                            ctl::dpx::fb<T> *image_buffer,
                            format_t *image_format)
{
    // Looking for compatibility then print WARNING if necessary
    std::map<const char*, uint32_t> MI = dpx_read_details_for_adx_int(inputFile, input_scale,
                                                                    image_buffer, image_format);
    std::map<const char*, float32_t> MF = dpx_read_details_for_adx_float(inputFile, input_scale,
                                                                       image_buffer, image_format);
    
    unsigned long nan_f = static_cast<unsigned long>(ctl::adx::udf32f);
    float32_t nan_vf = *(float32_t *)&nan_f;
    
    if (MI["18"] > 2 ) {
        fprintf(stderr,
                "WARNING: This Image May Contain More than 2 Elments"
                "Elements Beyond the 2nd One Will be Removed! \n");
    }
    
    if (MI["18"] >= 1){
        if (MI["21.6"] != 50
            && MI["21.6"] != 51
            && MI["21.6"] != 52
            && MI["21.6"] != 53) {
            THROW(Iex::ArgExc, "The Program does not Currently Support This Format.\n");
        }
        else if(MI["21.6"] == 157){
            fprintf(stderr,
                    "The Image is Currently in XYZ Space\n"
                    ", ctlrender will Treat and Process  \n"
                    "it as it was in RGB Space.\n");
            
        }
        image_buffer->channelAdjust(uint8_t(MI["21.6"]));
    }
    
    if (MI["21.1"] != 0x0 && MI["21.1"]) {
        fprintf(stderr,
                "WARNING: adx 21.1 Data Sign shall be Unsigned\n");
    }
    if (MI["22.1"] != 0x0 && MI["22.1"]) {
        fprintf(stderr,
                "WARNING: adx 22.1 Data Sign shall be Unsigned\n");
    }
    
    if (MI["21.2"] != 0x0 && MI["21.2"]) {
        fprintf(stderr,
                "WARNING: adx 21.2 Reference Low Data code shall be 0\n");
    }
    if (MI["22.2"] != 0x0 && MI["22.2"]) {
        fprintf(stderr,
                "WARNING: adx 22.2 Reference Low Data code shall be 0\n");
    }
    
    if (MF["21.3"] != nan_vf
        && MF["21.3"]) {
        fprintf(stderr,
                "WARNING: adx 21.3 Reference Low Quality shall be Undefined\n");
    }
    if (MF["22.3"] != nan_vf
        && MF["22.3"]) {
        fprintf(stderr,
                "WARNING: adx 22.3 Reference Low Quality shall be Undefined\n");
    }
    
    if(MI["21.9"] != 16 && MI["21.9"] != 10) {
        THROW(Iex::ArgExc, "ERROR: The Program is Currently Supporting ADX with Bit Depth \n"
                "         of 10 and 16 on the First Element) \n");
    }
    
    if(MI["18"] > 1
       && MI["22.6"] != 4) {
        THROW(Iex::ArgExc, "ERROR: The Program is Currently Supporting Alpha/Matte \n"
                "         on the Second Element");
    }
    else if(MI["18"] > 1
            && MI["22.6"] == 4
            && MI["22.9"] != 16
            && MI["22.9"] != 10
            && MI["22.9"] != 8
            && MI["22.9"] != 1){
        THROW(Iex::ArgExc, "ERROR: The Program is Currently Supporting ADX with Bit Depth of \n"
                "         1, 8, 10 and 16 on the Second Element) \n");
    }
    
    if (MI["21.4"]
        && MI["21.4"] != 65535
        && MI["21.9"] == 16) {
        fprintf(stderr,
                "WARNING: adx 21.4 Reference high data for ADX16 shall be 65535\n");
    }
    if (MI["21.4"]
        && MI["21.4"] != 1023
        && MI["21.9"] == 10) {
        fprintf(stderr,
                "WARNING: adx 21.4 Reference high data for ADX10 shall be 1023\n");
    }
    
    if (MI["18"] == 2) {
        if( MI["22.9"] == 16
            && MI["22.4"] != 65535) {
           fprintf(stderr,
                   "WARNING: adx 22.4 Reference high data for ADX16 shall be 65535\n");
        }
        if ( MI["22.9"] == 10
             && MI["22.4"] != 1023) {
            fprintf(stderr,
                    "WARNING: adx 22.4 Reference high data for ADX10 shall be 1023\n");
        }
        if ( MI["22.9"] == 8
             && MI["22.4"] != 255) {
            fprintf(stderr,
                    "WARNING: adx 22.4 Reference high data for ADX8 shall be 255\n");
        }
        if ( MI["22.9"] == 1
             && MI["22.4"] != 1) {
            fprintf(stderr,
                    "WARNING: adx 22.4 Reference high data for ADX1 shall be 1\n");
        }
    }
    
    if (MF["21.5"] != nan_vf
        && MF["21.5"]) {
        fprintf(stderr,
                "WARNING: adx 21.5 Reference high quality shall be Undefined\n");
    }
    if (MF["22.5"] != nan_vf
        && MF["22.5"]) {
        fprintf(stderr,
                "WARNING: adx 22.5 Reference high quality shall be Undefined\n");
    }
    
    if (MI["21.7"]  != 13 && MI["21.7"]) {
        fprintf(stderr,
                "WARNING: adx 21.7 Transfer characteristic shall be ADX\n");
    }
    if (MI["22.7"]  != 0 && MI["22.7"]) {
        fprintf(stderr,
                "WARNING: adx 22.2 Transfer characteristic shall be User-defined\n");
    }
    
    if (MI["21.8"]  != 13 && MI["21.8"]) {
        fprintf(stderr,
                "WARNING: adx 21.8 Colorimetric characteristic shall be ADX\n");
    }
    if (MI["22.8"]  != 0 && MI["22.8"]) {
        fprintf(stderr,
                "WARNING: adx 22.2 Colorimetric characteristic shall be User-defined\n");
    }
    
    if (MI["21.10"]
        && MI["21.10"] != 0x0
        && MI["21.9"] == 16) {
        fprintf(stderr,
                "WARNING: adx 21.10 Packing for ADX16 shall be 0\n");
    }
    else if (MI["21.10"]
             && MI["21.10"] != 0x1
             && MI["21.9"] == 10) {
        fprintf(stderr,
                "WARNING: adx 21.10 Packing for ADX10 shall be 1\n");
    }
    
    if (MI["18"] == 2
        && MI["22.10"] != 0x0
        && (MI["22.9"] == 16
            || MI["22.9"] == 8
            || MI["22.9"] == 1)) {
        fprintf(stderr,
                "WARNING: adx 22.10 Packing for ADX16/8/1 shall be 0\n");
    }
    else if (MI["18"] == 2
             && MF["22.10"] != 0x1
             && MF["22.9"] == 10) {
        fprintf(stderr,
                "WARNING: adx 22.10 Packing for ADX10 shall be 1\n");
    }
    
    fprintf(stderr,
            "\n");
    
    return image_buffer;
}

template ctl::dpx::fb<float>* prepareADX<float>(const char *inputFile,
                                                float input_scale,
                                                ctl::dpx::fb<float> *image_buffer,
                                                format_t *image_format);

bool adx_read(const char *name, float scale, ctl::dpx::fb<float> *pixels,
              format_t *format) {
	std::ifstream file;
	ctl::adx adxheader;

	file.open(name);

	if(!ctl::adx::check_magic(&file)) {
		return 0;
	}
	
	adxheader.read(&file);
	adxheader.read(&file, 0, pixels, scale);

	format->src_bps=adxheader.elements[0].bits_per_sample;
    pixels->swizzle(adxheader.elements[0].descriptor, FALSE);

	return 1;
}

void adx_write(const char *name, float scale, const ctl::dpx::fb<float> &pixels,
               format_t *format) {
	std::ofstream file;
	ctl::adx adxheader;

	file.open(name);

	adxheader.elements[0].data_sign=0;
	adxheader.elements[0].bits_per_sample=format->bps;
    
	adxheader.write(&file, 0, pixels, scale);
	adxheader.write(&file);
}

