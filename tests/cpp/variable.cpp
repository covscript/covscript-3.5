#include <covscript/types/variable.hpp>
#include <catch2/catch_all.hpp>

using namespace cs;

TEST_CASE("var default construction", "[var]")
{
	var v;
	REQUIRE(v.is_null() == true);
	REQUIRE(v.to_string().view() == "null");
	REQUIRE(v.to_integer() == 0);
	REQUIRE(v.hash() == 0);
}

TEST_CASE("var construct with numeric_t", "[var]")
{
	numeric_t n(42LL);
	var v = var::make<numeric_t>(n);

	REQUIRE(v.usable());
	REQUIRE(v.type() == typeid(numeric_t));
	REQUIRE(v.to_integer() == 42);
	REQUIRE(v.to_string().view() == "42");
}

TEST_CASE("var copy and move", "[var]")
{
	var v1 = var::make<numeric_t>(123LL);
	var v2(v1); // copy constructor

	REQUIRE(v2.usable());
	REQUIRE(v2.to_integer() == 123);

	var v3(std::move(v1)); // move constructor
	REQUIRE(v3.usable());
	REQUIRE(v3.to_integer() == 123);
}

TEST_CASE("var swap", "[var]")
{
	var a = var::make<numeric_t>(1LL);
	var b = var::make<numeric_t>(2LL);

	a.swap(b);

	REQUIRE(a.to_integer() == 2);
	REQUIRE(b.to_integer() == 1);
}

TEST_CASE("var detach does not crash", "[var]")
{
	var v = var::make<numeric_t>(99LL);
	REQUIRE_NOTHROW(v.detach());
}

TEST_CASE("var SVO enabled for small objects", "[var]")
{
	var v = var::make<numeric_t>(10LL);
	REQUIRE(v.internal_svo_enabled() == true);
}

TEST_CASE("var to_integer_var returns numeric_t wrapped var", "[var]")
{
	var v = var::make<numeric_t>(77LL);
	var vi = v.to_integer_var();

	REQUIRE(vi.usable());
	REQUIRE(vi.type() == typeid(numeric_t));
	REQUIRE(vi.to_integer() == 77);
}
