///////////////////////////////////////////////////////////////////////////////
// 
// Copyright (c) 2013 Academy of Motion Picture Arts and Sciences
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
//	DPX file header layout.
//
//----------------------------------------------------------------------------

#include <dpxHeader.h>


void
setU16 (unsigned short x, U16 u16, ByteOrder bo)
{
    if (bo == BO_LITTLE)
    {
	u16[0] = x;
	u16[1] = x >> 8;
    }
    else
    {
	u16[1] = x;
	u16[0] = x >> 8;
    }
}


unsigned short
getU16 (U16 u16, ByteOrder bo)
{
    if (bo == BO_LITTLE)
	return u16[0] | (u16[1] << 8);
    else
	return u16[1] | (u16[0] << 8);
}


void
setUndefinedU16 (U16 u16)
{
    u16[0] = 0xff;
    u16[1] = 0xff;
}


bool
isUndefinedU16 (U16 u16)
{
    return u16[0] == 0xff && u16[1] == 0xff;
}


void
setU32 (unsigned int x, U32 u32, ByteOrder bo)
{
    if (bo == BO_LITTLE)
    {
	u32[0] = x;
	u32[1] = x >> 8;
	u32[2] = x >> 16;
	u32[3] = x >> 24;
    }
    else
    {
	u32[3] = x;
	u32[2] = x >> 8;
	u32[1] = x >> 16;
	u32[0] = x >> 24;
    }
}


unsigned int
getU32 (U32 u32, ByteOrder bo)
{
    if (bo == BO_LITTLE)
	return u32[0] | (u32[1] << 8) | (u32[2] << 16) | (u32[3] << 24);
    else
	return u32[3] | (u32[2] << 8) | (u32[1] << 16) | (u32[0] << 24);
}


void
setUndefinedU32 (U32 u32)
{
    u32[0] = 0xff;
    u32[1] = 0xff;
    u32[2] = 0xff;
    u32[3] = 0xff;
}


bool
isUndefinedU32 (U32 u32)
{
    return u32[0] == 0xff && u32[1] == 0xff && u32[2] == 0xff && u32[3] == 0xff;
}


void
setR32 (float x, R32 r32, ByteOrder bo)
{
    union {float f; unsigned int i;} fi;
    fi.f = x;
    setU32 (fi.i, r32, bo);
}


float
getR32 (R32 r32, ByteOrder bo)
{
    union {float f; unsigned int i;} fi;
    fi.i = getU32 (r32, bo);
    return fi.f;
}


void
setUndefinedR32 (R32 r32)
{
    setUndefinedU32 (r32);
}


bool
isUndefinedR32 (R32 r32)
{
    return isUndefinedU32 (r32);
}
