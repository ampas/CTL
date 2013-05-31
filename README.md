# The Color Transformation Language #
 
The Color Transformation Language, or CTL, is a programming language for digital
color management.
 
Digital color management requires translating digital images between different
representations or color spaces.  For example, the pixels in an image may encode
the colors that should be seen when the image is displayed on a video monitor. 
Printing this image on paper, or recording it on motion picture film requires
transforming the pixels to an appropriate representation: Video, inks on paper
and film all have different color gamuts and dynamic ranges.  Color mixing is
additive for video, but subtractive for inks and film.  Video and film typically
use three color channels, while four or more inks are used for printing on
paper. A color management system must transform each pixel in the original image
to corresponding amounts of ink or film density values.
 
The details of how each pixel is transformed can be fairly complex, and they are
often subject to artistic decisions.  When images are exchanged between 
different parties, it is desirable to exchange exact descriptions of appropriate
color transforms along with the digital image files.  Two people in different
geographical locations may each have a copy of the same digital image file. 
When one of them prints the image on paper, he or she wants to be sure that the
result is the same as as for the other person.  In order to achieve identical
results, the two must agree on details of the printing process (for example,
inks and paper), and they must agree on the transform that converts pixels in
the file into amounts of ink on paper.  Of course, this requires a description
of the transform.
 
The Color Transformation Language, or CTL, is a small programming language that
was designed to serve as a building block for digital color management systems. 
CTL allows users to describe color transforms in a concise and unambiguous way
by expressing them as programs.  In order to apply a given transform to an
image, the color management system instructs a CTL interpreter to load and run
the CTL program that describes the transform.  The original and the transformed
image constitute the CTL program's input and output.
 
Color transforms can be shared by distributing CTL programs. Two parties with
the same CTL program can apply the same transform to an image.


## Package Contents ##
The CTL source code contains the following:

* `lib/` - CTL libraries and the CTL interpreter 
* `doc/` - CTL documentation
* `OpenEXR_CTL/` - sample CTL applications utilizing IlmImfCtl
* `ctlrender/` - an application that allows for application of CTL transforms to
  an image using one or more CTL scripts, potentially converting the file format
  in the process.
* `Nuke_CTL/` - an application for using CTL transforms in a Nuke node
* `config/` - CMake configuration files
* `unittest/` - unit test files

## Installation Prerequisites ##
### Required ###

__CMake__

CMake can be downloaded directly from www.cmake.org or use one of the commands 
below.

* Ubuntu

        $ sudo apt-get install cmake

* Redhat

        $ yum install cmake

* OS X (Install homebrew if not already installed)

        $ ruby -e "$(curl -fsSL https://raw.github.com/mxcl/homebrew/go)"
        $ brew install cmake

__IlmBase__

The CTL interpreter depends on the IlmBase software package, which can be
downloaded from http://www.openexr.com or use one of the commands below.

* Ubuntu

        $ sudo apt-get install libilmbase-dev

* Redhat

        $ yum install ilmbase-devel

* OS X (Install homebrew if not already installed)

        $ ruby -e "$(curl -fsSL https://raw.github.com/mxcl/homebrew/go)"
        $ brew install ilmBase


### Suggested ###

__OpenEXR__

If you want to use CTL together with the OpenEXR image file format, you should
download OpenEXR. The sample programs included with the CTL package (i.e.
ctlrender, dpxexr, exr_ctl_exr, etc.) require that OpenEXR be installed. OpenEXR 
can be downloaded from http://www.openexr.com/downloads.html or using one of the 
commands below.

* Ubuntu

        $ sudo apt-get install libopenexr-dev

* Redhat

        $ yum install OpenEXR-devel

* OS X (Install homebrew if not already installed)

        $ ruby -e "$(curl -fsSL https://raw.github.com/mxcl/homebrew/go)"
        $ brew install openexr

__ACES Container__

ctlrender is able to write files compliant with SMPTE S2065-4. This
functionality requires the aces_container library, the latest version of which 
can be downloaded from https://github.com/ampas/aces_container

## Installation ##

from the root source directory:

    $ mkdir build && cd build
    $ cmake ..
    $ make
    $ sudo make install

to run the optional unit tests:

    $ sudo make check

## License ##
 
Color Transformation Language is distributed under the following license:

Copyright © 2013 Academy of Motion Picture Arts and Sciences ("A.M.P.A.S.").
Portions contributed by others as indicated. All rights reserved.

A worldwide, royalty-free, non-exclusive right to copy, modify, create
derivatives, and use, in source and binary forms, is hereby granted, subject to
acceptance of this license. Performance of any of the aforementioned acts
indicates acceptance to be bound by the following terms and conditions:

* Copies of source code, in whole or in part, must retain the above copyright
notice, this list of conditions and the Disclaimer of Warranty.

* Use in binary form must retain the above copyright notice, this list of
conditions and the Disclaimer of Warranty in the documentation and/or other
materials provided with the distribution.

* Nothing in this license shall be deemed to grant any rights to trademarks,
copyrights, patents, trade secrets or any other intellectual property of
A.M.P.A.S. or any contributors, except as expressly stated herein.

* Neither the name "A.M.P.A.S." nor the name of any other contributors to this
software may be used to endorse or promote products derivative of or based on
this software without express prior written permission of A.M.P.A.S. or the
contributors, as appropriate.

This license shall be construed pursuant to the laws of the State of California, 
and any disputes related thereto shall be subject to the jurisdiction of the 
courts therein.

Disclaimer of Warranty: THIS SOFTWARE IS PROVIDED BY A.M.P.A.S. AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
NON-INFRINGEMENT ARE DISCLAIMED. IN NO EVENT SHALL A.M.P.A.S., OR ANY
CONTRIBUTORS OR DISTRIBUTORS, BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, RESITUTIONARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

WITHOUT LIMITING THE GENERALITY OF THE FOREGOING, THE ACADEMY SPECIFICALLY
DISCLAIMS ANY REPRESENTATIONS OR WARRANTIES WHATSOEVER RELATED TO PATENT OR
OTHER INTELLECTUAL PROPERTY RIGHTS IN THE ACADEMY COLOR ENCODING SYSTEM, OR
APPLICATIONS THEREOF, HELD BY PARTIES OTHER THAN A.M.P.A.S.,WHETHER DISCLOSED OR
UNDISCLOSED.