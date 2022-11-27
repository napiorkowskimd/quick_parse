#include "open_file.hpp"

#if _MSC_VER
#pragma warning(disable : 4996)
#endif

#if defined(__clang__) && _WIN32
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

#if _MSC_VER
std::wstring ModeToString(OpenFileMode mode)
{
  switch (mode) {
  case OpenFileMode::READ:
    return L"r";
  case OpenFileMode::WRITE:
    return L"w";
  }
  return L"r";
}
#else
std::string ModeToString(OpenFileMode mode)
{
  switch (mode) {
  case OpenFileMode::READ:
    return "r";
  case OpenFileMode::WRITE:
    return "w";
  }
  return "r";
}
#endif


gsl::owner<std::FILE *> OpenFile(const std::filesystem::path &path, const OpenFileMode &mode)
{
#ifdef _MSC_VER
  return _wfopen(path.c_str(), ModeToString(mode).c_str());
#else
  return std::fopen(path.c_str(), ModeToString(mode).c_str());
#endif
}

#if defined(__clang__) && _WIN32
#pragma clang diagnostic pop
#endif
