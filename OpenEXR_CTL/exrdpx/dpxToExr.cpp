///////////////////////////////////////////////////////////////////////////////
// 
// Copyright (c) 2006 Academy of Motion Picture Arts and Sciences
// ("A.M.P.A.S."). Portions contributed by others as indicated.
// All rights reserved.
// 
// A world-wide, royalty-free, non-exclusive right to distribute, copy,
// modify, create derivatives, and use, in source and binary forms, is
// hereby granted, subject to acceptance of this license. Performance of
// any of the aforementioned acts indicates acceptance to be bound by the
// following terms and conditions:
// 
//   * Redistributions of source code, in whole or in part, must
//     retain the above copyright notice, this list of conditions and
//     the Disclaimer of Warranty.
// 
//   * Redistributions in binary form must retain the above copyright
//     notice, this list of conditions and the Disclaimer of Warranty
//     in the documentation and/or other materials provided with the
//     distribution.
// 
//   * Nothing in this license shall be deemed to grant any rights to
//     trademarks, copyrights, patents, trade secrets or any other
//     intellectual property of A.M.P.A.S. or any contributors, except
//     as expressly stated herein.  Neither the name of. A.M.P.A.S. nor
//     any other contributors to this software may be used to endorse or
//     promote products derivative or, or based on this software without
//     express prior written permission of A.M.P.A.S. or contributor, as
//     appropriate.
// 
//   * This license shall be construed pursuant to the laws of the State
//     of California, and any disputes related thereto shall be subject
//     to the jurisdiction of the courts therein.
// 
// Disclaimer of Warranty: THIS SOFTWARE IS PROVIDED BY A.M.P.A.S. AND
// CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
// BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT ARE DISCLAIMED. IN NO
// EVENT SHALL A.M.P.A.S., OR ANY CONTRIBUTORS OR DISTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
// RESITUTIONARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
///////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
//
//	Read a DPX file (core-compiant reader only),
//	and store its contents in an OpenEXR file.
//
//----------------------------------------------------------------------------

#include <dpxToExr.h>

#include <dpxHeader.h>
#include <applyCtl.h>
#include <ImfHeader.h>
#include <ImfRgbaFile.h>
#include <ImfArray.h>
#include <ImfStandardAttributes.h>
#include <Iex.h>
#include <fstream>

using namespace std;
using namespace Imf;
using namespace Imath;
using namespace Iex;

