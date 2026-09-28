# 把任意二进制文件转成一个 C++ 源文件里的字节数组，用于把资源"编译进"可执行文件。
#
# 用法（CMake script 模式）:
#   cmake -DINPUT=<输入文件>
#         -DOUTPUT_CPP=<输出的 .cpp>
#         -DOUTPUT_HPP=<输出的 .h>
#         -DSYMBOL=<C++ 符号名>
#         -P cmake/embed_binary.cmake
#
# 生成的 .h 声明:
#   extern const unsigned char <SYMBOL>[];
#   extern const unsigned long <SYMBOL>_SIZE;
#
# 为什么用 CMake 而不是 xxd / objcopy / .rc:
#   这个仓库同时要在 Windows(MinGW) 和 Linux(NixOS) 上构建，
#   xxd 和 objcopy 不保证两平台都有，.rc 是 Windows 专有。
#   CMake 反正是构建的必需依赖，所以用它最省事、最可移植。

foreach(_var IN ITEMS INPUT OUTPUT_CPP OUTPUT_HPP SYMBOL)
  if(NOT DEFINED ${_var})
    message(FATAL_ERROR "embed_binary.cmake: 缺少 -D${_var}=...")
  endif()
endforeach()

if(NOT EXISTS "${INPUT}")
  message(FATAL_ERROR "embed_binary.cmake: 找不到输入文件 ${INPUT}")
endif()

# HEX 模式：文件内容变成连续的十六进制字符串，每字节 2 个字符
file(READ "${INPUT}" _hex HEX)
string(LENGTH "${_hex}" _hex_len)
math(EXPR _byte_len "${_hex_len} / 2")

if(_byte_len EQUAL 0)
  message(FATAL_ERROR "embed_binary.cmake: 输入文件是空的 ${INPUT}")
endif()

# 每行 16 字节，既好读又不会让单行过长
set(_body "")
set(_line "")
set(_i 0)
while(_i LESS _byte_len)
  math(EXPR _off "${_i} * 2")
  string(SUBSTRING "${_hex}" ${_off} 2 _b)
  string(APPEND _line "0x${_b},")
  math(EXPR _i "${_i} + 1")
  math(EXPR _col "${_i} % 16")
  if(_col EQUAL 0 OR _i EQUAL _byte_len)
    string(APPEND _body "${_line}\n")
    set(_line "")
  endif()
endwhile()

get_filename_component(_hpp_name "${OUTPUT_HPP}" NAME)

file(WRITE "${OUTPUT_HPP}"
"// 本文件由 cmake/embed_binary.cmake 自动生成，请勿手工修改。
// 源文件: ${INPUT}
#pragma once

extern const unsigned char ${SYMBOL}[];
extern const unsigned long ${SYMBOL}_SIZE;
")

file(WRITE "${OUTPUT_CPP}"
"// 本文件由 cmake/embed_binary.cmake 自动生成，请勿手工修改。
// 源文件: ${INPUT}（${_byte_len} 字节）
#include \"${_hpp_name}\"

const unsigned char ${SYMBOL}[] = {
${_body}};

const unsigned long ${SYMBOL}_SIZE = ${_byte_len}UL;
")

message(STATUS "embed_binary: ${INPUT} -> ${_byte_len} 字节 -> ${SYMBOL}")
