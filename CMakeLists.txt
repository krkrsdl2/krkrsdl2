# SPDX-License-Identifier: MIT
# Copyright (c) Kirikiri SDL2 Developers

cmake_minimum_required(VERSION 3.12)

if(IOS AND (NOT DEFINED CMAKE_OSX_DEPLOYMENT_TARGET))
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.0)
endif()

if((NOT IOS) AND (NOT DEFINED CMAKE_OSX_DEPLOYMENT_TARGET))
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.14)
endif()

project(krkrsdl2)

# Disable install for subprojects
if(NOT DEFINED _DISABLE_INSTALLATION)
set(_DISABLE_INSTALLATION FALSE)
macro(install)
if (NOT _DISABLE_INSTALLATION)
_install(${ARGN})
endif()
endmacro()
endif()

if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.13")
cmake_policy(SET CMP0077 NEW)
endif()
option(OPTION_ENABLE_ASYNC_IMAGE_LOAD "Enable async image load. Not applicable to Emscripten build." ON)
option(OPTION_ENABLE_CANVAS "Enable Canvas interface. Requires OpenGL ES or ANGLE at runtime." OFF)
option(OPTION_ENABLE_CASEINSENSITIVITY "Enable case insensitivity. May malfunction if directories in the path are not listable." ON)
option(OPTION_ENABLE_SDL_THREADS "Enable SDL threads. May be more useful on platforms that do not have a good implementation of std::thread." OFF)
option(OPTION_ENABLE_EXTERNAL_PLUGINS "Enable external plugins. This will make Plugins.link load a plugin instead of a no-op." ON)
option(OPTION_BUILD_MACOS_BUNDLE "Build an application bundle for macOS." ON)
option(KRKRSDL2_EMSCRIPTEN_ENABLE_JS_FALLBACK "Enable JavaScript fallback for the Emscripten build." OFF)
option(KRKRSDL2_EMSCRIPTEN_ENABLE_BIGINT "Enable BigInt support for the Emscripten build." ON)
option(KRKRSDL2_EMSCRIPTEN_ENABLE_WASM_SIMD "Enable WebAssembly SIMD for the Emscripten build." OFF)
option(KRKRSDL2_EMSCRIPTEN_ENABLE_THREADS "Enable threads for the Emscripten build." OFF)
option(KRKRSDL2_EMSCRIPTEN_ENABLE_SOURCE_MAPS "Enable threads for the Emscripten build." OFF)
set(KRKRSDL2_EMSCRIPTEN_INITIAL_MEMORY "134217728" CACHE STRING "Initial memory usage for the Emscripten build.")
option(KRKRSDL2_BUILD_ENABLE_WARNINGS "Enable warnings when building the project." OFF)

if((NOT DEFINED CMAKE_BUILD_TYPE) OR (NOT CMAKE_BUILD_TYPE))
set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

