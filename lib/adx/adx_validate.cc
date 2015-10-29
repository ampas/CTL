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


#include "adx_rw.hh"

namespace ctl {
namespace adxi {

void rwinfo::packing_for_bps(uint8_t bps, uint16_t *actual, uint16_t *header) {
	if(bps==8) {
		if(header!=NULL) {
			*header=0;
		} 
		if(actual!=NULL) {
			*actual=16;
		}
	} else if(bps==16) {
		if(header!=NULL) {
			*header=0;
		}
		if(actual!=NULL) {
			*actual=8;
		}
	} else if(bps>=11 && bps<17) {
		if(header!=NULL) {
			*header=1;
		}
		if(actual!=NULL) {
			*actual=9;
		}
	} else if(bps==10) {
		if(header!=NULL) {
			*header=1;
		}
		if(actual!=NULL) {
			*actual=1;
		}
	} else {
		// XXX
	}
}

void rwinfo::validate(adx *h, uint8_t element, uint8_t datatype,
                      uint8_t bps, uint8_t channels, uint64_t width,
                      uint64_t height) {

    unsigned long nan_f = static_cast<unsigned long>(adx::udf32f);
    float32_t nan_vf = *(float32_t *)&nan_f;
    
	// Make sure that all of the fields are filled out correctly...
	if(adx::isnull(h->image_orientation)) {
		h->image_orientation=0;
	}

	if(adx::isnull(h->pixels_per_line)) {
		h->pixels_per_line=width;
	}

	if(adx::isnull(h->lines_per_element)) {
		h->lines_per_element=height;
	}

	if(adx::isnull(h->number_of_elements) || h->number_of_elements<=element) {
		h->number_of_elements=element+1;
	}

	if(h->number_of_elements>2) {
		// XXX too many elements...
		h->_constraint_ok = FALSE;
		fprintf(stderr,
                "WARNING: adx field 18 -> Image Elements shall be\n"
                "1 or 2\n");
		h->number_of_elements = 2;
	}

	if(adx::isnull(h->elements[element].bits_per_sample)) {
		h->elements[element].bits_per_sample=bps;
	}
	else {
		 if(element == 0) {
		 	if (h->elements[element].bits_per_sample != 16
		 		&& h->elements[element].bits_per_sample != 10){
                THROW(Iex::ArgExc, "The Program does not Currently Support This Format");
                exit(1);		     }
		 }
		 else if(element == 1) {
		 	if (h->elements[element].bits_per_sample != 16
		 		&& h->elements[element].bits_per_sample != 10
		 		&& h->elements[element].bits_per_sample != 8
		 		&& h->elements[element].bits_per_sample != 1){
                THROW(Iex::ArgExc, "The Program does not Currently Support This Format");
                exit(1);		     }
		 }

//        std::cout << sizeof(((adx::element_t *) 0)->bits_per_sample) << std::endl;
	}
    
    h->elements[element].data_sign = 0x0;
    h->elements[element].ref_low_data_code = 0x0;
    h->elements[element].ref_low_quantity = nan_vf;

	if (adx::isnull(h->elements[element].ref_high_data_code)){
		if(h->elements[element].bits_per_sample == 16){
			h->elements[element].ref_high_data_code = 0xFFFF;
		}
		else if(h->elements[element].bits_per_sample == 10){
			h->elements[element].ref_high_data_code = 0x3FF;
		}
		else if(element == 1 && h->elements[element].bits_per_sample == 8){
			h->elements[element].ref_high_data_code = 0xFF;
		}
		else if(element == 1 && h->elements[element].bits_per_sample == 1){
			h->elements[element].ref_high_data_code = 0x1;
		}
	}
	else {
		if(element == 0) {
			if(h->elements[element].ref_high_data_code != 0xFFFF
			  && h->elements[element].bits_per_sample == 16) {
	        	h->elements[element].ref_high_data_code = 0xFFFF;
			}
			else if(h->elements[element].ref_high_data_code != 0x3FF
			  && h->elements[element].bits_per_sample == 10){
	        	h->elements[element].ref_high_data_code = 0x3FF;
			}
		}
		else if (element == 1){
			if(h->elements[element].ref_high_data_code != 0xFFFF
			  && h->elements[element].bits_per_sample == 16) {
	        	h->elements[element].ref_high_data_code = 0xFFFF;
			}
			else if(h->elements[element].ref_high_data_code != 0x3FF
			  && h->elements[element].bits_per_sample == 10){
	        	h->elements[element].ref_high_data_code = 0x3FF;
			}
			else if(h->elements[element].ref_high_data_code != 0xFF
			  && h->elements[element].bits_per_sample == 8){
	        	h->elements[element].ref_high_data_code = 0xFF;
			}
			else if(h->elements[element].ref_high_data_code != 0x1
			  && h->elements[element].bits_per_sample == 1){
	        	h->elements[element].ref_high_data_code = 0x1;
			}
		}
	}

	
    h->elements[element].ref_high_quantity = nan_vf;

	if(adx::isnull(h->elements[element].descriptor)) {
		switch(channels) {
			case 1:
				if (element == 1) {
					h->elements[element].descriptor = 4; // Alpha
                }
				break;

			case 3:
				if (element == 0) {
					h->elements[element].descriptor = 50; // RGB
				}
				break;

			default:
				// XXX unsupported
				break;
		}
	}
	else {
		if(static_cast<uint32_t>(element) == 0 
			&& static_cast<uint32_t>(h->elements[element].descriptor) != 0x32) {
            THROW(Iex::ArgExc, "The Program does not Currently Support This Format");
            exit(1);
		}
		else if(static_cast<uint32_t>(element) == 1 
			&& static_cast<uint32_t>(h->elements[element].descriptor) != 0x4) {
            THROW(Iex::ArgExc, "The Program does not Currently Support This Format");
            exit(1);
		}
	}

	if(adx::isnull(h->elements[element].transfer_characteristic)) {
		if(element == 0) {
			h->elements[element].transfer_characteristic = 0xD;
		}
		else if(element == 1) {
			h->elements[element].transfer_characteristic = 0x0;
		}
	}
	else {
		if(element == 0 
			&& static_cast<uint32_t>(h->elements[element].transfer_characteristic) != 0xD) {
			h->elements[element].transfer_characteristic = 0xD;
		}
		else if(element == 1 
			&& static_cast<uint32_t>(h->elements[element].transfer_characteristic) != 0x0) {
			h->elements[element].transfer_characteristic = 0x0;
		}
	}

    h->elements[element].colorimetric_characteristic=
    h->elements[element].transfer_characteristic;

	// We have an 'actual_packing'Field which drives the writer to
	// specify where the byte swaps occur, and a 'packing' field which
	// is what is put in the header. We can thank the excessively vague
	// adx 1.0 standard for this.
	if(adx::isnull(h->elements[element].packing)) {
		if(h->compliance==adx::adx1) {
			packing_for_bps(h->elements[element].bits_per_sample,
			                &(h->elements[element].actual_packing),
			                &(h->elements[element].packing));
        }
    }
    else if(h->elements[element].bits_per_sample==16) {
        // historic reasons... If we don't do this we'll
        // break every dpx 1.0 reader known to man
        h->elements[element].packing=0;
    } else if(h->elements[element].bits_per_sample==10) {
        h->elements[element].packing=1;
    } else if(h->elements[element].bits_per_sample==8
              && element == 1) {
        h->elements[element].packing=0;
    } else if(h->elements[element].bits_per_sample==1
              && element == 1) {
        h->elements[element].packing=0;
    } else {
        // XXX
    }

	if(adx::isnull(h->elements[element].actual_packing)) {
		h->elements[element].actual_packing=h->elements[element].packing;
	}

	if(adx::isnull(h->elements[element].encoding)) {
		h->elements[element].encoding=0;
	}

	if(adx::isnull(h->elements[element].eol_padding)) {
		h->elements[element].eol_padding=0;
	}

	if(adx::isnull(h->elements[element].eoi_padding)) {
		h->elements[element].eoi_padding=0;
	}

	// Now we start doing general validation (i.e. things that won't
	// possibly fly no mater what the validation_level is)
	rwinfo info(h, element, 0, adx::normal, FALSE);

	if(h->image_orientation>=8) {
		// XXX invalid image_orientation
	}

	if(h->pixels_per_line!=width) {
		// XXX invalid width
	}

	if(h->lines_per_element!=height) {
		// XXX invalid height
	}

	if(info.channels!=channels) {
		// XXX descriptor doesn't match channel count.
	}

	if(bps>(info.bytes_per_swap*8)) {
		// sample doesn't fit in swap boundary.
	}

	if((info.bytes_per_swap*8)%bps) {
		if((h->elements[element].packing&0x7)==0) {
			// Invalid packing
		}
	} else {
		if((h->elements[element].packing&0x7)>=3) {
			// Invalid packing
		}
	}

	if(h->elements[element].eol_padding!=0) {
		// EOL padding must be 0
	}

	if(h->elements[element].eoi_padding!=0) {
		// EOI padding must be 0
	}

	if(h->elements[element].encoding!=0) {
		// encoding must be 0
	}

	if(h->elements[element].data_sign!=0 &&
	   h->elements[element].data_sign!=2) {
		// invalid data_sign
	}

    // Constraints for ADX 60-62

    h->interlace = static_cast<uint8_t>(adx::udf8);
    h->field_number = static_cast<uint8_t>(adx::udf8);
    h->video_standard = static_cast<uint8_t>(adx::udf8);

    // Constraints for ADX 64-73
    h->horizontal_sampling_rate = nan_vf;
    h->temporal_sampling_rate = nan_vf;
    h->time_offset_sync_to_first_pixel = nan_vf;
    h->gamma = nan_vf;
    h->black_level_code = nan_vf;
    h->black_gain = nan_vf;
    h->breakpoint = nan_vf;
    h->white_level_code = nan_vf;
    h->integration_time = nan_vf;
}

void rwinfo::validate_adx_strict(adx *h, uint8_t element) {
	// We don't do full validation yet...
	validate_adx(h, element);
}

void rwinfo::validate_adx(adx *h, uint8_t element) {
}

// void rwinfo::validate_dpx(dpx *h, uint8_t element) {
// }

// void rwinfo::validate_dpx3(dpx *h, uint8_t element) {
// }

}; 

};
