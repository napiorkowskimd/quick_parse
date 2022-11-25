#include <cstdlib>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <optional>

#include <CLI/CLI.hpp>
#include <spdlog/spdlog.h>

// This file will be generated automatically when you run the CMake configuration step.
// It creates a namespace called `quick_parse`.
// You can modify the source template at `configured_files/config.hpp.in`.
#include <internal_use_only/config.hpp>

#include "file_reader.hpp"
#include "section_description.hpp"
#include "xxd_print.hpp"

using namespace std::literals::string_literals;

std::vector<SectionDescription>
  ToSectionDescriptions(const std::vector<std::string> &descriptions_str, bool &parse_failed)
{
  parse_failed = false;
  const auto transform_op = [&](const std::string &desc) {
    try {
      return SectionDescription::FromString(desc);
    } catch (const std::invalid_argument &err) {
      parse_failed = true;
      if (err.what() == "stoi"s || err.what() == "stoull"s) {
        fmt::print("One of the arguments for description: {:s} is not a number", desc);
        return SectionDescription{};
      } else {
        throw;
      }
    } catch (const std::out_of_range &err) {
      if (err.what() == "stoi"s || err.what() == "stoull"s) {
        fmt::print("One of the arguments for description: {:s} is too large", desc);
        return SectionDescription{};
      } else {
        throw;
      }
    }
  };
  std::vector<SectionDescription> descriptions;
  std::transform(descriptions_str.begin(),
    descriptions_str.end(),
    std::back_inserter(descriptions),
    transform_op);
  return descriptions;
}


int PrintSection(int count,
  int offset,
  std::span<const uint8_t> data,
  const SectionDescription &desc)
{
  std::optional<uint64_t> value;
  std::optional<uint64_t> masked_value;
  const unsigned int kBitsInByte = 8;
  if (desc.GetLength() <= 4) {
    value = 0;
    const auto length = std::min(data.size(), static_cast<std::size_t>(desc.GetLength()));
    for (unsigned int i = 0; i < length; ++i) {
      *value |= static_cast<uint64_t>(data[i]) << kBitsInByte * (length - i - 1);
    }
  }

  if (value && (desc.GetMask() != 0)) { masked_value = *value & desc.GetMask(); }
  if (value && masked_value) {
    fmt::print("{:s} ({:d})& {:x} = {:x}\n", desc.GetName(), *value, desc.GetMask(), *masked_value);
  } else if (value) {
    fmt::print("{:s} ({:d})\n", desc.GetName(), *value);
  } else {
    fmt::print("{:s}\n", desc.GetName());
  }
  if (desc.GetPeekStream()) {
    xxd_print(data, offset, count);
  } else {
    offset = xxd_print(data, offset, count);
  }

  return offset;
}


int PrintAndSaveSections(FileReader &reader,
  const std::vector<SectionDescription> &descriptions,
  int count,
  const std::map<std::string, std::string> &outputs)
{

  std::map<std::string, FilePtr> output_fds;

  for (const auto &output : outputs) {
    output_fds.emplace(
      std::make_pair(output.first, MakeFilePtr(fopen(output.second.c_str(), "w"))));
  }

  int offset = 0;
  for (const auto &desc : descriptions) {
    std::span<const uint8_t> data;
    const auto to_read = std::min(desc.GetLength(), count - offset);
    if (desc.GetPeekStream()) {
      data = reader.Peek(to_read);
    } else {
      data = reader.Read(to_read);
    }

    [&] {
      if (desc.GetOutputTag().empty()) { return; }
      const auto out_it = output_fds.find(desc.GetOutputTag());
      if (out_it == output_fds.end()) {
        fmt::print(stderr, "Warning: Undefined output tag {:s}\n", desc.GetOutputTag());
        return;
      }
      const auto written = std::fwrite(&data.front(), 1, data.size(), out_it->second.get());

      if (written < data.size()) {
        fmt::print(stderr,
          "Warning: Could not write all data to output {:s} ({:d}/{:d}\n",
          desc.GetOutputTag(),
          written,
          data.size());
      }
    }();

    offset = PrintSection(count, offset, data, desc);
    if (offset >= count) { break; }
  }
  return offset;
}

struct Arguments
{
  std::optional<int> exit_code;
  bool show_version = false;
  std::vector<SectionDescription> descriptions;
  std::string input_file;
  std::map<std::string, std::string> output_files;
  int count{ std::numeric_limits<int>::max() };
};

Arguments HandleCli(int argc, const char **argv)
{
  CLI::App app{ fmt::format(
    "{} version {}", quick_parse::cmake::project_name, quick_parse::cmake::project_version) };

  Arguments args;
  app.add_flag("--version", args.show_version, "Show version information");
  app.add_option("--input,-i", args.input_file, "Input File (- for stdin)")->required(true);
  app.add_option("--count,-c", args.count, "Stop reading file after [count] bytes");

  std::vector<std::string> descriptions_str;
  app.add_option("desc",
    descriptions_str,
    "Data section descriptions in format \"name[*]:length[^mask][>output]\"");

  std::stringstream output_file_mapping_str;
  app
    .add_option("--output,-o", output_file_mapping_str, "Output file in format output:path/to/file")
    ->multi_option_policy(CLI::MultiOptionPolicy::Join);

  try {
    (app).parse((argc), (argv));
  } catch (const CLI::ParseError &e) {
    args.exit_code = (app).exit(e);
    return args;
  }

  bool failed = false;
  args.descriptions = ToSectionDescriptions(descriptions_str, failed);
  if (failed) {
    args.exit_code = EXIT_FAILURE;
    return args;
  }

  while (true) {
    std::string name_path;
    std::getline(output_file_mapping_str, name_path);
    if (name_path.empty()) { break; }
    const auto colon_pos = name_path.find(':');
    if (colon_pos == std::string::npos) {
      fmt::print(
        "Could not parse output file name {:s}, output file must be in format output:path/to/file",
        name_path);
      args.exit_code = EXIT_FAILURE;
      break;
    }
    const auto name = std::string_view(name_path.begin(),
      name_path.begin() + static_cast<std::string::iterator::difference_type>(colon_pos));
    const auto path = std::string_view(
      name_path.begin() + static_cast<std::string::iterator::difference_type>(colon_pos) + 1,
      name_path.end());

    args.output_files.emplace(std::make_pair(name, path));
  }

  return args;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int argc, const char **argv)
{
  const auto args = HandleCli(argc, argv);
  if (args.show_version) {
    fmt::print("{}\n", quick_parse::cmake::project_version);
    return EXIT_SUCCESS;
  }

  if (args.exit_code.has_value()) { return *args.exit_code; }

  FileReader reader;
  if (args.input_file == "-") {
    reader = FileReader{ MakeFilePtrFromStdin() };
  } else {
    reader = FileReader{ MakeFilePtr(std::fopen(args.input_file.c_str(), "r")) };
  }

  if (!reader.IsValid()) {
    fmt::print("Could not open {}\n", args.input_file);
    return EXIT_FAILURE;
  }

  auto offset = PrintAndSaveSections(reader, args.descriptions, args.count, args.output_files);

  fmt::print("---\n");
  const int kChunkSize = 1024;
  while (offset < args.count) {
    auto chunk = reader.Read(std::min(args.count - offset, kChunkSize));
    if (chunk.empty()) { break; }
    offset = xxd_print(chunk, offset, args.count);
  }

  return EXIT_SUCCESS;
}
