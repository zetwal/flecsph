CMAKE_MINIMUM_REQUIRED(VERSION 3.3)

PROJECT(OutputTests)

# Compiler settings
set(CMAKE_C_COMPILER mpicc)
set(CMAKE_CXX_COMPILER mpic++)
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_CXX_FLAGS "-fPIC")
set(CMAKE_CXX_STANDARD 11)


# Include directories
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/..)
include_directories(/home/pascal/NGC/IO/phdf5-1.8.18/install/include/)
include_directories(/home/pascal/NGC/IO/hdf5-1.8.18/install/include/)
include_directories(/home/pascal/NGC/IO/H5hut-1.99.13/install/include/)


# Link directories
link_directories(/home/pascal/NGC/IO/phdf5-1.8.18/install/lib/)
link_directories(/home/pascal/NGC/IO/hdf5-1.8.18/install/lib/)
link_directories(/home/pascal/NGC/IO/H5hut-1.99.13/install/lib/)

# Sources
set(SRC_LIST main.cpp)


# Options
option (ENABLE_DEBUG "enable debugging mode" ON)


# Executable
#add_library(Example Example.cpp)
add_executable(hdf5Test main.cpp)
target_link_libraries(hdf5Test hdf5.so hdf5_cpp.so H5hut.a)