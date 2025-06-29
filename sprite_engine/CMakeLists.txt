cmake_minimum_required(VERSION 3.50)

project(sprite_engine)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(SOURCES
    ${SRC_PATH}/scene.cpp
    ${SRC_PATH}/sprite.cpp
    ${SRC_PATH}/bmp_sprite.cpp
    ${SRC_PATH}/prefab.cpp)

set(HEADERS
    ${SRC_PATH}/scene.hpp
    ${SRC_PATH}/sprite.hpp
    ${SRC_PATH}/bmp_sprite.hpp
    ${SRC_PATH}/prefab.hpp)

add_library(sprite_engine_lib SHARED ${SOURCES})
add_library(sprite_engine::lib ALIAS sprite_engine_lib)

target_include_directories(sprite_engine_lib PRIVATE ${CMAKE_SOURCE_DIR}/src/game_scripts)

add_library(sprite_engine_header INTERFACE)
target_include_directories(sprite_engine_header INTERFACE ${SRC_PATH})

set_target_properties(sprite_engine_lib PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

add_library(sprite_engine::header ALIAS sprite_engine_header)

target_link_libraries(sprite_engine_lib PRIVATE SDL3::SDL3)

target_link_libraries(sprite_engine_lib PRIVATE SDL3_image::SDL3_image)
target_include_directories(sprite_engine_lib PRIVATE ${CMAKE_SOURCE_DIR}/SDL_image/include)
