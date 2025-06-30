#cmake_minimum_required(VERSION 3.13)
#project(EmbeddedUkernelTest C CXX)

#set(CMAKE_C_STANDARD 11)
#set(CMAKE_CXX_STANDARD 17)

#add_library(hw_stub snitch_hw_vector_add.c)

#add_executable(main_test main_test.c)
#target_link_libraries(main_test hw_stub)

# Build the rewrite pass as part of mlir-opt plugin or standalone tool if needed

cmake_minimum_required(VERSION 3.13)
project(LinalgVectorAddPass)

set(CMAKE_CXX_STANDARD 17)

# MLIR installation location (from buddy-mlir or your IREE build)
set(MLIR_DIR "$ENV{HOME}/Projects/llvm-project/build/lib/cmake/mlir")
find_package(MLIR REQUIRED CONFIG)

message(STATUS "Found MLIR: ${MLIR_DIR}")
list(APPEND CMAKE_MODULE_PATH "${MLIR_DIR}")

include_directories(${MLIR_INCLUDE_DIRS})
include_directories(${LLVM_INCLUDE_DIRS})

message(STATUS "Found MLIR_INCLUDE_DIRS: ${MLIR_INCLUDE_DIRS}")
message(STATUS "Found LLVM_INCLUDE_DIRS: ${LLVM_INCLUDE_DIRS}")

# Use SHARED (not MODULE!) to get RTTI compatibility
add_library(LinalgVectorAddPass SHARED LowerLinalgVectorAddToHw.cpp)

target_link_libraries(LinalgVectorAddPass PRIVATE
  MLIRIR
  MLIRFuncDialect
  MLIRLinalgDialect
  MLIRArithDialect
  MLIRMemRefDialect
  MLIRPass
  MLIRSupport
  MLIRTransforms
)


# Make sure RTTI and PIC are enabled
set_target_properties(LinalgVectorAddPass PROPERTIES
  CXX_STANDARD 17
  POSITION_INDEPENDENT_CODE ON
)