namespace {

void
readHeader
    (ifstream &in,
     const char fileName[],
     ByteOrder &byteOrder,
     unsigned int &width,
     unsigned int &height,
     unsigned int &pixelOffset)
{
    FileInformation fileInfo;

    if (!in.read ((char *)&fileInfo, sizeof (fileInfo)))
	THROW_ERRNO ("Cannot read header from file " << fileName << " (%T).");

    switch (getU32 (fileInfo.magicNumber, BO_BIG))
    {
      case 0x53445058:
	byteOrder = BO_BIG;
	break;

      case 0x58504453:
	byteOrder = BO_LITTLE;
	break;

      default:
	THROW (InputExc, "File " << fileName << " is not a DPX file.");
    }

    ImageInformation imageInfo;

    if (!in.read ((char *)&imageInfo, sizeof (imageInfo)))
	THROW_ERRNO ("Cannot read header from file " << fileName << " (%T).");

    if (getU16 (imageInfo.imageOrientation, byteOrder) != 0)
	THROW (NoImplExc, "Cannot read DPX files with image orientation "
	                  "other than left-to-right, top-to-bottom.");

    if (getU16 (imageInfo.numberOfElements, byteOrder) != 1)
	THROW (NoImplExc, "Cannot read DPX files with "
			  "multiple image elements.");

    width = getU32 (imageInfo.pixelsPerLine, byteOrder);
    height = getU32 (imageInfo.linesPerImageElement, byteOrder);

    if (getU32 (imageInfo.imageElements[0].dataSign, byteOrder) != 0)
	THROW (NoImplExc, "Cannot read DPX files with signed data.");

    if (imageInfo.imageElements[0].descriptor != 50)
	THROW (NoImplExc, "Cannot read DPX files with data other than RGB.");

    if (imageInfo.imageElements[0].transferCharacteristic != 3)
	THROW (NoImplExc, "Cannot read DPX files with transfer "
	                  "characteristic other than logarithmic.");

    if (imageInfo.imageElements[0].colorimetricSpecification != 1)
	THROW (NoImplExc, "Cannot read DPX files with colorimetric "
	                  "specification other than printing density.");

    if (imageInfo.imageElements[0].bitSize != 10)
	THROW (NoImplExc, "Cannot read DPX files with bit size "
			  "other than 10.");

    if (getU16 (imageInfo.imageElements[0].packing, byteOrder) != 1)
	THROW (NoImplExc, "Cannot read DPX files with bit packing "
			  "other than \"filled to 32-bit words.\"");

    if (getU16 (imageInfo.imageElements[0].encoding, byteOrder) != 0)
	THROW (NoImplExc, "Cannot read DPX files with encoded data.");

    pixelOffset = getU32 (imageInfo.imageElements[0].offsetToData, byteOrder);
}


void
readPixels
    (ifstream &in,
     const char fileName[],
     ByteOrder byteOrder,
     unsigned int width,
     unsigned int height,
     unsigned int pixelOffset,
     Array2D<Rgba> &pixels)
{
    if (!in.seekg (pixelOffset, ios_base::beg))
	THROW_ERRNO ("Cannot seek to start of pixel data "
		     "in DPX file " << fileName << " (%T).");

    Array<unsigned char> rawLine (width * 4);

    for (int y = 0; y < height; ++y)
    {
	if (!in.read ((char *)&rawLine[0], width * 4))
	    THROW_ERRNO ("Cannot read scan line " << y << " "
			 "from DPX file " << fileName << " (%T).");

	for (int x = 0; x < width; ++x)
	{
	    unsigned int word = getU32 (rawLine + 4 * x, byteOrder);
	    Rgba &pixel = pixels[y][x];
	    pixel.r = half ((word >> 22) & 0x3ff);
	    pixel.g = half ((word >> 12) & 0x3ff);
	    pixel.b = half ((word >>  2) & 0x3ff);
	    pixel.a = 0;
	}
    }
}


} //namespace


void
dpxToExr (const char dpxFileName[],
          const char exrFileName[],
          vector<string> transformNames,
	  bool outputXyz,
	  bool outputYc,
	  Compression compression,
	  bool verbose)
{
    //
    // Read the DPX file
    //

    if (verbose)
	cout << "reading file " << dpxFileName << endl;

    ifstream in (dpxFileName, ios_base::binary);

    if (!in)
    {
	THROW_ERRNO ("Cannot open file " << dpxFileName << " "
		     "for reading (%T).");
    }

    ByteOrder byteOrder;
    unsigned int width;
    unsigned int height;
    unsigned int pixelOffset;

    readHeader (in, dpxFileName, byteOrder, width, height, pixelOffset);

    Array2D<Rgba> pixels (height, width);

    readPixels (in, dpxFileName, byteOrder, width, height, pixelOffset, pixels);

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

    Chromaticities chromaticities;

    if (outputXyz)
    {
	chromaticities.red = V2f (1, 0);
	chromaticities.green = V2f (0, 1);
	chromaticities.blue = V2f (0, 0);
	chromaticities.white = V2f (1.f/3.f, 1.f/3.f);
    }

    applyCtlDpxToExr (transformNames,
		      pixels,
		      width, height,
		      chromaticities,
		      pixels);

    //
    // Write the OpenEXR file
    //

    if (verbose)
	cout << "writing file " << exrFileName << endl;

    Header header (width, height);
    header.compression() = compression;
    addChromaticities (header, chromaticities);

    RgbaOutputFile out (exrFileName, header, outputYc? WRITE_YC: WRITE_RGB);
    out.setFrameBuffer (&pixels[0][0], 1, width);
    out.writePixels (height);

    if (verbose)
	cout << "done" << endl;
}
