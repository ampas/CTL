# Installing CTL
This document covers various ways to get CTL running on your system.

## 1. The Quickest Method (macOS)
If you are on macOS, using Homebrew is the recommended method, as it automatically handles all required and optional dependencies.

```
brew install ctl
```

## 2. Building from Source
Use this method for Linux (Ubuntu/Debian/Red Hat) or if you need a custom build on macOS.

### Prerequisites
You will need a C++ compiler and the following libraries. Use these commands to install them:

| Dependency | Ubuntu / Debian | Red Hat / CentOS | macOS |
| :- | :- | :- | :- | 
| CMake (3.12+) | `sudo apt install cmake` | `sudo yum install cmake` | `brew install cmake` |
| OpenEXR | `sudo apt install libopenexr-dev` | `sudo yum install OpenEXR-devel` | `brew install openexr` |
| libTIFF (optional) | `sudo apt install libtiff-dev` | `sudo yum install libtiff-devel` | `brew install libtiff` |
| ACES Container (optional) | [Build from source](https://github.com/aces-aswf/aces_container) | [Build from source](https://github.com/aces-aswf/aces_container) | `brew install aces_container` |

### Build steps
From the root of the CTL source directory:

```
mkdir build && cd build
cmake ..
make
sudo make install
```

## 3. Running in Docker
Docker is ideal if you want to use CTL without installing libraries directly onto your host operating system.

### Build the Image
```
$ docker build --rm -f Dockerfile -t ctl:latest .
```

### Run the Container
To actually process files, you should "mount" a folder from your computer into the container so the files can be shared. This will allow you to use CTL in the container to process files stored on your machine.

* On Windows (PowerShell):

    ```
    $ docker run -it --rm -v C:\your\data:/data ctl:latest
    ```

    > [!TIP]
    > This mounts the Windows directory `C:\your\data` (or any directory you choose) inside the container at `/data`. Anything you put in `C:\your\data` on your machine appears in `/data` inside the container. Any files written in `/data` by CTL will show up in your Windows `C:\your\data`.

* On Linux / macOS:

    ```
    $ docker run -it --rm -v /home/user/data:/data ctl:latest
    ```

    > [!TIP]
    > This mounts the directory `/home/user/data` (or any directory you choose) inside the container at `/data`. Anything you put in the mapped directory from your machine appears in `/data` inside the container. Any files written in `/data` by CTL will show up in your mapped directory. 

In the above commands:
* `-it` → makes an interactive terminal (so you can type commands)
* `--rm` → means the container is deleted when you exit        
* `ctl:latest` → starts a CTL container from the `ctl:latest` image


## Basic Usage
Once CTL is installed, you can verify everything is working by checking the help menu for the primary command-line tool, ctlrender:

```
ctlrender -help
```

To apply a transform to an image:
```
ctlrender -ctl transform.ctl input.exr output.exr
```


If you prefer an isolated environment, CTL can be built and run in a Docker container.


2. Run the container:


3. To use `ctlrender` inside the container:

    ```
    $ ctlrender -help
    ```

## Building from Source: Prerequisites

When building from source, the following dependencies are required or suggested.

### Required

#### CMake

Used to configure and build the project. 
CMake can be downloaded directly from www.cmake.org or by using one of the commands below.

* OS X

        $ brew install cmake

* Redhat

        $ yum install cmake

* Ubuntu

        $ sudo apt-get install cmake