#!/usr/bin/env bash

set -ex

git clone https://github.com/ampas/aces_container.git aces_container

if [[ "$OSTYPE" == "linux-gnu"* ]] || [[ "$OSTYPE" == "darwin"* ]]; then
    cmake \
        -S aces_container \
        -B aces_container/build \
        -DCMAKE_CXX_FLAGS="-Wno-c++11-narrowing"
    cmake --build aces_container/build
    sudo cmake --install aces_container/build
else
    cmake \
        -S aces_container \
        -B aces_container/build \
        -DCMAKE_INSTALL_PREFIX="." \
        -DBUILD_SHARED_LIBS=OFF
    cmake --build aces_container/build --config Release
    cmake --install aces_container/build --config Release
fi

cd ../..
