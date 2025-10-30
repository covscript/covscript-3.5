#include <covscript/types/types.hpp>
#include <catch2/catch_all.hpp>

using namespace csvm;

struct Small
{
	int v;
	Small() : v(0) {}
	explicit Small(int x) : v(x) {}
	bool operator==(const Small &o) const
	{
		return v == o.v;
	}
};

template <>
byte_string_borrower cs_impl::to_string<Small>(const Small &val)
{
	return std::to_string(val.v);
}

template <>
std::size_t cs_impl::hash<Small>(const Small &val)
{
	return cs_impl::hash<int>(val.v);
}

struct Large
{
	// make it large enough to go to heap path
	char data[1024];
	Large()
	{
		std::memset(data, 0, sizeof(data));
	}
	explicit Large(char c)
	{
		std::memset(data, c, sizeof(data));
	}
	bool operator==(const Large &o) const
	{
		return std::memcmp(data, o.data, sizeof(data)) == 0;
	}
};

TEST_CASE("basic_var: default/null behavior", "[basic_var]")
{
	cs::var v;
	REQUIRE(v.is_null());
	REQUIRE(!v.usable());
	REQUIRE(v.to_integer() == 0);
	REQUIRE(v.to_string().view() == "null");
	REQUIRE(v.hash() == 0);
	REQUIRE(v.type() == typeid(cs::null_t));
}

TEST_CASE("basic_var: SVO small type construction and access", "[basic_var][svo]")
{
	auto v = cs::var::make<Small>(42);
	REQUIRE(v.usable());
	REQUIRE(v.is_type_of<Small>());
	REQUIRE(v.type() == typeid(Small));
	REQUIRE(v.const_val<Small>().v == 42);
	REQUIRE(v.val<Small>().v == 42);

	SECTION("copy and equality")
	{
		auto v2 = cs::var::make<Small>(42);
		REQUIRE(v == v2);
		auto v3 = v; // copy ctor
		REQUIRE(v3 == v);
		cs::var v4;
		v4 = v;
		REQUIRE(v4 == v);
	}

	SECTION("move semantics")
	{
		cs::var tmp = cs::var::make<Small>(13);
		cs::var moved = std::move(tmp);
		REQUIRE(moved.is_type_of<Small>());
		REQUIRE(!tmp.usable()); // moved-from should be null
	}

	SECTION("type name and to_string")
	{
		REQUIRE(v.type_name().view() == cs_impl::get_name_of_type<Small>());
		// our mock to_string returns integer string for integral; Small isn't integral so type name returned
		REQUIRE(v.to_string().view() == std::to_string(v.const_val<Small>().v));
	}
}

TEST_CASE("basic_var: heap path with large type", "[basic_var][heap]")
{
	static_assert(sizeof(Large) > cs::basic_var<CSVM_SVO_ALIGN_SIZE>::internal_svo_threshold(), "Large should be heap-bound for test");
	auto v = cs::var::make<Large>(char(7));
	REQUIRE(v.usable());
	REQUIRE(v.is_type_of<Large>());
	REQUIRE(v.const_val<Large>().data[0] == 7);
	// copy should deep-copy
	auto v2 = v;
	REQUIRE(v2 == v); // uses compare() -> equality of Large's contents
	// move
	auto v3 = std::move(v);
	REQUIRE(v3.usable());
	REQUIRE(!v.usable()); // moved-from becomes null
}

TEST_CASE("basic_var: numeric to_integer and to_integer_var", "[basic_var][numeric]")
{
	auto nvar = cs::var::make<numeric_t>(123LL);
	REQUIRE(nvar.usable());
	REQUIRE(nvar.is_type_of<numeric_t>());
	REQUIRE(nvar.to_integer() == 123);
	auto vi = nvar.to_integer_var();
	REQUIRE(vi.is_type_of<numeric_t>());
	REQUIRE(vi.const_val<numeric_t>() == 123);
}

TEST_CASE("basic_var: wrong type and null exceptions from val/const_val", "[basic_var][exceptions]")
{
	auto v = cs::var::make<Small>(5);
	REQUIRE_THROWS_AS(v.const_val<int>(), runtime_error);
	REQUIRE_THROWS_AS(v.val<int>(), runtime_error);

	cs::var nullv;
	REQUIRE_THROWS_AS(nullv.val<Small>(), runtime_error);
	REQUIRE_THROWS_AS(nullv.const_val<Small>(), runtime_error);
}

TEST_CASE("basic_var: swap and hash", "[basic_var][misc]")
{
	auto a = cs::var::make<Small>(1);
	auto b = cs::var::make<Small>(2);
	auto ha = a.hash();
	auto hb = b.hash();
	REQUIRE(a != b);
	a.swap(b);
	REQUIRE(a.const_val<Small>().v == 2);
	REQUIRE(b.const_val<Small>().v == 1);
}
