cmake_minimum_required(VERSION 3.10)

project(krkrsdl2)

if((NOT ANDROID) AND (NOT IOS) AND (NOT VITA) AND (NOT NSWITCH))
message(WARN "The CMake file is provided for platforms that do not support Meson well. Please try the Meson build system first.")
endif()

option(BUILTIN_PLUGIN_ENABLE_KAGPARSER "Enable the KAGParser plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_EXTRANS "Enable the extrans plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_WUTCWF "Enable the wutcwf plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_CSVPARSER "Enable the csvparser plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_FSTAT "Enable the fstat plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_GETSAMPLE "Enable the getsample plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_JSON "Enable the json plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_LAYEREXBTOA "Enable the layerExBTOA plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_LAYEREXIMAGE "Enable the layerExImage plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_WUVORBIS "Enable the wuvorbis plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_KRATRAC9 "Enable the kratrac9 plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_KRATRAC9 "Enable the kratrac9 plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_KRI18N "Enable the kri18n plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_KRGLHWEBP "Enable the krglhwebp plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_KSUPPORT "Enable the ksupport plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_SAVESTRUCT "Enable the saveStruct plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_VARFILE "Enable the varfile plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_SCRIPTSEX "Enable the scriptsEx plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_BINARYSTREAM "Enable the binaryStream plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_LAYEREXAREAAVERAGE "Enable the layerExAreaAverage plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_LAYEREXLONGEXPOSURE "Enable the layerExLongExposure plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_LAYEREXRASTER "Enable the layerExRaster plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_LINEPARSER "Enable the lineParser plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_SHRINKCOPY "Enable the shrinkCopy plugin." ON)
option(BUILTIN_PLUGIN_ENABLE_FFTGRAPH "Enable the fftgraph plugin." ON)

if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

if(IOS AND (NOT DEFINED CMAKE_OSX_DEPLOYMENT_TARGET))
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.0)
endif()

if(IOS)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/library)
set(CMAKE_XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2")
endif()

if(VITA)
include("${DOLCESDK}/share/dolce.cmake" REQUIRED)

if(NOT VITA_APP_NAME)
set(VITA_APP_NAME "Kirikiri for Playstation Vita")
endif()
if(NOT VITA_TITLEID)
set(VITA_TITLEID  "KRKR00001")
endif()
if(NOT VITA_VERSION)
set(VITA_VERSION  "02.00")
endif()
endif()

if(NSWITCH)
if(NOT SWITCH_APP_NAME)
set(SWITCH_APP_NAME "Kirikiri for Nintendo Switch")
endif()
if(NOT SWITCH_AUTHOR)
set(SWITCH_AUTHOR   "uyjulian")
endif()
if(NOT SWITCH_VERSION)
set(SWITCH_VERSION  "v2.0.0")
endif()
endif()

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

list(APPEND KRKRSDL2_CFLAGS
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

list(APPEND KRKRSDL2_CXXFLAGS ${KRKRSDL2_CFLAGS})

list(APPEND KRKRSDL2_CXXFLAGS
	-Wno-non-virtual-dtor
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
list(APPEND KRKRSDL2_CXXFLAGS
	-fno-delete-null-pointer-checks
)
endif()

if(LINUX OR ANDROID)
list(APPEND KRKRSDL2_LDFLAGS
	-pthread
)
endif()

if(IOS)
set(SDL_SHARED_ENABLED_BY_DEFAULT OFF)
add_subdirectory(external/SDL)
find_library(SDL2 NAMES SDL2-static SDL2main)
list(APPEND KRKRSDL2_LIBS
	SDL2-static
	SDL2main
)
list(APPEND KRKRSDL2_CXXFLAGS
	-DUSE_SDL_MAIN
)
elseif(ANDROID)
set(SDL_SHARED_ENABLED_BY_DEFAULT ON)
set(SDL_STATIC_ENABLED_BY_DEFAULT OFF)
add_subdirectory(external/SDL)
find_library(SDL2 NAMES SDL2 SDL2main)
list(APPEND KRKRSDL2_LIBS
	SDL2
	SDL2main
)
list(APPEND KRKRSDL2_CXXFLAGS
	-DUSE_SDL_MAIN
)
elseif(VITA)
include_directories($ENV{DOLCESDK}/arm-dolce-eabi/include/SDL2)

list(APPEND KRKRSDL2_LIBS
	stdc++
	pthread
	stdc++

	SDL2
	pib
	SceAppMgr_stub
	SceAppMgrUser_stub
	SceAppUtil_stub
	SceShellSvc_stub
	SceDisplay_stub
	SceDisplayUser_stub
	SceCtrl_stub
	SceAudio_stub
	SceSysmodule_stub
	SceGxm_stub
	SceCommonDialog_stub
	SceTouch_stub
	SceHid_stub
	SceMotion_stub
	m
	z
)
list(APPEND KRKRSDL2_CXXFLAGS
	-DKRKRZ_USE_SDL_THREADS
)
elseif(NSWITCH)
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
target_link_libraries(${KRKRSDL2_NAME} ${KRKRSDL2_LIBS})

if(IOS)
set_target_properties(${KRKRSDL2_NAME} PROPERTIES
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
	SOURCE data src/ios-resources/Images.xcassets
	PROPERTY MACOSX_PACKAGE_LOCATION "Resources"
)
target_sources(${KRKRSDL2_NAME} PRIVATE 
	data src/ios-resources/Images.xcassets
)
endif()

if(VITA)
dolce_create_self(${KRKRSDL2_NAME}.self ${KRKRSDL2_NAME})

set(DOLCE_MKSFOEX_FLAGS "${DOLCE_MKSFOEX_FLAGS} -s CATEGORY=gdb")

dolce_create_vpk(${KRKRSDL2_NAME}.vpk ${VITA_TITLEID} ${KRKRSDL2_NAME}.self
    VERSION ${VITA_VERSION}
    NAME ${VITA_APP_NAME}
    FILE src/vita-resources/sce_sys/icon0.png sce_sys/icon0.png
    FILE src/vita-resources/sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
    FILE src/vita-resources/sce_sys/livearea/contents/icon.png sce_sys/livearea/contents/icon.png
    FILE src/vita-resources/sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
    FILE data data
)
endif()

if(NSWITCH)
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
        --icon=${CMAKE_SOURCE_DIR}/src/nswitch-resources/icon.jpg
        --nacp=${KRKRSDL2_NAME}.nacp
    DEPENDS ${KRKRSDL2_NAME}.nacp ${KRKRSDL2_NAME} ${CMAKE_SOURCE_DIR}/src/nswitch-resources/icon.jpg
)
endif()

