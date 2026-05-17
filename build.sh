#!/bin/bash
# Build script for llama-box + agent.cpp combined project

set -e

echo "=========================================="
echo "Building llama-box-agent combined project"
echo "=========================================="

cd ~/Desktop/llama-box-agent

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_STANDARD=17 \
  -G "Unix Makefiles"

# Build
echo "Building..."
cmake --build . --config Release -j $(nproc)

echo "=========================================="
echo "Build complete!"
echo "Binary location: build/llama-box-agent"
echo "=========================================="
