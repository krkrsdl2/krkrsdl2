cmake_minimum_required(VERSION 3.10)

project(krkrsdl2)

if((NOT ANDROID) AND (NOT IOS) AND (NOT VITA) AND (NOT NINTENDO_SWITCH) AND (NOT (APPLE AND NOT IOS)) AND (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")))
message(WARN "The CMake file is provided for platforms that do not support Meson well. Please try the Meson build system first.")
endif()

option(OPTION_ENABLE_CASEINSENSITIVITY "Enable case insensitivity. May malfunction if directories in the path are not listable." ON)
option(BUILTIN_PLUGIN_ENABLE_KAGPARSER "Enable the KAGParser plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_EXTRANS "Enable the extrans plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_WUTCWF "Enable the wutcwf plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_WUVORBIS "Enable the wuvorbis plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_FSTAT "Enable the fstat plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_JSON "Enable the json plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_VARFILE "Enable the varfile plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_KRGLHWEBP "Enable the krglhwebp plugin." ON)

if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

# Don't code sign by default.
set(KRKRSDL2_IOS_CODE_SIGN_IDENTITY "" CACHE STRING "The code sign identity to use when building the IPA.")

if(KRKRSDL2_IOS_CODE_SIGN_IDENTITY STREQUAL "")
set(KRKRSDL2_IOS_CODE_SIGNING_ENABLED NO)
else()
set(KRKRSDL2_IOS_CODE_SIGNING_ENABLED YES)
endif()

# iOS does not support case insensitivity properly.
if(IOS)
set(OPTION_ENABLE_CASEINSENSITIVITY OFF CACHE BOOL "" FORCE)
endif()

if(IOS AND (NOT DEFINED CMAKE_OSX_DEPLOYMENT_TARGET))
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.0)
endif()

if(IOS)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/library)
set(CMAKE_XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2")
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
file(STRINGS src/config/src_list/kirikirisdl2/includes.txt KIRIKIRISDL2_INCLUDES)

list(APPEND KRKRSDL2_SRC ${KIRIKIRISDL2_SRC})
list(APPEND KRKRSDL2_INCLUDES ${KIRIKIRISDL2_INCLUDES})

if(${BUILTIN_PLUGIN_ENABLE_KAGPARSER})
file(STRINGS src/config/src_list/kagparser/sources.txt KAGPARSER_SRC)
list(APPEND KRKRSDL2_SRC ${KAGPARSER_SRC})
endif()
if(${BUILTIN_PLUGIN_ENABLE_EXTRANS})
file(STRINGS src/config/src_list/extrans/sources.txt EXTRANS_SRC)
list(APPEND KRKRSDL2_SRC ${EXTRANS_SRC})
endif()
if(${BUILTIN_PLUGIN_ENABLE_WUTCWF})
file(STRINGS src/config/src_list/wutcwf/sources.txt WUTCWF_SRC)
list(APPEND KRKRSDL2_SRC ${WUTCWF_SRC})
endif()
if(${BUILTIN_PLUGIN_ENABLE_WUVORBIS})
file(STRINGS src/config/src_list/wuvorbis/sources.txt WUVORBIS_SRC)
file(STRINGS src/config/src_list/wuvorbis/includes.txt WUVORBIS_INCLUDES)
list(APPEND KRKRSDL2_SRC ${WUVORBIS_SRC})
list(APPEND KRKRSDL2_INCLUDES ${WUVORBIS_INCLUDES})
endif()
if(${BUILTIN_PLUGIN_ENABLE_FSTAT})
file(STRINGS src/config/src_list/fstat/sources.txt FSTAT_SRC)
list(APPEND KRKRSDL2_SRC ${FSTAT_SRC})
endif()
if(${BUILTIN_PLUGIN_ENABLE_JSON})
file(STRINGS src/config/src_list/json/sources.txt JSON_SRC)
list(APPEND KRKRSDL2_SRC ${JSON_SRC})
endif()
if(${BUILTIN_PLUGIN_ENABLE_VARFILE})
file(STRINGS src/config/src_list/varfile/sources.txt VARFILE_SRC)
list(APPEND KRKRSDL2_SRC ${VARFILE_SRC})
endif()
if(${BUILTIN_PLUGIN_ENABLE_KRGLHWEBP})
file(STRINGS src/config/src_list/krglhwebp/sources.txt KRGLHWEBP_SRC)
file(STRINGS src/config/src_list/krglhwebp/includes.txt KRGLHWEBP_INCLUDES)
list(APPEND KRKRSDL2_SRC ${KRGLHWEBP_SRC})
list(APPEND KRKRSDL2_INCLUDES ${KRGLHWEBP_INCLUDES})
endif()
if(VITA)
file(STRINGS src/config/src_list/varfile/sources.txt VARFILE_SRC)
list(APPEND KRKRSDL2_SRC src/support/roundevenf.c)
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

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
list(APPEND KRKRSDL2_LDFLAGS
	-pthread
)
list(APPEND KRKRSDL2_CFLAGS
	-pthread
)
endif()

if(WIN32)
list(APPEND KRKRSDL2_CFLAGS
	-DUNICODE
	-D_UNICODE
)
list(APPEND KRKRSDL2_LDFLAGS
	-municode
)
endif()

list(APPEND KRKRSDL2_CXXFLAGS ${KRKRSDL2_CFLAGS})

list(APPEND KRKRSDL2_CXXFLAGS
	-Wno-non-virtual-dtor
)

if(OPTION_ENABLE_CASEINSENSITIVITY)
list(APPEND KRKRSDL2_CXXFLAGS
	-DKRKRZ_CASE_INSENSITIVE
)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
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
elseif((APPLE AND NOT IOS) OR (${CMAKE_SYSTEM_NAME} STREQUAL "Linux"))
set(SDL_SHARED_ENABLED_BY_DEFAULT OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/SDL)
find_library(SDL2 NAMES SDL2-static)
list(APPEND KRKRSDL2_LIBS
	SDL2-static
)
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
add_subdirectory(external/SDL)
find_library(SDL2 NAMES SDL2-static)
list(APPEND KRKRSDL2_LIBS
	SDL2-static
)
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
list(APPEND KRKRSDL2_CXXFLAGS
	-DKRKRZ_USE_SDL_THREADS
)
elseif(NINTENDO_SWITCH)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)

list(APPEND KRKRSDL2_LIBS
	PkgConfig::SDL2
)
list(APPEND KRKRSDL2_CXXFLAGS
	-DKRKRZ_USE_SDL_THREADS
)
else()
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

list(APPEND KRKRSDL2_LIBS
	${SDL2_LIBRARIES}
)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(ANDROID)
set(KRKRSDL2_NAME main)
add_library(${KRKRSDL2_NAME} SHARED)
else()
set(KRKRSDL2_NAME krkrsdl2)
add_executable(${KRKRSDL2_NAME})
endif()
target_sources(${KRKRSDL2_NAME} PRIVATE ${KRKRSDL2_SRC})
target_compile_options(${KRKRSDL2_NAME} PUBLIC $<$<COMPILE_LANGUAGE:C>:${KRKRSDL2_CFLAGS}>)
target_compile_options(${KRKRSDL2_NAME} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${KRKRSDL2_CXXFLAGS}>)
target_include_directories(${KRKRSDL2_NAME} PUBLIC ${KRKRSDL2_INCLUDES})
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.13.0")
target_link_options(${KRKRSDL2_NAME} PUBLIC ${KRKRSDL2_LDFLAGS})
endif()
target_link_libraries(${KRKRSDL2_NAME} ${KRKRSDL2_LIBS})
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.16.0") 
target_precompile_headers(${KRKRSDL2_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/core/sdl2/SDLPrecompiledHeader.h)
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

if(VITA)
vita_create_self(${KRKRSDL2_NAME}.self ${KRKRSDL2_NAME})

set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -s CATEGORY=gdb")

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
add_custom_target(${KRKRSDL2_NAME}.nro ALL
    COMMAND $ENV{DEVKITPRO}/tools/bin/elf2nro
        ${KRKRSDL2_NAME}
        ${KRKRSDL2_NAME}.nro
        --icon=${SWITCH_RESOURCES_ROOT}/icon.jpg
        --nacp=${KRKRSDL2_NAME}.nacp
    DEPENDS ${KRKRSDL2_NAME}.nacp ${KRKRSDL2_NAME} ${SWITCH_RESOURCES_ROOT}/icon.jpg
)
endif()

