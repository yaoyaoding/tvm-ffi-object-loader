# Quick Start Example

This example demonstrates the basic usage of tvm-ffi-object-loader to compile C++ functions and load them dynamically at runtime.

## What's Included

- `add.cc` - Simple C++ source file with math functions exported via TVM-FFI
- `run.py` - Python script that loads and calls the compiled functions
- `compile.sh` - Script to compile the C++ code into an object file

## Prerequisites

- Python 3.8+
- C++ compiler (g++ or clang++)
- TVM-FFI and tvm-ffi-object-loader packages

## Installation

First, install the required packages:

```bash
# Navigate to the repository root
cd ../..

# Install TVM-FFI in editable mode
pip install -e 3rdparty/tvm-ffi

# Install tvm-ffi-object-loader in editable mode
pip install -e .

# Return to the example directory
cd examples/quick-start
```

After installation, `tvm-ffi-config` will be available in your PATH and used by the compile script to get the correct include directories and compiler flags.

## Steps

### 1. Compile the C++ code

```bash
./compile.sh
```

This will:
- Use `tvm-ffi-config` to get the correct compilation flags
- Create `add.o` - a compiled object file with exported functions

### 2. Run the Python loader

```bash
python run.py
```

This will:
- Load the object file using `ObjectLoader`
- Call the exported functions
- Print the results

## Expected Output

```
Loading object file: add.o
Calling add(10, 20) = 30
Calling multiply(7, 6) = 42
Calling fibonacci(10) = 55
```

## How It Works

1. **C++ Side** (`add.cc`):
   - Functions are exported using `TVM_FFI_DLL_EXPORT_TYPED_FUNC` macro
   - The macro registers functions with TVM-FFI's global function registry

2. **Python Side** (`run.py`):
   - `ObjectLoader` uses LLVM ORC JIT to load the `.o` file
   - `get_function()` looks up symbols in the JIT-compiled code
   - Functions are called like normal Python functions

## Next Steps

- Modify `add.cc` to add your own functions
- Recompile with `./compile.sh`
- Load and test in Python

For more advanced examples, see the parent `example.py` file.
