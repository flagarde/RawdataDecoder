cmake_minimum_required(VERSION 3.15)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Download CMakeCM and include it as soon as possible
include(GetCMakeMM)
cmmm(VERSION "2.3" REPOSITORY "flagarde/CMakeMM" VERBOSITY VERBOSE)

cmmm_modules_list(PROVIDER github REPOSITORY "SDHCAL/CMakeModules" BRANCH main DESTINATION "Modules")

project(streamout VERSION "0.1.0" DESCRIPTION "Rawdata to LCIO for HCAL-ECAL" HOMEPAGE_URL "https://github.com/flagarde/RawdataDecoder" LANGUAGES CXX)
