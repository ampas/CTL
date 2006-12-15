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

//
// A transform that converts density values as stored in
// DPX files into approximately scene-linear RGB values.
//

const Chromaticities rec709 =
{
    {0.6400, 0.3300},
    {0.3000, 0.6000},
    {0.1500, 0.0600},
    {0.3172, 0.3290}
};

void
transform_DPX_EXR
    (output varying half R,
     output varying half G,
     output varying half B,
     input varying half dpxR,
     input varying half dpxG,
     input varying half dpxB,
     input uniform Chromaticities exrChromaticities,
     input uniform float maxAimDensity[3] = {1.890, 2.046, 2.046},
     							// ARRI "carlos" aims
     input uniform float negGamma[3] = {0.55, 0.60, 0.60})
     							// gamma of negative
{
    //
    // Convert DPX code values to density
    //

    float density[3] =
    {
	(dpxR - 445.0) * (maxAimDensity[0] / 1023.0),
	(dpxG - 445.0) * (maxAimDensity[1] / 1023.0),
	(dpxB - 445.0) * (maxAimDensity[2] / 1023.0)
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

    float toExr[4][4] = mult_f44_f44 (RGBtoXYZ (rec709, 1.0), 
				      XYZtoRGB (exrChromaticities, 1.0));

    linear = mult_f3_f44 (linear, toExr);

    R = linear[0];
    G = linear[1];
    B = linear[2];
}
