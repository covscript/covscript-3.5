#include <iostream>
#include <any>
#include <vector>
#include <chrono>
#include <string>
#include <cstdint>
#include <covscript/types/variable.hpp>

using namespace std::chrono;

using Small = std::size_t;
using Large = cs::numeric_t;

constexpr size_t N = 10'000'000;

#define TIME_BLOCK(name, code)                                                                    \
    do                                                                                            \
    {                                                                                             \
        auto start = high_resolution_clock::now();                                                \
        code auto end = high_resolution_clock::now();                                             \
        std::cout << name << ": " << duration_cast<milliseconds>(end - start).count() << " ms\n"; \
    } while (0)

int main()
{
	std::cout << "=== Performance comparison std::any vs cs::basic_var<16> ===\n";

	std::cout << "cs::basic_var<16> SVO Threshold: " << cs::basic_var<16>::internal_svo_threshold() << std::endl;
	std::cout << "cs::basic_var<48> SVO Threshold: " << cs::basic_var<48>::internal_svo_threshold() << std::endl;
	std::cout << "sizeof(Small): " << sizeof(Small) << std::endl;
	std::cout << "sizeof(Large): " << sizeof(Large) << std::endl;
	std::cout << "sizeof(std::any): " << sizeof(std::any) << std::endl;
	std::cout << "sizeof(cs::basic_var<16>): " << sizeof(cs::basic_var<16>) << std::endl;
	std::cout << "sizeof(cs::basic_var<48>): " << sizeof(cs::basic_var<48>) << std::endl;

	TIME_BLOCK("std::any construct Small", {
		std::vector<std::any> vec;
		vec.reserve(N);
		for (size_t i = 0; i < N; ++i)
			vec.emplace_back(Small{i});
	});

	TIME_BLOCK("cs::basic_var<16> construct Small", {
		std::vector<cs::basic_var<16>> vec;
		vec.reserve(N);
		for (size_t i = 0; i < N; ++i)
			vec.emplace_back(Small{i});
	});

	TIME_BLOCK("cs::basic_var<48> construct Small", {
		std::vector<cs::basic_var<48>> vec;
		vec.reserve(N);
		for (size_t i = 0; i < N; ++i)
			vec.emplace_back(Small{i});
	});

	TIME_BLOCK("std::any construct Large", {
		std::vector<std::any> vec;
		vec.reserve(N);
		for (size_t i = 0; i < N; ++i)
			vec.emplace_back(Large{});
	});

	TIME_BLOCK("cs::basic_var<16> construct Large", {
		std::vector<cs::basic_var<16>> vec;
		vec.reserve(N);
		for (size_t i = 0; i < N; ++i)
			vec.emplace_back(Large{});
	});

	TIME_BLOCK("cs::basic_var<48> construct Large", {
		std::vector<cs::basic_var<48>> vec;
		vec.reserve(N);
		for (size_t i = 0; i < N; ++i)
			vec.emplace_back(Large{});
	});

	{
		std::vector<std::any> any_vec;
		std::vector<cs::basic_var<16>> var32_vec;
		std::vector<cs::basic_var<48>> var64_vec;
		any_vec.reserve(N);
		var32_vec.reserve(N);
		var64_vec.reserve(N);
		for (size_t i = 0; i < N; ++i) {
			any_vec.emplace_back(Small{i});
			var32_vec.emplace_back(Small{i});
			var64_vec.emplace_back(Small{i});
		}

		TIME_BLOCK("std::any access Small", {
			int sum = 0;
			for (auto &a : any_vec)
				sum += std::any_cast<Small>(a);
			volatile int dummy = sum;
		});

		TIME_BLOCK("cs::basic_var<16> access Small", {
			int sum = 0;
			for (auto &v : var32_vec)
				sum += v.const_val<Small>();
			volatile int dummy = sum;
		});

		TIME_BLOCK("cs::basic_var<48> access Small", {
			int sum = 0;
			for (auto &v : var64_vec)
				sum += v.const_val<Small>();
			volatile int dummy = sum;
		});
	}

	{
		std::vector<std::any> any_vec;
		std::vector<cs::basic_var<16>> var32_vec;
		std::vector<cs::basic_var<48>> var64_vec;
		any_vec.reserve(N);
		var32_vec.reserve(N);
		var64_vec.reserve(N);
		for (size_t i = 0; i < N; ++i) {
			any_vec.emplace_back(Small{i});
			var32_vec.emplace_back(Small{i});
			var64_vec.emplace_back(Small{i});
		}

		TIME_BLOCK("std::any copy Small", {
			std::vector<std::any> vec2 = any_vec;
		});

		TIME_BLOCK("cs::basic_var<16> copy Small", {
			std::vector<cs::basic_var<16>> vec2 = var32_vec;
		});

		TIME_BLOCK("cs::basic_var<48> copy Small", {
			std::vector<cs::basic_var<48>> vec2 = var64_vec;
		});
	}

	{
		std::vector<std::any> any_vec;
		std::vector<cs::basic_var<16>> var32_vec;
		std::vector<cs::basic_var<48>> var64_vec;
		any_vec.reserve(N);
		var32_vec.reserve(N);
		var64_vec.reserve(N);
		for (size_t i = 0; i < N; ++i) {
			any_vec.emplace_back(Large{});
			var32_vec.emplace_back(Large{});
			var64_vec.emplace_back(Large{});
		}

		TIME_BLOCK("std::any copy Large", {
			std::vector<std::any> vec2 = any_vec;
		});

		TIME_BLOCK("cs::basic_var<16> copy Large", {
			std::vector<cs::basic_var<16>> vec2 = var32_vec;
		});

		TIME_BLOCK("cs::basic_var<48> copy Large", {
			std::vector<cs::basic_var<48>> vec2 = var64_vec;
		});
	}

	return 0;
}
