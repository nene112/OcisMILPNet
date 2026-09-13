cmake_minimum_required(VERSION 3.20)
project(OcisMILPNetSparsityStandalone LANGUAGES CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SCIP_ROOT "${CMAKE_SOURCE_DIR}/third_party/scip" CACHE PATH
    "SCIP Optimization Suite installation root")
find_path(SCIP_INCLUDE_DIR
    NAMES scip/scip.h
    HINTS "${SCIP_ROOT}/include"
)
find_library(SCIP_LIBRARY
    NAMES libscip scip
    HINTS "${SCIP_ROOT}/lib"
)
set(SCIP_RUNTIME_DIR "${SCIP_ROOT}/bin" CACHE PATH
    "Directory containing SCIP runtime DLLs")

if(NOT SCIP_INCLUDE_DIR OR NOT SCIP_LIBRARY)
    message(FATAL_ERROR
        "SCIP was not found. Set -DSCIP_ROOT=<SCIP Optimization Suite root>; "
        "the directory must contain include/scip/scip.h and lib/libscip.lib.")
endif()

add_subdirectory(OcisMILPNet_dll)
add_subdirectory(OcisMILPNet_bin)
add_subdirectory(OcisMILPNet_dlltest)
