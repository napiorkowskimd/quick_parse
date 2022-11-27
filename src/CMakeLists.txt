find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(CLI11 CONFIG REQUIRED)
find_package(Microsoft.GSL CONFIG REQUIRED)

# Generic test that uses conan libs
add_executable(quick_parse
  file_reader.cpp
  main.cpp
  open_file.cpp
  section_description.cpp
  xxd_print.cpp)

target_link_libraries(
  quick_parse
  PUBLIC project_options project_warnings
  PRIVATE CLI11::CLI11 fmt::fmt spdlog::spdlog Microsoft.GSL::GSL)

target_include_directories(quick_parse PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
