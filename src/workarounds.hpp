#ifndef WORKAROUNDS_HPP_
#define WORKAROUNDS_HPP_

#include <cassert>

// NOLINTNEXTLINE(hicpp-no-array-decay,cppcoreguidelines-pro-bounds-array-to-pointer-decay)
#define workaroud_assert(expr) assert(expr)

#endif