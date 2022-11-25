#ifndef SECTION_DESCRIPTION_HPP__
#define SECTION_DESCRIPTION_HPP__

#include <string>
#include <string_view>
#include <vector>

class SectionDescription
{
  std::string name_;
  std::string output_tag_;
  std::uint64_t mask_{ 0 };
  bool peek_stream_{ false };
  int length_{ 0 };

public:
  static SectionDescription FromString(const std::string_view &data);
  [[nodiscard]] const std::string &GetName() const;
  [[nodiscard]] const std::string &GetOutputTag() const;
  [[nodiscard]] std::uint64_t GetMask() const;
  [[nodiscard]] bool GetPeekStream() const;
  [[nodiscard]] int GetLength() const;
};

#endif// SECTION_DESCRIPTION_HPP__