#include <covscript/types/numeric.hpp>
#include <catch2/catch_all.hpp>

using namespace csvm;

TEST_CASE("numeric_t constructors and type detection", "[numeric_t]")
{
	numeric_t a; // default
	REQUIRE(a.is_integer());
	REQUIRE(a.as_integer() == 0);

	numeric_t b(42LL);
	REQUIRE(b.is_float() == false);
	REQUIRE(b.as_integer() == 42);

	numeric_t c(3.14L);
	REQUIRE(c.is_float());
	REQUIRE(c.as_float() == Catch::Approx(3.14L));

	numeric_t d = b;
	REQUIRE(d.as_integer() == 42);

	numeric_t e = std::move(c);
	REQUIRE(e.as_float() == Catch::Approx(3.14L));
}

TEST_CASE("numeric_t assignment", "[numeric_t]")
{
	numeric_t a;
	a = 123LL;
	REQUIRE(a.is_float() == false);
	REQUIRE(a.as_integer() == 123);

	a = 1.5L;
	REQUIRE(a.is_float());
	REQUIRE(a.as_float() == Catch::Approx(1.5L));
}

TEST_CASE("numeric_t arithmetic between same types", "[numeric_t]")
{
	numeric_t i1(10LL), i2(4LL);
	numeric_t f1(2.5L), f2(0.5L);

	REQUIRE((i1 + i2).as_integer() == 14);
	REQUIRE((i1 - i2).as_integer() == 6);
	REQUIRE((i1 * i2).as_integer() == 40);
	REQUIRE((i1 / i2).as_integer() == 2);

	REQUIRE((f1 + f2).as_float() == Catch::Approx(3.0L));
	REQUIRE((f1 - f2).as_float() == Catch::Approx(2.0L));
	REQUIRE((f1 * f2).as_float() == Catch::Approx(1.25L));
	REQUIRE((f1 / f2).as_float() == Catch::Approx(5.0L));
}

TEST_CASE("numeric_t arithmetic between int and float", "[numeric_t]")
{
	numeric_t i(10LL);
	numeric_t f(2.5L);

	REQUIRE((i + f).as_float() == Catch::Approx(12.5L));
	REQUIRE((f + i).as_float() == Catch::Approx(12.5L));
	REQUIRE((i - f).as_float() == Catch::Approx(7.5L));
	REQUIRE((f - i).as_float() == Catch::Approx(-7.5L));
	REQUIRE((i * f).as_float() == Catch::Approx(25.0L));
	REQUIRE((f / i).as_float() == Catch::Approx(0.25L));
}

TEST_CASE("numeric_t arithmetic with literals", "[numeric_t]")
{
	numeric_t i(10LL);
	numeric_t f(2.5L);

	REQUIRE((i + 2).as_integer() == 12);
	REQUIRE((i * 3).as_integer() == 30);
	REQUIRE((f + 1.5L).as_float() == Catch::Approx(4.0L));
	REQUIRE((f * 2.0L).as_float() == Catch::Approx(5.0L));
}

TEST_CASE("numeric_t division with remainder produces float", "[numeric_t]")
{
	numeric_t a(7LL), b(2LL);
	auto result = a / b;
	REQUIRE(result.is_float());
	REQUIRE(result.as_float() == Catch::Approx(3.5L));
}

TEST_CASE("numeric_t comparisons", "[numeric_t]")
{
	numeric_t i1(10LL), i2(20LL);
	numeric_t f1(2.5L), f2(2.5L);

	REQUIRE(i1 < i2);
	REQUIRE(i2 > i1);
	REQUIRE(i1 <= i2);
	REQUIRE(i2 >= i1);
	REQUIRE(i1 != i2);
	REQUIRE(f1 == f2);

	REQUIRE(i1 < 15);
	REQUIRE(i2 > 15);
	REQUIRE(f1 == Catch::Approx(2.5L));
	REQUIRE(f1 <= 3.0L);
	REQUIRE(f1 >= 2.0L);
}

TEST_CASE("numeric_t increment and decrement", "[numeric_t]")
{
	numeric_t i(5LL);
	numeric_t f(1.5L);

	REQUIRE((i++).as_integer() == 5);
	REQUIRE(i.as_integer() == 6);

	REQUIRE((++i).as_integer() == 7);
	REQUIRE((i--).as_integer() == 7);
	REQUIRE(i.as_integer() == 6);

	REQUIRE((--i).as_integer() == 5);

	REQUIRE((f++).as_float() == Catch::Approx(1.5L));
	REQUIRE(f.as_float() == Catch::Approx(2.5L));

	REQUIRE((--f).as_float() == Catch::Approx(1.5L));
}

TEST_CASE("numeric_t negation", "[numeric_t]")
{
	numeric_t i(10LL);
	numeric_t f(2.5L);
	REQUIRE((-i).as_integer() == -10);
	REQUIRE((-f).as_float() == Catch::Approx(-2.5L));
}
