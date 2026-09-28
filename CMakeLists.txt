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

# ---- 把资源编译进 exe --------------------------------------------------------
# assets/atlas.png 会被转成一个字节数组直接编进可执行文件，所以发布时
# 只需要发 minecraft.exe，不用附带 assets/ 目录。
# 生成物在 <build>/generated/ 下；atlas.png 或脚本一变就会自动重新生成。
set(ASSET_GEN_DIR "${CMAKE_BINARY_DIR}/generated")
set(ATLAS_PNG     "${CMAKE_SOURCE_DIR}/assets/atlas.png")
set(ATLAS_GEN_CPP "${ASSET_GEN_DIR}/embedded_atlas.cpp")
set(ATLAS_GEN_HPP "${ASSET_GEN_DIR}/embedded_atlas.h")

file(MAKE_DIRECTORY "${ASSET_GEN_DIR}")
include_directories("${ASSET_GEN_DIR}")

add_custom_command(
    OUTPUT  "${ATLAS_GEN_CPP}" "${ATLAS_GEN_HPP}"
    COMMAND "${CMAKE_COMMAND}"
            "-DINPUT=${ATLAS_PNG}"
            "-DOUTPUT_CPP=${ATLAS_GEN_CPP}"
            "-DOUTPUT_HPP=${ATLAS_GEN_HPP}"
            "-DSYMBOL=EMBEDDED_ATLAS_PNG"
            -P "${CMAKE_SOURCE_DIR}/cmake/embed_binary.cmake"
    DEPENDS "${ATLAS_PNG}" "${CMAKE_SOURCE_DIR}/cmake/embed_binary.cmake"
    COMMENT "Embedding assets/atlas.png into the executable"
    VERBATIM
)

set(PROJECT_SOURCES
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/utils.cpp
    ${CMAKE_SOURCE_DIR}/src/ui.cpp
    ${CMAKE_SOURCE_DIR}/src/texture_atlas.cpp
    ${CMAKE_SOURCE_DIR}/src/world.cpp
    ${CMAKE_SOURCE_DIR}/src/player.cpp
    ${CMAKE_SOURCE_DIR}/src/chunk.cpp
    ${CMAKE_SOURCE_DIR}/src/block.cpp
    # 生成的文件：列进 sources，CMake 才会把它和上面的 custom command 接起来
    "${ATLAS_GEN_CPP}"
)

include(FetchContent)
# raylib is pinned to a release tag on purpose: tracking `master` means an upstream
# breaking change can break the build with no local commit involved. Override to
# follow upstream with:  cmake -B build -DRAYLIB_GIT_TAG=master
set(RAYLIB_GIT_TAG "5.5" CACHE STRING "raylib git tag/branch/commit used by FetchContent")
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG ${RAYLIB_GIT_TAG}
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(raylib)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
# MinGW links the GCC runtime (libstdc++/libgcc/libwinpthread) dynamically by
# default, so the exe would refuse to start unless three DLLs sat next to it.
# Link it statically instead: build/ stays runnable as-is and the UCRT is still
# taken from Windows itself. -static-libgcc/-static-libstdc++ are not enough --
# libwinpthread-1.dll still shows up in `objdump -p minecraft.exe`.
if(MINGW)
    target_link_options(${PROJECT_NAME} PRIVATE -static)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
