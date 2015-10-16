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
#include <half.h>
#include <math.h>
#include "adx_bits.hh"
#include "adx_rw.hh"

namespace ctl {
namespace adxi {

// To handle the reads in raw mode (integer types only)...
// T needs to be the same type as indicated by the packing field.
template <class T>
void read_data(std::istream *i, ctl::dpx::fb<T> *buffer, const rwinfo &ri) {
	buffer->init(ri.words_for_raw<T>(), 1, 1);

	read_ptr(i, buffer->ptr(), buffer->count(), ri.need_byteswap);
}

// When we know that the read type and buffer will align with what is on
// the disk.
template <class T>
void read_fb(std::istream *i, ctl::dpx::fb<T> *buffer, const rwinfo &ri) {
	buffer->init(ri.width, ri.height, ri.channels);

	read_ptr(i, buffer->ptr(), buffer->count(), ri.need_byteswap);
}

// Perform bit unpacking...
template <class O, class I>
void unpack(ctl::dpx::fb<O> *out, const I *i, const rwinfo &ri) {
	uint64_t u, v, count;
	uint64_t pad, onbit, down;
	I mask;
	I t;
	O *o;

	out->init(ri.width, ri.height, ri.channels);

	o=out->ptr();

#if 1
	count=out->count();
	if(ri.bps==10) {
		if(ri.pack==1) {
			for(u=0; u<out->pixels(); u++) {
				t=*(i++);
				t=t>>2;
				o[2]=(t&0x3ff); t=t>>10;
				o[1]=(t&0x3ff); t=t>>10;
				o[0]=(t&0x3ff);
				o=o+3;
			}
			return;
		} else if(ri.pack==2) {
			for(u=0; u<out->pixels(); u++) {
				t=*(i++);
				o[2]=(t&0x3ff); t=t>>10;
				o[1]=(t&0x3ff); t=t>>10;
				o[0]=(t&0x3ff);
				o=o+3;
			}
			return;
		}
	}
#endif

	// Yes... We shift off the *top* since we need to keep the pixels in
	// an 'big endian' order (in relationship to the bytestream).
	down=(sizeof(I)*8-ri.bps);
	mask=max_int_for_bits[ri.bps];
	mask=mask<<down;

	if((ri.pack&0x7)==0) {
		// XXX not supported
	} else if((ri.pack&0x7)==1) {
		pad=0;
	} else if((ri.pack&0x7)==2) {
		pad=(sizeof(I)*8)%ri.bps;
	} else {
		// XXX not supported
	}

	count=out->width()*out->depth();
	for(u=0; u<out->height(); u++) {
		onbit=0;
		for(v=0; v<count; v++) {
			if(onbit<ri.bps) {
				t=*(i++);
				t=t<<pad;
				onbit=sizeof(O)*8-pad;
			}
			*(o++)=(t&mask)>>down;
			t=t<<ri.bps;
			onbit=onbit-ri.bps;
		}
	}
}

template <class O, class I>
void read(ctl::dpx::fb<O> *out, const I *in, const rwinfo &ri) {
	ctl::dpx::fb<uint64_t>  upfbu64;	
	ctl::dpx::fb<uint32_t>  upfbu32;	
	ctl::dpx::fb<uint16_t>  upfbu16;	
	ctl::dpx::fb<uint8_t>   upfbu8;	

	// std::cout << "I am here in adx read" << std::endl;

	if(ri.bps<=sizeof(I)*8) {
		// not good...
	}
	if(ri.datatype==0) {
		if(((ri.pack&0x7)==1 || (ri.pack&0x7)==2) && ((sizeof(I)*8)%ri.bps)!=0) {
			if(ri.bps<=8) {
				unpack(&upfbu8, in, ri);
				convertfb(out, sizeof(O)*8, upfbu8.ptr(), ri.bps, ri.scale);
			} else if(ri.bps<=16) {
				unpack(&upfbu16, in, ri);
				convertfb(out, sizeof(O)*8, upfbu16.ptr(), ri.bps, ri.scale);
			} else if(ri.bps<=32) {
				unpack(&upfbu32, in, ri);
				convertfb(out, sizeof(O)*8, upfbu32.ptr(), ri.bps, ri.scale);
			} else if(ri.bps<=64) {
				unpack(&upfbu64, in, ri);
				convertfb(out, sizeof(O)*8, upfbu64.ptr(), ri.bps, ri.scale);
			} else {
				// XXX badness...
			}
		} else if((ri.pack&0x7)>=0 && (ri.pack&0x7)<=2 &&
		          sizeof(I)*8%ri.bps==0) {
			convertfb(out, sizeof(O)*8, in, ri.bps, ri.scale);
		} else {
			// XXX
		}
	} else if(ri.datatype==2) {
		switch(ri.bps) {
			case 16:
				convertfb(out, sizeof(O)*8, (float16_t *)in, ri.bps, ri.scale);
				break;

			case 32:
				convertfb(out, sizeof(O)*8, (float32_t *)in, ri.bps, ri.scale);
				break;

			case 64:
				convertfb(out, sizeof(O)*8, (float64_t *)in, ri.bps, ri.scale);
				break;

			default:
				// XXX
				break;
		}
	} else {
		// XXX
	}
}

// Catch-all super-transforming read.
template <class T>
void read(std::istream *i, ctl::dpx::fb<T> *out, const rwinfo &ri) {
	ctl::dpx::fb<uint64_t>  fbu64;
	ctl::dpx::fb<uint32_t>  fbu32;
	ctl::dpx::fb<uint16_t>  fbu16;
	ctl::dpx::fb<uint8_t>   fbu8;
	rwinfo new_ri;

	// ri.datatype==0 unsigned integer
	// ri.datatype==1 signed integer (unsupported)
	// ri.datatype==2 floating point

	if(ri.version==0x30) {
		// read size determined by packing...
		if(ri.pack<8) {
			read_data(i, &fbu32, ri);
			read(out, fbu32.ptr(), ri);
		} else if(ri.pack<16) {
			read_data(i, &fbu16, ri);
			read(out, fbu16.ptr(), ri);
		} else if(ri.pack<24) {
			read_data(i, &fbu8, ri);
			read(out, fbu8.ptr(), ri);
		} else if(ri.pack<32) {
			read_data(i, &fbu64, ri);
			read(out, fbu64.ptr(), ri);
		} else {
			// XXX
		}
	} else {
		// read size determined by bps...
		new_ri=ri;
		// should be redundant... But you never know...
		new_ri.pack=ri.pack&0x07;
		if(ri.bps<=8) {
			read_data(i, &fbu8, new_ri);
			read(out, fbu8.ptr(), new_ri);
		} else if(ri.bps<=10) {
			read_data(i, &fbu32, new_ri);
			read(out, fbu32.ptr(), new_ri);
		} else if(ri.pack<=16) {
			read_data(i, &fbu16, new_ri);
			read(out, fbu16.ptr(), new_ri);
		} else {
			// XXX
		}
	}
}

// This is what all of the 'normal' class read methods call.
template <class T>
void read(std::istream *i, uint8_t element, ctl::dpx::fb<T> *buffer,
          const rwinfo &ri) {
	std::istream::pos_type start;

	start=i->tellg();

	i->seekg(ri.offset_to_data+start);

	if(ri.mode==adx::unformatted) {
		read_data(i, buffer, ri);
	} else {
		buffer->init(ri.width, ri.height, ri.channels);

		if(ri.direct) {
			read_fb(i, buffer, ri);
		} else {
			read(i, buffer, ri);
		}
	}

	i->seekg(start);
}

};

void adx::read(std::istream *i, uint8_t e,
               ctl::dpx::fb<half> *buffer, float64_t scale) {
	adxi::rwinfo ri(this, e, scale, normal, FALSE);

	adxi::read(i, e, buffer, ri);
}

void adx::read(std::istream *i, uint8_t e,
               ctl::dpx::fb<float32_t> *buffer, float64_t scale) {
	adxi::rwinfo ri(this, e, scale, normal, FALSE);

	adxi::read(i, e, buffer, ri);
}

void adx::read(std::istream *i, uint8_t e,
               ctl::dpx::fb<float64_t> *buffer, float64_t scale) {
	adxi::rwinfo ri(this, e, scale, normal, FALSE);

	adxi::read(i, e, buffer, ri);
}

void adx::read(std::istream *i, uint8_t e, ctl::dpx::fb<uint8_t> *buffer,
               float64_t scale, intmode_e mode) {
	adxi::rwinfo ri(this, e, scale, mode, TRUE);

	adxi::read(i, e, buffer, ri);
}

void adx::read(std::istream *i, uint8_t e, ctl::dpx::fb<uint16_t> *buffer,
               float64_t scale, intmode_e mode) {
	adxi::rwinfo ri(this, e, scale, mode, TRUE);

	adxi::read(i, e, buffer, ri);
}

void adx::read(std::istream *i, uint8_t e, ctl::dpx::fb<uint32_t> *buffer,
               float64_t scale, intmode_e mode) {
	adxi::rwinfo ri(this, e, scale, mode, TRUE);

	adxi::read(i, e, buffer, ri);
}

void adx::read(std::istream *i, uint8_t e, ctl::dpx::fb<uint64_t> *buffer,
               float64_t scale, intmode_e mode) {
	adxi::rwinfo ri(this, e, scale, mode, TRUE);

	adxi::read(i, e, buffer, ri);
}

};
