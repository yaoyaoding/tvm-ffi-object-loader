"""
Example demonstrating how to use tvm-ffi-object-loader

This example assumes you have object files compiled with TVM-FFI exports.
For a working demo, see demos/incremental_loading/ in the parent directory.
"""

from tvm_ffi_object_loader import ObjectLoader


def main():
    # Create loader
    loader = ObjectLoader()
    
    # Example 1: Load a single object file
    print("Example 1: Single object file")
    loader.load("../demos/incremental_loading/tvm_ffi_example.o")
    
    # Get and call functions
    simple_add = loader.get_function("simple_add")
    result = simple_add(10, 20)
    print(f"simple_add(10, 20) = {result}")
    
    simple_multiply = loader.get_function("simple_multiply")
    result = simple_multiply(7, 8)
    print(f"simple_multiply(7, 8) = {result}")
    
    factorial = loader.get_function("factorial")
    result = factorial(5)
    print(f"factorial(5) = {result}")
    
    # Example 2: Incremental loading
    print("\nExample 2: Incremental loading")
    loader.load("../demos/incremental_loading/tvm_ffi_example2.o")
    
    # Functions from the second object file
    power = loader.get_function("power")
    result = power(2, 10)
    print(f"power(2, 10) = {result}")
    
    fibonacci = loader.get_function("fibonacci")
    result = fibonacci(10)
    print(f"fibonacci(10) = {result}")
    
    # Verify functions from first object file still work after incremental load
    result = simple_add(100, 200)
    print(f"simple_add(100, 200) = {result} (still works after incremental loading)")
    
    print("\n✓ All examples completed successfully!")


if __name__ == "__main__":
    main()
