#!/usr/bin/env python3
"""
Quick Start Example - Load and call functions from add.o

This script demonstrates how to:
1. Create an ObjectLoader instance
2. Load a compiled object file
3. Get functions by name
4. Call them like regular Python functions
"""

import sys
from pathlib import Path

# Add the parent python directory to path for imports
sys.path.insert(0, str(Path(__file__).parent.parent.parent / "python"))

from tvm_ffi_object_loader import ObjectLoader
import os


def main():
    # Check if object file exists
    obj_file = "add.o"
    if not os.path.exists(obj_file):
        print(f"Error: {obj_file} not found!")
        print("Please run ./compile.sh first to compile the C++ code.")
        return 1
    
    print(f"Loading object file: {obj_file}")
    
    # Create loader and load the object file
    loader = ObjectLoader()
    loader.load(obj_file)
    
    print("✓ Object file loaded successfully\n")
    
    # Get and call the 'add' function
    print("=== Testing add function ===")
    add = loader.get_function("add")
    result = add(10, 20)
    print(f"add(10, 20) = {result}")
    assert result == 30, f"Expected 30, got {result}"
    
    # Get and call the 'multiply' function
    print("\n=== Testing multiply function ===")
    multiply = loader.get_function("multiply")
    result = multiply(7, 6)
    print(f"multiply(7, 6) = {result}")
    assert result == 42, f"Expected 42, got {result}"
    
    # Get and call the 'fibonacci' function
    print("\n=== Testing fibonacci function ===")
    fibonacci = loader.get_function("fibonacci")
    result = fibonacci(10)
    print(f"fibonacci(10) = {result}")
    assert result == 55, f"Expected 55, got {result}"
    
    # Get and call the 'concat' function
    print("\n=== Testing concat function ===")
    concat = loader.get_function("concat")
    result = concat("Hello, ", "World!")
    print(f"concat('Hello, ', 'World!') = '{result}'")
    assert result == "Hello, World!", f"Expected 'Hello, World!', got '{result}'"
    
    print("\n" + "="*50)
    print("✓ All tests passed successfully!")
    print("="*50)
    
    return 0


if __name__ == "__main__":
    exit(main())
