#
# Copyright (c) 2013 Pavlo Lavrenenko
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

cmake_minimum_required (VERSION 2.6)
project (RUBIK)

if (CMAKE_COMPILER_IS_GNUCC)
    execute_process (COMMAND ${CMAKE_CXX_COMPILER} -dumpversion
                     OUTPUT_VARIABLE GCC_VERSION)
    string (STRIP ${GCC_VERSION} GCC_VERSION)
endif ()

if (DEFINED GCC_VERSION AND GCC_VERSION VERSION_GREATER 4.7.0)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=gnu++11")
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
    set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
    set (CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -Os")
    set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -g -O3")
else ()
    message (FATAL_ERROR "GCC 4.7.0 or higher is required!")
endif ()

find_package (OpenGL REQUIRED)

include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2_TTF REQUIRED SDL2_ttf)
pkg_search_module(SDL2_IMAGE REQUIRED SDL2_image)
pkg_search_module(GLEW REQUIRED glew)
pkg_search_module(MATH REQUIRED math)

set (RUBIK_DATADIR ${CMAKE_INSTALL_PREFIX}/share/rubik
     CACHE PATH "Data directory")
set (RUBIK_RESOURCES fonts meshes shaders textures)
set (RUBIK_INCLUDE src src/common src/game src/opengl src/utils)

file (GLOB_RECURSE RUBIK_SOURCES src/*.cpp)
include_directories (${RUBIK_INCLUDE} ${MATH_INCLUDE_DIRS}
                     ${PROJECT_BINARY_DIR})

set (RUBIK_NAME Rubik)
set (RUBIK_DESCRIPTION "Rubik's Cube game")
set (RUBIK_VERSION 0.1.9)

configure_file (Config.h.in Config.h @ONLY)

set (RUBIK_EXECUTABLE rubik)
add_executable (${RUBIK_EXECUTABLE} ${RUBIK_SOURCES})

target_link_libraries (${RUBIK_EXECUTABLE} ${OPENGL_LIBRARIES})
target_link_libraries (${RUBIK_EXECUTABLE} ${SDL2_LIBRARIES})
target_link_libraries (${RUBIK_EXECUTABLE} ${SDL2_TTF_LIBRARIES})
target_link_libraries (${RUBIK_EXECUTABLE} ${SDL2_IMAGE_LIBRARIES})
target_link_libraries (${RUBIK_EXECUTABLE} ${GLEW_LIBRARIES})
target_link_libraries (${RUBIK_EXECUTABLE} ${MATH_LIBRARIES})

install (TARGETS ${RUBIK_EXECUTABLE} DESTINATION bin)
install (DIRECTORY ${RUBIK_RESOURCES} DESTINATION ${RUBIK_DATADIR})
