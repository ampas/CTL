///////////////////////////////////////////////////////////////////////////////
// 
// Copyright (c) 2007 Academy of Motion Picture Arts and Sciences
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
//	Read the R, G and B channels from an OpenEXR file,
//	appliy one or more CTL programs to the pixels, and
//	store the result in another OpenEXR file.
//
//----------------------------------------------------------------------------

#include <exrCtlExr.h>

#include <ImfRgbaFile.h>
#include <ImfArray.h>
#include <Iex.h>

using namespace Imf;
using namespace Imath;
using namespace Iex;
using namespace std;


void
exrCtlExr (const char inFileName[],
	   const char outFileName[],
	   const std::vector<std::string> &transformNames,
	   const AttrMap &extraAttrs,
	   bool verbose)
{
    //
    // Read the input file
    //

    if (verbose)
	cout << "reading file " << inFileName << endl;

    RgbaInputFile in (inFileName);
    const Box2i &dw = in.dataWindow();
    int w = dw.max.x - dw.min.x + 1;
    int h = dw.max.y - dw.min.y + 1;

    Array2D<Rgba> inPixels (h, w);

    in.setFrameBuffer (&inPixels[0][0] - dw.min.x - dw.min.y * w, 1, w);
    in.readPixels (dw.min.y, dw.max.y);

    //
    // Apply the CTL transforms to the R, G and B channels of the input file
    //

    if (verbose)
    {
	cout << "applying CTL transforms:";

	for (int i = 0; i < transformNames.size(); ++i)
	    cout << " " << transformNames[i];

	cout << endl;
    }

    Header outHeader = in.header();
    Array2D<Rgba> outPixels (h, w);

    applyCtlExrToExr (in.header(), outHeader,
		      inPixels, outPixels,
		      w, h,
		      transformNames,
		      extraAttrs);

    //
    // Copy the A channel verbatim
    //

    if (in.channels() & WRITE_A)
    {
	const Rgba *inPtr = &inPixels[0][0];
	Rgba *outPtr = &outPixels[0][0];
	size_t numPixels = w * h;

	for (size_t i = 0; i < numPixels; ++i)
	    (outPtr++)->a = (inPtr++)->a;
    }

    //
    // Write the output file
    //

    if (verbose)
	cout << "writing file " << outFileName << endl;

    RgbaOutputFile out (outFileName, in.header(), in.channels());
    out.setFrameBuffer (&outPixels[0][0] - dw.min.x - dw.min.y * w, 1, w);
    out.writePixels (h);
}
