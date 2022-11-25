#include <cstring>
#include <fmt/format.h>
#include <string>
#include <utility>

#include "section_description.hpp"

using namespace std::literals::string_literals;

auto print_desc(const uint8_t *Data, size_t Size)
{
  std::string input(Size, char{ 0 });
  std::memcpy(input.data(), Data, Size);
  try {
    const auto desc = SectionDescription::FromString(input);
    fmt::print("name: {}, len: {}, mask: {}, output: {}, peek: {}\n",
      desc.GetName(),
      desc.GetLength(),
      desc.GetMask(),
      desc.GetOutputTag(),
      desc.GetPeekStream());
  } catch (const std::invalid_argument &err) {
    if (err.what() != "stoi"s && err.what() != "stoull"s) { throw; }
  } catch (const std::out_of_range &err) {
    if (err.what() != "stoi"s && err.what() != "stoull"s) { throw; }
  }
}

// cppcheck-suppress unusedFunction symbolName=LLVMFuzzerTestOneInput
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  print_desc(Data, Size);
  return 0;
}
