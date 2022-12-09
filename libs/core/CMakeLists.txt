include(Fmt)
include(Spdlog)

# Generate a version file for streamout
include(Semver)

semverfile(INPUT "${CMAKE_CURRENT_SOURCE_DIR}/include/AppVersion.h.in" OUTPUT "${CMAKE_BINARY_DIR}/generated/AppVersion.h")
add_library(AppVersion INTERFACE)
target_include_directories(AppVersion INTERFACE $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/generated/> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/RawdataDecoder>)
set_target_properties(AppVersion PROPERTIES PUBLIC_HEADER "${CMAKE_BINARY_DIR}/generated/AppVersion.h")

add_library(RawdataDecoder src/Bits.cc src/Version.cc src/Formatters.cc src/Filesystem.cc)
target_include_directories(RawdataDecoder PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/RawdataDecoder>)
target_link_libraries(RawdataDecoder PUBLIC spdlog::spdlog fmt::fmt semver::semver AppVersion)
set_target_properties(RawdataDecoder PROPERTIES PUBLIC_HEADER "include/Bits.h;include/Buffer.h;include/Version.h")
add_library(RawdataDecoder::RawdataDecoder ALIAS RawdataDecoder)

install(TARGETS RawdataDecoder spdlog AppVersion EXPORT RawdataDecoderTargets INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/RawdataDecoder/" PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/RawdataDecoder/")
