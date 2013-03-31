cmake_minimum_required (VERSION 2.6)
project (RUBIK)

if (CMAKE_COMPILER_IS_GNUCC)
    execute_process (COMMAND ${CMAKE_CXX_COMPILER} -dumpversion
                     OUTPUT_VARIABLE GCC_VERSION)
    string (STRIP ${GCC_VERSION} GCC_VERSION)

    if (GCC_VERSION VERSION_GREATER 4.7.0)
        set (CMAKE_CXX_FLAGS "-Wall -pedantic -std=c++11 -g")
    else ()
        message(FATAL_ERROR "GCC 4.7.0 or higher is required!")
    endif ()
endif ()

# Override include directiries for SDL2
set (SDL_INCLUDE_DIR "/usr/include/SDL2")
set (SDL_TTF_INCLUDE_DIRS "/usr/include/SDL2")
set (SDL_IMAGE_INCLUDE_DIRS "/usr/include/SDL2")

find_package (OpenGL REQUIRED)
find_package (SDL REQUIRED)
find_package (SDL_ttf REQUIRED)
find_package (SDL_image REQUIRED)

file (GLOB_RECURSE RUBIK_SOURCES "src/*.cpp")
include_directories ("src" "src/common" "src/game" "src/math" "src/opengl" "src/utils")

set (RUBIK_EXECUTABLE "rubik")

# Override libraries for SDL2
set (SDL_LIBRARY_FIXED "/usr/lib64/libSDL2.so")
set (SDL_TTF_LIBRARIES_FIXED "/usr/lib64/libSDL2_ttf.so")
set (SDL_IMAGE_LIBRARIES_FIXED "/usr/lib64/libSDL2_image.so")

add_executable (${RUBIK_EXECUTABLE} ${RUBIK_SOURCES})

target_link_libraries (${RUBIK_EXECUTABLE} ${OPENGL_LIBRARIES})
target_link_libraries (${RUBIK_EXECUTABLE} ${SDL_LIBRARY_FIXED})
target_link_libraries (${RUBIK_EXECUTABLE} ${SDL_TTF_LIBRARIES_FIXED})
target_link_libraries (${RUBIK_EXECUTABLE} ${SDL_IMAGE_LIBRARIES_FIXED})
target_link_libraries (${RUBIK_EXECUTABLE} "fontconfig")
