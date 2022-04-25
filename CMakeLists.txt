cmake_minimum_required(VERSION 3.10)

# set the project name
project(GoLpp VERSION 0.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(OpenCL REQUIRED)
#find_package(OpenCLHeadersCpp REQUIRED)
find_package(Threads REQUIRED)
find_package(PkgConfig)

pkg_check_modules(GTKMM REQUIRED gtkmm-3.0 cairomm-1.0)

add_subdirectory(src)


