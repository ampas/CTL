///////////////////////////////////////////////////////////////////////////////
// 
// Copyright (c) 2007 Academy of Motion Picture Arts and Sciences
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
//	exr_ctl_exr -- a program that reads an OpenEXR file,
//	applies one or more CTL programs to the pixels, and
//	stores the result in another OpenEXR file.
//
//-----------------------------------------------------------------------------

#include <exrCtlExr.h>
#include <iostream>
#include <string>
#include <exception>
#include <ImfFloatAttribute.h>
#include <ImfVecAttribute.h>

using namespace Imath;
using namespace Imf;
using namespace std;

namespace {

void
usageMessage (const char argv0[], bool verbose = false)
{
    cerr << "usage: " << argv0 << " [options] infile outfile" << endl;

    if (verbose)
    {
	cerr << "\n"
		"Reads an OpenEXR file, applies one or more CTL programs\n"
		"to the pixels, and stores the result in another OpenEXR\n"
		"file.\n"
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
		"-float s f\n"
		"       An input argument of type float, with name s and\n"
		"       value f is made available to the CTL transforms.\n"
		"\n"
		"-float2 s f1 f2\n"
		"       An input argument of type float[2], with name s\n"
		"       and the value {f1, f2}, is made available to the\n"
		"       CTL transforms.\n"
		"\n"
		"-float3 s f1 f2 f3\n"
		"       An input argument of type float[3], with name s\n"
		"       and the value {f1, f2, f3}, is made available to\n"
		"       the CTL transforms.\n"
		"\n"
		"-v     verbose mode\n"
		"\n"
		"-h     prints this message\n"
		"\n"
		"CTL transforms:\n"
		"\n"
		"       Files that contain the CTL transforms are located\n"
		"       using the CTL_MODULE_PATH environment variable.\n"
		"\n"
		"       Transforms can read the following input data:\n"
		"\n"
		"       * R, G and B channels of the input file\n"
		"\n"
		"       * values specified on the command line\n"
		"\n"
		"       * attributes in the header of the input file\n"
		"         (chromaticities and adoptedNeutral attributes\n"
		"         are guaranteed to be present)\n"
		"\n"
		"       * displayChromaticites, displayWhiteLuminance and\n"
		"         displaySurroundLuminance (values are supplied by\n"
		"         environment variables CTL_DISPLAY_CHROMATICITIES,\n"
		"         CTL_DISPLAY_WHITE_LUMINANCE and\n"
		"         CTL_DISPLAY_SURROUND_LUMINANCE)\n"
		"\n"
		"       Transforms can set the following output data:\n"
		"\n"
		"       * R, G and B channels of the output file (ROut,\n"
		"         GOut and BOut)\n"
		"\n"
		"       * attributes in the header of the output file; in\n"
		"         particular, transforms can set the output file's\n"
		"         chromaticities and adoptedNeutral attributes\n"
		"         (chromaticitiesOut and adoptedNeutralOut)\n";

	 cerr << endl;
    }

    exit (1);
}


void
getNameAndFloat (int argc,
		 char **argv,
		 int &i,
		 AttrMap &attrs)
{
    if (i > argc - 3)
	usageMessage (argv[0]);

    const char *attrName = argv[i + 1];
    float f = strtod (argv[i + 2], 0);
    attrs[attrName] = new FloatAttribute (f);
    i += 3;
}


void
getNameAndV2f (int argc,
	       char **argv,
	       int &i,
	       AttrMap &attrs)
{
    if (i > argc - 4)
	usageMessage (argv[0]);

    const char *attrName = argv[i + 1];

    V2f f (strtod (argv[i + 2], 0),
	   strtod (argv[i + 3], 0));

    attrs[attrName] = new V2fAttribute (f);
    i += 4;
}


void
getNameAndV3f (int argc,
	       char **argv,
	       int &i,
	       AttrMap &attrs)
{
    if (i > argc - 4)
	usageMessage (argv[0]);

    const char *attrName = argv[i + 1];

    V3f f (strtod (argv[i + 2], 0),
	   strtod (argv[i + 3], 0),
	   strtod (argv[i + 4], 0));

    attrs[attrName] = new V3fAttribute (f);
    i += 5;
}

} // namespace

int
main(int argc, char **argv)
{
    const char *inFileName = 0;
    const char *outFileName = 0;
    vector<string> transformNames;
    AttrMap extraAttrs;
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
	else if (!strcmp (argv[i], "-float"))
	{
	    //
	    // Pass a value of type float to the CTL transforms
	    //

	    getNameAndFloat (argc, argv, i, extraAttrs);
	}
	else if (!strcmp (argv[i], "-float2"))
	{
	    //
	    // Pass a value of type float[2] to the CTL transforms
	    //

	    getNameAndV2f (argc, argv, i, extraAttrs);
	}
	else if (!strcmp (argv[i], "-float3"))
	{
	    //
	    // Pass a value of type float[3] to the CTL transforms
	    //

	    getNameAndV3f (argc, argv, i, extraAttrs);
	}
	else if (!strcmp (argv[i], "-v"))
	{
	    //
	    // Verbose mode
	    //

	    verbose = true;
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
		inFileName = argv[i];
	    else if (j == 2)
		outFileName = argv[i];
	    else
		usageMessage (argv[0]);

	    i += 1;
	    j += 1;
	}
    }

    if (inFileName == 0 || outFileName == 0)
	usageMessage (argv[0]);

    if (transformNames.empty())
    {
	cerr << "no CTL transforms specified" << endl;
	return 1;
    }

    //
    // Do the work
    //

    int exitStatus = 0;

    try
    {
	exrCtlExr (inFileName,
		   outFileName,
		   transformNames,
		   extraAttrs,
		   verbose);
    }
    catch (const exception &e)
    {
	cerr << e.what() << endl;
	exitStatus = 1;
    }

    return exitStatus;
}
