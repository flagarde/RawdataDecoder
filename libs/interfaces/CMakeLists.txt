add_library(Interfaces INTERFACE)
add_library(RawdataDecoder::Interfaces ALIAS Interfaces)

add_subdirectory(RawdataReader)

add_subdirectory(LCIOWriter)

add_subdirectory(ROOTWriter)
