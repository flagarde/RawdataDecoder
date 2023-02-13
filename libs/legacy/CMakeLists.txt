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

if(NOT ROOT_FOUND)
  missive(ERROR "ROOT not found ! skipping ECAL and AHCAL legacy codes !")
  return()
endif()
get_property(ROOT_CXX_STANDARD TARGET ROOT::Core PROPERTY INTERFACE_COMPILE_FEATURES)

add_subdirectory(ECAL)
add_subdirectory(AHCAL)
