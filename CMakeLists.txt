cmake_minimum_required (VERSION 2.6)
project (RUBIK)

if (CMAKE_COMPILER_IS_GNUCC)
    execute_process (COMMAND ${CMAKE_CXX_COMPILER} -dumpversion
                     OUTPUT_VARIABLE GCC_VERSION)
    string (STRIP ${GCC_VERSION} GCC_VERSION)

    if (GCC_VERSION VERSION_GREATER 4.7.0)
        set (CMAKE_CXX_FLAGS "-Wall -pedantic -std=gnu++11")
    else ()
        message(FATAL_ERROR "GCC 4.7.0 or higher is required!")
    endif ()
endif ()

find_package (OpenGL REQUIRED)

include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2_TTF REQUIRED SDL2_ttf)
pkg_search_module(SDL2_IMAGE REQUIRED SDL2_image)

file (GLOB_RECURSE RUBIK_SOURCES "src/*.cpp")
include_directories ("src" "src/common" "src/game" "src/math" "src/opengl" "src/utils")

set (RUBIK_EXECUTABLE "rubik")
add_executable (${RUBIK_EXECUTABLE} ${RUBIK_SOURCES})

target_link_libraries (${RUBIK_EXECUTABLE} ${OPENGL_LIBRARIES})
target_link_libraries (${RUBIK_EXECUTABLE} ${SDL2_LIBRARIES})
target_link_libraries (${RUBIK_EXECUTABLE} ${SDL2_TTF_LIBRARIES})
target_link_libraries (${RUBIK_EXECUTABLE} ${SDL2_IMAGE_LIBRARIES})
