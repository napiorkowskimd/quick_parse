#ifndef XXD_PRINT_HPP__
#define XXD_PRINT_HPP__

#include <span>
#include <cstdint>

int xxd_print(std::span<const std::uint8_t> data, int offset, int count);

#endif