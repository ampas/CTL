# Color Transformation Language (CTL)

## What is CTL?
The Color Transformation Language, or CTL, is a small programming language
designed to serve as a building block for digital color management systems. It
allows users to describe color transforms in a concise and unambiguous way by
expressing them as programs. 

### How it Works
A color management system that supports CTL includes an "interpreter", a
software program that performs the operations described in a CTL program on the
pixels of an image. To apply a transform, the system instructs the interpreter
to load and run the specific CTL program. The original and the transformed
images constitute the program's input and output. 

**Interchange:** Color transforms can be easly shared by distributing CTL
programs. Two parties using the same CTL program can apply the identical
transform to an image.

**Parameters:** CTL programs can include input parameters (such as "exposure")
that adjust the transformation. To guarantee identical results, parties must
agree on both the CTL program and the settings for the transform's parameters.

### Why A Domain-Specific Language?
While general-purpose programming languages like C, C++ or Python can describe
color transforms, they are often unsuitable for transform interchange due to
several factors:

* Some languages require the recipient to explicitly compile and link source
  code before it can be executed. 
* Code must be carefully written to ensure it behaves identially and remains
  portable across different operating systems.
* If general purpose code is executed inside a larger application, bugs can
  cause the entire application to malfunction or crash. 
* Providing reliable protection against viruses and Trojan horses is difficult
  with most general-purpose programming languages.

By contrast, CTL is designed to allow only the kinds of operations that are
needed to describe color transforms. This focused approach improves program
portability, protects users against application software crashes and malicious
code, and allows for efficient interpreter implementations.

### Scope and Constraints
CTL is a specialized tool and not intended for general-purpose image processing.

* CTL is restricted to color space transforms and other single-pixel operations.
* CTL cannot express operations that require surrounding pixel data, such as
convolving an image with a filter kernel (blurs/sharpens) or calculatating
global image statistics (like a sum of all pixels in an image).

> [!IMPORTANT] 
> While the source code for CTL is maintained within the ACES project ecosystem,
> CTL is very much its own standalone project. It is important to distinguish
> the language from its usage:
>
> * CTL is a color transformation language, and can be used to express any color transform.
> * ACES makes use of CTL as language to provide its reference transforms.
> * CTL ≠ ACES - CTL's utility extends far beyond its usage in the ACES framework.

## Respository Contents

* `.github/` - Github CI workflow files
* `cmake/` - cmake support files
* `ctlrender/` - a command-line tool to apply CTL transforms to images
* `doc/` - CTL documentation
* `docker/` - dockerfiles that compile CTL on various platforms 
* `lib/` - CTL libraries and the CTL interpreter 
* `OpenEXR_CTL/` - sample CTL applications utilizing IlmImfCtl
* `resources/` - scripts and support files for unit tests
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

* OS X
    
    * Install homebrew if not already installed

            $ ruby -e "$(curl -fsSL https://raw.github.com/Homebrew/homebrew/go/install)"
            
    * Install cmake
            
            $ brew install cmake

__OpenEXR__

* Ubuntu

        $ sudo apt-get install libopenexr-dev

* Redhat

        $ yum install OpenEXR-devel

* OS X

    * Install homebrew if not already installed

            $ ruby -e "$(curl -fsSL https://raw.github.com/Homebrew/homebrew/go/install)"
            
    * Install OpenEXR
    
            $ brew install openexr                        

### Suggested ###

__TIFF__

If you want to use CTL together with the TIFF image file format, you should download libTiff. libTiff can be downloaded from [http://www.remotesensing.org/libtiff/](https://libtiff.gitlab.io/libtiff/) or using one of the commands below.

* Ubuntu

        $ sudo apt-get install libtiff-dev

* Redhat

        $ yum install libtiff-devel

* OS X

    * Install homebrew if not already installed

            $ ruby -e "$(curl -fsSL https://raw.github.com/Homebrew/homebrew/go/install)"
            
    * Install TIFF
    
            $ brew install libtiff                        

__ACES Container__

ctlrender is able to write files compliant with SMPTE ST2065-4. This
functionality requires the aces_container library, the latest version of which 
can be downloaded from https://github.com/ampas/aces_container

* OS X

    * Install homebrew if not already installed

            $ ruby -e "$(curl -fsSL https://raw.github.com/Homebrew/homebrew/go/install)"
            
    * Install ACES Container
    
            $ brew install aces_container

## Installation ##

* OS X

    * Install homebrew if not already installed

            $ ruby -e "$(curl -fsSL https://raw.github.com/Homebrew/homebrew/go/install)"
            
    * Install CTL
    
        Homebrew will install all dependancies automatically by default.  No need to install each manually.
    
            $ brew install ctl

* Redhat, Ubuntu

    from the root source directory:

        $ mkdir build && cd build
        $ cmake ..
        $ make
        $ sudo make install

    to run the optional unit tests:

        $ ctest

* Docker

    build docker

        $ docker build --rm -f Dockerfile -t ctl:latest .
        
    run docker image (in isolated container)

        $ docker run -it --rm ctl:latest

    run docker image (mapping C:\temp on Windows host to /tmp in container)

        $ docker run -it --rm -v C:\\temp:/tmp/ ctl:latest

    run docker image (mapping /mnt/c/temp on Linux host to /tmp in container)

        $ docker run -it --rm -v /mnt/c/temp:/tmp/ ctl:latest

    run ctlrender
        
        $ ctlrender -help 

## License ##
 
CTL is licensed under the [Apache 2.0 license](./LICENSE).