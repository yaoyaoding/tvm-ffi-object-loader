#!/bin/bash
# Build script for tvm-ffi-object-loader

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}Building tvm-ffi-object-loader${NC}"

# Check if tvm-ffi is available
if [ ! -d "../3rdparty/tvm-ffi" ]; then
    echo -e "${YELLOW}Warning: tvm-ffi not found in ../3rdparty/tvm-ffi${NC}"
    echo "Make sure tvm-ffi is installed in your system or set TVM_FFI_ROOT"
fi

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo -e "${GREEN}Configuring with CMake...${NC}"
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -G Ninja

# Build
echo -e "${GREEN}Building...${NC}"
cmake --build . -j$(nproc)

echo -e "${GREEN}Build complete!${NC}"
echo ""
echo "Shared library: build/libtvm_ffi_object_loader.so"
echo ""
echo "To install the Python package, run:"
echo "  pip install -e ."
