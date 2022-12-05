include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

include(Fmt)
include(Spdlog)

add_library(rawdataDecoder src/Bits.cc)
target_include_directories(rawdataDecoder PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/RawdataDecoder>)
target_link_libraries(rawdataDecoder PUBLIC spdlog::spdlog fmt::fmt)
set_target_properties(rawdataDecoder PROPERTIES PUBLIC_HEADER
                      "include/Bits.h")
add_library(rawdataDecoder::rawdataDecoder ALIAS rawdataDecoder)

install(TARGETS rawdataDecoder spdlog EXPORT rawdataDecoderTargets INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/RawdataDecoder/" PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/RawdataDecoder/")

configure_package_config_file("${PROJECT_SOURCE_DIR}/cmake/RawdataDecoderConfig.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/RawdataDecoderConfig.cmake" INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/RawdataDecoder")
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/RawdataDecoderVersion.cmake" COMPATIBILITY SameMajorVersion)

install(EXPORT RawdataDecoderTargets FILE RawdataDecoderTargets.cmake NAMESPACE RawdataDecoder:: DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/RawdataDecoder")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/RawdataDecoderConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/RawdataDecoderVersion.cmake" DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/RawdataDecoder")
export(EXPORT RawdataDecoderTargets FILE "${PROJECT_BINARY_DIR}/cmake/RawdataDecoderConfig.cmake" NAMESPACE RawdataDecoder::)