if(NOT DEFINED KRKRSDL2_REPO_DIR)
set(KRKRSDL2_REPO_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()

if((OPTION_ENABLE_ASYNC_IMAGE_LOAD) AND (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")))
set(EVAL_ENABLE_ASYNC_IMAGE_LOAD ON)
else()
set(EVAL_ENABLE_ASYNC_IMAGE_LOAD OFF)
endif()

# iOS does not support case insensitivity properly.
if((OPTION_ENABLE_CASEINSENSITIVITY) AND (NOT IOS))
set(EVAL_ENABLE_CASEINSENSITIVITY ON)
else()
set(EVAL_ENABLE_CASEINSENSITIVITY OFF)
endif()

if((OPTION_ENABLE_SDL_THREADS) OR (VITA OR NINTENDO_SWITCH))
set(EVAL_ENABLE_SDL_THREADS ON)
else()
set(EVAL_ENABLE_SDL_THREADS OFF)
endif()

if((OPTION_ENABLE_EXTERNAL_PLUGINS) AND (NOT ((${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten") OR VITA OR NINTENDO_SWITCH)))
set(EVAL_ENABLE_EXTERNAL_PLUGINS ON)
else()
set(EVAL_ENABLE_EXTERNAL_PLUGINS OFF)
endif()

if(NOT KRKRSDL2_ENABLE_PRECOMPILED_HEADERS)
if(WIN32)
set(KRKRSDL2_ENABLE_PRECOMPILED_HEADERS OFF)
else()
set(KRKRSDL2_ENABLE_PRECOMPILED_HEADERS ON)
endif()
endif()

# Don't code sign by default.
set(KRKRSDL2_IOS_CODE_SIGN_IDENTITY "" CACHE STRING "The code sign identity to use when building the IPA.")

if(KRKRSDL2_IOS_CODE_SIGN_IDENTITY STREQUAL "")
set(KRKRSDL2_IOS_CODE_SIGNING_ENABLED NO)
else()
set(KRKRSDL2_IOS_CODE_SIGNING_ENABLED YES)
endif()

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-fno-delete-null-pointer-checks KRKRSDL2_COMPILER_SUPPORTS_NO_DELETE_NULL_POINTER_CHECKS)

if(IOS)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/library)
set(CMAKE_XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2")
endif()

if(WIN32)
set(WIN32_RESOURCES_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src/win32-resources CACHE FILEPATH "The path to the resources for Win32.")
endif()

if(APPLE AND IOS)
set(IOS_RESOURCES_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src/ios-resources CACHE FILEPATH "The path to the resources for iOS.")
endif()

if(APPLE AND NOT IOS)
set(MACOS_RESOURCES_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src/macos-resources CACHE FILEPATH "The path to the resources for macOS.")
endif()

if(VITA)
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_APP_NAME "Kirikiri SDL2" CACHE STRING "The application name to use for the VPK.")
set(VITA_TITLEID  "KRKR00001"     CACHE STRING "The title ID to use for the VPK.")
set(VITA_VERSION  "02.00"         CACHE STRING "The version to use for the VPK.")
set(VITA_RESOURCES_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src/vita-resources CACHE FILEPATH "The path to the resources for Vita.")
endif()

if(NINTENDO_SWITCH)
set(SWITCH_APP_NAME "Kirikiri SDL2" CACHE STRING "The application name to use for the NRO.")
set(SWITCH_AUTHOR   "uyjulian"      CACHE STRING "The author to use for the NRO.")
set(SWITCH_VERSION  "v2.0.0"        CACHE STRING "The version to use for the NRO.")
set(SWITCH_RESOURCES_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src/nswitch-resources CACHE FILEPATH "The path to the resources for Switch.")
endif()

set(EMBED_DATA_PATH ${CMAKE_CURRENT_SOURCE_DIR}/data CACHE FILEPATH "The path to the data file or directory.")
get_filename_component(EMBED_DATA_PATH_BASENAME ${EMBED_DATA_PATH} NAME)

file(STRINGS src/config/src_list/kirikirisdl2/sources.txt KIRIKIRISDL2_SRC)
file(STRINGS src/config/src_list/kirikirisdl2/sources_sse2.txt KIRIKIRISDL2_SSE2_SRC)
file(STRINGS src/config/src_list/kirikirisdl2/sources_avx2.txt KIRIKIRISDL2_AVX2_SRC)
file(STRINGS src/config/src_list/kirikirisdl2/includes.txt KIRIKIRISDL2_INCLUDES)

list(APPEND KRKRSDL2_SRC ${KIRIKIRISDL2_SRC})
list(APPEND KRKRSDL2_SRC ${KIRIKIRISDL2_SSE2_SRC})
list(APPEND KRKRSDL2_SRC ${KIRIKIRISDL2_AVX2_SRC})
list(APPEND KRKRSDL2_INCLUDES ${KIRIKIRISDL2_INCLUDES})

if(VITA)
list(APPEND KRKRSDL2_SRC src/support/roundevenf.c)
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
list(APPEND KRKRSDL2_SRC src/plugins/kremscripten.cpp)
endif()

list(APPEND KRKRSDL2_CFLAGS
	-DTVP_COMPILING_KRKRSDL2
	-DTVP_OPUS_DECODER_IMPLEMENT
	-DTVP_LOG_TO_COMMANDLINE_CONSOLE
	-DHAVE_CONFIG_H
	-DFT2_BUILD_LIBRARY
	-DUSE_ALLOCA
	-DOPUS_BUILD
	-DHAVE_LRINTF
	-DHAVE_LRINT
	-DFLOAT_APPROX
	-DPNG_ARM_NEON_OPT=0
)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
list(APPEND KRKRSDL2_LDFLAGS
	-sDISABLE_EXCEPTION_CATCHING=0
	-sEXIT_RUNTIME=1
	-sALLOW_MEMORY_GROWTH=1
	-sINITIAL_MEMORY=${KRKRSDL2_EMSCRIPTEN_INITIAL_MEMORY}
	-sFORCE_FILESYSTEM=1
	-sASSERTIONS=0
	-sSTACK_OVERFLOW_CHECK=0
	-sMODULARIZE=1
	-sMAIN_MODULE=1
	-sEXPORT_NAME="instantiate_krkrsdl2_module"
	-sEXPORTED_RUNTIME_METHODS=['FS', 'IDBFS']
	-sUSE_SDL=2
	-lidbfs.js
	--bind
	--no-heap-copy
	-fPIC
)
list(APPEND KRKRSDL2_CFLAGS
	-sDISABLE_EXCEPTION_CATCHING=0
	-sUSE_SDL=2
	-fPIC
)
if(${KRKRSDL2_EMSCRIPTEN_ENABLE_WASM_SIMD} EQUAL ON)
list(APPEND KRKRSDL2_CFLAGS
	-msimd128
	-msse4.2
)
endif()
if(${KRKRSDL2_EMSCRIPTEN_ENABLE_THREADS} EQUAL ON)
list(APPEND KRKRSDL2_CFLAGS
	-sUSE_PTHREADS=1
	-sPROXY_TO_PTHREAD=1
	-sOFFSCREEN_FRAMEBUFFER=1
)
endif()
if(${KRKRSDL2_EMSCRIPTEN_ENABLE_JS_FALLBACK} EQUAL ON)
list(APPEND KRKRSDL2_LDFLAGS
	-sWASM=2
)
endif()
if(${KRKRSDL2_EMSCRIPTEN_ENABLE_BIGINT} EQUAL ON)
list(APPEND KRKRSDL2_LDFLAGS
	-sWASM_BIGINT=1
)
endif()
if(${KRKRSDL2_EMSCRIPTEN_ENABLE_THREADS} EQUAL ON)
list(APPEND KRKRSDL2_LDFLAGS
	-sUSE_PTHREADS=1
	-sPROXY_TO_PTHREAD=1
	-sOFFSCREEN_FRAMEBUFFER=1
)
endif()
if(${KRKRSDL2_EMSCRIPTEN_ENABLE_SOURCE_MAPS} EQUAL ON)
list(APPEND KRKRSDL2_LDFLAGS
	-g4
	--source-map-base http://127.0.0.1:8080/
)
endif()
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
list(APPEND KRKRSDL2_LDFLAGS
	-pthread
)
list(APPEND KRKRSDL2_CFLAGS
	-pthread
)
endif()

if(KRKRSDL2_BUILD_ENABLE_WARNINGS)
list(APPEND KRKRSDL2_CFLAGS
	-Wall
)
else()
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
list(APPEND KRKRSDL2_CFLAGS
	-Wno-everything
)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
list(APPEND KRKRSDL2_CFLAGS
	-w
)
endif()
endif()

if(WIN32)
list(APPEND KRKRSDL2_CFLAGS
	-DUNICODE
	-D_UNICODE
)
list(APPEND KRKRSDL2_LDFLAGS
	-static
	-static-libstdc++
	-static-libgcc
	-municode
	-Wl,--subsystem,windows
)
set_source_files_properties(${KIRIKIRISDL2_SSE2_SRC} PROPERTIES COMPILE_FLAGS -msse2)
set_source_files_properties(${KIRIKIRISDL2_AVX2_SRC} PROPERTIES COMPILE_FLAGS -mavx2)
list(APPEND KRKRSDL2_SRC
	src/core/sound/sdl2/tvpsnd.c
)
endif()

list(APPEND KRKRSDL2_CXXFLAGS ${KRKRSDL2_CFLAGS})

list(APPEND KRKRSDL2_CXXFLAGS
	-Wno-non-virtual-dtor
)

if(EVAL_ENABLE_ASYNC_IMAGE_LOAD)
list(APPEND KRKRSDL2_CXXFLAGS
	-DKRKRSDL2_ENABLE_ASYNC_IMAGE_LOAD
)
endif()

if(OPTION_ENABLE_CANVAS)
list(APPEND KRKRSDL2_CXXFLAGS
	-DKRKRZ_ENABLE_CANVAS
)
list(APPEND KRKRSDL2_SRC
	external/krkrz/visual/opengl/CanvasIntf.cpp
	external/krkrz/visual/opengl/DrawCycleTimer.cpp
	external/krkrz/visual/opengl/GLFrameBufferObject.cpp
	external/krkrz/visual/opengl/GLShaderUtil.cpp
	external/krkrz/visual/opengl/Matrix32Intf.cpp
	external/krkrz/visual/opengl/Matrix44Intf.cpp
	external/krkrz/visual/opengl/OffscreenIntf.cpp
	external/krkrz/visual/opengl/ShaderProgramIntf.cpp
	external/krkrz/visual/opengl/TextureIntf.cpp
	external/krkrz/visual/opengl/VertexBinderIntf.cpp
	external/krkrz/visual/opengl/VertexBufferIntf.cpp
	src/core/visual/sdl2/opengl/OpenGLScreenSDL2.cpp
)
list(APPEND KRKRSDL2_INCLUDES
	src/core/visual/sdl2/opengl
	external/krkrz/external/angle/include
	external/krkrz/external/glm
)
if(${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
list(APPEND KRKRSDL2_LDFLAGS
	-sMAX_WEBGL_VERSION=2
	-sFULL_ES3=1
)
endif()
endif()

if(EVAL_ENABLE_CASEINSENSITIVITY)
list(APPEND KRKRSDL2_CXXFLAGS
	-DKRKRZ_CASE_INSENSITIVE
)
endif()

if(EVAL_ENABLE_SDL_THREADS)
list(APPEND KRKRSDL2_CXXFLAGS
	-DKRKRZ_USE_SDL_THREADS
)
endif()

if(EVAL_ENABLE_EXTERNAL_PLUGINS)
list(APPEND KRKRSDL2_CXXFLAGS
	-DKRKRSDL2_ENABLE_PLUGINS
)
endif()

if(KRKRSDL2_COMPILER_SUPPORTS_NO_DELETE_NULL_POINTER_CHECKS)
list(APPEND KRKRSDL2_CXXFLAGS
	-fno-delete-null-pointer-checks
)
endif()

if(ANDROID)
list(APPEND KRKRSDL2_LIBS
	android
)
endif()

if(IOS)
set(SDL_SHARED_ENABLED_BY_DEFAULT OFF)
set(SDL_SHARED OFF)
add_subdirectory(external/SDL)
find_library(SDL2 NAMES SDL2-static SDL2main)
list(APPEND KRKRSDL2_LIBS
	SDL2-static
	SDL2main
)
list(APPEND KRKRSDL2_CXXFLAGS
	-DUSE_SDL_MAIN
)
get_target_property(SDL2_INCLUDE_DIRS SDL2-static INCLUDE_DIRECTORIES)
set(SDL2_LIBRARIES SDL2-static)
elseif((APPLE AND NOT IOS) OR (${CMAKE_SYSTEM_NAME} STREQUAL "Linux"))
set(SDL_SHARED_ENABLED_BY_DEFAULT OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/SDL)
find_library(SDL2 NAMES SDL2-static)
list(APPEND KRKRSDL2_LIBS
	SDL2-static
)
get_target_property(SDL2_INCLUDE_DIRS SDL2-static INCLUDE_DIRECTORIES)
set(SDL2_LIBRARIES SDL2-static)
elseif(ANDROID)
set(SDL_SHARED_ENABLED_BY_DEFAULT ON)
set(SDL_STATIC_ENABLED_BY_DEFAULT OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/SDL)
find_library(SDL2 NAMES SDL2 SDL2main)
list(APPEND KRKRSDL2_LIBS
	SDL2
	SDL2main
)
list(APPEND KRKRSDL2_CXXFLAGS
	-DUSE_SDL_MAIN
)
elseif(WIN32)
set(SDL_SHARED_ENABLED_BY_DEFAULT OFF)
set(SDL_SHARED OFF)
set(HAVE_WINDOWS_GAMING_INPUT_H NO)
add_subdirectory(external/SDL)
find_library(SDL2 NAMES SDL2-static)
list(APPEND KRKRSDL2_LIBS
	SDL2-static
)
get_target_property(SDL2_INCLUDE_DIRS SDL2-static INCLUDE_DIRECTORIES)
set(SDL2_LIBRARIES SDL2-static)
elseif(VITA)
set(SDL_SHARED_ENABLED_BY_DEFAULT OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/SDL)
find_library(SDL2 NAMES SDL2-static)

list(APPEND KRKRSDL2_LIBS
	stdc++
	pthread
	stdc++

	SDL2-static
	SceAppUtil_stub
	SceShellSvc_stub
	m
	z
)
get_target_property(SDL2_INCLUDE_DIRS SDL2-static INCLUDE_DIRECTORIES)
set(SDL2_LIBRARIES SDL2-static)
elseif(NINTENDO_SWITCH)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)

list(APPEND KRKRSDL2_LIBS
	PkgConfig::SDL2
)
else()
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

list(APPEND KRKRSDL2_LIBS
	${SDL2_LIBRARIES}
)
endif()

if(NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten"))
set(XNASONG OFF CACHE BOOL "Intentionally disabled for FAudio: enable XNA_Song.c" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Intentionally disabled for FAudio: Build shared libraries" FORCE)
set(_DISABLE_INSTALLATION_OLD ${_DISABLE_INSTALLATION})
set(_DISABLE_INSTALLATION TRUE)
add_subdirectory(external/FAudio)
set(_DISABLE_INSTALLATION ${_DISABLE_INSTALLATION_OLD})
list(APPEND KRKRSDL2_LIBS
	FAudio
)
list(APPEND KRKRSDL2_CXXFLAGS
	-DTVP_FAUDIO_IMPLEMENT
)
endif()

set(KRKRSDL2_NAME krkrsdl2)

if(ANDROID)
add_library(${KRKRSDL2_NAME} SHARED)
set_target_properties(${KRKRSDL2_NAME} PROPERTIES OUTPUT_NAME "main")
else()
add_executable(${KRKRSDL2_NAME})
if(WIN32)
if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
set_target_properties(${KRKRSDL2_NAME} PROPERTIES OUTPUT_NAME "tvpwin64")
else()
set_target_properties(${KRKRSDL2_NAME} PROPERTIES OUTPUT_NAME "tvpwin32")
endif()
else()
set_target_properties(${KRKRSDL2_NAME} PROPERTIES OUTPUT_NAME "krkrsdl2")
endif()
endif()
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
set_target_properties(${KRKRSDL2_NAME} PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
endif()
set_target_properties(${KRKRSDL2_NAME} PROPERTIES CXX_STANDARD 14)
target_sources(${KRKRSDL2_NAME} PRIVATE ${KRKRSDL2_SRC})
target_compile_options(${KRKRSDL2_NAME} PUBLIC $<$<COMPILE_LANGUAGE:C>:${KRKRSDL2_CFLAGS}>)
target_compile_options(${KRKRSDL2_NAME} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${KRKRSDL2_CXXFLAGS}>)
target_include_directories(${KRKRSDL2_NAME} PUBLIC ${KRKRSDL2_INCLUDES})
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.13.0")
target_link_options(${KRKRSDL2_NAME} PUBLIC ${KRKRSDL2_LDFLAGS})
endif()
target_link_libraries(${KRKRSDL2_NAME} ${KRKRSDL2_LIBS})
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.16.0") 
if(KRKRSDL2_ENABLE_PRECOMPILED_HEADERS)
target_precompile_headers(${KRKRSDL2_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/core/sdl2/SDLPrecompiledHeader.h)
endif()
endif()

if(WIN32)
enable_language("RC")
target_sources(${KRKRSDL2_NAME} PRIVATE
	${WIN32_RESOURCES_ROOT}/krkrsdl2.rc
)
endif()

if(IOS)
set_target_properties(${KRKRSDL2_NAME} PROPERTIES
	XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "${KRKRSDL2_IOS_CODE_SIGN_IDENTITY}"
	XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "${KRKRSDL2_IOS_CODE_SIGNING_ENABLED}"
	XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "${KRKRSDL2_IOS_CODE_SIGNING_ENABLED}"
	BUNDLE TRUE
	MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/src/config/iOSBundleInfo.plist.in
	MACOSX_BUNDLE TRUE
	MACOSX_BUNDLE_GUI_IDENTIFIER pw.uyjulian.krkrsdl2
	XCODE_ATTRIBUTE_ENABLE_BITCODE NO
	XCODE_ATTRIBUTE_SKIP_INSTALL NO
	XCODE_ATTRIBUTE_INSTALL_PATH "/Applications"
	XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon"
	XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_LAUNCHIMAGE_NAME "LaunchImage"
)
set_property(
	SOURCE ${EMBED_DATA_PATH} ${IOS_RESOURCES_ROOT}/Images.xcassets
	PROPERTY MACOSX_PACKAGE_LOCATION "Resources"
)
target_sources(${KRKRSDL2_NAME} PRIVATE 
	${EMBED_DATA_PATH}
	${IOS_RESOURCES_ROOT}/Images.xcassets
)
endif()

if(OPTION_BUILD_MACOS_BUNDLE)
if(APPLE AND NOT IOS)
set_target_properties(${KRKRSDL2_NAME} PROPERTIES
	BUNDLE TRUE
	MACOSX_BUNDLE TRUE
	MACOSX_BUNDLE_GUI_IDENTIFIER pw.uyjulian.krkrsdl2
	MACOSX_BUNDLE_ICON_FILE "krkrsdl2.icns"
	XCODE_ATTRIBUTE_SKIP_INSTALL NO
	XCODE_ATTRIBUTE_INSTALL_PATH "/Applications"
)
set_property(
	SOURCE ${EMBED_DATA_PATH} ${MACOS_RESOURCES_ROOT}/krkrsdl2.icns
	PROPERTY MACOSX_PACKAGE_LOCATION "Resources"
)
target_sources(${KRKRSDL2_NAME} PRIVATE 
	${EMBED_DATA_PATH}
	${MACOS_RESOURCES_ROOT}/krkrsdl2.icns
)
endif()
endif()

if(VITA)
vita_create_self(${KRKRSDL2_NAME}.self ${KRKRSDL2_NAME})

set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -s CATEGORY=gdb -d ATTRIBUTE2=12")

vita_create_vpk(${KRKRSDL2_NAME}.vpk ${VITA_TITLEID} ${KRKRSDL2_NAME}.self
    VERSION ${VITA_VERSION}
    NAME ${VITA_APP_NAME}
    FILE ${VITA_RESOURCES_ROOT}/sce_sys/icon0.png sce_sys/icon0.png
    FILE ${VITA_RESOURCES_ROOT}/sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
    FILE ${VITA_RESOURCES_ROOT}/sce_sys/livearea/contents/icon.png sce_sys/livearea/contents/icon.png
    FILE ${VITA_RESOURCES_ROOT}/sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
    FILE ${EMBED_DATA_PATH} ${EMBED_DATA_PATH_BASENAME}
)
endif()

if(NINTENDO_SWITCH)
add_custom_target(${KRKRSDL2_NAME}.nacp
    COMMAND $ENV{DEVKITPRO}/tools/bin/nacptool
        --create
        ${SWITCH_APP_NAME}
        ${SWITCH_AUTHOR}
        ${SWITCH_VERSION}
        ${KRKRSDL2_NAME}.nacp
)
add_custom_target(romfs ALL
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/romfs
	COMMAND ${CMAKE_COMMAND} -E create_symlink ${EMBED_DATA_PATH} ${CMAKE_CURRENT_BINARY_DIR}/romfs/${EMBED_DATA_PATH_BASENAME}
)
add_custom_target(${KRKRSDL2_NAME}.nro ALL
    COMMAND $ENV{DEVKITPRO}/tools/bin/elf2nro
        ${KRKRSDL2_NAME}
        ${KRKRSDL2_NAME}.nro
        --icon=${SWITCH_RESOURCES_ROOT}/icon.jpg
        --nacp=${KRKRSDL2_NAME}.nacp
        --romfsdir=${CMAKE_CURRENT_BINARY_DIR}/romfs
    DEPENDS ${KRKRSDL2_NAME}.nacp ${KRKRSDL2_NAME} romfs ${SWITCH_RESOURCES_ROOT}/icon.jpg
)
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
install(TARGETS ${KRKRSDL2_NAME} DESTINATION bin)
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
configure_file(
	src/config/index.html.in
	index.html
	@ONLY
	NEWLINE_STYLE UNIX
)
endif()

get_directory_property(HAS_PARENT_SCOPE PARENT_DIRECTORY)
if(HAS_PARENT_SCOPE)
# Share certain variables to projects that include this one
set(KRKRSDL2_NAME ${KRKRSDL2_NAME} PARENT_SCOPE)
set(KRKRSDL2_REPO_DIR ${KRKRSDL2_REPO_DIR} PARENT_SCOPE)
endif()
