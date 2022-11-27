#include "open_file.hpp"

gsl::owner<std::FILE *> OpenFile(const std::filesystem::path &path, const std::string &mode)
{
#if _WIN32
  gsl::owner<std::FILE *> file{};
  const auto err = std::fopen_s(&file, path.c_str(), mode.c_str());
  if (err != 0) { return nullptr; }
  return file;
#else
  return std::fopen(path.c_str(), mode.c_str());
#endif
}
