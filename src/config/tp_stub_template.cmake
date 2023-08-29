# SPDX-License-Identifier: MIT
# Copyright (c) Kirikiri SDL2 Developers

cmake_minimum_required(VERSION 3.12)
project(tp_stub_krkrsdl2)

add_library(tp_stub STATIC)
set_target_properties(tp_stub PROPERTIES CXX_STANDARD 11)
set_target_properties(tp_stub PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
target_sources(tp_stub PRIVATE tp_stub.cpp)
target_include_directories(tp_stub PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
if(${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
target_compile_options(tp_stub PRIVATE
	-sDISABLE_EXCEPTION_CATCHING=0
	-fPIC
)
endif()
