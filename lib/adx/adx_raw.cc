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

#include <adx.hh>
#include "adx_raw.hh"
#include <string.h>

namespace ctl {

namespace adxi {

uint32_t read_uint32(std::istream *is, bool need_byteswap) {
	uint32_t i;

	is->read((char *)&i, sizeof(i));

	if(need_byteswap) {
		swap32(&i);
	}

	// if (i != 0){
	// 	std::stringstream errMsg = "The Data Sign Shall be Unsigned";
	// 	throw std::exception(this->errMsg.str());
	// }

	return i;
}

uint16_t read_uint16(std::istream *is, bool need_byteswap) {
	uint16_t i;

	is->read((char *)&i, sizeof(i));

	if(need_byteswap) {
		swap16(&i);
	}
	return i;
}

uint8_t read_uint8(std::istream *is, bool need_byteswap) {
	uint8_t i;

	is->read((char *)&i, sizeof(i));

	return i;
}

float32_t read_float32(std::istream *is, bool need_byteswap) {
	float32_t f;

	is->read((char *)&f, sizeof(f));

	if(need_byteswap) {
		swap32(&f);
	}
	return f;
}

void read_string(std::istream *is, char *bytes, int len_plus_one)
{
	char *c;

	memset(bytes, 0, len_plus_one);
	is->read(bytes, len_plus_one-1);

	c=bytes;
	while(c<(bytes+len_plus_one)) {
		if(*c==0) {
			memset(c, 0, len_plus_one-(c-bytes));
			break;
		}
		c++;
	}
}

void write_uint32(std::ostream *os, uint32_t i, bool need_byteswap) {
	if(need_byteswap) {
		swap32(&i);
	}

	os->write((char *)&i, sizeof(i));
}

void write_uint16(std::ostream *os, uint16_t i, bool need_byteswap) {
	if(need_byteswap) {
		swap16(&i);
	}

	os->write((char *)&i, sizeof(i));
}

void write_uint8(std::ostream *os, uint8_t i, bool need_byteswap) {
	os->write((char *)&i, sizeof(i));
}

void write_float32(std::ostream *os, float32_t f, bool need_byteswap) {
	if(need_byteswap) {
		swap32(&f);
	}

	os->write((char *)&f, sizeof(f));
}

void write_string(std::ostream *os, char *bytes, int len_plus_one) {
	char *c;
	c=bytes;
	// Don't need null termiate fields, but we make sure that
	// if there's a null everything after it is zero.
	while(c<(bytes+len_plus_one)) {
		if(*c==0) {
			memset(c, 0, len_plus_one-(c-bytes));
			break;
		}
		c++;
	}
	os->write(bytes, len_plus_one-1);
}

void write_fill(std::ostream *os, char b, int count) {
	while(count-->0) {
		os->write((char *)&b, 1);
	}
}

}

void adx::nullify(float32_t *v) {
	*((uint32_t *)v)=(uint32_t)-1;
}

void adx::nullify(uint8_t *v) {
	*v=(uint8_t)-1;
}

void adx::nullify(uint16_t *v) {
	*v=(uint16_t)-1;
}

void adx::nullify(uint32_t *v) {
	*v=(uint32_t)-1;
}

void adx::nullify(uint64_t *v) {
	*v=(uint64_t)-1;
}

bool adx::isnull(uint64_t v) {
	return v==(uint64_t)-1;
}

bool adx::isnull(uint32_t v) {
	return v==(uint32_t)-1;
}

bool adx::isnull(uint16_t v) {
	return v==(uint16_t)-1;
}

bool adx::isnull(uint8_t v) {
	return v==(uint8_t)-1;
}

bool adx::isnull(float32_t v) {
	return *((uint32_t *)&v)==(uint32_t)-1;
}

}
