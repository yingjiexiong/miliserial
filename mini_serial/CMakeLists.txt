cmake_minimum_required(VERSION 3.0.0)
project(miniserial )
set(miniserial_VERSION_MAJOR 0)
set(miniserial_VERSION_MINOR 1)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(CTest)
enable_testing()

# find all source file in current directory then save the name that source file to DIR_SRCS
aux_source_directory(. DIR_SRCS)

# generate .dll
ADD_LIBRARY(miniserial SHARED  ${DIR_SRCS})

SET(LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
include(CPack)