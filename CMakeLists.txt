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
# raylib is pinned on purpose: tracking `master` means an upstream breaking change
# can break the build with no local commit involved. A release tarball plus
# URL_HASH pins the exact bytes, which is stricter than pinning a tag (a tag can
# be moved). To move to another release, change RAYLIB_VERSION *and* its
# RAYLIB_SHA256:  cmake -B build -DRAYLIB_VERSION=5.6 -DRAYLIB_SHA256=<sha256>
# A tarball is used instead of GIT_REPOSITORY because `git clone` is not
# dependable everywhere: the pack transfer gets cut on some networks (the ref
# handshake still succeeds, so it looks fine until the clone dies), which used
# to hang `build.sh clean` for half an hour. A plain HTTPS download survives
# that, is much faster, and drops the git dependency entirely.
set(RAYLIB_VERSION "5.5" CACHE STRING "raylib version used by FetchContent")
set(RAYLIB_SHA256 "aea98ecf5bc5c5e0b789a76de0083a21a70457050ea4cc2aec7566935f5e258e"
    CACHE STRING "sha256 of the raylib release tarball for RAYLIB_VERSION")

# .cache/ is gitignored and already holds the tarball on machines with a slow or
# flaky link: reusing it keeps every `clean` build from re-downloading ~42 MB.
# Same bytes either way, so URL_HASH still applies.
set(RAYLIB_TARBALL "${CMAKE_SOURCE_DIR}/.cache/raylib-${RAYLIB_VERSION}.tar.gz")
if(EXISTS "${RAYLIB_TARBALL}")
    set(RAYLIB_SOURCE "${RAYLIB_TARBALL}")
else()
    set(RAYLIB_SOURCE "https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz")
endif()

set(RAYLIB_DECLARE_ARGS URL ${RAYLIB_SOURCE} URL_HASH "SHA256=${RAYLIB_SHA256}")
# Silences CMP0135 (added in 3.24); older ExternalProject rejects the keyword.
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.24)
    list(APPEND RAYLIB_DECLARE_ARGS DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
endif()
FetchContent_Declare(raylib ${RAYLIB_DECLARE_ARGS})
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

# ---- Linux/NixOS: 让 GLFW 能 dlopen 到 X11 客户端库 ----------------------------
# GLFW 不用链接的方式依赖 X11，而是在 InitWindow() 里 dlopen 它们的 SONAME
# （libX11.so.6 / libXrandr.so.2 / ...）。链接期没有这些依赖，CMake 就不知道该把
# 它们的目录写进可执行文件的 run path。普通发行版有 ldconfig 兜底看不出来；NixOS
# 没有 ldconfig，第一次 InitWindow 就死：
#     WARNING: GLFW: Error: 65544 Description: X11: Failed to load Xlib
# 这里把「编译时用到的那批 X11 库」所在目录补进 run path。
# 注意 NixOS 的 -dev 输出只有头文件、lib 目录里一个 .so 都没有，所以必须经
# nix-support/propagated-build-inputs 解析到运行时的（非 -dev）输出才算数。
# 在有 ldconfig 的发行版上通常一个目录都加不上 => 纯 no-op，不会变坏。
if(UNIX AND NOT APPLE)
    set(_glfw_dlopen_libs
        libX11.so.6 libX11-xcb.so.1
        libXrandr.so.2 libXinerama.so.1 libXcursor.so.1 libXi.so.6
        libXext.so.6 libXrender.so.1 libXxf86vm.so.1 libxkbcommon.so.0)

    set(_lib_dirs "")
    set(_rpath "")
    # CMake feeds the environment's CMAKE_PREFIX_PATH to its own find_* internals
    # but does NOT expose it as a CMake variable, so reading the variable yields
    # nothing. Read $ENV{} first, fall back to the variable, and merge both.
    # The environment form is colon-separated on Unix, not a CMake list.
    set(_prefixes "$ENV{CMAKE_PREFIX_PATH};${CMAKE_PREFIX_PATH}")
    string(REPLACE ":" ";" _prefixes "${_prefixes}")
    list(FILTER _prefixes EXCLUDE REGEX "^$")
    list(REMOVE_DUPLICATES _prefixes)
    set(_prefix_count 0)
    foreach(_prefix IN LISTS _prefixes)
        math(EXPR _prefix_count "${_prefix_count} + 1")
        # multi-arch distros (Debian/Ubuntu) keep libraries in lib/<triplet>/
        file(GLOB _subdirs "${_prefix}/lib/*/")
        list(APPEND _lib_dirs "${_prefix}/lib" ${_subdirs})
        # NixOS: -dev output -> runtime output. Paths in that file are separated
        # by spaces (several per line). Do NOT use a POSIX character class here:
        # CMake's regex engine does not support [:space:] inside a negated
        # bracket and silently matches nothing.
        set(_propagated "${_prefix}/nix-support/propagated-build-inputs")
        if(EXISTS "${_propagated}")
            file(READ "${_propagated}" _propagated_text)
            string(REGEX MATCHALL "[^ \t\n]+" _propagated_deps "${_propagated_text}")
            foreach(_dep IN LISTS _propagated_deps)
                if(IS_DIRECTORY "${_dep}/lib")
                    list(APPEND _lib_dirs "${_dep}/lib")
                endif()
            endforeach()
        endif()
    endforeach()

    foreach(_soname IN LISTS _glfw_dlopen_libs)
        foreach(_dir IN LISTS _lib_dirs)
            if(EXISTS "${_dir}/${_soname}")
                list(APPEND _rpath "${_dir}")
                break()
            endif()
        endforeach()
    endforeach()
    if(_rpath)
        list(REMOVE_DUPLICATES _rpath)
        # Deliberately -Wl,-rpath link options instead of the BUILD_RPATH property:
        # with the nix toolchain the property never reaches the link line (verified
        # in link.txt), while an explicit flag is passed straight to the linker and
        # shows up in `tr ' ' '\n' < link.txt | grep -c -- -Wl,-rpath`.
        foreach(_dir IN LISTS _rpath)
            target_link_options(${PROJECT_NAME} PRIVATE "-Wl,-rpath,${_dir}")
        endforeach()
    endif()
    list(LENGTH _rpath _rpath_count)
    message(STATUS "GLFW dlopen: ${_prefix_count} prefix(es), ${_rpath_count} run path dir(s)")
    foreach(_dir IN LISTS _rpath)
        message(STATUS "GLFW dlopen run path: ${_dir}")
    endforeach()
endif()


target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# src/main.cpp pre-flights libX11 with dlopen() to turn the "GLFW: Failed to
# initialize GLFW" core dump into a readable error; CMAKE_DL_LIBS is empty on
# Windows and expands to -ldl where it is needed.
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_DL_LIBS})
