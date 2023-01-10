list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
if(NOT $ENV{ROOTSYS} STREQUAL "")
  set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}" "$ENV{ROOTSYS}")
  set(ROOT_DIR "$ENV{ROOTSYS}")
endif()

find_package(ROOT COMPONENTS Core Tree)
include("${ROOT_USE_FILE}")
string(REPLACE "-L " "-L" ROOT_EXE_LINKER_FLAGS "${ROOT_EXE_LINKER_FLAGS}")
# cmake-lint: disable=E1120
separate_arguments(ROOT_EXE_LINKER_FLAGS)
# pass
#include_directories(${ROOT_INCLUDE_DIRS})

if(NOT ROOT_FOUND)
  missive(ERROR "ROOT not found ! skipping ROOTWriter !")
  return()
endif()
get_property(ROOT_CXX_STANDARD TARGET ROOT::Core PROPERTY INTERFACE_COMPILE_FEATURES)

add_library(LegacyRawdata src/Decode.cxx)
target_include_directories(LegacyRawdata PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/ROOTWriter>)
set_target_properties(LegacyRawdata PROPERTIES PUBLIC_HEADER "include/RawtoRoot.h")
target_link_libraries(LegacyRawdata ROOT::Core ROOT::Tree Interfaces::Utilities)
target_compile_features(LegacyRawdata PUBLIC ${ROOT_CXX_STANDARD})
#install(TARGETS LegacyRawdata EXPORT LegacyRawdataTargets INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/RawdataDecoder/interfaces/ROOTWriter" PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/RawdataDecoder/interfaces/ROOTWriter")
#install(EXPORT LegacyRawdata FILE LegacyRawdataTargets.cmake NAMESPACE RawdataDecoder:: DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/RawdataDecoder")
add_library(RawdataDecoder::LegacyRawdata ALIAS LegacyRawdata)
