cmake_minimum_required(VERSION 3.10)
project(tomopackets)

include_directories(
    "include"
)
include(FetchContent)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add vcpkg toolchain file
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
set(EXEC_NAME "test_server")

set(
    SOURCE_NAMES
    "test/test_server.cpp"
)

# --------------------------------------------------------------------------------------------
# Remove old target if it exists
if (TARGET ${EXEC_NAME})
    remove_executable(${EXEC_NAME})
endif()

# --------------------------------------------------------------------------------------------
# ZeroMQ version 4.X
if (NOT TARGET zmq)
find_package(ZeroMQ CONFIG REQUIRED)
if (ZeroMQ_FOUND)
    add_library(zmq INTERFACE)
    target_include_directories(zmq INTERFACE ${ZeroMQ_INCLUDE_DIRS})
    target_link_libraries(zmq INTERFACE ${ZeroMQ_LIBRARIES})
else()
    message(FATAL_ERROR "ZeroMQ not found. Please install it using vcpkg.")
endif()
endif()

# --------------------------------------------------------------------------------------------
# cppzmq

# vvvvvvvvvvvvvvvvvvvvvvvvv this is the important part
set(CPPZMQ_BUILD_TESTS OFF)
# ^^^^^^^^^^^^^^^^^^^^^^^^^

FetchContent_Declare(cppzmq
    GIT_REPOSITORY https://github.com/zeromq/cppzmq.git
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/cppzmq
)
FetchContent_GetProperties(cppzmq)
if(NOT cppzmq_POPULATED)
    FetchContent_Populate(cppzmq)
    add_subdirectory(${cppzmq_SOURCE_DIR} ${cppzmq_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# --------------------------------------------------------------------------------------------
# Add executable
add_executable(${EXEC_NAME} ${SOURCE_NAMES})
target_link_libraries(${EXEC_NAME} zmq cppzmq)

# --------------------------------------------------------------------------------------------
# Boost
find_package(Boost REQUIRED)
# --------------------------------------------------------------------------------------------
set(
    LIB_NAMES
    "zmq"
    "cppzmq"
    "Boost::boost"
)

if (NOT TOMOP_LIB_ONLY)

target_link_libraries(${EXEC_NAME} ${LIB_NAMES})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${EXEC_NAME} PRIVATE /W4 /EHsc /std:c++17 /permissive-)
else()
    target_compile_options(${EXEC_NAME} PRIVATE /W4 /EHsc /std:c++17 /permissive-)
endif()


# --------------------------------------------------------------------------------------------
# pybind11
set(PYBIND11_CPP_STANDARD /std:c++17)

FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
)
FetchContent_MakeAvailable(pybind11)

set(BINDING_NAME "py_tomop")
set(BINDING_SOURCES "tomop/module.cpp")

pybind11_add_module(${BINDING_NAME} ${BINDING_SOURCES})
target_link_libraries(${BINDING_NAME} PRIVATE ${LIB_NAMES})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${BINDING_NAME} PRIVATE /W4 /EHsc  /std:c++17 /permissive-)
else()
    target_compile_options(${BINDING_NAME} PRIVATE /W4 /EHsc  /std:c++17 /permissive-)
endif()
endif()

add_library(tomop INTERFACE)
target_include_directories(tomop INTERFACE "include")
target_link_libraries(tomop INTERFACE cppzmq zmq)

