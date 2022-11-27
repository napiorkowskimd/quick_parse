#ifndef OPEN_FILE_HPP_
#define OPEN_FILE_HPP_

#include <filesystem>
#include <gsl/gsl>
#include <string>

enum class OpenFileMode {
  READ,
  WRITE
};

gsl::owner<std::FILE *> OpenFile(const std::filesystem::path &path, const OpenFileMode &mode);

#endif// OPEN_FILE_HPP_