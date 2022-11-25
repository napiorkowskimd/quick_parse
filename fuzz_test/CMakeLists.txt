# A fuzz test runs until it finds an error. This particular one is going to rely on libFuzzer.
#

find_package(fmt)

# Allow short runs during automated testing to see if something new breaks
set(FUZZ_RUNTIME
    10
    CACHE STRING "Number of seconds to run fuzz tests during ctest run") # Default of 10 seconds

add_executable(parser_fuzz_test
  parser_fuzz_test.cpp
  "${CMAKE_SOURCE_DIR}/src/section_description.cpp")

target_include_directories(
  parser_fuzz_test
  PRIVATE
  "${CMAKE_SOURCE_DIR}/src/"
)
target_link_libraries(
  parser_fuzz_test
  PRIVATE project_options
          project_warnings
          fmt::fmt
          -coverage
          -fsanitize=fuzzer,undefined,address)
target_compile_options(parser_fuzz_test PRIVATE -fsanitize=fuzzer,undefined,address)


add_test(NAME parser_fuzz_test_run COMMAND parser_fuzz_test -max_total_time=${FUZZ_RUNTIME})

add_executable(xxd_print_fuzz_test
  xxd_print_fuzz_test.cpp
  "${CMAKE_SOURCE_DIR}/src/xxd_print.cpp")

target_include_directories(
  xxd_print_fuzz_test
  PRIVATE
  "${CMAKE_SOURCE_DIR}/src/"
)
target_link_libraries(
  xxd_print_fuzz_test
  PRIVATE project_options
          project_warnings
          fmt::fmt
          -coverage
          -fsanitize=fuzzer,undefined,address)
target_compile_options(xxd_print_fuzz_test
                        PRIVATE
                        -fsanitize=fuzzer,undefined,address)


add_test(NAME xxd_print_fuzz_test_run COMMAND xxd_print_fuzz_test -max_total_time=${FUZZ_RUNTIME})