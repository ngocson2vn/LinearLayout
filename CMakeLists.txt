cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

project(LinearLayout CXX C)


# #########
# LLVM
# #########
set(LLVM_BINARY_DIR ${PROJECT_SOURCE_DIR}/llvm-project/build)
add_subdirectory(llvm-project/llvm ${LLVM_BINARY_DIR})

# LLVM and MLIR headers
include_directories(${PROJECT_SOURCE_DIR}/llvm-project/llvm/include)
include_directories(${PROJECT_SOURCE_DIR}/llvm-project/mlir/include)
include_directories(".")
include_directories(third_party)

add_subdirectory(third_party/f2reduce)

add_library(LinearLayout
  GenericSwizzling.cpp
  LayoutUtils.cpp
  LinearLayout.cpp
)

add_dependencies(LinearLayout
  MLIRIR
  MLIRLLVMDialect
  f2reduce
)

#======================================================
# main
#======================================================
add_llvm_executable(main main.cpp PARTIAL_SOURCES_INTENDED)
target_compile_options(main PRIVATE -fno-rtti -g -O0)
llvm_update_compile_flags(main)
target_link_libraries(main PRIVATE
  LinearLayout
  # MLIR core
  MLIRIR
)

mlir_check_all_link_libraries(main)

