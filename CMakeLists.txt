cmake_minimum_required(VERSION 3.50)

project(VSRG)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "" FORCE)

set(CACHE CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -finput-charset=UTF-8")

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

set(SOURCES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/draw.cpp
    ${SRC_DIR}/playfield.cpp
    ${SRC_DIR}/map_parser.cpp
)

set(HEADERS
#    ${SRC_DIR}/draw.h
#    ${SRC_DIR}/playfield.h
    ${SRC_DIR}/map_parser.hpp
)

add_executable(vsrg ${SOURCES} ${HEADERS})

add_subdirectory(SDL)

set_target_properties(vsrg PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(vsrg PRIVATE SDL3::SDL3)
target_link_libraries(vsrg PRIVATE SDL3::Headers)