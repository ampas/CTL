///////////////////////////////////////////////////////////////////////////
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

#ifndef INCLUDED_CTL_LOOKUP_TABLE_H
#define INCLUDED_CTL_LOOKUP_TABLE_H

//-----------------------------------------------------------------------------
//
//	1D and 3D table lookups with linear and trilinear interpolation.
//
//	lookup1D(t,s,pMin,pMax,p)
//
//		Lookup table t, which contains s entries, defines a
//		piecewise linear function, f, with s-1 segments, such
//		that f(pMin) == t[0] and f(pMax) == t(s-1).
//
//		lookup1D(t,s,pMin,pMax,p) returns f(clamp(p,pMin,pMax)).
//
//
//	lookup3D(t,s,pMin,pMax,p)
//
//		Lookup table t, which contains s.x by s.y by s.z
//		entries, defines a piecewise trilinear function, f, with
//		(s.x - 1) by (s.y - 1) by (s.z - 1) segments, such that
//		f(pMin) == t[0][0][0] and f(pMax) == t[s.x-1][s.y-1][s.z-1].
//
//		lookup1D(t,s,pMin,pMax,p) returns f(pClamp), where
//
//		pClamp == V3f (clamp (p.x, pMin.x, pMax.x),
//		               clamp (p.y, pMin.y, pMax.y),
//		               clamp (p.z, pMin.z, pMax.z))
//
//		Since C++ doesn't directly support 3D arrays whose size
//		is not known at compile time, lookup table t is passed
//		to lookup3D as a 1D array, with table entry t[i][j][k]
//		at location t[(i * s.y + j) * s.z + k];
//
//-----------------------------------------------------------------------------

#include <ImathVec.h>

namespace Ctl {

float	 	lookup1D (const float table[],
			  const int size,
			  float pMin,
			  float pMax,
			  float p);

Imath::V3f	lookup3D (const Imath::V3f table[],
			  const Imath::V3i &size,
			  const Imath::V3f &pMin,
			  const Imath::V3f &pMax,
			  const Imath::V3f &p);

} // namespace Ctl

#endif
