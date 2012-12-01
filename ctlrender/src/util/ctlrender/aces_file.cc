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

#include "aces_file.hh"

#if 1//defined( HAVE_ACESFILE )
#include <aces/aces_Writer.h>
#include <stdexcept>
#include <iostream>

void aces_write(const char *name, float scale, 
               uint32_t width, uint32_t height, uint32_t channels,
               const void *pixels,
               format_t *format) {

	if (channels != 3) 
		throw std::invalid_argument("Only RGB file supported");
		
	half *in = (half*)pixels;
	std::vector<half> scaled_pixels;
	
	if (scale != 1.0f && scale != 0.0f) 
	{	
		scaled_pixels.resize(height*width*channels);
		half *out = &scaled_pixels[0];
		for(size_t i=0; i<scaled_pixels.size(); i++) {
			*(out++)=*(in++)/scale;
		}

		in = &scaled_pixels[0];
	}
	std::vector<std::string> filenames;
	filenames.push_back( name );
	
	aces_Writer x;
	
	MetaWriteClip writeParams;
	
	writeParams.duration				= 1;	
	writeParams.outputFilenames			= filenames;
	
	writeParams.outputRows				= height;
	writeParams.outputCols				= width;	
	
	writeParams.hi = x.getDefaultHeaderInfo();	
	writeParams.hi.originalImageFlag	= 1;	
	writeParams.hi.software				= "ctlrender";

	DynamicMetadata dynamicMeta;
	dynamicMeta.imageIndex = 0;
	dynamicMeta.imageCounter = 0;
	
	x.configure ( writeParams );
	x.newImageObject ( dynamicMeta );		

	for ( uint32 row = 0; row < height; row++) {
		half *rgbData = in + width*channels*row;
		x.storeHalfRow ( rgbData, row ); 
	}

#if 0
	std::cout << "saving aces file" << std::endl;
	std::cout << "size " << width << "x" << height << "x" << channels << std::endl;
	std::cout << "size " << writeParams.outputCols << "x" << writeParams.outputRows << std::endl;
	std::cout << "duration " << writeParams.duration << std::endl;
	std::cout << writeParams.hi;
	std::cout << "\ndynamic meta" << std::endl;
	std::cout << "imageIndex " << dynamicMeta.imageIndex << std::endl;
	std::cout << "imageCounter " << dynamicMeta.imageCounter << std::endl;
	std::cout << "timeCode " << dynamicMeta.timeCode << std::endl;
	std::cout << "keyCode " << dynamicMeta.keyCode << std::endl;
	std::cout << "capDate " << dynamicMeta.capDate << std::endl;
	std::cout << "uuid " << dynamicMeta.uuid << std::endl;
#endif

	x.saveImageObject ( );	
}

#else 

void aces_write(const char *name, float scale,
               const ctl::dpx::fb<half> &pixels,
               format_t *format) {
	std::cerr << "AcesContainer library not found" << std::endl;
}

#endif


