#include "xxd_print.hpp"

#include <span>
#include <string>

#include <fmt/format.h>

int xxd_print(std::span<const std::uint8_t> data, int offset, int count)
{
  // 00000000: 0000 0000 0000 0000 0000 0000 0000 0000  ................
  static constexpr unsigned int kItemsNum = 16;
  static constexpr int kHexDigitsStrLen = 39;

  auto data_it = data.begin();

  while (data_it != data.end() && offset <= count) {
    const auto offset_str = fmt::format("{:0>8x}", offset);
    auto ascii_text = std::string(kItemsNum, ' ');
    auto hex_digits = std::string(kHexDigitsStrLen, ' ');
    auto hex_digits_it = hex_digits.begin();
    for (unsigned int i = 0; i < kItemsNum; ++i) {
      if (data_it == data.end()) { break; }
      const auto value = *data_it;
      hex_digits_it = fmt::format_to(hex_digits_it, "{:0>2x}", value);
      if ((i & 1U) != 0U) { ++hex_digits_it; }
      const char value_ascii = value > 0x1f && value < 0x7f ? static_cast<char>(value) : '.';
      ascii_text[i] = value_ascii;
      ++offset;
      ++data_it;
    }

    fmt::print("{:s}: {:s} {:s}\n", offset_str, hex_digits, ascii_text);
  }
  return offset;
}