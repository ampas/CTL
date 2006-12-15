#ifndef INCLUDED_DPX_HEADER_H
#define INCLUDED_DPX_HEADER_H

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
//	DPX file header layout.
//	For more information, see SMPTE standard 268M.
//
//----------------------------------------------------------------------------


typedef char ASCII;
typedef unsigned char U8;
typedef unsigned char U16[2];
typedef unsigned char U32[4];
typedef unsigned char R32[4];


enum ByteOrder {BO_LITTLE, BO_BIG};


void setU16 (unsigned short x, U16 u16, ByteOrder bo);
unsigned short getU16 (U16 u16, ByteOrder bo);
void setUndefinedU16 (U16 u16);
bool isUndefinedU16 (U16 u16);


void setU32 (unsigned int x, U32 u32, ByteOrder bo);
unsigned int getU32 (U32 u32, ByteOrder bo);
void setUndefinedU32 (U32 u32);
bool isUndefinedU32 (U32 u32);


void setR32 (float x, R32 r32, ByteOrder bo);
float getR32 (R32 r32, ByteOrder bo);
void setUndefinedR32 (R32 r32);
bool isUndefinedR32 (R32 r32);


struct FileInformation
{
    U32		magicNumber;
    U32		offsetToImageData;
    ASCII	versionNumber[8];
    U32		fileSize;
    U32		dittoKey;
    U32		genericHeaderSize;
    U32		industryHeaderSize;
    U32		userDataSize;
    ASCII	fileName[100];
    ASCII	creationTime[24];
    ASCII	creator[100];
    ASCII	projectName[200];
    ASCII	copyright[200];
    U32		encryptionKey;
    U8		reserved[104];
};


struct ImageElement
{
    U32		dataSign;
    U32		referenceLowData;
    R32		referenceLowQuantity;
    U32		referenceHighData;
    R32		referenceHighQuantity;
    U8		descriptor;
    U8		transferCharacteristic;
    U8		colorimetricSpecification;
    U8		bitSize;
    U16		packing;
    U16		encoding;
    U32		offsetToData;
    U32		endOfLinePadding;
    U32		endOfImagePadding;
    ASCII	description[32];
};


struct ImageInformation
{
    U16		imageOrientation;
    U16		numberOfElements;
    U32		pixelsPerLine;
    U32		linesPerImageElement;
    ImageElement imageElements[8];
    U8		reserved[52];
};


struct ImageOrientation
{
    U32		xOffset;
    U32		yOffset;
    R32		xCenter;
    R32		yCenter;
    U32		xOrigSize;
    U32		yOrigSize;
    ASCII	sourceImageFileName[100];
    ASCII	creationTime[24];
    ASCII	inputDev[32];
    ASCII	inputSerial[32];
    U16		borderValidity[4];
    U32		pixelAspectRatio[2];
    U8		reserved[28];
};


struct MotionPictureFilmHeader
{
    ASCII	filmManufacturerId[2];
    ASCII	filmType[2];
    ASCII	offset[2];
    ASCII	prefix[6];
    ASCII	count[4];
    ASCII	format[32];
    U32		framePosition;
    U32		sequenceLength;
    U32		heldCount;
    R32		frameRate;
    R32		shutterAngle;
    ASCII	frameIdentification[32];
    ASCII	slateInformation[100];
    U8		reserved[56];
};


struct TelevisionHeader
{
    U32		timeCode;
    U32		userBits;
    U8		interlace;
    U8		fieldNumber;
    U8		videoSignalStandard;
    U8		zero;
    R32		horizontalSamplingRate;
    R32		verticalSamplingRate;
    R32		temporalSamplingRate;
    R32		timeOffset;
    R32		gamma;
    R32		blackLevel;
    R32		blackGain;
    R32		breakpoint;
    R32		referenceWhiteLevelCodeValue;
    R32		integrationTime;
    U8		reserved[76];
};

#endif
