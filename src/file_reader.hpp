#ifndef FILE_READER_H__
#define FILE_READER_H__

#include <cstdio>
#include <memory>
#include <span>
#include <vector>
#include <gsl/gsl>

using FilePtr = std::unique_ptr<std::FILE, void(*)(std::FILE*)>;

FilePtr MakeFilePtr(gsl::owner<std::FILE*> handle);
FilePtr MakeFilePtrFromStdin();

class FileReader
{
private:
  FilePtr file_{nullptr, +[](gsl::owner<std::FILE*>) {}};
  std::vector<uint8_t> buffer_;
  using vec_size_t = std::vector<uint8_t>::iterator::difference_type;
  vec_size_t buf_start_{0};
  vec_size_t buf_end_{0};
public:
  FileReader() = default;
  explicit FileReader(FilePtr file) : file_(std::move(file)) {}
  [[nodiscard]] std::span<uint8_t> Read(int length);
  [[nodiscard]] std::span<uint8_t> Peek(int length);
  [[nodiscard]] bool IsValid() const;
private:
  [[nodiscard]] bool HasEnoughInBuffer(int length) const;
  void ReadIntoBuffer(int length);

};

#endif// FILE_READER_H__