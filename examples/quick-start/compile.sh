#!/bin/bash
# Compile script for quick-start example

set -e

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${GREEN}Compiling add.cc to object file...${NC}"

# Check if tvm-ffi-config is available
if ! command -v tvm-ffi-config &> /dev/null; then
    echo -e "${RED}Error: tvm-ffi-config not found${NC}"
    echo "Make sure apache-tvm-ffi is installed:"
    echo "  pip install -e ../../3rdparty/tvm-ffi"
    exit 1
fi

# Get compilation flags from tvm-ffi-config
echo -e "${GREEN}Getting compilation flags from tvm-ffi-config...${NC}"
CXXFLAGS=$(tvm-ffi-config --cxxflags)
LDFLAGS=$(tvm-ffi-config --ldflags)

# Override C++ standard to C++20 (needed for lambda in unevaluated context)
CXXFLAGS=$(echo "$CXXFLAGS" | sed 's/-std=c++17/-std=c++20/')

echo "  CXXFLAGS: $CXXFLAGS"
echo "  LDFLAGS: $LDFLAGS"

# Compile to object file
echo -e "${GREEN}Compiling...${NC}"
g++ -c add.cc \
    -o add.o \
    $CXXFLAGS \
    -fPIC \
    -O2

echo -e "${GREEN}Successfully compiled add.o${NC}"
echo ""
echo "You can now run: python run.py"
