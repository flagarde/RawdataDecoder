cmake_minimum_required(VERSION 3.15)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Download CMakeCM and include it as soon as possible
include(GetCMakeMM)
cmmm(VERSION "2.3" REPOSITORY "flagarde/CMakeMM" VERBOSITY VERBOSE)

cmmm_modules_list(PROVIDER github REPOSITORY "SDHCAL/CMakeModules" BRANCH main DESTINATION "Modules")

include(Missives)

project(streamout VERSION "0.1.0" DESCRIPTION "Rawdata to LCIO for HCAL-ECAL" HOMEPAGE_URL "https://github.com/flagarde/RawdataDecoder" LANGUAGES CXX)
option(BUILD_TESTS "Build the test" ON)
option(BUILD_APPS "Build the apps" ON)
option(BUILD_DOCS "Build the docs" OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_MACOSX_RPATH TRUE)
#set(CMAKE_SKIP_BUILD_RPATH FALSE)
#set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}" "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
#link_directories("${CMAKE_INSTALL_PREFIX}/lib")

add_subdirectory(libs)

if(BUILD_APPS)
  add_subdirectory(apps)
endif()

if(BUILD_DOCS)
  add_subdirectory(docs)
endif()

if(BUILD_TESTS)
  include(CTest)
  include(Doctest)
  add_subdirectory(tests)
endif()
