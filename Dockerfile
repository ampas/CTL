FROM ubuntu:focal

RUN apt-get update

# disable interactive install 
ENV DEBIAN_FRONTEND noninteractive

# install developement tools
RUN apt-get -y install cmake
RUN apt-get -y install g++

# install CTL dependencies
RUN apt-get -y install libilmbase-dev
RUN apt-get -y install libopenexr-dev
RUN apt-get -y install libtiff-dev

# build CTL
WORKDIR /usr/src/CTL
COPY . .
WORKDIR /usr/src/CTL/build
RUN cmake ..
RUN make
RUN make install

# finalize docker environment
WORKDIR /usr/src/CTL

