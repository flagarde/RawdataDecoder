missive(WARN "Build with LCIO interface")

set(LCIO_BUILD_LCIO_STANDARD_EXAMPLES TRUE)
include(Lcio)

add_library(LCIOWriter src/LCIOWriter.cc)
target_include_directories(LCIOWriter PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/RawdataDecoder>)
target_link_libraries(LCIOWriter PUBLIC RawdataDecoder::RawdataDecoder LCIO::LCIO Interfaces::Utilities)
install(TARGETS LCIOWriter EXPORT LCIOWriterTargets INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/RawdataDecoder/interfaces/LCIOWriter" PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/RawdataDecoder/interfaces/LCIOWriter")
install(EXPORT LCIOWriterTargets FILE LCIOWriterTargets.cmake NAMESPACE RawdataDecoder:: DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/RawdataDecoder")
add_library(RawdataDecoder::Interface::LCIOWriter ALIAS LCIOWriter)
set_target_properties(LCIOWriter PROPERTIES PUBLIC_HEADER "include/LCIOWriter.h")
target_link_libraries(Interfaces INTERFACE RawdataDecoder::Interface::LCIOWriter)
