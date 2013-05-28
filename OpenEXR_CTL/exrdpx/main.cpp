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

//-----------------------------------------------------------------------------
//
//	exrdpx -- an OpenEXR/DPX file converter
//
//-----------------------------------------------------------------------------

#include <dpxToExr.h>
#include <exrToDpx.h>
#include <ImfTestFile.h>
#include <ImfThreading.h>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <exception>

using namespace std;
using namespace Imf;

namespace {

void
usageMessage (const char argv0[], bool verbose = false)
{
    cerr << "usage: " << argv0 << " [options] dpxFile exrFile" << endl;
    cerr << "or:    " << argv0 << " [options] exrFile dpxFile" << endl;

    if (verbose)
    {
	cerr << "\n"
		"Converts an OpenEXR file into a DPX file\n"
		"or a DPX file into an OpenEXR file.\n"
		"\n"
		"Options:\n"
		"\n"
		"-C s   CTL transform s is applied to the input pixels\n"
		"       before the output file is written.  Option -C can\n"
		"       be specified multiple times to apply a series of\n"
		"       transforms to the pixels.  The transforms are\n"
		"       applied in the order in which they appear on the\n"
		"       command line.\n"
		"\n"
		"-z x   sets the data compression for the OpenEXR output\n"
		"       file to x (none/rle/zip/piz/pxr24/b44, default\n"
		"       is piz)\n"
		"\n"
		"-xyz   the OpenEXR output file will contain CIE XYZ\n"
		"       instead of Rec. 709 pixel data\n"
		"\n"
		"-yc    the pixels in the OpenEXR output file will be\n"
		"       in luminance/chroma instead of RGB format\n"
		"\n"
		"-s     the header of the DPX input file is checked less\n"
		"       strictly; some fields are ignored even though they\n"
		"       are part of the core set.\n"
		"\n"
		"-t n	process the pixels using n parallel threads\n"
		"\n"
		"-v     verbose mode\n"
		"\n"
		"-h     prints this message\n"
		"\n"
		"CTL transforms:\n"
		"\n"
		"       If one or more CTL transforms are specified on\n"
		"       the command line (using the -C flag), then those\n"
		"       transforms are applied to the input pixels.\n"
		"       If no CTL transforms are specified then a default\n"
		"       transform is applied.  The name of the default\n"
		"       transform is \"transform_EXR_DPX\" for OpenEXR to\n"
		"       DPX conversion and \"transform_DPX_EXR\" for DPX to\n"
		"       OpenEXR conversion.\n"
		"       The files that contain the transforms are located\n"
		"       using the CTL_MODULE_PATH environment variable.\n";

	 cerr << endl;
    }

    exit (1);
}


Compression
getCompression (const string &str)
{
    Compression c;

    if (str == "no" || str == "none" || str == "NO" || str == "NONE")
    {
	c = NO_COMPRESSION;
    }
    else if (str == "rle" || str == "RLE")
    {
	c = RLE_COMPRESSION;
    }
    else if (str == "zip" || str == "ZIP")
    {
	c = ZIP_COMPRESSION;
    }
    else if (str == "piz" || str == "PIZ")
    {
	c = PIZ_COMPRESSION;
    }
    else if (str == "pxr24" || str == "PXR24")
    {
	c = PXR24_COMPRESSION;
    }
    else if (str == "b44" || str == "B44")
    {
	c = B44_COMPRESSION;
    }
    else
    {
	cerr << "Unknown compression method \"" << str << "\"." << endl;
	exit (1);
    }

    return c;
}

} // namespace

int
main(int argc, char **argv)
{
    const char *fileName1 = 0;
    const char *fileName2 = 0;
    vector<string> transformNames;
    bool outputXyz = false;
    bool outputYc = false;
    bool strict = true;
    Compression compression = PIZ_COMPRESSION;
    int numThreads = 0;
    bool verbose = false;
    
    //
    // Parse the command line.
    //

    if (argc < 2)
	usageMessage (argv[0], true);

    int i = 1;
    int j = 1;

    while (i < argc)
    {
	if (!strcmp (argv[i], "-C"))
	{
	    //
	    // Apply a CTL transform
	    //

	    if (i > argc - 2)
		usageMessage (argv[0]);

	    transformNames.push_back (argv[i + 1]);
	    i += 2;
	}
	else if (!strcmp (argv[i], "-z"))
	{
	    //
	    // Select compression method
	    //

	    if (i > argc - 2)
		usageMessage (argv[0]);

	    compression = getCompression (argv[i + 1]);
	    i += 2;
	}
	else if (!strcmp (argv[i], "-xyz"))
	{
	    //
	    // Output OpenEXR with CIE XYZ primaries
	    //

	    outputXyz = true;
	    i += 1;
	}
	else if (!strcmp (argv[i], "-yc"))
	{
	    //
	    // Output OpenEXR in luminance/chroma format
	    //

	    outputYc = true;
	    i += 1;
	}
	else if (!strcmp (argv[i], "-t"))
	{
	    //
	    // Set number of threads
	    //

	    if (i > argc - 2)
		usageMessage (argv[0]);

	    numThreads = strtol (argv[i + 1], 0, 0);

	    if (numThreads < 0)
	    {
		cerr << "Number of threads cannot be negative." << endl;
		return 1;
	    }

	    i += 2;
	}
	else if (!strcmp (argv[i], "-v"))
	{
	    //
	    // Verbose mode
	    //

	    verbose = true;
	    i += 1;
	}
	else if (!strcmp (argv[i], "-s"))
	{
	    //
	    // Less strict DPX header checking
	    //

	    strict = false;
	    i += 1;
	}
	else if (!strcmp (argv[i], "-h"))
	{
	    //
	    // Print help message
	    //

	    usageMessage (argv[0], true);
	}
	else
	{
	    //
	    // image file name
	    //

	    if (j == 1)
		fileName1 = argv[i];
	    else if (j == 2)
		fileName2 = argv[i];
	    else
		usageMessage (argv[0]);

	    i += 1;
	    j += 1;
	}
    }

    if (fileName1 == 0 || fileName2 == 0)
	usageMessage (argv[0]);

    //
    // Do the work
    //

    int exitStatus = 0;

    try
    {
	setGlobalThreadCount (numThreads);

	if (isOpenExrFile (fileName1))
	{
	    if (transformNames.empty())
		transformNames.push_back ("transform_EXR_DPX");

	    exrToDpx (fileName1,
		      fileName2,
		      transformNames,
		      verbose);
	}
	else
	{
	    if (transformNames.empty())
		transformNames.push_back ("transform_DPX_EXR");

	    dpxToExr (fileName1,
		      fileName2,
		      transformNames,
		      outputXyz,
		      outputYc,
		      compression,
		      strict,
		      verbose);
	}
    }
    catch (const exception &e)
    {
	cerr << e.what() << endl;
	exitStatus = 1;
    }

    return exitStatus;
}
