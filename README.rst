The Color Transformation Language
 
The Color Transformation Language, or CTL, is a programming language for digital color management.
 
Digital color management requires translating digital images between different representations or color spaces.  For example, the pixels in an image may encode the colors that should be seen when the image is displayed on a video monitor.  Printing this image on paper, or recording it on motion picture film requires transforming the pixels to an appropriate representation: Video, inks on paper and film all have different color gamuts and dynamic ranges.  Color mixing is additive for video, but subtractive for inks and film.  Video and film typically use three color channels, while four or more inks are used for printing on paper. A color management system must transform each pixel in the original image to corresponding amounts of ink or film density values.
 
The details of how each pixel is transformed can be fairly complex, and they are often subject to artistic decisions.  When images are exchanged between different parties, it is desirable to exchange exact descriptions of appropriate color transforms along with the digital image files.  Two people in different geographical locations may each have a copy of the same digital image file.  When one of them prints the image on paper, he or she wants to be sure that the result is the same as as for the other person.  In order to achieve identical results, the two must agree on details of the printing process (for example, inks and paper), and they must agree on the transform that converts pixels in the file into amounts of ink on paper.  Of course, this requires a description of the transform.
 
The Color Transformation Language, or CTL, is a small programming language that was designed to serve as a building block for digital color management systems.  CTL allows users to describe color transforms in a concise and unambiguous way by expressing them as programs.  In order to apply a given transform to an image, the color management system instructs a CTL interpreter to load and run the CTL program that describes the transform.  The original and the transformed image constitute the CTL program's input and output.
 
Color transforms can be shared by distributing CTL programs. Two parties with the same CTL program can apply the same transform to an image.
 
License
 
CTL is distributed under the following License:
 
Copyright © 2006 Academy of Motion Picture Arts and Sciences
("A.M.P.A.S."). Portions contributed by others as indicated.
All rights reserved.
 
A world-wide, royalty-free, non-exclusive right to distribute, copy,
modify, create derivatives, and use, in source and binary forms, is
hereby granted, subject to acceptance of this license. Performance of
any of the aforementioned acts indicates acceptance to be bound by the
following terms and conditions:
 
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the Disclaimer of Warranty.
 
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the Disclaimer of Warranty
    in the documentation and/or other materials provided with the
    distribution.
 
  * Nothing in this license shall be deemed to grant any rights to
    trademarks, copyrights, patents, trade secrets or any other
    intellectual property of A.M.P.A.S. or any contributors, except
    as expressly stated herein, and neither the name of A.M.P.A.S.
    nor of any other contributors to this software, may be used to
    endorse or promote products derived from this software without
    specific prior written permission of A.M.P.A.S. or contributor,
    as appropriate.
 
This license shall be governed by the laws of the State of California,
and subject to the jurisdiction of the courts therein.
 
Disclaimer of Warranty: THIS SOFTWARE IS PROVIDED BY A.M.P.A.S. AND
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT ARE DISCLAIMED. IN NO
EVENT SHALL A.M.P.A.S., ANY CONTRIBUTORS OR DISTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Installation
============

Repository Organization
***********************

This directory and subdirectories contain the source code for the ctlrender application.

prereqs - contains the expanded source trees for the prerequisites required to build ctlrender.
ctlrender - contains the expanded source tree for the ctlrender package which includes a version of CTL.


Instructions for building ctlrender on Mac OS X
***********************************************

Mac OS X builds require Apple's Xcode developer tools to be installed:

For Mac OS X 10.6.8, make sure you have Xcode 4.0.2.
For Mac OS X 10.5.8, make sure you have Xcode 3.1.4.
The build has not been tested on the new Mac OS X Lion (10.7), but it should
work on that version as well. Lion requires Xcode version 4.1

Xcode can be downloaded from the Apple Developer web site at  http://developer.apple.com

To build ctlrender, first download the ACES Developer Resources from
git@github.com:ampas/aces-dev.git.  For the purpose of these instructions, it
is assumed the downloaded repository directory named aces-dev is in the home
directory (i.e. ~/aces-dev). If you choose another directory name, substitute
that name in the instructions below.

Open a Terminal command-line window.

Change directories to the ACES subdirectory containing the ctlrender source::

	cd ~/aces-dev/apps/ctlrender

Create the following soft link::

	ln -s /usr/bin/true /bin/true

Build ilmbase-1.0.2::

	cd ~/aces-dev/apps/ctlrender/prereqs/ilmbase-1.0.2
	./configure
	make
	sudo make install

Build openexr-1.7.0::

	cd ~/aces-dev/apps/ctlrender/prereqs/openexr-1.7.0
	./configure
	make
	sudo make install

Build jpeg-6b::

	cd ~/aces-dev/apps/ctlrender/prereqs/jpeg-6b
	./configure
	make
	sudo make install

Build tiff-3.8.2::

	cd ~/aces-dev/apps/ctlrender/prereqs/tiff-3.8.2
	./configure
	make
	sudo make install

Build ctlrender::

	cd ~/aces-dev/apps/ctlrender/src
	./configure
	make
	sudo make install

A newly built ctlrender program should exist in /usr/local/bin

Instructions for building ctlrender on Ubuntu 10.04.2 (64-bit)
**************************************************************

Open a Terminal command-line window.

Install the developer tools::

	sudo apt-get install build-essential
	sudo apt-get install autoconf
	sudo apt-get install subversion

Install the zlib library (required by openexr-1.7.0)::

	sudo apt-get install zlib1g-dev

To build ctlrender, first download the ACES Developer Resources from
git@github.com:ampas/aces-dev.git.  For the purpose of these instructions, it
is assumed the downloaded repository directory named aces-dev is in the home
directory (i.e. ~/aces-dev). If you choose another directory name, substitute
that name in the instructions below.

Enter the following command::

	export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
	Note: To make the LD_LIBRARY_PATH setting permanent, add the above line to ~/.bash_profile

Change directories to the ACES subdirectory containing the ctlrender source::

	cd ~/aces-dev/apps/ctlrender

Build ilmbase-1.0.2::

	cd ~/aces-dev/apps/ctlrender/prereqs/ilmbase-1.0.2
	./configure
	make
	sudo make install

Build openexr-1.7.0::

	cd ~/aces-dev/apps/ctlrender/prereqs/openexr-1.7.0
	./configure
	make
	sudo make install

Build jpeg-6b::

	cd ~/aces-dev/apps/ctlrender/prereqs/jpeg-6b
	./configure
	make
	sudo make install

Build tiff-3.8.2::

	cd ~/aces-dev/apps/ctlrender/prereqs/tiff-3.8.2
	./configure
	make
	sudo make install

Build ctlrender::

	cd ~/aces-dev/apps/ctlrender/src
	./configure
	make
	sudo make install

A newly built ctlrender program should exist in /usr/local/bin
