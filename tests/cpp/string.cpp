#include <csvm/types/string.hpp>
#include <catch2/catch_all.hpp>

using namespace csvm;

TEST_CASE("String: basic operations", "[string]")
{
	byte_string_t str = "Hello, World!";
	REQUIRE(str.length() == 13);
	REQUIRE(str.substr(7, 5) == "World");
	str.replace(7, 5, "C++");
	REQUIRE(str == "Hello, C++!");
}

TEST_CASE("String: unicode operations", "[string]")
{
	unicode_string_t unicode_str = unicode::byte_to_unicode("你好，世界！");
	REQUIRE(unicode_str.length() == 6);
	REQUIRE(unicode::unicode_to_byte(unicode_str.substr(0, 2)) == "你好");
}

TEST_CASE("basic_string_borrower default and borrow constructors", "[basic_string_borrower]")
{
	SECTION("default constructor")
	{
		byte_string_borrower b;
		REQUIRE(b.view().empty());
		REQUIRE(b.access() == nullptr);
	}

	SECTION("borrow from const char*")
	{
		const char *s = "hello";
		byte_string_borrower b(s);
		REQUIRE(b.view() == "hello");
		REQUIRE(b.access() == nullptr);
	}

	SECTION("borrow from string_view")
	{
		byte_string_t str = "world";
		byte_string_view view = str;
		byte_string_borrower b = view;
		REQUIRE(b.view() == "world");
		REQUIRE(b.access() == nullptr);
	}
}

TEST_CASE("basic_string_borrower own constructor", "[basic_string_borrower]")
{
	byte_string_t str = "owned";
	byte_string_borrower b(std::move(str));

	REQUIRE(b.view() == "owned");
	REQUIRE(b.access() != nullptr);

	*b.access() = "changed";
	REQUIRE(b.view() == "changed");
}

TEST_CASE("basic_string_borrower copy constructor", "[basic_string_borrower]")
{
	byte_string_borrower original(byte_string_t("copyme"));
	byte_string_borrower copy = original;

	REQUIRE(copy.view() == "copyme");

	// Modifying original should not affect copy
	*original.access() = "changed";
	REQUIRE(copy.view() == "copyme");
	REQUIRE(original.view() == "changed");
}

TEST_CASE("basic_string_borrower move constructor", "[basic_string_borrower]")
{
	byte_string_borrower original(byte_string_t("moveme"));
	byte_string_borrower moved = std::move(original);

	REQUIRE(moved.view() == "moveme");
	REQUIRE(original.view().empty());
	REQUIRE(original.access() == nullptr);
}

TEST_CASE("basic_string_borrower copy assignment", "[basic_string_borrower]")
{
	byte_string_borrower a(byte_string_t("a"));
	byte_string_borrower b(byte_string_t("b"));

	b = a;
	REQUIRE(b.view() == "a");

	*a.access() = "changed";
	REQUIRE(b.view() == "a"); // copy remains unaffected
}

TEST_CASE("basic_string_borrower move assignment", "[basic_string_borrower]")
{
	byte_string_borrower a(byte_string_t("move_a"));
	byte_string_borrower b(byte_string_t("move_b"));

	b = std::move(a);
	REQUIRE(b.view() == "move_a");
	REQUIRE(a.view().empty());
	REQUIRE(a.access() == nullptr);
}

TEST_CASE("basic_string_borrower view conversion", "[basic_string_borrower]")
{
	byte_string_borrower b(byte_string_t("convert"));
	byte_string_view v = b.view();
	REQUIRE(v == "convert");
}

TEST_CASE("basic_string_borrower access returns nullptr for borrowed", "[basic_string_borrower]")
{
	const char *s = "borrowed";
	byte_string_borrower b(s);
	REQUIRE(b.access() == nullptr);
}