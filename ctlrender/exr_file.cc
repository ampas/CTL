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

#include "exr_file.hh"

#if defined(HAVE_OPENEXR)
#include <ImfInputFile.h>
#include <ImfOutputFile.h>
#include <ImfRgbaFile.h>
#include <ImfArray.h>
#include <Iex.h>

bool exr_read(const char *name, float scale, ctl::dpx::fb<half> *pixels,
              format_t *format) {
	std::ifstream ins;
	unsigned int magic, endian;

	ins.open(name);

	ins.read((char *)&magic, sizeof(magic));
	endian=0x01020304;
	if(((unsigned char *)(&endian))[0]==0x01) {
		if(magic!=0x762f3101) {
			return 0;
		}
	} else {
		if(magic!=0x01312f76) {
			return 0;
		}
	}
	
    Imf::RgbaInputFile in(name);
    Imath::Box2i dw = in.dataWindow();
	uint64_t i;
	half *p;
  
	format->src_bps=16;

	pixels->init(dw.max.x-dw.min.x+1, dw.max.y-dw.min.y+1, 4);

	in.setFrameBuffer((Imf::Rgba *)(pixels->ptr()-dw.min.x-dw.min.y*pixels->width()), 1, pixels->width());
	in.readPixels(dw.min.y, dw.max.y);

	if(scale==0.0 || scale==1.0) {
		return 1;
	}

	p=pixels->ptr();
	for(i=0; i<pixels->count(); i++) {
		*p=*p*scale;
		p++;
	}
	return 1;
}

void exr_write(const char *name, float scale, const ctl::dpx::fb<half> &pixels,
               format_t *format) {
	const half *in;
	half *out;
	uint8_t channels;

	if(format->bps!=16) {
		THROW(Iex::ArgExc, "EXR files only support 16 bps half at the moment.");
	}

	if(scale!=0.0 && scale!=1.0) {
		// Yes... I should lookup table this. I *know*!
		ctl::dpx::fb<half> scaled_pixels;
		uint64_t i;

		scaled_pixels.init(pixels.height(), pixels.width(), pixels.depth());
		scaled_pixels.alpha(1.0);
		in=pixels.ptr();
		out=scaled_pixels.ptr();
		for(i=0; i<scaled_pixels.count(); i++) {
			*(out++)=*(in++)/scale;
		}

		channels=scaled_pixels.depth();	
		in=scaled_pixels.ptr();
	} else {
		channels=pixels.depth();	
		in=pixels.ptr();
	}

	Imf::RgbaOutputFile file(name, pixels.width(), pixels.height(),
	                         channels==4 ? Imf::WRITE_RGBA : Imf::WRITE_RGB, 1, Imath::V2f (0, 0), 1,
	                         Imf::INCREASING_Y, Imf::PIZ_COMPRESSION);

	file.setFrameBuffer((Imf::Rgba *)in, 1, pixels.width());
	file.writePixels(pixels.height());
}

#else

bool exr_read(const char *name, float scale,
              ctl::dpx::fb<half> *pixels,
              format_t *bpp) {
	return FALSE;
}

void exr_write(const char *name, float scale,
               const ctl::dpx::fb<half> &pixels,
               format_t *format) {
}

#endif


