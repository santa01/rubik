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

cmake_minimum_required (VERSION 3.12)
project (RUBIK)

find_path (GRAPHENE_INCLUDE_DIRS NAMES GrapheneApi.h HINTS ENV GRAPHENE_DIR PATH_SUFFIXES include)
find_library (GRAPHENE_LIBRARIES NAMES graphene graphene-shared HINTS ENV GRAPHENE_DIR PATH_SUFFIXES lib)
if (NOT GRAPHENE_INCLUDE_DIRS OR NOT GRAPHENE_LIBRARIES)
    message (FATAL_ERROR "Could NOT find Graphene")
endif ()

find_path (MATH_INCLUDE_DIRS NAMES MathApi.h HINTS ENV MATH_DIR PATH_SUFFIXES include)
find_library (MATH_LIBRARIES NAMES math math-shared HINTS ENV MATH_DIR PATH_SUFFIXES lib)
if (NOT MATH_INCLUDE_DIRS OR NOT MATH_LIBRARIES)
    message (FATAL_ERROR "Could NOT find Math")
endif ()

find_path (SIGNALS_INCLUDE_DIRS NAMES Signals.h HINTS ENV SIGNALS_DIR PATH_SUFFIXES include)
if (NOT SIGNALS_INCLUDE_DIRS)
    message (FATAL_ERROR "Could NOT find Signals")
endif ()

set (RUBIK_NAME Rubik)
set (RUBIK_DESCRIPTION "Rubik's Cube game")
set (RUBIK_VERSION 0.2.1)

set (RUBIK_DATADIR ${CMAKE_INSTALL_PREFIX}/$<IF:$<BOOL:UNIX>,share/rubik,data> CACHE PATH "Data directory")

set (RUBIK_EXECUTABLE rubik)
set (RUBIK_RESOURCE_DIRS assets fonts shaders textures)

file (GLOB_RECURSE RUBIK_SOURCES src/*.cpp)
include_directories (src ${PROJECT_BINARY_DIR} ${GRAPHENE_INCLUDE_DIRS} ${MATH_INCLUDE_DIRS} ${SIGNALS_INCLUDE_DIRS})

add_executable (${RUBIK_EXECUTABLE} ${RUBIK_SOURCES})
set_target_properties (${RUBIK_EXECUTABLE} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
target_compile_options (${RUBIK_EXECUTABLE} PUBLIC
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Werror -pedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/WX>
)

set (RUBIK_LINK_LIBRARIES ${GRAPHENE_LIBRARIES} ${MATH_LIBRARIES})
target_link_libraries (${RUBIK_EXECUTABLE} ${RUBIK_LINK_LIBRARIES})

configure_file (Config.h.in Config.h @ONLY)

install (TARGETS ${RUBIK_EXECUTABLE} DESTINATION bin)
install (DIRECTORY ${RUBIK_RESOURCE_DIRS} DESTINATION ${RUBIK_DATADIR})
