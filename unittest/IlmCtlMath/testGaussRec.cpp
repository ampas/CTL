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

#include <string>
#include <fstream>
#include <iostream>
#include <assert.h>
#include <CtlRbfInterpolator.h>
#include <ImathVec.h>

using namespace std;

void
runTestGaussian(int numSamples, int numTests)
{
    const double var = 1.; 
    srand(113131);

#ifdef HAVE_DARWIN
    //  gcc on mac requires V3f to be used before it is used below
    //  (tested on g++-4.0.1)
    Imath::V3f q;
#endif

    typedef Imath::V3f V3fPair[2];
    V3fPair *p = new V3fPair[numSamples];

    for ( int s = 0; s < numSamples; s++) 
    {
	p[s][0][0] = (float) rand() / (float) RAND_MAX;
	p[s][0][1] = (float) rand() / (float) RAND_MAX;
	p[s][0][2] = (float) rand() / (float) RAND_MAX;
	
	p[s][1][0] = exp( -static_cast<double>(p[s][0][0])*static_cast<double>(p[s][0][0]) * var);
	p[s][1][1] = exp( -static_cast<double>(p[s][0][1])*static_cast<double>(p[s][0][1]) * var);
	p[s][1][2] = exp( -static_cast<double>(p[s][0][2])*static_cast<double>(p[s][0][2]) * var);
    }
    
    Ctl::RbfInterpolator rbfItp(numSamples, p);

    cout << "\nTesting interpolation" << endl;
    double maxErr  = .0;
    double meanErr = .0;

    for ( int s = 0; s < numSamples; s++) 
    {
	double err = (rbfItp.value(p[s][0]) - p[s][1]).length();

	if (err > maxErr)
	    maxErr = err;

	meanErr += err;
    }

    assert (maxErr < 1E-6);
    assert (meanErr/double(numSamples) < 1E-6);

    delete [] p;
}

void
testGaussRecSmall()
{
    const int numSamples = 10;
    cout << "Testing the reconstruction of a Gaussian function. f(x) = exp(-x^2)." << endl;
    cout << "With " << numSamples << " samples." << endl;
    
    try
    {
	runTestGaussian(numSamples, 1000);
    }
    catch (const std::exception &e)
    {
	cerr << "ERROR -- caught exception: " << e.what() << endl;
	assert (false);
    }
    cout << "ok" << endl;

}

void
testGaussRecLarge()
{
    const int numSamples = 1000;
    cout << "Testing the reconstruction of a Gaussian function. f(x) = exp(-x^2)." << endl;
    cout << "With " << numSamples << " samples." << endl;
    
    try
    {
	runTestGaussian(numSamples, 1000);
    }
    catch (const std::exception &e)
    {
	cerr << "ERROR -- caught exception: " << e.what() << endl;
	assert (false);
    }
    cout << "ok" << endl;

}
