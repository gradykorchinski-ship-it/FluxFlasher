#!/bin/bash

# Build Rust library
echo "Building Rust core library..."
cargo build --release

if [ $? -ne 0 ]; then
    echo "Rust build failed!"
    exit 1
fi

# Build C++ UI
echo "Building C++ UI..."
mkdir -p build
cd build
cmake ..
make

if [ $? -ne 0 ]; then
    echo "C++ build failed!"
    exit 1
fi

echo "Build complete! Executable: build/FluxFlasher"
