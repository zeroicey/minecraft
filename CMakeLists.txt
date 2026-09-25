cmake_minimum_required(VERSION 3.15)
project(minecraft LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Optional toolchain hints for NixOS/X11 + OpenGL. They are cache variables because
# CMake's FindX11/FindOpenGL modules do not reliably import arbitrary env vars.
if(DEFINED ENV{X11_X11_INCLUDE_PATH})
  set(X11_X11_INCLUDE_PATH "$ENV{X11_X11_INCLUDE_PATH}" CACHE PATH "X11 include path")
endif()
if(DEFINED ENV{X11_X11_LIB})
  set(X11_X11_LIB "$ENV{X11_X11_LIB}" CACHE FILEPATH "X11 library")
endif()
foreach(_var IN ITEMS X11_Xkb_INCLUDE_PATH X11_Xrandr_INCLUDE_PATH X11_Xinerama_INCLUDE_PATH X11_Xcursor_INCLUDE_PATH X11_Xi_INCLUDE_PATH X11_Xshape_INCLUDE_PATH OPENGL_INCLUDE_DIR)
  if(DEFINED ENV{${_var}})
    set(${_var} "$ENV{${_var}}" CACHE PATH "Nix development path")
  endif()
endforeach()
foreach(_var IN ITEMS OPENGL_gl_LIBRARY OPENGL_opengl_LIBRARY)
  if(DEFINED ENV{${_var}})
    set(${_var} "$ENV{${_var}}" CACHE FILEPATH "OpenGL library")
  endif()
endforeach()

include_directories(${CMAKE_SOURCE_DIR}/include)

set(PROJECT_SOURCES
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/utils.cpp
    ${CMAKE_SOURCE_DIR}/src/ui.cpp
    ${CMAKE_SOURCE_DIR}/src/world.cpp
    ${CMAKE_SOURCE_DIR}/src/player.cpp
    ${CMAKE_SOURCE_DIR}/src/chunk.cpp
    ${CMAKE_SOURCE_DIR}/src/block.cpp
)

include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG master
)
FetchContent_MakeAvailable(raylib)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
