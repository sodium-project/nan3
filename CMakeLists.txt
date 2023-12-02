cmake_minimum_required(VERSION 3.23)

project(nan3 LANGUAGES CXX)

#==================================================================================================
# 0. Get Clang installation directory
#==================================================================================================
if(DEFINED CLANG_ROOT)
    set(NAN3_CLANG_PACKAGE_DIR "${CLANG_ROOT}/../../..")
elseif(DEFINED CLANG_DIR)
    set(NAN3_CLANG_PACKAGE_DIR "${CLANG_DIR}/../../..")
endif()
mark_as_advanced(NAN3_CLANG_PACKAGE_DIR)

# Set this to a valid Clang installation directory. This is most likely where
# LLVM is installed on your system.
set(NAN3_CLANG_INSTALL_DIR "${NAN3_CLANG_PACKAGE_DIR}" CACHE PATH "Clang installation directory")

#==================================================================================================
# 1. Verify Clang installation directory
#==================================================================================================
set(NAN3_LLVM_INCLUDE_DIR "${NAN3_CLANG_INSTALL_DIR}/include/llvm")
if(NOT EXISTS "${NAN3_LLVM_INCLUDE_DIR}")
    message(FATAL_ERROR " NAN3_CLANG_INSTALL_DIR (${NAN3_LLVM_INCLUDE_DIR}) is invalid.")
endif()

set(NAN3_LLVM_CMAKE_FILE "${NAN3_CLANG_INSTALL_DIR}/lib/cmake/clang/ClangConfig.cmake")
if(NOT EXISTS "${NAN3_LLVM_CMAKE_FILE}")
    message(FATAL_ERROR " NAN3_LLVM_CMAKE_FILE (${NAN3_LLVM_CMAKE_FILE}) is invalid.")
endif()

#==================================================================================================
# 2. Load Clang configuration
#==================================================================================================
list(APPEND CMAKE_PREFIX_PATH "${NAN3_CLANG_INSTALL_DIR}/lib/cmake/clang/")

find_package(Clang REQUIRED CONFIG)

# Sanity check. As Clang does not expose e.g. `CLANG_VERSION_MAJOR` through
# AddClang.cmake, we have to use LLVM_VERSION_MAJOR instead.
# TODO: Revisit when next version is released.
if("${LLVM_VERSION_MAJOR}" VERSION_LESS 16)
  message(FATAL_ERROR "Found LLVM ${LLVM_VERSION_MAJOR}, but need LLVM 16 or above")
endif()

message(STATUS "Found Clang ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using ClangConfig.cmake in: ${NAN3_CLANG_INSTALL_DIR}")

message("CLANG STATUS:
  Includes (clang)    ${CLANG_INCLUDE_DIRS}
  Includes (llvm)     ${LLVM_INCLUDE_DIRS}"
)

# Set the LLVM and Clang header and library paths
include_directories(SYSTEM "${LLVM_INCLUDE_DIRS};${CLANG_INCLUDE_DIRS}")

#==================================================================================================
# 3. nan3 build configuration
#==================================================================================================
set(CMAKE_CXX_STANDARD 20 CACHE STRING "")

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type (default Debug):" FORCE)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fdiagnostics-color=always")

# LLVM/Clang is normally built without RTTI. Be consistent with that.
if(NOT LLVM_ENABLE_RTTI)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

# -fvisibility-inlines-hidden is set when building LLVM and on Darwin warnings
# are triggered if llvm-tutor is built without this flag (though otherwise it
# builds fine). For consistency, add it here too.
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-fvisibility-inlines-hidden" SUPPORTS_FVISIBILITY_INLINES_HIDDEN_FLAG)
if(${SUPPORTS_FVISIBILITY_INLINES_HIDDEN_FLAG} EQUAL "1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
endif()

# Set the build directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

include(AddLLVM)

#==================================================================================================
# 4. Add sub-targets
# Doing this at the end so that all definitions and link/include paths are
# available for the sub-projects.
#==================================================================================================
add_subdirectory(lib)
add_subdirectory(test)