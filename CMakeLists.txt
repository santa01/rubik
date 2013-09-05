cmake_minimum_required (VERSION 2.6)
project (RUBIK)

if (CMAKE_COMPILER_IS_GNUCC)
    execute_process (COMMAND ${CMAKE_CXX_COMPILER} -dumpversion
                     OUTPUT_VARIABLE GCC_VERSION)
    string (STRIP ${GCC_VERSION} GCC_VERSION)

    if (GCC_VERSION VERSION_GREATER 4.7.0)
        set (CMAKE_CXX_FLAGS "-O2 -Wall -pedantic -std=gnu++11")
    else ()
        message(FATAL_ERROR "GCC 4.7.0 or higher is required!")
    endif ()
endif ()

find_package (OpenGL REQUIRED)

include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2_TTF REQUIRED SDL2_ttf)
pkg_search_module(SDL2_IMAGE REQUIRED SDL2_image)
pkg_search_module(GLEW REQUIRED glew)

set (RUBIK_DATADIR ${CMAKE_INSTALL_PREFIX}/share/rubik CACHE PATH "Data directory")
set (RUBIK_INCLUDE "src" "src/common" "src/game" "src/math" "src/opengl" "src/utils")

file (GLOB_RECURSE RUBIK_SOURCES "src/*.cpp")
include_directories (${RUBIK_INCLUDE} ${PROJECT_BINARY_DIR})

set (RUBIK_NAME "Rubik")
set (RUBIK_DESCRIPTION "Rubik's Cube game")
set (RUBIK_VERSION 0.1.1)

configure_file (Config.h.in Config.h @ONLY)

set (RUBIK_EXECUTABLE "rubik")
add_executable (${RUBIK_EXECUTABLE} ${RUBIK_SOURCES})

target_link_libraries (${RUBIK_EXECUTABLE} ${OPENGL_LIBRARIES})
target_link_libraries (${RUBIK_EXECUTABLE} ${SDL2_LIBRARIES})
target_link_libraries (${RUBIK_EXECUTABLE} ${SDL2_TTF_LIBRARIES})
target_link_libraries (${RUBIK_EXECUTABLE} ${SDL2_IMAGE_LIBRARIES})
target_link_libraries (${RUBIK_EXECUTABLE} ${GLEW_LIBRARIES})
