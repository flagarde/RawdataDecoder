include(CLI11)
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
missive(WARN "Build with ROOT interface")
get_property(ROOT_CXX_STANDARD TARGET ROOT::Core PROPERTY INTERFACE_COMPILE_FEATURES)

add_library(Comparator src/Comparator.cc src/Ploter.cc)
target_include_directories(Comparator PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/compare>)
target_link_libraries(Comparator PRIVATE ROOT::ROOTDataFrame ROOT::Core ROOT::Tree)
install(TARGETS Comparator)

add_library(CECPStyle src/CEPCCDRstyle.cc)
target_include_directories(CECPStyle PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/compare>)
target_link_libraries(CECPStyle PRIVATE ROOT::Core)
install(TARGETS CECPStyle)

add_executable(Comparaison src/Comparaison.cc)
target_link_libraries(Comparaison PRIVATE CECPStyle Comparator CLI11::CLI11 RawdataDecoder::Filesystem)
install(TARGETS Comparaison)
