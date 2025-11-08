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
 * \file orcjit_library.h
 * \brief LLVM ORC JIT based library implementation
 */
#ifndef TVM_FFI_OBJECT_LOADER_ORCJIT_LIBRARY_H_
#define TVM_FFI_OBJECT_LOADER_ORCJIT_LIBRARY_H_

#include <tvm/ffi/extra/module.h>
#include <tvm/ffi/object.h>
#include <tvm/ffi/string.h>
#include <ffi/extra/module_internal.h>  // For Library and CreateLibraryModule
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <memory>
#include <string>

namespace tvm {
namespace ffi {
namespace object_loader {

/*!
 * \brief Library implementation using LLVM ORC JIT v2
 * 
 * This class provides dynamic loading of object files using LLVM's
 * ORC JIT v2 API (LLJIT). It supports incremental loading of multiple
 * object files and symbol lookup.
 */
class ORCJITLibrary final : public Library {
 public:
  /*!
   * \brief Default constructor
   * \note Required for make_object
   */
  ORCJITLibrary();

  /*!
   * \brief Create an ORC JIT library by loading an object file
   * \param path Path to the object file to load
   * \return The created library instance
   */
  static ObjectPtr<Library> Create(const std::string& path);

  /*!
   * \brief Get a symbol address from the loaded library
   * \param name The symbol name to look up
   * \return Pointer to the symbol, or nullptr if not found
   */
  void* GetSymbol(const String& name) final;

  /*!
   * \brief Load an additional object file into this library
   * \param path Path to the object file to load
   * 
   * This allows incremental loading of multiple object files.
   * Previously loaded symbols remain accessible.
   */
  void LoadAdditionalObject(const std::string& path);

 private:
  /*! \brief Initialize the LLJIT instance */
  void Initialize();

  /*! \brief The LLVM ORC JIT instance */
  std::unique_ptr<llvm::orc::LLJIT> jit_;
};

}  // namespace object_loader
}  // namespace ffi
}  // namespace tvm

#endif  // TVM_FFI_OBJECT_LOADER_ORCJIT_LIBRARY_H_
