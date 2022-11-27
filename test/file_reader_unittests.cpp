#include <cstdio>
#include <memory>

#include <catch2/catch.hpp>

#include "file_reader.hpp"
#include "open_file.hpp"

static constexpr const char *test_dat_file = "file_reader_test_data.txt";
static constexpr const auto kASectionEnd = 4;
static constexpr const auto kBSectionEnd = 8;
static constexpr const auto kDSectionEnd = 136;


TEST_CASE("SanityTestDataCanBeOpened", "[FileReader]")
{
  const FileReader reader{ MakeFilePtr(OpenFile(test_dat_file, OpenFileMode::READ)) };
  REQUIRE(reader.IsValid());
}

TEST_CASE("Peeking doesn't move read head", "[FileReader]")
{
  FileReader reader{ MakeFilePtr(OpenFile(test_dat_file, OpenFileMode::READ)) };
  const auto four_A = reader.Peek(4);
  REQUIRE(four_A.size() == 4);
  for (const auto &elem : four_A) { REQUIRE(elem == 'A'); }

  const auto four_A_prim = reader.Read(4);
  REQUIRE(four_A_prim.size() == 4);
  for (const auto &elem : four_A_prim) { REQUIRE(elem == 'A'); }
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST_CASE("Resize buffer_ after Peek", "[FileReader]")
{
  FileReader reader{ MakeFilePtr(OpenFile(test_dat_file, OpenFileMode::READ)) };
  const auto suffix = reader.Peek(kBSectionEnd);
  REQUIRE(suffix.size() == kBSectionEnd);
  for (unsigned i = 0; i < kASectionEnd; ++i) { REQUIRE(suffix[i] == 'A'); }

  for (unsigned i = kASectionEnd; i < kBSectionEnd; ++i) { REQUIRE(suffix[i] == 'B'); }

  const auto data = reader.Read(kDSectionEnd);
  REQUIRE(data.size() == kDSectionEnd);
  for (unsigned i = 0; i < kASectionEnd; ++i) { REQUIRE(data[i] == 'A'); }

  for (unsigned i = kASectionEnd; i < kBSectionEnd; ++i) { REQUIRE(data[i] == 'B'); }

  for (unsigned i = kBSectionEnd; i < kDSectionEnd; ++i) { REQUIRE(data[i] == 'D'); }
}

TEST_CASE("Read moves the head", "[FileReader]")
{
  FileReader reader{ MakeFilePtr(OpenFile(test_dat_file, OpenFileMode::READ)) };
  const auto four_A = reader.Read(4);
  REQUIRE(four_A.size() == 4);
  const auto four_B = reader.Read(4);
  REQUIRE(four_B.size() == 4);
  for (const auto &elem : four_B) { REQUIRE(elem == 'B'); }
}

TEST_CASE("Read larger chunk of data", "[FileReader]")
{
  FileReader reader{ MakeFilePtr(OpenFile(test_dat_file, OpenFileMode::READ)) };
  const auto suffix_data = reader.Read(8);
  REQUIRE(suffix_data.size() == 8);

  const auto data = reader.Read(128);
  REQUIRE(data.size() == 128);
  for (const auto &elem : data) { REQUIRE(elem == 'D'); }
}

TEST_CASE("Read past the end of file", "[FileReader]")
{
  FileReader reader{ MakeFilePtr(OpenFile(test_dat_file, OpenFileMode::READ)) };
  const auto suffix_data = reader.Read(136);
  REQUIRE(suffix_data.size() == 136);

  const auto data = reader.Read(8);
  REQUIRE(data.size() == 4);
  for (const auto &elem : data) { REQUIRE(elem == 'C'); }
}

TEST_CASE("Peek whole file then read some", "[FileReader]")
{
  FileReader reader{ MakeFilePtr(OpenFile(test_dat_file, OpenFileMode::READ)) };
  const auto suffix_data = reader.Peek(140);
  REQUIRE(suffix_data.size() == 140);

  const auto data = reader.Read(4);
  REQUIRE(data.size() == 4);
  const auto data2 = reader.Read(4);
  REQUIRE(data2.size() == 4);
  for (const auto &elem : data2) { REQUIRE(elem == 'B'); }
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST_CASE("Edge Peek then Read one Byte more", "[FileReader]")
{
  FileReader reader{ MakeFilePtr(OpenFile(test_dat_file, OpenFileMode::READ)) };
  const auto suffix = reader.Peek(kASectionEnd);
  REQUIRE(suffix.size() == kASectionEnd);
  for (unsigned i = 0; i < kASectionEnd; ++i) { REQUIRE(suffix[i] == 'A'); }
  const auto data = reader.Read(kASectionEnd + 1);
  REQUIRE(data.size() == kASectionEnd + 1);
  for (unsigned i = 0; i < kASectionEnd; ++i) { REQUIRE(data[i] == 'A'); }

  REQUIRE(data[kASectionEnd] == 'B');
}
