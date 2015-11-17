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


#include <stdio.h>
#include <string.h>

void usage(const char *section) {
	if(section==NULL) {
		fprintf(stdout, ""
"ctlrender - transforms an image using one or more CTL scripts, potentially\n"
"            converting the file format in the process\n"
"\nusage:\n"
"    ctlrender [<options> ...] <source file...> <destination>\n"
"\n"
"\n"
"options:\n"
"\n"
"    <source file...>      One or more source files may be specified in a\n"
"                          space separated list. Note to non-cygwin using\n"
"                          Windows users: wild card ('*') expansions are not\n"
"                          supported.\n"
"\n"
"    <destination>         In the case that only one source file is specified\n"
"                          this may be either a filename or a directory. If\n"
"                          a file is specified, then the output format of the\n"
"                          file is determined from the input file type and\n"
"                          the extension of the output file type.\n"
"                          If more than one source file is specified then\n"
"                          this must specify an existing directory. To\n"
"                          perform a file type conversion, the '-format'\n"
"                          option must be used.\n"
"                          See below for details on the '-format' option.\n"
"\n"
"    -input_scale <value>  Specifies a scaling value for the input.\n"
"                          Details on this are provided with '-help scale'.\n"
"\n"
"    -output_scale <value> Specifies a scaling value for the output file.\n"
"                          Details on this are provided with '-help scale'.\n"
"\n"
"    -format <output_fmt>  Specifies the output file format. If ony one\n"
"                          source file is specified then the extension of\n"
"                          destination file is used to determine the file\n"
"                          format. Details on this are provided with\n"
"                          '-help format'\n"
"\n"
"    -compression <type>   Specifies OpenEXR compression type. Value will\n"
"                          be ignored when not saving an exr file\n"
"                          '-help compression'\n"
"\n"
"    -ctl <filename>       Specifies the name of a CTL file to be applied\n"
"                          to the input images. More than one CTL file may\n"
"                          be provided (each must be delineated by a '-ctl'\n"
"                          option), and they are applied in-order.\n"
"\n"
"    -param1 ...           Specifies the value of a CTL script parameter.\n"
"    -param2 ...           Details on this and similar options are provided\n"
"    -param3 ...           with '-help param'\n"
"\n"
"    -verbose              Increases the level of output verbosity.\n"
"    -quiet                Decreases the level of output verbosity.\n"
"");
	} else if(!strncmp(section, "format", 1)) {
		fprintf(stdout, ""
"format conversion:\n"
"\n"
"    ctlrender provides file format conversion either implicitly by the\n"
"    extension of the output file, or via the use of the '-format' option.\n"
"    Valid values for the '-format' option are:\n"
"\n"
"        dpx10   Produces a DPX file with a 10 bits per sample (32 bit \n"
"                packed) format\n"
"\n"
"        dpx16   Produces a DPX file with a 16 bits per sample format\n"
"\n"
"        dpx     Produces a DPX file with the same bit depth as the source\n"
"                image\n"
"\n"
"        adx10   Produces a ADX file with a 10 bits per sample (32 bit \n"
"                packed) format\n"
"\n"
"        adx16   Produces a ADX file with a 16 bits per sample format\n"
"\n"
"        tiff8   Produces a TIFF file in the 8 bits per sample format\n"
"\n"
"        tiff16  Produces a TIFF file in the 16 bits per sample format\n"
"\n"
"        tiff32  Produces a TIFF file in the 32 bits per sample format\n"
"\n"
"        tiff    Produces a TIFF file with the same bit depth as the source\n"
"                image\n"
"\n"
"        exr16   Produces an exr file in the half (16 bit float) per sample\n"
"                format\n"
"\n"
"        exr32   Produces an exr file in the float (32 bit float) per sample\n"
"                format\n"
"\n"
"        exr     Produces an exr file with the same bit depth as the source\n"
"\n"
"        aces    Produces an aces compliant exr file\n"
"\n"
"    When only one source file is specified with a destination file name,\n"
"    the extension is interpreted the same way as an argument to '-format',\n"
"    and will not be changed.\n"
"\n"
"    When the destination is a directory and the -format is provided, the\n"
"    file extension will be changed to the type specified in the -format\n"
"    option with the bit depth removed.\n"
"\n"
"    Note that no automatic depth scaling is performed, please see\n"
"    '-help scale' for more details on how scaling is performed.\n"
"");
    } else if(!strncmp(section, "compression", 2)) {
#if defined(HAVE_OPENEXR)
        fprintf(stdout, ""
"exr compression:\n"
"\n"
"    ctlrender provides the option of a compression scheme when saving an \n"
"    OpenEXR image. If '-compression' option is not given, PIZ will be used.\n"
"    Valid values for the '-compression' option are:\n"
"\n"
"        NONE    Do not compress.\n"
"\n"
"        PIZ     (lossless) Ideal for photographic images.\n"
"                Default compression scheme.\n"
"\n"
"        ZIPS    (lossless) ZIP one scanline at a time.\n"
"\n"
"        ZIP     (lossless) Ideal for texture maps.\n"
"\n"
"        RLE     (lossless) Ideal for images with large flat areas.\n"
"\n"
"        PXR24   (lossy) Ideal for images with a large range of values but\n"
"                full 32-bit accuracy is not necessary (e.g. depth buffer).\n"
"                HALF and UINT channels are preserved exactly.\n"
"\n"
"        B44     (lossy) Possibly advantageous to real-time playback systems.\n"
"\n"
"        B44A    (lossy) Like B44 but smaller for images containing large\n"
"                uniform areas.\n"
"");
#else
        fprintf(stdout, ""
"exr compression:\n"
"\n"
"    ctlrender provides the option of a compression scheme when saving an \n"
"    OpenEXR image. If '-compression' option is not given, PIZ will be used.\n"
"    Valid values for the '-compression' option are:\n"
"\n"
"        NONE    Do not compress.\n"
"\n"
"    OpenEXR support must be enabled for the '-compression' option to be\n"
"    meaningful. Please see build documentation for details.\n"
"");
#endif
	} else if(!strncmp(section, "ctl", 1)) {
		fprintf(stdout, ""
"ctl file interpretation:\n"
"    ctlrender treats all ctl files as if they take their input as 'R', 'G',\n"
"    'B', and 'A' (optional) channels, and produce output as 'R', 'G', and\n"
"    'B', and 'A' (if required) channels. In the event of a single channel\n"
"    input file only the 'G' channel will be used.\n"
"");
//"    The *LAST* function in the file is the function that will be called to\n"
//"    provide the transform. This is to maintain compatability with scripts\n"
//"    developed for Autodesk's TOXIC product.\n"
	} else if(!strncmp(section, "scale", 1)) {
		fprintf(stderr, ""
"input and output value scaling:\n"
"\n"
"    To deal with differences in input and output file bit depth, the ability\n"
"    to scale input and output values has been provided. While these options\n"
"    are primarily of use for integral file formats, they can be used with\n"
"    file formats that store data in floating point or psuedo-floating point\n"
"    formats. The default handling of the input and output scaling is variant\n"
"    on the format of the input (and output) file, but is intended to behave \n"
"    as one expects.\n"
"\n"
"    integral input files (integer tiff, integer dpx):\n"
"        If the '-input_scale' option is provided then the sample value from\n"
"        the file is *divided by* the specified scale.\n"
"        If the '-input_scale' option is not provided, then the input values\n"
"        are scaled to the range 0.0-1.0 (inclusive). For the purposes of\n"
"        this argument, DPX files are considered an integral file format,\n"
"        however ACES files are *not*. This is equivalent to specifying\n"
"        -input_scale <bits_per_sample_in_input_file>\n"
"\n"    
"    floating point input files (exr, floatint point TIFF, floating point\n"
"    dpx):\n"
"        If the '-input_scale' option is provided then the sample values\n"
"        are *multiplied by* the scale value.\n"
"        If the '-input_scale' option is not provided then the sample values\n"
"        from the file is used as-is (with a scale of 1.0).\n"
"\n"
"    integral output files (integer tiff, integer dpx):\n"
"        If the '-output_scale' option is provided then the sample value from\n"
"        the CTL transformation is *multiplied by* the scale factor.\n"
"        If the '-output_scale' option is not provided, then the values of\n"
"        0.0-1.0 from the CTL transformation are scaled to the bit depth of\n"
"        the output file. For the purposes of this argument, DPX files are\n"
"        considered an integral file format, however ACES files are *not*.\n"
"        This is equivalent to specifying\n"
"        -output_scale <bits_per_sample_in_output_file>\n"
"\n"
"    floating point output files (exr, floatint point TIFF, floating point\n"
"    dpx):\n"
"        If the '-output_scale' option is provided then the sample values\n"
"        are *divided by* the scale value.\n"
"        If the '-output_scale' option is not provided then the sample values\n"
"        from the file is used as-is (with a scale of 1.0).\n"
"\n"
"    In all cases the CTL output values (after output_scaling) are clipped\n"
"    to the maximum values supported by the output file format.\n"
"");
	} else if(!strncmp(section, "param", 1)) {
		fprintf(stdout, ""
"ctl parameters:\n"
"\n"
"    In CTL scripts it is possible to define parameters that are not set\n"
"    until runtime. These parameters take one, two, or three floating point\n"
"    values. There are three options that allow you to specify the name\n"
"    of the parameter and the associated values. The options are as follows:\n"
"\n"
"        -param1 <name> <float1>\n"
"        -param2 <name> <float1> <float2>\n"
"        -param3 <name> <float1> <float2> <float3>\n"
"\n"
"        -global_param1 <name> <float1>\n"
"        -global_param2 <name> <float1> <float2>\n"
"        -global_param3 <name> <float1> <float2> <float3>\n"
"");
	} else {
		fprintf(stdout, ""
"The '%s' section of the help does not exist. Try running ctlrender with\n"
"only the -help option.\n"
"", section);
	}
}
