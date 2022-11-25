#include <limits>
#include <span>
#include <cstdint>

#include "xxd_print.hpp"


auto do_test(const uint8_t *Data, size_t Size)
{

  std::span<const uint8_t> input{Data, Size};
  int offset = 0;
  if (Size > 0) {
    offset = *Data;
  }
  xxd_print(input, offset, std::numeric_limits<int>::max());
}

// cppcheck-suppress unusedFunction symbolName=LLVMFuzzerTestOneInput
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  do_test(Data, Size);
  return 0;
}
