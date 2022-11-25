#include "section_description.hpp"

#include <charconv>

char NextChar(std::string_view &str)
{
  const char value = str.front();
  str = str.substr(1);
  return value;
}

// static
SectionDescription SectionDescription::FromString(const std::string_view &data)
{
  static constexpr const char PEEK_MODE_TOKEN = '*';
  static constexpr const char SEPARATOR_TOKEN = ':';
  static constexpr const char MASK_TOKEN = '^';
  static constexpr const char OUTPUT_TAG_TOKEN = '>';

  const int kParsedValuePreAllocSize = 128;

  enum State { kParseName, kParseLength, kParseMask, kParseOutputTag };

  constexpr auto IsSpecialToken = [](char token) {
    return token == PEEK_MODE_TOKEN || token == SEPARATOR_TOKEN || token == MASK_TOKEN
           || token == OUTPUT_TAG_TOKEN;
  };

  SectionDescription parsed_desc;

  // name[*]:length[^mask][>output]
  std::string_view to_parse = data;
  std::string last_parsed_value;
  last_parsed_value.reserve(kParsedValuePreAllocSize);
  char last_token{};
  State state = kParseName;

  const auto AssignValue = [&]() {
    if (last_parsed_value.empty()) { return; }
    switch (state) {
    case kParseName:
      parsed_desc.name_ = last_parsed_value;
      break;
    case kParseLength:
      parsed_desc.length_ = std::stoi(last_parsed_value, nullptr, 0);
      break;
    case kParseMask:
      parsed_desc.mask_ = std::stoull(last_parsed_value, nullptr, 0);
      break;
    case kParseOutputTag:
      parsed_desc.output_tag_ = last_parsed_value;
      break;
    }
    last_parsed_value.clear();
  };

  const auto SetNextState = [&]() {
    switch (last_token) {
    case PEEK_MODE_TOKEN:
      parsed_desc.peek_stream_ = true;
      [[fallthrough]];
    case SEPARATOR_TOKEN:
      state = kParseLength;
      break;
    case MASK_TOKEN:
      state = kParseMask;
      break;
    case OUTPUT_TAG_TOKEN:
      state = kParseOutputTag;
      break;
    default:
      break;
    }
  };

  while (!to_parse.empty()) {
    last_token = NextChar(to_parse);
    if (!IsSpecialToken(last_token)) {
      last_parsed_value += last_token;
      continue;
    }
    AssignValue();
    SetNextState();
  }

  AssignValue();

  return parsed_desc;
}

const std::string &SectionDescription::GetName() const { return name_; }
const std::string &SectionDescription::GetOutputTag() const { return output_tag_; }
std::uint64_t SectionDescription::GetMask() const { return mask_; }
bool SectionDescription::GetPeekStream() const { return peek_stream_; }
int SectionDescription::GetLength() const { return length_; }
