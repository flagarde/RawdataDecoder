include(ExternalData)

add_executable(Buffer.test Buffer.test.cc)
target_include_directories(Buffer.test PUBLIC include)
target_link_libraries(Buffer.test PUBLIC RawdataDecoder::RawdataDecoder doctest_with_main)
install(TARGETS Buffer.test RUNTIME DESTINATION bin/tests LIBRARY DESTINATION lib ARCHIVE DESTINATION lib PUBLIC_HEADER DESTINATION include)
add_test(NAME Buffer.test COMMAND Buffer.test)
