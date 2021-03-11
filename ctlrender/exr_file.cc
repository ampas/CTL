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

#include <ImfInputFile.h>
#include <ImfOutputFile.h>
#include <ImfRgbaFile.h>
#include <ImfArray.h>
#include <ImfHeader.h>
#include <ImfChannelList.h>
#include <Iex.h>

bool exr_read(const char *name, float scale, ctl::dpx::fb<float> *pixels,
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
	//////////////////////////
    
    Imf::InputFile file(name);
    Imath::Box2i dw = file.header().dataWindow();
    
    if (file.header().channels().begin().channel().type == Imf::HALF)
        format->src_bps=16;
    else
        format->src_bps=32;
        
    int width = dw.max.x - dw.min.x + 1;
    int height = dw.max.y - dw.min.y + 1;
    
    pixels->init(width, height, 4);
    Imf::PixelType pixelType = Imf::FLOAT;
    
    int xstride = sizeof (*pixels->ptr()) * pixels->depth();
    int ystride = sizeof (*pixels->ptr()) * pixels->depth() * pixels->width();
    
    Imf::FrameBuffer frameBuffer;
    frameBuffer.insert ("R",
                        Imf::Slice (pixelType,
                                    (char *) pixels->ptr(),
                                    xstride, ystride,
                                    1, 1,
                                    0.0));
    
    frameBuffer.insert ("G",
                        Imf::Slice (pixelType,
                                    (char *) (pixels->ptr()+1),
                                    xstride, ystride,
                                    1, 1,
                                    0.0));
    
    frameBuffer.insert ("B",
                        Imf::Slice (pixelType,
                                    (char *) (pixels->ptr()+2),
                                    xstride, ystride,
                                    1, 1,
                                    0.0));
    
    frameBuffer.insert ("A",
                        Imf::Slice (pixelType,
                                    (char *) (pixels->ptr()+3),
                                    xstride, ystride,
                                    1, 1,
                                    1.0));
    
    file.setFrameBuffer(frameBuffer);
    file.readPixels(dw.min.y, dw.max.y);
    
	if(scale==0.0 || scale==1.0) {
		return 1;
	}

	float *p=pixels->ptr();
	for(uint64_t i=0; i<pixels->count(); i++) {
		*p=*p*scale;
		p++;
	}
	return 1;
}

void exr_write32(const char *name, float scale, const ctl::dpx::fb<float> &pixels, Compression *compression)
{
    int depth = pixels.depth();
    float width = pixels.width();
    float height = pixels.height();
    float const *pixelPtr = pixels.ptr();
    
	ctl::dpx::fb<float> scaled_pixels;
    if (scale != 0.0 && scale != 1.0) {
        scaled_pixels.init(pixels.height(), pixels.width(), pixels.depth());
        scaled_pixels.alpha(1.0);
        
        const float *fIn=pixels.ptr();
        float *out=scaled_pixels.ptr();
        
        for(uint64_t i=0; i<pixels.count(); i++) {
            *(out++)=*(fIn++)/scale;
        }
        
        depth = scaled_pixels.depth();
        width = scaled_pixels.width();
        height = scaled_pixels.height();
        pixelPtr = scaled_pixels.ptr();
    }
    
    Imf::PixelType pixelType = Imf::FLOAT;
    
    Imf::Header header(width, height);
    header.compression() = (Imf::Compression)compression->exrCompressionScheme;
    
    header.channels().insert("R", Imf::Channel(pixelType));
    header.channels().insert("G", Imf::Channel(pixelType));
    header.channels().insert("B", Imf::Channel(pixelType));
    
    if (depth == 4)
        header.channels().insert("A", Imf::Channel(pixelType));
    
    Imf::OutputFile file (name, header);
    
    Imf::FrameBuffer frameBuffer;
    
    int xstride = sizeof (*pixelPtr) * depth;
    int ystride = sizeof (*pixelPtr) * depth * width;
    
    frameBuffer.insert ("R",
                        Imf::Slice (pixelType,
                                    (char *) pixelPtr,
                                    xstride, ystride));
    
    frameBuffer.insert ("G",
                        Imf::Slice (pixelType,
                                    (char *) (pixelPtr+1),
                                    xstride, ystride));
    
    frameBuffer.insert ("B",
                        Imf::Slice (pixelType,
                                    (char *) (pixelPtr+2),
                                    xstride, ystride));
    
    if (depth == 4)
        frameBuffer.insert ("A",
                            Imf::Slice (pixelType,
                                        (char *) (pixelPtr+3),
                                        xstride, ystride));
    
    file.setFrameBuffer (frameBuffer);
    file.writePixels (height);
}

void exr_write16(const char *name, float scale, const ctl::dpx::fb<float> &pixels, Compression *compression) {
	if (scale == 0.0) scale = 1.0;

    ctl::dpx::fb<half> scaled_pixels;
    scaled_pixels.init(pixels.height(), pixels.width(), pixels.depth());
    scaled_pixels.alpha(1.0);
    
    const float *fIn=pixels.ptr();
    half *out=scaled_pixels.ptr();
    
    // Yes... I should lookup table this. I *know*!
    for(uint64_t i=0; i<pixels.count(); i++) {
        *(out++)=*(fIn++)/scale;
    }
    
	uint8_t channels=scaled_pixels.depth();
    const half *in=scaled_pixels.ptr();
    
	Imf::RgbaOutputFile file(name, pixels.width(), pixels.height(),
	                         channels==4 ? Imf::WRITE_RGBA : Imf::WRITE_RGB, 1, Imath::V2f (0, 0), 1,
	                         Imf::INCREASING_Y, (Imf::Compression)compression->exrCompressionScheme);
    
	file.setFrameBuffer((Imf::Rgba *)in, 1, pixels.width());
	file.writePixels(pixels.height());
}

void exr_write(const char *name, float scale, const ctl::dpx::fb<float> &pixels,
               format_t *format, Compression *compression)
{
    if(format->bps == 32) {
        exr_write32(name, scale, pixels, compression);
    }
    else if(format->bps == 16) {
        exr_write16(name, scale, pixels, compression);
    }
    else {
        THROW(Iex::ArgExc, "EXR files only support 16 or 32 bps at the moment.");
    }
}

