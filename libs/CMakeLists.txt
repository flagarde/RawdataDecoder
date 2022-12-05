include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_subdirectory(core)
add_subdirectory(interfaces)

configure_package_config_file("${PROJECT_SOURCE_DIR}/cmake/RawdataDecoderConfig.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/RawdataDecoderConfig.cmake" INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/RawdataDecoder")
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/RawdataDecoderVersion.cmake" COMPATIBILITY SameMajorVersion)

install(EXPORT RawdataDecoderTargets FILE RawdataDecoderTargets.cmake NAMESPACE RawdataDecoder:: DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/RawdataDecoder")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/RawdataDecoderConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/RawdataDecoderVersion.cmake" DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/RawdataDecoder")
export(EXPORT RawdataDecoderTargets FILE "${PROJECT_BINARY_DIR}/cmake/RawdataDecoderConfig.cmake" NAMESPACE RawdataDecoder::)
