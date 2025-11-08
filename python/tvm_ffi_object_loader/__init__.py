"""TVM-FFI Object Loader

This module provides functionality to load object files (.o) compiled with TVM-FFI
exports using LLVM ORC JIT v2.

Example:
    >>> from tvm_ffi_object_loader import ObjectLoader
    >>> loader = ObjectLoader()
    >>> loader.load("example.o")
    >>> func = loader.get_function("my_function")
    >>> result = func(arg1, arg2)
"""

from .loader import ObjectLoader

__all__ = ["ObjectLoader"]
__version__ = "0.1.0"
