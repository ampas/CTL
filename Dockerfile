FROM ubuntu:latest

RUN apt-get update

# disable interactive install 
ENV DEBIAN_FRONTEND=noninteractive

# install developement tools
RUN apt-get -y install cmake
RUN apt-get -y install g++
RUN apt-get -y install git

# install developement debugging tools
RUN apt-get -y install valgrind

# install CTL dependencies
RUN apt-get -y install libopenexr-dev
RUN apt-get -y install libtiff-dev

# build CTL
WORKDIR /usr/src/CTL
COPY . .
WORKDIR /usr/src/CTL/build
RUN rm -R * || true
RUN cmake ..
RUN make
RUN make install

# finalize docker environment
WORKDIR /usr/src/CTL

