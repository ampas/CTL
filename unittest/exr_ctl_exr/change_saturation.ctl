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

//
// Function change_saturation() saturates or desaturates an RGB image by
// converting the pixels to CIELAB, multiplying the a* and b* components
// by a caller-supplied factor, sScale, and converting back to RGB.
//
// Parameters:
//
//	R, G, B			Input pixel value
//
//	ROut, GOut, BOut	Output pixel value
//
//	sScale			Saturation scale factor:
//				sScale > 1 increases saturation,
//				0 <= sScale < 1 decreases saturation,
//
//	chromaticities		CIE (x,y) coordinates of the primaries
//				and white point for the input and output
//				pixels
//
//	whiteLuminance		Luminance of pixels with R = G = B = 1
//
//	adoptedNeutral		CIE (x,y) coordinates of the white stimulus
//				for RGB-to-LAB and LAB-to-RGB conversion.
//				Decreasing or increasing saturation makes
//				the pixel colors move towards or away from
//				the adoptedNeutral value.
//

void
change_saturation
    (output varying half ROut,
     output varying half GOut,
     output varying half BOut,
     varying half R,
     varying half G,
     varying half B,
     float adoptedNeutral[2],
     Chromaticities chromaticities,
     float whiteLuminance = 1.0,
     float sScale = 1.0)
{
    //
    // Compute the XYZ coordinates of the white stimulus.
    //

    float XYZn[3];
    XYZn[0] = adoptedNeutral[0] * whiteLuminance / adoptedNeutral[1];
    XYZn[1] = whiteLuminance;
    XYZn[2] = whiteLuminance / adoptedNeutral[1] - XYZn[0] - whiteLuminance;

    //
    // Convert input RGB value first to XYZ, then to LAB
    //

    float toXYZ[4][4] = RGBtoXYZ (chromaticities, whiteLuminance);
    float RGB[3] = {R, G, B};
    float XYZ[3] = mult_f3_f44 (RGB, toXYZ);
    float Lab[3] = XYZtoLab (XYZ, XYZn);

    //
    // Change saturation by scaling the a* and b* coordinates.
    //

    Lab[1] = Lab[1] * sScale;
    Lab[2] = Lab[2] * sScale;

    //
    // Convert back to XYZ and then to RGB
    //

    XYZ = LabtoXYZ (Lab, XYZn);
    float toRGB[4][4] = XYZtoRGB (chromaticities, whiteLuminance);
    RGB = mult_f3_f44 (XYZ, toRGB);
    ROut = RGB[0];
    GOut = RGB[1];
    BOut = RGB[2];
}
