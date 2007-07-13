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

//-----------------------------------------------------------------------------
//
//	Apply CTL transforms
//
//-----------------------------------------------------------------------------

#include <applyCtl.h>

#include <ImfCtlApplyTransforms.h>
#include <CtlSimdInterpreter.h>
#include <ImfStandardAttributes.h>
#include <ImfHeader.h>
#include <ImfFrameBuffer.h>
#include <cassert>
#include <iostream>
#include <vector>
#include <string>

using namespace std;
using namespace Ctl;
using namespace Imf;
using namespace Imath;


#define WARNING(message) (cerr << "Warning: " << message << endl)


namespace {

void
initializeFrameBuffer
    (int w,
     int h,
     const Array2D<Rgba> &pixels,
     FrameBuffer &fb)
{
    fb.insert ("R", Slice (HALF,				// type
			   (char *)(&pixels[0][0].r),		// base
			   sizeof (pixels[0][0]),		// xStride
			   sizeof (pixels[0][0]) * w));		// yStride

    fb.insert ("G", Slice (HALF,				// type
			   (char *)(&pixels[0][0].g),		// base
			   sizeof (pixels[0][0]),		// xStride
			   sizeof (pixels[0][0]) * w));		// yStride

    fb.insert ("B", Slice (HALF,				// type
			   (char *)(&pixels[0][0].b),		// base
			   sizeof (pixels[0][0]),		// xStride
			   sizeof (pixels[0][0]) * w));		// yStride
}


void
initializeEnvHeader (Header &envHeader)
{
    //
    // Initialize the "environment header" for the CTL
    // transforms by adding displayChromaticities,
    // displayWhiteLuminance and displaySurroundLuminance
    // attributes.
    //

    //
    // Get the chromaticities of the display's primaries and
    // white point from an environment variable.  If this fails,
    // assume chromaticities according to Rec. ITU-R BT.709.
    //

    static const char chromaticitiesEnv[] = "CTL_DISPLAY_CHROMATICITIES";
    Chromaticities c;  // default-initialized according to Rec. 709

    if (const char *chromaticities = getenv (chromaticitiesEnv))
    {
	Chromaticities tmp;

	int n = sscanf (chromaticities,
			" red %f %f green %f %f blue %f %f white %f %f",
			&tmp.red.x, &tmp.red.y,
			&tmp.green.x, &tmp.green.y,
			&tmp.blue.x, &tmp.blue.y,
			&tmp.white.x, &tmp.white.y);

	if (n == 8)
	    c = tmp;
	else
	    WARNING ("Cannot parse environment variable " <<
		     chromaticitiesEnv << "; using default value "
		     "(chromaticities according to Rec. ITU-R BT.709).");
    }

    envHeader.insert ("displayChromaticities", ChromaticitiesAttribute (c));

    //
    // Get the display's white luminance from an environment variable.
    // If this fails, assume 120 candelas per square meter.
    // (Screen aim luminance according to SMPTE RP 166.)
    //

    static const char whiteLuminanceEnv[] = "CTL_DISPLAY_WHITE_LUMINANCE";
    static const float whiteLuminanceDefault = 120.0;
    float wl = whiteLuminanceDefault;

    if (const char *whiteLuminance = getenv (whiteLuminanceEnv))
    {
	int n = sscanf (whiteLuminance, " %f", &wl);

	if (n != 1)
	    WARNING ("Cannot parse environment variable " <<
		     whiteLuminanceEnv << "; using default value "
		     "(" << wl << " candelas per square meter).");
    }

    envHeader.insert ("displayWhiteLuminance", FloatAttribute (wl));

    //
    // Get the display's surround luminance from an environment variable.
    // If this fails, assume 10% of the display's white luminance.
    // (Recommended setup according to SMPTE RP 166.)
    //

    static const char surroundLuminanceEnv[] = "CTL_DISPLAY_SURROUND_LUMINANCE";
    float sl = wl * 0.1f;

    if (const char *surroundLuminance = getenv (surroundLuminanceEnv))
    {
	int n = sscanf (surroundLuminance, " %f", &sl);

	if (n != 1)
	    WARNING ("Cannot parse environment variable " <<
		     surroundLuminanceEnv << "; using default value "
		     "(" << sl << " candelas per square meter).");
    }

    envHeader.insert ("displaySurroundLuminance", FloatAttribute (sl));
}

} // namespace


void
applyCtlExrToExr
    (Imf::Header inHeader,
     Imf::Header &outHeader,
     const Imf::Array2D<Imf::Rgba> &inPixels,
     Imf::Array2D<Imf::Rgba> &outPixels,
     int w,
     int h,
     const std::vector<std::string> &transformNames,
     const AttrMap &extraAttrs)
{
    //
    // Make sure that the input and output headers contain
    // chromaticities and adoptedNeutral attributes.
    //

    if (!hasChromaticities (inHeader))
	addChromaticities (inHeader, Chromaticities());

    if (!hasAdoptedNeutral (inHeader))
	addAdoptedNeutral (inHeader, chromaticities(inHeader).white);

    if (!hasChromaticities (outHeader))
	addChromaticities (outHeader, chromaticities (inHeader));

    if (!hasAdoptedNeutral (outHeader))
	addAdoptedNeutral (outHeader, adoptedNeutral (inHeader));

    //
    // Add extraAttrs to the input header, possibly overriding
    // the values of existing input header attributes.
    //

    for (AttrMap::const_iterator i = extraAttrs.begin();
	 i != extraAttrs.end();
	 ++i)
    {
	inHeader.insert (i->first.c_str(), *i->second);
    }

    //
    // Initialize an "environment" header with the same data that
    // would be available to rendering transforms in an image viewing
    // program.  This allows transforms to bake color rendering into
    // the output file's pixels.
    //

    Header envHeader;
    initializeEnvHeader (envHeader);

    //
    // Set up input and output FrameBuffer objects for the transforms.
    //

    FrameBuffer inFb;
    initializeFrameBuffer (w, h, inPixels, inFb);

    FrameBuffer outFb;
    initializeFrameBuffer (w, h, outPixels, outFb);

    //
    // Run the CTL transforms
    //

    Box2i transformWindow (V2i (0, 0), V2i (w - 1, h - 1));

    SimdInterpreter interpreter;

    #ifdef CTL_MODULE_BASE_PATH

	//
	// The configuration script has defined a default
	// location for CTL modules.  Include this location
	// in the CTL module search path.
	//

	vector<string> paths = interpreter.modulePaths();
	paths.push_back (CTL_MODULE_BASE_PATH);
	interpreter.setModulePaths (paths);

    #endif

    ImfCtl::applyTransforms (interpreter,
			     transformNames,
			     transformWindow,
			     envHeader,
			     inHeader,
			     inFb,
			     outHeader,
			     outFb);
}
