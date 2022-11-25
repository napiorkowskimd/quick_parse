#include "file_reader.hpp"

#include <gsl/gsl>
#include <spdlog/spdlog.h>

void fclose_deleter(gsl::owner<std::FILE *> handle)
{
  const auto result = fclose(handle);
  if (result != 0) { spdlog::error("Could not close file handle"); }
}

void noop_deleter(gsl::owner<std::FILE *> /*handle*/) {}

FilePtr MakeFilePtr(gsl::owner<std::FILE *> handle) { return FilePtr{ handle, &fclose_deleter }; }

FilePtr MakeFilePtrFromStdin() { return FilePtr{ stdin, &noop_deleter }; }


bool FileReader::HasEnoughInBuffer(int length) const { return (buf_end_ - buf_start_) >= length; }

void FileReader::ReadIntoBuffer(int length)
{
  const auto currently_in_buffer = buf_end_ - buf_start_;
  const auto to_read = static_cast<size_t>(length - currently_in_buffer);
  if (to_read >= buffer_.size() - static_cast<size_t>(buf_end_)) {
    buffer_.resize(static_cast<std::size_t>(std::ssize(buffer_) + length));
  }
  const auto new_data_size = std::fread(&*(buffer_.begin() + buf_end_), 1, to_read, file_.get());
  buf_end_ += static_cast<vec_size_t>(new_data_size);
}

std::span<uint8_t> FileReader::Peek(int length)
{
  assert(length > 0);
  if (!HasEnoughInBuffer(length)) { ReadIntoBuffer(length); }
  const auto signed_length = static_cast<vec_size_t>(length);
  if (signed_length > (buf_end_ - buf_start_)) {
    const auto end = buffer_.begin() + buf_end_;
    return { buffer_.begin() + buf_start_, end };
  } else {
    const auto end = buffer_.begin() + buf_start_ + signed_length;
    return { buffer_.begin() + buf_start_, end };
  }
}

std::span<uint8_t> FileReader::Read(int length)
{
  auto result = Peek(length);
  buf_start_ += static_cast<vec_size_t>(result.size());
  if (buf_end_ == buf_start_) {
    buf_start_ = 0;
    buf_end_ = 0;
  }
  return result;
}

bool FileReader::IsValid() const { return file_.get() != nullptr; }
