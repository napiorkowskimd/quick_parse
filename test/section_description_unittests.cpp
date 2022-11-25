#include <catch2/catch.hpp>


#include "section_description.hpp"


TEST_CASE("ParseSimpleCase", "[SectionDescription]")
{
  const auto desc = SectionDescription::FromString("test:4");
  REQUIRE(desc.GetLength() == 4);
  REQUIRE(desc.GetName() == "test");
  REQUIRE(desc.GetOutputTag().empty());
  REQUIRE(desc.GetMask() == 0);
  REQUIRE(desc.GetPeekStream() == false);
}

TEST_CASE("ParseWithPeekMarker", "[SectionDescription]")
{
  const auto desc = SectionDescription::FromString("test*:4");
  REQUIRE(desc.GetLength() == 4);
  REQUIRE(desc.GetName() == "test");
  REQUIRE(desc.GetOutputTag().empty());
  REQUIRE(desc.GetMask() == 0);
  REQUIRE(desc.GetPeekStream() == true);
}

TEST_CASE("ParseWithHexMask", "[SectionDescription]")
{
  const auto desc = SectionDescription::FromString("test:4^0xffff");
  REQUIRE(desc.GetLength() == 4);
  REQUIRE(desc.GetName() == "test");
  REQUIRE(desc.GetOutputTag().empty());
  REQUIRE(desc.GetMask() == 0xffff);
  REQUIRE(desc.GetPeekStream() == false);
}

TEST_CASE("ParseWithDecMask", "[SectionDescription]")
{
  const auto desc = SectionDescription::FromString("test:4^128");
  REQUIRE(desc.GetLength() == 4);
  REQUIRE(desc.GetName() == "test");
  REQUIRE(desc.GetOutputTag().empty());
  REQUIRE(desc.GetMask() == 128);
  REQUIRE(desc.GetPeekStream() == false);
}

TEST_CASE("ParseWithNegativeHexMask", "[SectionDescription]")
{
  const auto desc = SectionDescription::FromString("test:4^-0x1");
  REQUIRE(desc.GetLength() == 4);
  REQUIRE(desc.GetName() == "test");
  REQUIRE(desc.GetOutputTag().empty());
  REQUIRE(desc.GetMask() == std::numeric_limits<uint64_t>::max());
  REQUIRE(desc.GetPeekStream() == false);
}

TEST_CASE("ParseWithNegativeDecMask", "[SectionDescription]")
{
  const auto desc = SectionDescription::FromString("test:4^-1");
  REQUIRE(desc.GetLength() == 4);
  REQUIRE(desc.GetName() == "test");
  REQUIRE(desc.GetOutputTag().empty());
  REQUIRE(desc.GetMask() == std::numeric_limits<uint64_t>::max());
  REQUIRE(desc.GetPeekStream() == false);
}

TEST_CASE("ParseWithOutputTag", "[SectionDescription]")
{
  const auto desc = SectionDescription::FromString("test:4>out1");
  REQUIRE(desc.GetLength() == 4);
  REQUIRE(desc.GetName() == "test");
  REQUIRE(desc.GetOutputTag() == "out1");
  REQUIRE(desc.GetMask() == 0);
  REQUIRE(desc.GetPeekStream() == false);
}

TEST_CASE("ParseFull", "[SectionDescription]")
{
  const auto desc = SectionDescription::FromString("test*:4^-0x1>out1");
  REQUIRE(desc.GetLength() == 4);
  REQUIRE(desc.GetName() == "test");
  REQUIRE(desc.GetOutputTag() == "out1");
  REQUIRE(desc.GetMask() == std::numeric_limits<uint64_t>::max());
  REQUIRE(desc.GetPeekStream() == true);
}

TEST_CASE("ParseLengthIsNotANumber", "[SectionDescription]")
{
  REQUIRE_THROWS(SectionDescription::FromString("test:asdf"));
}

TEST_CASE("ParseMaskIsNotANumber", "[SectionDescription]")
{
  REQUIRE_THROWS(SectionDescription::FromString("test:1234^asdf"));
}