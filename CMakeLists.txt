cmake_minimum_required(VERSION 3.20)

### Configure project ###

project(LICO)
set(CMAKE_CXX_STANDARD 17)

### Compiler Flags ###

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
CHECK_CXX_COMPILER_FLAG("-fopenmp" COMPILER_SUPPORTS_OPENMP)
CHECK_CXX_COMPILER_FLAG("-Rpass=loop-vectorize -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize" COMPILER_SUPPORTS_AUTO_VEC)

### Project Options ###

option(OPTIMIZE_FOR_NATIVE "Build with -march=native" ON)
option(USE_OPENMP "Build with OpenMP support" ON)
option(BUILD_STATIC "Build with static libraries" OFF)
option(ALLOW_STATIC "Allow building static libraries from source if not found locally" ON)
option(USE_HIGHWAY "Build with Google's Highway portable SIMD library" ON)

### Debugging Options ###

option(DBG_AUTO_VEC "Debug auto-vectorization" OFF)

### Set CFlags ###

if(OPTIMIZE_FOR_NATIVE AND COMPILER_SUPPORTS_MARCH_NATIVE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()

if(USE_OPENMP AND COMPILER_SUPPORTS_OPENMP)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
endif()

if(DBG_AUTO_VEC AND COMPILER_SUPPORTS_AUTO_VEC) # The following assumes using Clang
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Rpass=loop-vectorize -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize")
endif()

### Libraries ###

if(USE_HIGHWAY)
    find_package(hwy)
    if(hwy_FOUND AND NOT BUILD_STATIC)
        message(STATUS "Highway found, enabling USE_HIGHWAY")
    elseif(ALLOW_STATIC) # BUILD_STATIC flag should fallthrough to this.
        message(STATUS "Highway not found, building from source!")
        include(FetchContent)
        FetchContent_Declare(
            hwy
            GIT_REPOSITORY https://github.com/google/highway.git
            GIT_TAG        master
        )
        FetchContent_MakeAvailable(hwy)
    else()
        message(FATAL_ERROR "Highway not found, set ALLOW_STATIC to build it from source!")
    endif()
    add_definitions(-DUSE_HIGHWAY)
endif()

### Source and Build Directory Checks ###

if(CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL ERROR "Redirect build object directory to sepak/bin instead of LICO/src!")
endif()

if(CMAKE_SOURCE_DIR MATCHES " ")
    message("Source directory contains unescaped spaces, may cause compilation errors!")
endif()
if(CMAKE_BUILD_DIR MATCHES " ")
    message("Build directory contains unescaped spaces, may cause compilation errors!")
endif()

### Fetch and batch source files from src/* ###

file(GLOB SRC_LICO_CPP_FILES 
  "src/LICO-compressor.cpp"
)
file(GLOB SRC_UNLICO_CPP_FILES 
  "src/LICO-decompressor.cpp"
)
file(GLOB SRC_HPP_FILES 
  "include/*.hpp"
)
file(GLOB SRC_H_FILES 
  "include/*.h"
)

### Create executables, then link libraries ###

add_executable(lico
    ${SRC_LICO_CPP_FILES}
    ${SRC_HPP_FILES}   
    ${SRC_H_FILES}
)

add_executable(unlico
    ${SRC_UNLICO_CPP_FILES}
    ${SRC_HPP_FILES}   
    ${SRC_H_FILES}
)

if(USE_HIGHWAY)
    target_link_libraries(lico PRIVATE hwy::hwy)
    target_link_libraries(unlico PRIVATE hwy::hwy)
endif()
