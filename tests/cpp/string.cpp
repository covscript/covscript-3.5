#include <covscript/types/string.hpp>
#include <catch2/catch_all.hpp>

TEST_CASE("String: basic operations", "[string]")
{
	cs::byte_string_t str = "Hello, World!";
	REQUIRE(str.length() == 13);
	REQUIRE(str.substr(7, 5) == "World");
	str.replace(7, 5, "C++");
	REQUIRE(str == "Hello, C++!");
}

TEST_CASE("String: unicode operations", "[string]")
{
	cs::unicode_string_t unicode_str = cs::unicode::byte_to_unicode("你好，世界！");
	REQUIRE(unicode_str.length() == 6);
	REQUIRE(cs::unicode::unicode_to_byte(unicode_str.substr(0, 2)) == "你好");
}
