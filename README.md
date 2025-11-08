# TVM-FFI Object Loader

A Python package that enables dynamic loading of TVM-FFI exported object files (`.o`) using LLVM ORC JIT v2.

## Features

- **Dynamic Loading**: Load compiled object files at runtime using LLVM's ORC JIT v2
- **Incremental Loading**: Add multiple object files to the same loader instance
- **TVM-FFI Integration**: Seamlessly works with TVM-FFI's stable C ABI
- **Python API**: Simple Pythonic interface for loading and calling compiled functions
- **Standalone Package**: Works alongside apache-tvm-ffi without conflicts

## Installation

### Prerequisites

- Python 3.8+
- CMake 3.18+
- LLVM 14+ (with ORC JIT support)
- Ninja build system (recommended)

### Build from Source

1. Clone the repository with submodules:

```bash
git clone --recursive git@github.com:yaoyaoding/tvm-ffi-object-loader.git
cd tvm-ffi-object-loader
```

2. Build TVM-FFI dependency:

```bash
cd 3rdparty/tvm-ffi
mkdir build && cd build
cmake .. -G Ninja
ninja
cd ../../..
```

3. Build the package:

```bash
./build.sh
```

4. Install:

```bash
# Using pip
pip install .

# Or for development (editable install)
pip install -e .
```

## Usage

### Basic Example

```python
from tvm_ffi_object_loader import ObjectLoader

# Create a loader instance
loader = ObjectLoader()

# Load an object file
loader.load("example.o")

# Get and call a function
add_func = loader.get_function("simple_add")
result = add_func(1, 2)
print(f"Result: {result}")  # Output: Result: 3
```

### Incremental Loading

Load multiple object files and access functions from all of them:

```python
from tvm_ffi_object_loader import ObjectLoader

loader = ObjectLoader()

# Load first object file
loader.load("math_ops.o")
add = loader.get_function("simple_add")

# Load second object file - functions from first remain accessible
loader.load("string_ops.o")
concat = loader.get_function("string_concat")

# Both functions work
print(add(10, 20))           # From math_ops.o
print(concat("Hello", "World"))  # From string_ops.o
```

### Direct Module Access

You can also use TVM-FFI's `load_module` directly (`.o` files are automatically handled):

```python
import tvm_ffi

# Load object file as a module
module = tvm_ffi.load_module("example.o")

# Get function
func = module.get_function("my_function")
result = func(arg1, arg2)
```

## How It Works

1. **C++ Backend**: The package implements a `Library` subclass using LLVM's ORC JIT v2 (`LLJIT`)
2. **Registration**: Registers with TVM-FFI as a loader for `.o` files via `ffi.Module.load_from_file.o`
3. **Symbol Resolution**: Uses LLJIT's `lookup()` to resolve TVM-FFI exported symbols
4. **Module Integration**: Wraps the ORC JIT library in `LibraryModuleObj` for seamless TVM-FFI integration

## Development

### Building with CMake

```bash
mkdir build && cd build
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build .
```

### Project Structure

```
tvm-ffi-object-loader/
├── CMakeLists.txt              # CMake build configuration
├── pyproject.toml              # Python package metadata
├── README.md                   # This file
├── build.sh                    # Build script
├── example.py                  # Usage example
├── 3rdparty/
│   └── tvm-ffi/               # TVM-FFI submodule
├── include/
│   └── tvm/ffi/object_loader/
│       └── orcjit_library.h    # C++ header
├── src/
│   └── ffi/
│       └── orcjit_library.cc   # C++ implementation
└── python/
    └── tvm_ffi_object_loader/
        ├── __init__.py         # Module exports
        └── loader.py           # Python ObjectLoader class
```

## Examples

See `example.py` for a complete demonstration of incremental loading.

## Requirements

The package depends on:
- `apache-tvm-ffi>=0.1.0` - TVM-FFI library
- LLVM 14+ (linked at build time) - For ORC JIT functionality

## License

Apache License 2.0

## Contributing

Contributions are welcome! Please ensure that:
1. Code follows the existing style
2. New features include tests
3. Documentation is updated

## Troubleshooting

### Symbol not found errors

Make sure your object file was compiled with TVM-FFI export macros:
```cpp
#include <tvm/ffi/c_ffi_api.h>

TVM_FFI_DLL_EXPORT_TYPED_FUNC(my_function, [](int a, int b) {
    return a + b;
});
```

### LLVM version mismatch

Ensure the LLVM version used to build this package matches your system's LLVM installation.

### TVM-FFI not found

Make sure you've initialized and built the submodule:
```bash
git submodule update --init --recursive
cd 3rdparty/tvm-ffi && mkdir build && cd build
cmake .. -G Ninja && ninja
```
