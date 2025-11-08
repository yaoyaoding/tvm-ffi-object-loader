/*
 * Quick Start Example - Simple Math Functions
 * 
 * This file demonstrates how to export C++ functions using TVM-FFI
 * so they can be loaded dynamically at runtime with tvm-ffi-object-loader.
 */

#include <tvm/ffi/function.h>

// Simple addition function
TVM_FFI_DLL_EXPORT_TYPED_FUNC(add, [](int a, int b) {
    return a + b;
});

// Multiplication function
TVM_FFI_DLL_EXPORT_TYPED_FUNC(multiply, [](int a, int b) {
    return a * b;
});

// Fibonacci function (recursive)
int fib_impl(int n) {
    if (n <= 1) return n;
    return fib_impl(n - 1) + fib_impl(n - 2);
}

TVM_FFI_DLL_EXPORT_TYPED_FUNC(fibonacci, [](int n) {
    return fib_impl(n);
});

// String concatenation example
TVM_FFI_DLL_EXPORT_TYPED_FUNC(concat, [](std::string a, std::string b) {
    return a + b;
});
