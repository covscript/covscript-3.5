#include <csvm/types/basic.hpp>
#include <catch2/catch_all.hpp>

using namespace csvm;

TEST_CASE("stack basic operations", "[stack]")
{
	stack<int> s;

	REQUIRE(s.empty());
	REQUIRE(s.size() == 0);

	SECTION("push and top")
	{
		s.push(10);
		REQUIRE(!s.empty());
		REQUIRE(s.size() == 1);
		REQUIRE(s.top() == 10);

		s.push(20);
		REQUIRE(s.size() == 2);
		REQUIRE(s.top() == 20);
	}

	SECTION("pop and pop_no_return")
	{
		s.push(1);
		s.push(2);
		s.push(3);

		int val = s.pop();
		REQUIRE(val == 3);
		REQUIRE(s.size() == 2);

		s.pop_no_return();
		REQUIRE(s.size() == 1);
		REQUIRE(s.top() == 1);
	}

	SECTION("bottom and index access")
	{
		s.push(5);
		s.push(6);
		s.push(7);

		REQUIRE(s.bottom() == 5);
	}

	SECTION("clear")
	{
		s.push(42);
		s.push(43);
		s.clear();
		REQUIRE(s.empty());
		REQUIRE(s.size() == 0);
	}
}

TEST_CASE("stack iterator", "[stack]")
{
	stack<int> s;
	for (int i = 0; i < 5; ++i)
	{
		s.push(i);
	}

	SECTION("non-const iterator")
	{
		int expected = 4;
		for (auto it = s.begin(); it != s.end(); ++it)
		{
			REQUIRE(*it == expected);
			--expected;
		}
	}

	SECTION("const iterator")
	{
		const stack<int> &cs = s;
		int expected = 4;
		for (auto it = cs.begin(); it != cs.end(); ++it)
		{
			REQUIRE(*it == expected);
			--expected;
		}
	}
}

TEST_CASE("stack with different types", "[stack]")
{
	stack<std::string> s;
	s.push("hello");
	s.push("world");

	REQUIRE(s.top() == "world");
	REQUIRE(s.bottom() == "hello");

	std::string val = s.pop();
	REQUIRE(val == "world");
	REQUIRE(s.top() == "hello");
}
