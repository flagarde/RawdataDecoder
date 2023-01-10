include(CLI11)

add_executable(RawdataToLCIO RawdataToLCIO.cc)
target_link_libraries(RawdataToLCIO PRIVATE RawdataDecoder::RawdataDecoder RawdataDecoder::Interfaces CLI11::CLI11)
install(TARGETS RawdataToLCIO)

add_executable(RawdataToROOT RawdataToROOT.cc)
target_link_libraries(RawdataToROOT PRIVATE RawdataDecoder::RawdataDecoder RawdataDecoder::Interfaces CLI11::CLI11)
install(TARGETS RawdataToROOT)

add_subdirectory(compare)
