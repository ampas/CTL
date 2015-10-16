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

#include <adx.hh>
#include "adx_util.hh"

namespace ctl {

std::string adx::descriptor_to_string(uint8_t id) {
	if(id==0) {
		return std::string("user defined single channel (0)");
	} else if(id==5) {
		return std::string("not specified");
	} else if(id>=10 && id<=49) {
		return adxi::strprintf("reserved single component (%u)", id);
	} else if(id>=53 && id<=99) {
		return adxi::strprintf("reserved RGB format (%u)", id);
	} else if(id>=104 && id<=149) {
		return adxi::strprintf("reserved cbYcr format (%u)", id);
	} else if(id>=157) {
		return adxi::strprintf("reserved future format (%u)", id);
	}

	switch(id) {
		case 1:
			return std::string("red (R)");

		case 2:
			return std::string("green (G)");
						
		case 3:
			return std::string("blue (B)");
			
		case 4:
			return std::string("alpha (matte)");

		case 6:
			return std::string("luma (Y)");

		case 7:
			return std::string("color difference (Cb, Cr)");

		case 8:
			return std::string("depth (Z)");

		case 9:
			return std::string("composite video");

		case 50:
			return std::string("RGB (note 2)");
			
		case 51:
			return std::string("RGBA (note 2)");

		case 52:
			return std::string("ABGR (note 3)");
	
		case 100:
			return std::string("CbYCrY (4:2:2) SMPTE 125M");

		case 101:
			return std::string("CbYACrYA (4:2:2:4)");

		case 102:
			return std::string("CbYCr (4:4:4)");

		case 103:
			return std::string("CbYCrA (4:4:4:4)");

		case 150:
		case 151:
		case 152:
		case 153:
		case 154:
		case 155:
		case 156:
			return adxi::strprintf("user defined %u component element", id-150+2);

		case 255:
			return std::string("unset");

		default:
			break;
	}

	return adxi::strprintf("ARRRGH something outside of case (%u)", id);
}

std::string adx::orientation_to_string(uint8_t id) {
	switch(id) {
		case 0:
			return std::string("left to right, top to bottom");

		case 1:
			return std::string("right to left, top to bottom");

		case 2:
			return std::string("left to right, bottom to top");

		case 3:
			return std::string("right to left, bottom to top");

		case 4:
			return std::string("top to bottom, left to right");

		case 5:
			return std::string("top to bottom, right to left");

		case 6:
			return std::string("bottom to top, left to right");

		case 7:
			return std::string("bottom to top, right to left");

		case 255:
			return std::string("unset");

		default:
			break;
	}
	
	return adxi::strprintf("reserved for future use (%u)", id);
}

std::string adx::bits_per_sample_to_string(uint8_t id) {
	switch(id) {
		case 1:
			return std::string("1 bit");

		case 8:
		case 10:
		case 12:
		case 16:
			return adxi::strprintf("%u bits", id);

		case 32:
			return std::string("32 bit FP");
			
		case 64:
			return std::string("64 bit FP");

		case 255:
			return std::string("unset");
	}

	return adxi::strprintf("invalid bit depth (%u)", id);
}

std::string adx::packing_to_string(uint8_t id) {
	switch(id) {
		case 0:
			return std::string("32 bit word packed");

		case 1:
			return std::string("32 bit word filled (method A)");

		case 2:
			return std::string("32 bit word filled (method B)");

		case 255:
			return std::string("unset");

		default:
			break;
	}

	return adxi::strprintf("reserved component packing (%u)", id);
}

std::string adx::encoding_to_string(uint8_t id) {
	switch(id)
	{
		case 0:
			return std::string("no encoding");

		case 1:
			return std::string("RLE encoding");

		case 255:
			return std::string("unset");

		default:
			break;
	}

	return adxi::strprintf("reserved component encoding (%u)", id);
}

std::string adx::video_to_string(uint8_t id) {
	if(id>=5 && id<=49) {
		return adxi::strprintf("reserved composite video (%u)", id);
	} else if(id>=52 && id<=99) {
		return adxi::strprintf("reserved component video (%u)", id);
	} else if(id>=102 && id<=149) {
		return adxi::strprintf("reserved widescreen (%u)", id);
	} else if(id>=154 && id<=199) {
		return adxi::strprintf("reserved high definition interlace (%u)", id);
	} else if(id>=204 && id<=254) {
		return adxi::strprintf("reserved high definition progressive (%u)", id);
	}

	switch(id) {
		case 0:
			return std::string("undefined");

		case 1:
			return std::string("NTSC");

		case 2:
			return std::string("PAL");

		case 3:
			return std::string("PAL-M");

		case 4:
			return std::string("SECAM");

		case 50:
			return std::string("YCbCr ITU-R 601-5 525 line 2:1 interlace, "
			                  "4:3 aspect");

		case 51:
			return std::string("YCbCr ITU-R 601-5 625 line 2:1 interlace, "
			                  "4:3 aspect");

		case 100:
			return std::string("YCbCr ITU-R 601-5 525 line 2:1 interlace, "
			                  "4:3 aspect");

		case 101:
			return std::string("YCbCr ITU-R 601-5 625 line 2:1 interlace, "
			                  "4:3 aspect");

		case 150:
			return std::string("YCbCr 1050 line 2:1 interlace, 16:9 aspect");
			
		case 151:
			return std::string("YCbCr 1125 line 2:1 interlace, 16:9 aspect "
			                  "SMPTE 274M");
			
		case 152:
			return std::string("YCbCr 1250 line 2:1 interlace, 16:9 aspect");
			
		case 153:
			return std::string("YCbCr 1125 line 2:1 interlace, 16:9 aspect "
			                  "SMPTE 240M");
			
		case 200:
			return std::string("YCbCr 525 line 1:1 progressive, 16:9 aspect");

		case 201:
			return std::string("YCbCr 625 line 1:1 progressive, 16:9 aspect");

		case 202:
			return std::string("YCbCr 750 line 1:1 progressive, 16:9 aspect "
			                  "SMPTE 296M");

		case 203:
			return std::string("YCbCr 1125 line 1:1 progressive, 16:9 aspect "
			                  "SMPTE 274M");


		case 255:
			return std::string("unset");

		default:
			break;
	}

	return adxi::strprintf("reserved video format %u", id);
}

std::string adx::transfer_to_string(uint8_t id) {
	switch(id) {
		case 0:
			return std::string("user defined");

		case 1:
			return std::string("printing density");

		case 2:
			return std::string("linear");

		case 3:
			return std::string("logarithmic (SMPTE I23)");

		case 4:
			return std::string("unspecified video");

		case 5:
			return std::string("SMPTE 274M");

		case 6:
			return std::string("ITU-R 709-4");

		case 7:
			return std::string("ITU-R 601-5 sytstem B or G");

		case 8:
			return std::string("ITU-R 601-5 sytstem M");

		case 9:
			return std::string("composite video (NTSC) SMPTE 170M");

		case 10:
			return std::string("composite video (PAL) ITU-R 624-4");

		case 11:
			return std::string("Z (depth) - linear");

		case 12:
			return std::string("Z (depth) - homogeneous");

		case 255:
			return std::string("unset");

		default:
			break;
	}

	return adxi::strprintf("reserved transfer characteristic %u", id);
}

std::string adx::colormetric_to_string(uint8_t id) {
	switch(id) {
		case 0:
			return std::string("user defined");

		case 1:
			return std::string("printing density");

		case 4:
			return std::string("unspecified video");

		case 5:
			return std::string("SMPTE 274M");

		case 6:
			return std::string("ITU-R 709-4");

		case 7:
			return std::string("ITU-R 601-5 sytstem B or G");

		case 8:
			return std::string("ITU-R 601-5 sytstem M");

		case 9:
			return std::string("composite video (NTSC) SMPTE 170M");

		case 10:
			return std::string("composite video (PAL) ITU-R 624-4");

		case 2:
		case 3:
		case 11:
		case 12:
			return adxi::strprintf("not applicable (%u)", id);

		case 255:
			return std::string("unset");

		default:
			break;
	}

	return adxi::strprintf("reserved transfer characteristic (%u)", id);
}

std::string adx::smpte_timecode_to_string(uint32_t tc) {
	char h[2];
	char m[2];
	char s[2];
	char f[2];

	if(tc==0xffffffff)
	{
		return std::string("<undefined>");
	}

	h[0]=(tc&0xf0000000) >> 28;
	h[1]=(tc&0x0f000000) >> 24;
	m[0]=(tc&0x00f00000) >> 20;
	m[1]=(tc&0x000f0000) >> 16;
	s[0]=(tc&0x0000f000) >> 12;
	s[1]=(tc&0x00000f00) >>  8;
	f[0]=(tc&0x000000f0) >>  4;
	f[1]=(tc&0x0000000f) >>  0;

	return adxi::strprintf("%d%d:%d%d:%d%d:%d%d", h[0], h[1], m[0], m[1],
	                                        s[0], s[1], f[0], f[1]);
}

};
