/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*!
 * \file orcjit_library.cc
 * \brief LLVM ORC JIT based library implementation
 */

#include <tvm/ffi/object_loader/orcjit_library.h>
#include <tvm/ffi/object_loader/orcjit_library.h>
#include <tvm/ffi/memory.h>
#include <tvm/ffi/reflection/registry.h>
#include <tvm/ffi/extra/c_env_api.h>
#include <tvm/ffi/cast.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/MemoryBuffer.h>
#include <iostream>
#include <memory>
#include <string>

namespace tvm {
namespace ffi {
namespace object_loader {

// Initialize LLVM native target (only once)
struct LLVMInitializer {
  LLVMInitializer() {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
  }
};

static LLVMInitializer llvm_initializer;

ORCJITLibrary::ORCJITLibrary() : jit_(nullptr) {}

void ORCJITLibrary::Initialize() {
  // Create LLJIT instance
  auto jit_or_err = llvm::orc::LLJITBuilder().create();
  if (!jit_or_err) {
    auto err = jit_or_err.takeError();
    std::string err_msg;
    llvm::handleAllErrors(std::move(err), [&](const llvm::ErrorInfoBase& eib) {
      err_msg = eib.message();
    });
    throw std::runtime_error("Failed to create LLJIT: " + err_msg);
  }
  jit_ = std::move(*jit_or_err);
}

ObjectPtr<Library> ORCJITLibrary::Create(const std::string& path) {
  auto lib = make_object<ORCJITLibrary>();
  lib->Initialize();
  lib->LoadAdditionalObject(path);
  return lib;
}

void ORCJITLibrary::LoadAdditionalObject(const std::string& path) {
  if (!jit_) {
    throw std::runtime_error("LLJIT not initialized");
  }

  // Load object file
  auto buffer_or_err = llvm::MemoryBuffer::getFile(path);
  if (!buffer_or_err) {
    throw std::runtime_error("Failed to read object file: " + path);
  }

  // Add object file to JIT
  auto err = jit_->addObjectFile(std::move(*buffer_or_err));
  if (err) {
    std::string err_msg;
    llvm::handleAllErrors(std::move(err), [&](const llvm::ErrorInfoBase& eib) {
      err_msg = eib.message();
    });
    throw std::runtime_error("Failed to add object file: " + err_msg);
  }
}

void* ORCJITLibrary::GetSymbol(const String& name) {
  if (!jit_) {
    return nullptr;
  }

  auto symbol_or_err = jit_->lookup(name.c_str());
  if (!symbol_or_err) {
    // Symbol not found, consume the error
    llvm::consumeError(symbol_or_err.takeError());
    return nullptr;
  }

  return symbol_or_err->toPtr<void*>();
}

}  // namespace object_loader

// Minimal LibraryModuleObj implementation for wrapping our Library
class LibraryModuleObj final : public ModuleObj {
 public:
  explicit LibraryModuleObj(ObjectPtr<Library> lib) : lib_(std::move(lib)) {}

  const char* kind() const final { return "orcjit_library"; }

  int GetPropertyMask() const final { return Module::kRunnable; }

  Optional<Function> GetFunction(const String& name) final {
    // Special handling for __load to enable incremental loading
    if (name == "__load") {
      // Create a weak reference to the library to avoid circular reference
      Library* lib_ptr = lib_.get();
      return Function::FromPacked([lib_ptr](PackedArgs args, Any* rv) {
        TVM_FFI_ICHECK_EQ(args.size(), 1) << "Expected 1 argument (path), got " << args.size();
        String path = args[0].cast<String>();
        auto* orcjit_lib = static_cast<object_loader::ORCJITLibrary*>(lib_ptr);
        orcjit_lib->LoadAdditionalObject(path.c_str());
      });
    }
    
    TVMFFISafeCallType faddr;
    faddr = reinterpret_cast<TVMFFISafeCallType>(lib_->GetSymbolWithSymbolPrefix(name));
    // ensure the function keeps the Library Module alive
    Module self_strong_ref = GetRef<Module>(this);
    if (faddr != nullptr) {
      return Function::FromPacked([faddr, self_strong_ref](PackedArgs args, Any* rv) {
        TVM_FFI_ICHECK_LT(rv->type_index(), TypeIndex::kTVMFFIStaticObjectBegin);
        TVM_FFI_CHECK_SAFE_CALL((*faddr)(nullptr, reinterpret_cast<const TVMFFIAny*>(args.data()),
                                         args.size(), reinterpret_cast<TVMFFIAny*>(rv)));
      });
    }
    return std::nullopt;
  }

 private:
  ObjectPtr<Library> lib_;
};

// Helper to create a library module from a Library object
// This implements our own version since the tvm-ffi internal one isn't exported
Module CreateLibraryModule(ObjectPtr<Library> lib) {
  return Module(make_object<LibraryModuleObj>(lib));
}

}  // namespace ffi
}  // namespace tvm

// Register the object file loader with TVM-FFI module system
TVM_FFI_STATIC_INIT_BLOCK() {
  using namespace tvm::ffi;
  using namespace tvm::ffi::object_loader;
  namespace refl = tvm::ffi::reflection;

  // Register loader for .o files
  refl::GlobalDef().def("ffi.Module.load_from_file.o",
                        [](const String& library_path, const String&) {
                          return CreateLibraryModule(ORCJITLibrary::Create(library_path.c_str()));
                        });
}
