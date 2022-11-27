#include "open_file.hpp"

#if _MSC_VER
#pragma warning(disable:2220)
#endif

#if defined(__clang__) && _WIN32
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif


gsl::owner<std::FILE *> OpenFile(const std::filesystem::path &path, const std::string &mode)
{
  return std::fopen(path.c_str(), mode.c_str());
}

#if defined(__clang__) && _WIN32
#pragma clang diagnostic pop
#endif
