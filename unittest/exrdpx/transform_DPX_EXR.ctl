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
// A transform that converts density values as stored in
// DPX files into approximately scene-linear ACES RGB values.
//

import "utilities";

void
transform_DPX_EXR
    (output varying half R,
     output varying half G,
     output varying half B,
     input varying half DR_film,
     input varying half DG_film,
     input varying half DB_film,
     input uniform Chromaticities chromaticities,
     input uniform float maxAimDensity[3] = {1.890, 2.046, 2.046},
    					// ARRI "carlos" aims
     input uniform float negGamma[3] = {0.49, 0.57, 0.60})
     					// gamma of negative (Kodak 5201)
{
    //
    // Convert DPX code values to density
    //

    float density[3] =
    {
	(DR_film - 445.0) * (maxAimDensity[0] / 1023.0),
	(DG_film - 445.0) * (maxAimDensity[1] / 1023.0),
	(DB_film - 445.0) * (maxAimDensity[2] / 1023.0)
    };

    //
    // Convert densities to approximately linear Rec. 709 values
    //
    
    float linear[3] =
    {
	pow10_h (density[0] / negGamma[0]) * 0.18,
	pow10_h (density[1] / negGamma[1]) * 0.18,
	pow10_h (density[2] / negGamma[2]) * 0.18
    };

    //
    // Convert from Rec. 709 to output primaries and white point
    //

    float toExr[4][4] = mult_f44_f44 (RGBtoXYZ (rec709Chromaticities, 1.0), 
				      XYZtoRGB (chromaticities, 1.0));

    linear = mult_f3_f44 (linear, toExr);

    R = linear[0];
    G = linear[1];
    B = linear[2];
}
