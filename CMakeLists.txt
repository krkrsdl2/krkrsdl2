cmake_minimum_required(VERSION 3.10)

project(krkrsdl2)

if((NOT ANDROID) AND (NOT IOS))
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

if(IOS AND (NOT DEFINED CMAKE_OSX_DEPLOYMENT_TARGET))
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.0)
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
if(${BUILTIN_PLUGIN_ENABLE_CSVPARSER})
file(STRINGS src/config/src_list/csvparser/sources.txt CSVPARSER_SRC)
list(APPEND KRKRSDL2_SRC ${CSVPARSER_SRC})
endif()
if(${BUILTIN_PLUGIN_ENABLE_FSTAT})
file(STRINGS src/config/src_list/fstat/sources.txt FSTAT_SRC)
list(APPEND KRKRSDL2_SRC ${FSTAT_SRC})
endif()
if(${BUILTIN_PLUGIN_ENABLE_GETSAMPLE})
file(STRINGS src/config/src_list/getsample/sources.txt GETSAMPLE_SRC)
list(APPEND KRKRSDL2_SRC ${GETSAMPLE_SRC})
endif()
if(${BUILTIN_PLUGIN_ENABLE_JSON})
file(STRINGS src/config/src_list/json/sources.txt JSON_SRC)
list(APPEND KRKRSDL2_SRC ${JSON_SRC})
endif()
if(${BUILTIN_PLUGIN_ENABLE_LAYEREXBTOA})
file(STRINGS src/config/src_list/layerexbtoa/sources.txt LAYEREXBTOA_SRC)
list(APPEND KRKRSDL2_SRC ${LAYEREXBTOA_SRC})
endif()
if(${BUILTIN_PLUGIN_ENABLE_LAYEREXIMAGE})
file(STRINGS src/config/src_list/layereximage/sources.txt LAYEREXIMAGE_SRC)
list(APPEND KRKRSDL2_SRC ${LAYEREXIMAGE_SRC})
endif()
if(${BUILTIN_PLUGIN_ENABLE_WUVORBIS})
file(STRINGS src/config/src_list/wuvorbis/sources.txt WUVORBIS_SRC)
file(STRINGS src/config/src_list/wuvorbis/includes.txt WUVORBIS_INCLUDES)
list(APPEND KRKRSDL2_SRC ${WUVORBIS_SRC})
list(APPEND KRKRSDL2_INCLUDES ${WUVORBIS_INCLUDES})
endif()
if(${BUILTIN_PLUGIN_ENABLE_KRATRAC9})
file(STRINGS src/config/src_list/kratrac9/sources.txt KRATRAC9_SRC)
list(APPEND KRKRSDL2_SRC ${KRATRAC9_SRC})
endif()
if(${BUILTIN_PLUGIN_ENABLE_KRGLHWEBP})
file(STRINGS src/config/src_list/krglhwebp/sources.txt KRGLHWEBP_SRC)
file(STRINGS src/config/src_list/krglhwebp/includes.txt KRGLHWEBP_INCLUDES)
list(APPEND KRKRSDL2_SRC ${KRGLHWEBP_SRC})
list(APPEND KRKRSDL2_INCLUDES ${KRGLHWEBP_INCLUDES})
endif()
if(${BUILTIN_PLUGIN_ENABLE_KRI18N})
file(STRINGS src/config/src_list/kri18n/sources.txt KRI18N_SRC)
list(APPEND KRKRSDL2_SRC ${KRI18N_SRC})
endif()
if(${BUILTIN_PLUGIN_ENABLE_KSUPPORT})
file(STRINGS src/config/src_list/ksupport/sources.txt KSUPPORT_SRC)
list(APPEND KRKRSDL2_SRC ${KSUPPORT_SRC})
endif()
if(${BUILTIN_PLUGIN_ENABLE_SAVESTRUCT})
file(STRINGS src/config/src_list/savestruct/sources.txt SAVESTRUCT_SRC)
list(APPEND KRKRSDL2_SRC ${SAVESTRUCT_SRC})
endif()
if(${BUILTIN_PLUGIN_ENABLE_VARFILE})
file(STRINGS src/config/src_list/varfile/sources.txt VARFILE_SRC)
list(APPEND KRKRSDL2_SRC ${VARFILE_SRC})
endif()
if(${BUILTIN_PLUGIN_ENABLE_SCRIPTSEX})
file(STRINGS src/config/src_list/scriptsex/sources.txt SCRIPTSEX_SRC)
list(APPEND KRKRSDL2_SRC ${SCRIPTSEX_SRC})
endif()
if(${BUILTIN_PLUGIN_ENABLE_BINARYSTREAM})
file(STRINGS src/config/src_list/binarystream/sources.txt BINARYSTREAM_SRC)
list(APPEND KRKRSDL2_SRC ${BINARYSTREAM_SRC})
endif()
if(${BUILTIN_PLUGIN_ENABLE_LAYEREXAREAAVERAGE})
file(STRINGS src/config/src_list/layerexareaaverage/sources.txt LAYEREXAREAAVERAGE_SRC)
list(APPEND KRKRSDL2_SRC ${LAYEREXAREAAVERAGE_SRC})
endif()
if(${BUILTIN_PLUGIN_ENABLE_LAYEREXLONGEXPOSURE})
file(STRINGS src/config/src_list/layerexlongexposure/sources.txt LAYEREXLONGEXPOSURE_SRC)
list(APPEND KRKRSDL2_SRC ${LAYEREXLONGEXPOSURE_SRC})
endif()
if(${BUILTIN_PLUGIN_ENABLE_LAYEREXRASTER})
file(STRINGS src/config/src_list/layerexraster/sources.txt LAYEREXRASTER_SRC)
list(APPEND KRKRSDL2_SRC ${LAYEREXRASTER_SRC})
endif()
if(${BUILTIN_PLUGIN_ENABLE_LINEPARSER})
file(STRINGS src/config/src_list/lineparser/sources.txt LINEPARSER_SRC)
list(APPEND KRKRSDL2_SRC ${LINEPARSER_SRC})
endif()
if(${BUILTIN_PLUGIN_ENABLE_SHRINKCOPY})
file(STRINGS src/config/src_list/shrinkcopy/sources.txt SHRINKCOPY_SRC)
list(APPEND KRKRSDL2_SRC ${SHRINKCOPY_SRC})
endif()
if(${BUILTIN_PLUGIN_ENABLE_FFTGRAPH})
file(STRINGS src/config/src_list/fftgraph/sources.txt FFTGRAPH_SRC)
list(APPEND KRKRSDL2_SRC ${FFTGRAPH_SRC})
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

if(ANDROID OR IOS)
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
	MACOSX_BUNDLE TRUE
	MACOSX_BUNDLE_GUI_IDENTIFIER pw.uyjulian.krkrsdl2
)
set_property(SOURCE data
	PROPERTY MACOSX_PACKAGE_LOCATION "Resources/"
)
target_sources(${KRKRSDL2_NAME} PRIVATE data)
endif()


