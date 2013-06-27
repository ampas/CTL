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

//----------------------------------------------------------------------------
//
//	Read the R, G and B channels from an OpenEXR file, and
//	store them in a 10-bit log-encoded printing-density DPX file.
//
//----------------------------------------------------------------------------

#include <exrToDpx.h>

#include <dpxHeader.h>
#include <applyCtl.h>
#include <ImfHeader.h>
#include <ImfRgbaFile.h>
#include <ImfArray.h>
#include <ImathFun.h>
#include <Iex.h>
#include <string.h>
#include <fstream>

using namespace std;
using namespace Imf;
using namespace Imath;
using namespace Iex;

namespace {

void
writeHeader
    (ofstream &out,
     const char fileName[],
     unsigned int width,
     unsigned int height)
{
    union
    {
	struct
	{
	    FileInformation		fileInfo;
	    ImageInformation		imageInfo;
	    ImageOrientation		orientation;
	    MotionPictureFilmHeader	mph;
	    TelevisionHeader		tvh;
	};

	unsigned char			padding[8192];
    } header;

    memset (header.padding, 0xff, sizeof (header));

    setU32 (0x53445058, header.fileInfo.magicNumber, BO_BIG);
    setU32 (sizeof (header), header.fileInfo.offsetToImageData, BO_BIG);
    strcpy (header.fileInfo.versionNumber, "V2.0");

    setU32 (sizeof (header) + 4 * width * height,
	    header.fileInfo.fileSize,
	    BO_BIG);

    header.fileInfo.fileName[0] = 0;
    header.fileInfo.creationTime[0] = 0;
    header.fileInfo.creator[0] = 0;
    header.fileInfo.projectName[0] = 0;
    header.fileInfo.copyright[0] = 0;
    
    setU16 (0, header.imageInfo.imageOrientation, BO_BIG);
    setU16 (1, header.imageInfo.numberOfElements, BO_BIG);
    setU32 (width, header.imageInfo.pixelsPerLine, BO_BIG);
    setU32 (height, header.imageInfo.linesPerImageElement, BO_BIG);

    setU32 (0, header.imageInfo.imageElements[0].dataSign, BO_BIG); // unsigned
    header.imageInfo.imageElements[0].descriptor = 50;	// RGB
    header.imageInfo.imageElements[0].transferCharacteristic = 3; // log
    header.imageInfo.imageElements[0].colorimetricSpecification = 1; // density
    header.imageInfo.imageElements[0].bitSize = 10;
    setU16 (1, header.imageInfo.imageElements[0].packing, BO_BIG); // filled
    setU16 (0, header.imageInfo.imageElements[0].encoding, BO_BIG); // none

    setU32 (sizeof (header),
	    header.imageInfo.imageElements[0].offsetToData,
	    BO_BIG); // none

    for (int i = 0; i < 7; ++i)
	header.imageInfo.imageElements[i].description[0] = 0;

    header.orientation.sourceImageFileName[0] = 0;
    header.orientation.creationTime[0] = 0;
    header.orientation.inputDev[0] = 0;
    header.orientation.inputSerial[0] = 0;

    header.mph.filmManufacturerId[0] = 0;
    header.mph.filmType[0] = 0;
    header.mph.offset[0] = 0;
    header.mph.prefix[0] = 0;
    header.mph.count[0] = 0;
    header.mph.format[0] = 0;

    if (!out.write ((const char *)&header, sizeof (header)))
	THROW_ERRNO ("Cannot write header to file " << fileName << " (%T).");
}


void
writePixels
    (ofstream &out,
     const char fileName[],
     unsigned int width,
     unsigned int height,
     const Array2D<Rgba> &pixels)
{
    Array<unsigned char> rawLine (width * 4);

    for (int y = 0; y < height; ++y)
    {
	for (int x = 0; x < width; ++x)
	{
	    const Rgba &pixel = pixels[y][x];

	    unsigned int r =
		(unsigned int) (clamp (float (pixel.r), 0.0f, 1023.0f) + 0.5f);

	    unsigned int g =
		(unsigned int) (clamp (float (pixel.g), 0.0f, 1023.0f) + 0.5f);

	    unsigned int b =
		(unsigned int) (clamp (float (pixel.b), 0.0f, 1023.0f) + 0.5f);

	    unsigned int word = (r << 22) | (g << 12) | (b << 2);

	    setU32 (word, rawLine + 4 * x, BO_BIG);
	}

	if (!out.write ((const char *)&rawLine[0], width * 4))
	    THROW_ERRNO ("Cannot write scan line " << y << " "
			 "to DPX file " << fileName << " (%T).");
    }
}

} // namespace


void
exrToDpx (const char exrFileName[],
	  const char dpxFileName[],
	  std::vector<std::string> transformNames,
	  bool verbose)
{
    //
    // Read the OpenEXR file
    //

    if (verbose)
	cout << "reading file " << exrFileName << endl;

    RgbaInputFile in (exrFileName);
    Box2i dw = in.dataWindow();
    unsigned int width  = (unsigned int) (dw.max.x - dw.min.x + 1);
    unsigned int height = (unsigned int) (dw.max.y - dw.min.y + 1);

    Array2D<Rgba> pixels (height, width);
    in.setFrameBuffer (&pixels[0][0] - dw.min.x - dw.min.y * width, 1, width);
    in.readPixels (dw.min.y, dw.max.y);

    //
    // Apply the CTL transforms
    //

    if (verbose)
    {
	cout << "applyging CTL transforms:";

	for (int i = 0; i < transformNames.size(); ++i)
	    cout << " " << transformNames[i];

	cout << endl;
    }

    applyCtlExrToDpx (transformNames,
		      in.header(),
		      pixels,
		      width, height,
		      pixels);

    //
    // Write the DPX file
    //

    if (verbose)
	cout << "writing file " << dpxFileName << endl;

    ofstream out (dpxFileName, ios_base::binary);

    if (!out)
    {
	THROW_ERRNO ("Cannot open file " << dpxFileName << " "
		     "for writing (%T).");
    }

    writeHeader (out, dpxFileName, width, height);
    writePixels (out, dpxFileName, width, height, pixels);

    if (verbose)
	cout << "done" << endl;
}
