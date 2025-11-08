"""Python wrapper for ORC JIT object loader"""

import os
import ctypes
import tvm_ffi as ffi

# Ensure tvm_ffi's C library is loaded first with RTLD_GLOBAL
# This is needed because our library depends on symbols from libtvm_ffi.so
import tvm_ffi._ffi_api  # This loads the core library

# Load the C++ extension library to register the .o file loader
# This must happen before we try to load any .o files
def _load_library():
    """Load the C++ extension library."""
    # Try various locations
    search_paths = [
        # Installed location (site-packages)
        os.path.join(os.path.dirname(__file__), '..', '..', 'libtvm_ffi_object_loader.so'),
        # Development build
        os.path.join(os.path.dirname(__file__), '..', '..', 'build', 'lib', 'libtvm_ffi_object_loader.so'),
    ]
    
    # Also check sys.path
    import sys
    for path in sys.path:
        search_paths.append(os.path.join(path, 'libtvm_ffi_object_loader.so'))
    
    for lib_path in search_paths:
        lib_path = os.path.abspath(lib_path)
        if os.path.exists(lib_path):
            try:
                ctypes.CDLL(lib_path, mode=ctypes.RTLD_GLOBAL)
                return
            except OSError:
                continue
    
    import warnings
    warnings.warn("Could not find libtvm_ffi_object_loader.so - .o file loading may not work")

_load_library()

class ObjectLoader:
    """Load and manage object files with TVM-FFI exported functions.
    
    This class uses LLVM ORC JIT v2 to dynamically load object files and
    provides access to TVM-FFI exported functions.
    
    Example:
        >>> loader = ObjectLoader()
        >>> loader.load("example.o")
        >>> func = loader.get_function("simple_add")
        >>> result = func(1, 2)
    """
    
    def __init__(self):
        """Initialize the object loader."""
        # The actual loading will be handled by the C++ module registered
        # with tvm_ffi.load_module for .o files
        self._module = None
    
    def load(self, path: str):
        """Load an object file.
        
        Args:
            path: Path to the object file (.o)
            
        Returns:
            self for method chaining
        """
        if self._module is None:
            self._module = ffi.load_module(path)
        else:
            # Incremental loading using the __load function
            load_func = self._module.get_function("__load")
            load_func(path)
        return self
    
    def get_function(self, name: str):
        """Get a function by name from loaded object files.
        
        Args:
            name: Name of the function to retrieve
            
        Returns:
            TVM-FFI Function object that can be called
            
        Raises:
            ValueError: If no module is loaded or function not found
        """
        if self._module is None:
            raise ValueError("No object file loaded. Call load() first.")
        return self._module.get_function(name)
    
    @property
    def module(self):
        """Get the underlying TVM-FFI Module object."""
        return self._module
