#set(CMAKE_CXX_STANDARD 14)
###############################################
# Project hbuana
###############################################

#cmake minimum requirement
cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 17)

#Project name and version
project(hbuana VERSION 1.0.0 LANGUAGES C CXX)

#CMake type
set(CMAKE_BUILD_TYPE Debug)

#External packages
find_package(ROOT COMPONENTS Matrix Hist RIO MathCore Physics)
find_package(yaml-cpp REQUIRED)

#set run time output directory as bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

#Add include directories
include_directories(src)
include_directories(include)
include_directories(${YAML_CPP_INCLUDE_DIR})
message("hey ${YAML_CPP_INCLUDE_DIR}")
include(${ROOT_USE_FILE})

#Add static librarys

#add executable
add_executable(hbuana src/main.cxx src/DatManager.cxx src/PedestalManager.cxx src/DacManager.cxx src/config.cxx)
#link libraries
target_link_libraries(hbuana ${ROOT_LIBRARIES} yaml-cpp)

#Add scripts to make setup.sh to include hbuana into environment
execute_process(COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/config/setup.sh ${PROJECT_BINARY_DIR})
execute_process(COMMAND sed -i "s:PROJECTHERE:${CMAKE_CURRENT_SOURCE_DIR}:g" ${PROJECT_BINARY_DIR}/setup.sh)
execute_process(COMMAND chmod +x ${PROJECT_BINARY_DIR}/setup.sh)
