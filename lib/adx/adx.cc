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

#include "adx.hh"
#include "adx_raw.hh"
#include "adx_bits.hh"
#include <string.h>

#if !defined(TRUE)
#define TRUE 1
#endif
#if !defined(FALSE)
#define FALSE 0
#endif

namespace ctl
{

adx::adx() {
	endian_mode=default_endian_mode;
	compliance=automatic;

	clear();

	header_start=0;
	_need_byteswap=FALSE;
	_constraint_ok=TRUE;
	current_ostream=FALSE;
	current_endian_mode=default_endian_mode;
	current_compliance=automatic;
}

adx::~adx() {
}

void adx::clear(void) {
	uint8_t i;

	nullify(&magic);
	magic=0x53445058;
	nullify(&data_offset);
	memset(header_version, 0, sizeof(header_version));
	nullify(&total_file_size);
	nullify(&ditto_key);
	nullify(&generic_header_length);
	nullify(&industry_header_length);
	nullify(&user_header_length);

	memset(filename, 0, sizeof(filename));
	memset(creation_time, 0, sizeof(creation_time));
	memset(creator, 0, sizeof(creator));
	memset(project_name, 0, sizeof(project_name));
	memset(copyright, 0, sizeof(copyright));
	nullify(&encryption_key);

	nullify(&image_orientation);
	nullify(&number_of_elements);
	nullify(&pixels_per_line);
	nullify(&lines_per_element);

	for(i=0; i<8; i++) {
		nullify(&(elements[i].data_sign));
		nullify(&(elements[i].ref_low_data_code));
		nullify(&(elements[i].ref_low_quantity));
		nullify(&(elements[i].ref_high_data_code));
		nullify(&(elements[i].ref_high_quantity));
		nullify(&(elements[i].descriptor));
		nullify(&(elements[i].transfer_characteristic));
		nullify(&(elements[i].colorimetric_characteristic));
		nullify(&(elements[i].bits_per_sample));
		nullify(&(elements[i].packing));
		nullify(&(elements[i].actual_packing));
		nullify(&(elements[i].encoding));
		nullify(&(elements[i].offset_to_data));
		nullify(&(elements[i].eol_padding));
		nullify(&(elements[i].eoi_padding));
		memset(elements[i].description, 0, sizeof(elements[i].description));
	};

	nullify(&x_offset);
	nullify(&y_offset);
	nullify(&x_center);
	nullify(&y_center);
	nullify(&x_origional_size);
	nullify(&y_origional_size);

	memset(source_filename, 0, sizeof(source_filename));
	memset(source_creation_time, 0, sizeof(source_creation_time));
	memset(input_device, 0, sizeof(input_device));
	memset(input_device_serial_number, 0, sizeof(input_device_serial_number));

	nullify(&xl_border_validity);
	nullify(&xr_border_validity);
	nullify(&yt_border_validity);
	nullify(&yb_border_validity);
	nullify(&horizonal_par);
	nullify(&vertical_par);

	nullify(&x_scanned_size);
	nullify(&y_scanned_size);

	memset(keycode_film_id, 0, sizeof(keycode_film_id));
	memset(keycode_film_type_char, 0, sizeof(keycode_film_type_char));
	memset(keycode_perf_offset_char, 0, sizeof(keycode_perf_offset_char));
	memset(keycode_prefix, 0, sizeof(keycode_prefix));
	memset(keycode_count_char, 0, sizeof(keycode_count_char));

	memset(format, 0, sizeof(format));
	nullify(&frame_position_in_sequence);
	nullify(&sequence_length);
	nullify(&held_count);
	nullify(&frame_rate);
	nullify(&shutter_angle);
	memset(frame_attribute, 0, sizeof(frame_attribute));
	memset(frame_slate_info, 0, sizeof(frame_slate_info));
//	nullify(&film_gague);
// 	nullify(&film_orientation);
//	nullify(&film_perf_per_foot);
//	nullify(&film_perf_per_frame);
//	memset(film_manufacturer, 0, sizeof(film_manufacturer));
//	memset(film_stock_name, 0, sizeof(film_stock_name));

	nullify(&smpte_timecode);
	nullify(&smpte_userbits);
	nullify(&interlace);
	nullify(&field_number);
	nullify(&video_standard);
	nullify(&horizontal_sampling_rate);
	nullify(&vertical_sampling_rate);
	nullify(&temporal_sampling_rate);
	nullify(&time_offset_sync_to_first_pixel);
	nullify(&gamma);
	nullify(&black_level_code);
	nullify(&black_gain);
	nullify(&breakpoint);
	nullify(&white_level_code);
	nullify(&integration_time);

	memset(user_id, 0, sizeof(user_id));
}

bool adx::check_magic(std::istream *io) {
	uint32_t magic;
	std::istream::pos_type start;

	start=io->tellg();
	io->read((char *)&magic, sizeof(magic));
	io->seekg(start);

	if(magic==0x53445058 || magic==0x58504453) {
		return TRUE;
	}
	return FALSE;
}

bool adx::check_constraint(void) {
    if (this->_constraint_ok) {
		return TRUE;
    }
    return FALSE;
}

void adx::read(std::istream *is) {
    std::istream::pos_type start;
    bool cpu_is_little_endian;
    int i;
    
    magic=0x01020304;
    cpu_is_little_endian=FALSE;
    if(((const char *)&magic)[0]==0x04) {
        cpu_is_little_endian=TRUE;
    }
    
    clear();
    
    header_start=is->tellg();
    magic=adxi::read_uint32(is, 0);
    if(magic==0x53445058) {
        _need_byteswap=FALSE;
        if(cpu_is_little_endian) {
            endian_mode=little_endian;
        } else {
            endian_mode=big_endian;
        }
    } else {
        is->seekg(header_start);
        magic=adxi::read_uint32(is, 1);
        if(magic!=0x53445058) {
            // XXX This is bad... Need to fail nicely..
        }
        _need_byteswap=TRUE;
        if(cpu_is_little_endian) {
            endian_mode=big_endian;
        } else {
            endian_mode=little_endian;
        }
    }
    
    data_offset=adxi::read_uint32(is, _need_byteswap);
    adxi::read_string(is, header_version, sizeof(header_version));
    total_file_size=adxi::read_uint32(is, _need_byteswap);
    ditto_key=adxi::read_uint32(is, _need_byteswap);
    generic_header_length=adxi::read_uint32(is, _need_byteswap);
    industry_header_length=adxi::read_uint32(is, _need_byteswap);
    user_header_length=adxi::read_uint32(is, _need_byteswap);
    
    adxi::read_string(is, filename, sizeof(filename));
    adxi::read_string(is, creation_time, sizeof(creation_time));
    adxi::read_string(is, creator, sizeof(creator));
    adxi::read_string(is, project_name, sizeof(project_name));
    adxi::read_string(is, copyright, sizeof(copyright));
    encryption_key=adxi::read_uint32(is, _need_byteswap);
    
    is->ignore(104);
    
    image_orientation=adxi::read_uint16(is, _need_byteswap);
    number_of_elements=adxi::read_uint16(is, _need_byteswap);
    pixels_per_line=adxi::read_uint32(is, _need_byteswap);
    lines_per_element=adxi::read_uint32(is, _need_byteswap);
    
    // just read the first two elements
    for(i=0; i<2; i++) {
        elements[i].data_sign=adxi::read_uint32(is, _need_byteswap);
        elements[i].ref_low_data_code=adxi::read_uint32(is, _need_byteswap);
        elements[i].ref_low_quantity=adxi::read_float32(is, _need_byteswap);
        elements[i].ref_high_data_code=adxi::read_uint32(is, _need_byteswap);
        elements[i].ref_high_quantity=adxi::read_float32(is, _need_byteswap);
        elements[i].descriptor=adxi::read_uint8(is, _need_byteswap);
        elements[i].transfer_characteristic=adxi::read_uint8(is, _need_byteswap);
        elements[i].colorimetric_characteristic=adxi::read_uint8(is, _need_byteswap);
        elements[i].bits_per_sample=adxi::read_uint8(is, _need_byteswap);
        elements[i].packing=adxi::read_uint16(is, _need_byteswap);
        elements[i].encoding=adxi::read_uint16(is, _need_byteswap);
        elements[i].offset_to_data=adxi::read_uint32(is, _need_byteswap);
        if(i<number_of_elements &&
           (elements[i].offset_to_data==(uint32_t)-1 ||
            elements[i].offset_to_data==0)) {
               // Both of the above have been seen in the wild. And both
               // are non-spec (since offset_to_data is a core field it canot
               // be marked as UNDEF, and the spec states that offset_to_data
               // is relative to file start, not the data_offset field). The
               // second (arguably more problematic) is generated by cinepaint.
               elements[i].offset_to_data=data_offset;
           }
        elements[i].eol_padding=adxi::read_uint32(is, _need_byteswap);
        elements[i].eoi_padding=adxi::read_uint32(is, _need_byteswap);
        adxi::read_string(is, elements[i].description,
                          sizeof(elements[i].description));
    };
    
    is->ignore(52);
    
    x_offset=adxi::read_uint32(is, _need_byteswap);
    y_offset=adxi::read_uint32(is, _need_byteswap);
    x_center=adxi::read_float32(is, _need_byteswap);
    y_center=adxi::read_float32(is, _need_byteswap);
    x_origional_size=adxi::read_uint32(is, _need_byteswap);
    y_origional_size=adxi::read_uint32(is, _need_byteswap);
    
    adxi::read_string(is, source_filename, sizeof(source_filename));
    adxi::read_string(is, source_creation_time, sizeof(source_creation_time));
    adxi::read_string(is, input_device, sizeof(input_device));
    adxi::read_string(is, input_device_serial_number,
                      sizeof(input_device_serial_number));
    
    xl_border_validity=adxi::read_uint16(is, _need_byteswap);
    xr_border_validity=adxi::read_uint16(is, _need_byteswap);
    yt_border_validity=adxi::read_uint16(is, _need_byteswap);
    yb_border_validity=adxi::read_uint16(is, _need_byteswap);
    horizonal_par=adxi::read_uint32(is, _need_byteswap);
    vertical_par=adxi::read_uint32(is, _need_byteswap);
    
    x_scanned_size=adxi::read_float32(is, _need_byteswap);
    y_scanned_size=adxi::read_float32(is, _need_byteswap);
    
    is->ignore(20);
    
    adxi::read_string(is, keycode_film_id, sizeof(keycode_film_id));
    adxi::read_string(is, keycode_film_type_char,
                      sizeof(keycode_film_type_char));
    adxi::read_string(is, keycode_perf_offset_char,
                      sizeof(keycode_perf_offset_char));
    adxi::read_string(is, keycode_prefix, sizeof(keycode_prefix));
    adxi::read_string(is, keycode_count_char, sizeof(keycode_count_char));
    
    adxi::read_string(is, format, sizeof(format));
    frame_position_in_sequence=adxi::read_uint32(is, _need_byteswap);
    sequence_length=adxi::read_uint32(is, _need_byteswap);
    held_count=adxi::read_uint32(is, _need_byteswap);
    frame_rate=adxi::read_float32(is, _need_byteswap);
    shutter_angle=adxi::read_float32(is, _need_byteswap);
    adxi::read_string(is, frame_attribute, sizeof(frame_attribute));
    adxi::read_string(is, frame_slate_info, sizeof(frame_slate_info));
    film_gague=adxi::read_float32(is, _need_byteswap);
    film_orientation=adxi::read_uint8(is, _need_byteswap);
    film_perf_per_foot=adxi::read_uint8(is, _need_byteswap);
    film_perf_per_frame=adxi::read_uint8(is, _need_byteswap);
    adxi::read_string(is, film_manufacturer, sizeof(film_manufacturer));
    adxi::read_string(is, film_stock_name, sizeof(film_stock_name));
    
    is->ignore(21);
    
    smpte_timecode=adxi::read_uint32(is, _need_byteswap);
    smpte_userbits=adxi::read_uint32(is, _need_byteswap);
    interlace=adxi::read_uint8(is, _need_byteswap);
    field_number=adxi::read_uint8(is, _need_byteswap);
    video_standard=adxi::read_uint8(is, _need_byteswap);
    is->ignore(1);
    horizontal_sampling_rate=adxi::read_float32(is, _need_byteswap);
    vertical_sampling_rate=adxi::read_float32(is, _need_byteswap);
    temporal_sampling_rate=adxi::read_float32(is, _need_byteswap);
    time_offset_sync_to_first_pixel=adxi::read_float32(is, _need_byteswap);
    gamma=adxi::read_float32(is, _need_byteswap);
    black_level_code=adxi::read_float32(is, _need_byteswap);
    black_gain=adxi::read_float32(is, _need_byteswap);
    breakpoint=adxi::read_float32(is, _need_byteswap);
    white_level_code=adxi::read_float32(is, _need_byteswap);
    integration_time=adxi::read_float32(is, _need_byteswap);
    
    is->ignore(76);
    adxi::read_string(is, user_id, sizeof(user_id));
    
    is->seekg(header_start);
    // struct tm source_creation_time_tm;
    // time_t source_creation_time_time;
    // uint8_t keycode_perf_offset_int;
    // uint16_t keycode_film_type_int;
    // uint8_t keycode_prefix_int;
    // uint16_t keycode_count_int;
    // uint64_t keycode_absolute_perf_offset;
    // struct tm creation_time_tm;
    // time_t creation_time_time;
}

void adx::write(std::ostream *os)
{
	uint32_t _magic;
	bool cpu_is_little_endian;
	int i;

	_magic=0x01020304;
	cpu_is_little_endian=FALSE;
	if(((const char *)&_magic)[0]==0x04) {
		cpu_is_little_endian=TRUE;
	}

	_need_byteswap=FALSE;
	if(cpu_is_little_endian != little_endian) {
		_need_byteswap=TRUE;
	}

	// struct tm source_creation_time_tm;
	// time_t source_creation_time_time;
    // uint8_t keycode_perf_offset_int;
	// uint16_t keycode_film_type_int;
    // uint8_t keycode_prefix_int;
    // uint16_t keycode_count_int;
	// uint64_t keycode_absolute_perf_offset;
	// struct tm creation_time_tm;
	// time_t creation_time_time;

	os->seekp(header_start, std::ios::beg);

	if(header_version[0]==0) {
		if(compliance==adx1) {
			strcpy(header_version, "v1.0");
		} else if(compliance==adx3) {
			strcpy(header_version, "v3.0");
		} else {
 			// XXX invalid validation_level
		}
	}

	magic=0x53445058;
	adxi::write_uint32(os, magic, _need_byteswap);
	adxi::write_uint32(os, data_offset, _need_byteswap);
	adxi::write_string(os, header_version, sizeof(header_version));
	adxi::write_uint32(os, total_file_size, _need_byteswap);
	adxi::write_uint32(os, ditto_key, _need_byteswap);
	adxi::write_uint32(os, generic_header_length, _need_byteswap);
	adxi::write_uint32(os, industry_header_length, _need_byteswap);
	adxi::write_uint32(os, user_header_length, _need_byteswap);

	adxi::write_string(os, filename, sizeof(filename));
	adxi::write_string(os, creation_time, sizeof(creation_time));
	adxi::write_string(os, creator, sizeof(creator));
	adxi::write_string(os, project_name, sizeof(project_name));
	adxi::write_string(os, copyright, sizeof(copyright));
	adxi::write_uint32(os, encryption_key, _need_byteswap);

	adxi::write_fill(os, 0x00, 104);

	adxi::write_uint16(os, image_orientation, _need_byteswap);
    
    // adx field 18 -> Image Elements shall be 1 or 2
    // Don't change the actual data
    if(static_cast<uint32_t>(number_of_elements) != 1
       && static_cast<uint32_t>(number_of_elements) != 2){
        this->_constraint_ok = FALSE;
    }

    if (number_of_elements <= 0x1){
        adxi::write_uint16(os, 0x1, _need_byteswap);
    }
    else if (number_of_elements > 0x1) {
        number_of_elements = 0x2;
        adxi::write_uint16(os, 0x2, _need_byteswap);
    }

	adxi::write_uint32(os, pixels_per_line, _need_byteswap);
	adxi::write_uint32(os, lines_per_element, _need_byteswap);
    
    uint8_t alphaElement = 1;
    bool hasAlphaElement = 0;
	for(i=0; i<8; i++) {
        if (static_cast<uint32_t>(elements[i].descriptor) == 13) {
            hasAlphaElement = 1;
            alphaElement = i;
        }
        
        break;
    };
    
    // the First Element
    // adx 21.1 Data Sign shall be Unsigned
    if (elements[0].data_sign != 0x0){
        this->_constraint_ok = FALSE;
    }
    adxi::write_uint32(os, 0x0, _need_byteswap);
    
    // adx 21.2 Reference low data code shall be 0
    if (elements[0].ref_low_data_code != 0x0) {
        this->_constraint_ok = FALSE;
    }
    adxi::write_uint32(os, 0x0, _need_byteswap);
    
    unsigned long nan_f = static_cast<unsigned long>(udf32f);
    float32_t nan_vf = *(float32_t *)&nan_f;

    // adx 21.3 Reference low quantity represented shall be Undefined
    if (elements[0].ref_low_quantity != nan_vf) {
        this->_constraint_ok = FALSE;
    }
    adxi::write_float32(os, nan_vf, _need_byteswap);

    // adx 21.4 Reference high data code value for ADX16 shall be 65535 / 0xFFFF
    //          Reference high data code value for ADX10 shall be 1023 / 0x3FF
    if (elements[0].ref_high_data_code != 0xFFFF
        && elements[0].ref_high_data_code != 0x3FF) {
        this->_constraint_ok = FALSE;
    }
   
    if (static_cast<uint32_t>(elements[0].bits_per_sample) == 10){
        adxi::write_uint32(os, 0x3FF, _need_byteswap);
    }
    else if (static_cast<uint32_t>(elements[0].bits_per_sample) == 16){
        adxi::write_uint32(os, 0xFFFF, _need_byteswap);
    }
    else {
        exit(1);
    }

    // adx 21.5 Reference high quantity represented shall be Undefined
    if (elements[0].ref_high_quantity != nan_vf) {
        this->_constraint_ok = FALSE;
    }
    adxi::write_float32(os, nan_vf, _need_byteswap);

    // adx 21.6 Channel Descriptor shall be R, G, B /50(0x32)
    if (static_cast<uint32_t>(elements[0].descriptor) != 0x32){
        this->_constraint_ok = FALSE;
    }
    adxi::write_uint8(os, 0x32, _need_byteswap);
    
    // adx 21.7 Transfer characteristic shall be ADX/13(0xD)
    if (static_cast<uint32_t>(elements[0].transfer_characteristic) != 0xD){
        this->_constraint_ok = FALSE;
    }
    adxi::write_uint8(os, 0xD, _need_byteswap);

    // adx 21.8 Colorimetric specification shall be ADX/13(0xD)
    if (static_cast<uint32_t>(elements[0].colorimetric_characteristic) != 0xD) {
        this->_constraint_ok = FALSE;
    }
    adxi::write_uint8(os, 0xD, _need_byteswap);

    // adx 21.9 Bit Depth shall be 16 for ADX16 and 10 for ADX10
    if (static_cast<uint32_t>(elements[0].bits_per_sample) == 10
        || static_cast<uint32_t>(elements[0].bits_per_sample) == 16){
        adxi::write_uint8(os, elements[0].bits_per_sample, _need_byteswap);
    }
    
    // adx 21.10 Packing shall be 0 for ADX16 and the datum shall be packed sequentially 
    // 16 bits per sample (i.e. no padding shall be used) Packing shall be 1 for ADX10 and 
    // the datum shall be packed three samples into a 32-bit word with 2 pad bits located in LSB.
    if (static_cast<uint32_t>(elements[0].bits_per_sample) == 16) {
        if (elements[0].packing != 0x0) {
            this->_constraint_ok = FALSE;
        }
        adxi::write_uint16(os, 0x0, _need_byteswap);
    }
    else if (static_cast<uint32_t>(elements[0].bits_per_sample) == 10) {
        if (elements[0].packing != 0x1) {
            this->_constraint_ok = FALSE;
        }
        adxi::write_uint16(os, 0x1, _need_byteswap);
    }
    else {
        adxi::write_uint16(os, elements[0].packing, _need_byteswap);
    }

    
    adxi::write_uint16(os, elements[0].encoding, _need_byteswap);
    adxi::write_uint32(os, elements[0].offset_to_data, _need_byteswap);
    adxi::write_uint32(os, elements[0].eol_padding, _need_byteswap);
    adxi::write_uint32(os, elements[0].eoi_padding, _need_byteswap);
    adxi::write_string(os, elements[0].description,
                       sizeof(elements[0].description));


    // The Second Element
    if (hasAlphaElement) {
        // adx 22.1 Data Sign shall be Unsigned
        if (elements[alphaElement].data_sign != 0x0){
            this->_constraint_ok = FALSE;
        }
        adxi::write_uint32(os, 0x0, _need_byteswap);
        
        // adx 22.2 Reference low data code shall be 0
        if (elements[alphaElement].ref_low_data_code != 0x0) {
            this->_constraint_ok = FALSE;
        }
        adxi::write_uint32(os, 0x0, _need_byteswap);

        // adx 22.3 Reference low quantity represented shall be Undefined
        if (elements[alphaElement].ref_low_quantity != nan_vf) {
            this->_constraint_ok = FALSE;
        }
        adxi::write_float32(os, nan_vf, _need_byteswap);
        
        // adx 22.4 Reference high data code value for ADX16 shall be 65535 / 0xFFFF for 16
        // 			Reference high data code value for ADX10 shall be 1023   / 0x3FF for 10
        // 			Reference high data code value for ADX10 shall be 255      / 0xFF for 8
        // 			Reference high data code value for ADX10 shall be 1         / 0x1 for 1
        if (elements[alphaElement].bits_per_sample == 16){
            if (elements[alphaElement].ref_high_data_code != 0xFFFF) {
                this->_constraint_ok = FALSE;
            }
            adxi::write_uint32(os, 0xFFFF, _need_byteswap);
        }
        else if (elements[alphaElement].bits_per_sample == 10){
            if (elements[alphaElement].ref_high_data_code != 0x3FF) {
                this->_constraint_ok = FALSE;
            }
            adxi::write_uint32(os, 0x3FF, _need_byteswap);
        }
        else if (elements[alphaElement].bits_per_sample == 8){
            if (elements[alphaElement].ref_high_data_code != 0xFF) {
                this->_constraint_ok = FALSE;
            }
            adxi::write_uint32(os, 0xFF, _need_byteswap);
        }
        else if (elements[alphaElement].bits_per_sample == 1){
            if (elements[alphaElement].ref_high_data_code != 0x1) {
                this->_constraint_ok = FALSE;
            }
            adxi::write_uint32(os, 0x1, _need_byteswap);
        }
        else {
            this->_constraint_ok = FALSE;
        }

        // adx 22.5 Reference high quantity represented shall be Undefined
        if (elements[alphaElement].ref_high_quantity != nan_vf) {
            this->_constraint_ok = FALSE;
        }
        adxi::write_float32(os, nan_vf, _need_byteswap);

        // adx 22.6 Channel Descriptor shall be Alpha (matte) / 4
        if (static_cast<uint32_t>(elements[alphaElement].descriptor) != 0x4){
            exit(1);
        }
        adxi::write_uint8(os, 0x4, _need_byteswap);

        // adx 22.7 Transfer characteristic shall be User defined/0
        if (static_cast<uint32_t>(elements[alphaElement].transfer_characteristic) != 0x0){
            this->_constraint_ok = FALSE;
        }
        adxi::write_uint8(os, 0x0, _need_byteswap);

        // adx 22.8 Colorimetric specification shall be User defined/0
        if (static_cast<uint32_t>(elements[alphaElement].colorimetric_characteristic) != 0x0) {
            this->_constraint_ok = FALSE;
        }
        adxi::write_uint8(os, 0x0, _need_byteswap);

        // adx 22.9 Bit Depth shall be 1, 8, 10, or 16
        if (elements[alphaElement].bits_per_sample == 16
            || elements[alphaElement].bits_per_sample == 10
            || elements[alphaElement].bits_per_sample == 8
            || elements[alphaElement].bits_per_sample == 1) {
            adxi::write_uint8(os, elements[alphaElement].bits_per_sample, _need_byteswap);
        }
        else {
            exit(1);
        }
        
        // adx 22.10 Padding shall be 0 when the value of 22.9 is 1, 8, or 16.
        // Padding shall be 1 when the value of 22.9 is 10 and the datum shall 
        // be packed three samples into a 32-bit word with 2 pad bits located in LSB.
        if (static_cast<uint32_t>(elements[alphaElement].bits_per_sample) == 16) {
            if (elements[alphaElement].packing != 0x0) {
                this->_constraint_ok = FALSE;
            }
            adxi::write_uint16(os, 0x0, _need_byteswap);
        }
        else if (static_cast<uint32_t>(elements[alphaElement].bits_per_sample) == 8) {
            if (elements[alphaElement].packing != 0x0) {
                this->_constraint_ok = FALSE;
            }
            adxi::write_uint16(os, 0x0, _need_byteswap);
        }
        else if (static_cast<uint32_t>(elements[alphaElement].bits_per_sample) == 1) {
            if (elements[alphaElement].packing != 0x0) {
                this->_constraint_ok = FALSE;
            }
            adxi::write_uint16(os, 0x0, _need_byteswap);
        }
        else if (static_cast<uint32_t>(elements[alphaElement].bits_per_sample) == 10) {
            if (elements[alphaElement].packing != 0x1) {
                this->_constraint_ok = FALSE;
            }
            adxi::write_uint16(os, 0x1, _need_byteswap);
        }

        adxi::write_uint16(os, elements[alphaElement].encoding, _need_byteswap);
        adxi::write_uint32(os, elements[alphaElement].offset_to_data, _need_byteswap);
        adxi::write_uint32(os, elements[alphaElement].eol_padding, _need_byteswap);
        adxi::write_uint32(os, elements[alphaElement].eoi_padding, _need_byteswap);
        adxi::write_string(os, elements[alphaElement].description,
                           sizeof(elements[alphaElement].description));

    }
    
    for(i=0; i<8; i++) {
        nullify(&(elements[i].data_sign));
        nullify(&(elements[i].ref_low_data_code));
        nullify(&(elements[i].ref_low_quantity));
        nullify(&(elements[i].ref_high_data_code));
        nullify(&(elements[i].ref_high_quantity));
        nullify(&(elements[i].descriptor));
        nullify(&(elements[i].transfer_characteristic));
        nullify(&(elements[i].colorimetric_characteristic));
        nullify(&(elements[i].bits_per_sample));
        nullify(&(elements[i].packing));
        nullify(&(elements[i].encoding));
        nullify(&(elements[i].offset_to_data));
        nullify(&(elements[i].eol_padding));
        nullify(&(elements[i].eoi_padding));
        memset(elements[i].description, 0,
               sizeof(elements[i].description));
    };

	adxi::write_fill(os, 0x00, 52);
	adxi::write_uint32(os, x_offset, _need_byteswap);
	adxi::write_uint32(os, y_offset, _need_byteswap);
	adxi::write_float32(os, x_center, _need_byteswap);
	adxi::write_float32(os, y_center, _need_byteswap);
	adxi::write_uint32(os, x_origional_size, _need_byteswap);
	adxi::write_uint32(os, y_origional_size, _need_byteswap);

	adxi::write_string(os, source_filename, sizeof(source_filename));
	adxi::write_string(os, source_creation_time, sizeof(source_creation_time));
	adxi::write_string(os, input_device, sizeof(input_device));
	adxi::write_string(os, input_device_serial_number,
	                  sizeof(input_device_serial_number));

	adxi::write_uint16(os, xl_border_validity, _need_byteswap);
	adxi::write_uint16(os, xr_border_validity, _need_byteswap);
	adxi::write_uint16(os, yt_border_validity, _need_byteswap);
	adxi::write_uint16(os, yb_border_validity, _need_byteswap);
	adxi::write_uint32(os, horizonal_par, _need_byteswap);
	adxi::write_uint32(os, vertical_par, _need_byteswap);

	adxi::write_float32(os, x_scanned_size, _need_byteswap);
	adxi::write_float32(os, y_scanned_size, _need_byteswap);

	adxi::write_fill(os, 0x00, 20);

	adxi::write_string(os, keycode_film_id, sizeof(keycode_film_id));
	adxi::write_string(os, keycode_film_type_char,
	                   sizeof(keycode_film_type_char));
	adxi::write_string(os, keycode_perf_offset_char,
	                   sizeof(keycode_perf_offset_char));
	adxi::write_string(os, keycode_prefix, sizeof(keycode_prefix));
	adxi::write_string(os, keycode_count_char, sizeof(keycode_count_char));

	adxi::write_string(os, format, sizeof(format));
	adxi::write_uint32(os, frame_position_in_sequence, _need_byteswap);
	adxi::write_uint32(os, sequence_length, _need_byteswap);
	adxi::write_uint32(os, held_count, _need_byteswap);
	adxi::write_float32(os, frame_rate, _need_byteswap);
	adxi::write_float32(os, shutter_angle, _need_byteswap);
	adxi::write_string(os, frame_attribute, sizeof(frame_attribute));
	adxi::write_string(os, frame_slate_info, sizeof(frame_slate_info));
//	adxi::write_float32(os, film_gague, _need_byteswap);
// 	adxi::write_uint8(os, film_orientation, _need_byteswap);
//	adxi::write_uint8(os, film_perf_per_foot, _need_byteswap);
//	adxi::write_uint8(os, film_perf_per_frame, _need_byteswap);
//	adxi::write_string(os, film_manufacturer, sizeof(film_manufacturer));
//	adxi::write_string(os, film_stock_name, sizeof(film_stock_name));
	adxi::write_fill(os, 0x00, 108);

	adxi::write_fill(os, 0x00, 21);

	adxi::write_uint32(os, smpte_timecode, _need_byteswap);
	adxi::write_uint32(os, smpte_userbits, _need_byteswap);
    

    // Constraints for ADX 60-62
    if (static_cast<uint32_t>(interlace) != udf8) {
        this->_constraint_ok = FALSE;
    }
    adxi::write_uint8(os, static_cast<uint8_t>(udf8), _need_byteswap);

    if (static_cast<uint32_t>(field_number) != udf8) {
        this->_constraint_ok = FALSE;
    }
    adxi::write_uint8(os, static_cast<uint8_t>(udf8), _need_byteswap);

    if (static_cast<uint32_t>(video_standard) != udf8) {
        this->_constraint_ok = FALSE;
    }
    adxi::write_uint8(os, static_cast<uint8_t>(udf8), _need_byteswap);

	adxi::write_fill(os, 0x00, 1);

    // Constraints for ADX 64-73
    if (horizontal_sampling_rate != nan_vf) {
        this->_constraint_ok = FALSE;
    }
	adxi::write_float32(os, nan_vf, _need_byteswap);

    if (vertical_sampling_rate != nan_vf) {
        this->_constraint_ok = FALSE;
    }
    adxi::write_float32(os, nan_vf, _need_byteswap);

    if (temporal_sampling_rate != nan_vf) {
        this->_constraint_ok = FALSE;
    }
    adxi::write_float32(os, nan_vf, _need_byteswap);

    if (time_offset_sync_to_first_pixel != nan_vf) {
        this->_constraint_ok = FALSE;
    }
    adxi::write_float32(os, nan_vf, _need_byteswap);

    if (gamma != nan_vf) {
        this->_constraint_ok = FALSE;
    }
    adxi::write_float32(os, nan_vf, _need_byteswap);

    if (black_level_code != nan_vf) {
        this->_constraint_ok = FALSE;
    }
    adxi::write_float32(os, nan_vf, _need_byteswap);

    if (black_gain != nan_vf) {
        this->_constraint_ok = FALSE;
    }
    adxi::write_float32(os, nan_vf, _need_byteswap);

    if (breakpoint != nan_vf) {
        this->_constraint_ok = FALSE;
    }
    adxi::write_float32(os, nan_vf, _need_byteswap);

    if (white_level_code != nan_vf) {
        this->_constraint_ok = FALSE;
    }
    adxi::write_float32(os, nan_vf, _need_byteswap);

    if (integration_time != nan_vf) {
        this->_constraint_ok = FALSE;
    }
    adxi::write_float32(os, nan_vf, _need_byteswap);

	adxi::write_fill(os, 0x00, 76);
	adxi::write_string(os, user_id, sizeof(user_id));

	current_ostream=NULL;
}	

}
