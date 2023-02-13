find_package(yaml-cpp REQUIRED)
include_directories(src)
include_directories(include)
include_directories(${YAML_CPP_INCLUDE_DIR})
#add executable
add_executable(hbuana src/main.cxx src/DatManager.cxx src/PedestalManager.cxx src/DacManager.cxx src/config.cxx)
#link libraries
target_link_libraries(hbuana ${ROOT_LIBRARIES} yaml-cpp)
install(TARGETS hbuana)
