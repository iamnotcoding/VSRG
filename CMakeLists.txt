cmake_minimum_required(VERSION 3.50)

project(VSRG)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "" FORCE)

set(CACHE CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -finput-charset=UTF-8")

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(SOURCES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/game.cpp
    ${SRC_DIR}/game_scripts/map_parser.cpp
    ${SRC_DIR}/game_scripts/main_script.cpp
    ${SRC_DIR}/test.cpp
    ${SRC_DIR}/play_scene.cpp
)

set(HEADERS
    ${SRC_DIR}/game.hpp
    ${SRC_DIR}/game_scripts/map_parser.hpp
    ${SRC_DIR}/game_scripts/main_script.hpp
    ${SRC_DIR}/test.hpp
    ${SRC_DIR}/json_util.hpp
    ${SRC_DIR}/play_scene.hpp
)

add_executable(vsrg ${SOURCES} ${HEADERS})


set_target_properties(vsrg PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

add_subdirectory(SDL)

target_link_libraries(vsrg PRIVATE SDL3::SDL3)
target_link_libraries(vsrg PRIVATE SDL3::Headers)

add_subdirectory(SDL_image)

target_link_libraries(vsrg PRIVATE SDL3_image::SDL3_image)
target_include_directories(vsrg PRIVATE ${CMAKE_SOURCE_DIR}/SDL_image/include)

add_subdirectory(json)
target_link_libraries(vsrg PUBLIC nlohmann_json::nlohmann_json)

add_subdirectory(sprite_engine)

target_link_libraries(vsrg PRIVATE sprite_engine::lib)
target_link_libraries(vsrg PRIVATE sprite_engine::header)

#starget_include_directories(vsrg PRIVATE ${CMAKE_SOURCE_DIR}/src/game_scripts)