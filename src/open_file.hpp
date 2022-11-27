#ifndef OPEN_FILE_HPP_
#define OPEN_FILE_HPP_

#include <filesystem>
#include <gsl/gsl>
#include <string>

gsl::owner<std::FILE *> OpenFile(const std::filesystem::path &path, const std::string &mode);

#endif// OPEN_FILE_HPP_