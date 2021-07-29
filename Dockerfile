FROM ubuntu:focal

RUN apt-get update

# disable interactive install 
ENV DEBIAN_FRONTEND noninteractive

RUN apt-get -y install git
RUN apt-get -y install cmake
RUN apt-get -y install g++

# install CTL render
RUN apt-get -y install libilmbase-dev
RUN apt-get -y install libopenexr-dev
RUN apt-get -y install libtiff-dev
WORKDIR /usr/src/
RUN git clone https://github.com/michaeldsmith/CTL.git
WORKDIR /usr/src/CTL
RUN git pull
WORKDIR /usr/src/CTL/build
RUN cmake ..
RUN make
RUN make install

# finalize docker environment
WORKDIR /usr/src/CTL

