// Hashmap_Benchmarks.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#pragma comment(lib, "benchmark.lib")
#pragma comment(lib, "Shlwapi.lib")
#ifdef _DEBUG
	#pragma comment(lib, "RobinHood_Hashmap_Debug.lib")
#else
	#pragma comment(lib, "RobinHood_Hashmap.lib")
#endif

#include <unordered_set>
#include <unordered_map>
#include <ranges>
#include <iostream>
#include <chrono>
#include <numeric>
#include <cassert>
#include <fstream>
#include <filesystem>
#include <print>
#include <variant>
// #include "benchmark/benchmark.h"
#include "RobinHood_Hashmap.h"
#include "sfc64.h"


static constexpr uint64_t SEED	= 19990827;
static constexpr uint64_t SEED2 = 2020024357;


static constexpr auto it_amount = 1'000'000;

auto results = std::vector<uint64_t>();

void clear_file(std::string path)
{
	std::ofstream ofile(path);
	if (ofile.is_open())
	{
		ofile << "";
		ofile.close();
	}
}

void append_to_file(std::string path, std::string str)
{
	std::ofstream ofile(path, std::ios_base::app);
	if (ofile.is_open())
	{
		ofile << str;
		ofile.close();
	}
}

// 1. init
// 2. copy
// 3. remove all
// 4. insert/remove (7 : 3)
// 5. insert/remove (3 : 7)
// 6. find (contain 100)
// 7. find (false 100)
//
auto run_benchmark(auto benchmark_name, auto expected_result, auto lambda_to_benchmark, auto lambda_actual_result)
{
	std::cout << "running " << benchmark_name << std::endl;
	auto begin = std::chrono::high_resolution_clock::now();
	lambda_to_benchmark();
	auto end		   = std::chrono::high_resolution_clock::now();
	auto duration	   = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
	auto actual_result = lambda_actual_result();
	if (expected_result != actual_result)
	{
		std::println("test_failed, expected {}, actual {}", expected_result, actual_result);
	}

	std::println("duration : {}", duration);
	std::println("actual result : {}", actual_result);
	std::println("==========================");

	results.push_back(duration.count());

	append_to_file(std::string("result.txt"), std::format("{} : | total_duration : {}\n", benchmark_name, duration));
	return duration;
}

auto generate_unique_set(auto dist, auto gen, size_t size)
{
	auto set = std::unordered_set<uint64_t>();
	while (set.size() < size)
	{
		auto rand = dist(gen);
		if (set.contains(rand) == false)
		{
			set.insert(rand);
		}
	}

	return set;
}

auto generate_false_key_set(auto dist, auto gen, size_t size, auto& ref_set)
{
	auto set = std::unordered_set<uint64_t>();
	while (set.size() < size)
	{
		auto rand = dist(gen);
		if (ref_set.contains(rand) == false)
		{
			set.insert(rand);
		}
	}

	return set;
}

void run_benchmark_insert_100M(auto& map)
{
	auto rng = sfc64(SEED);
	run_benchmark(
		std::format("insert and access 100M"), 100'000'000, [&]() {
			for (int i = 0; i < 100'000'000; ++i)
			{
				map[rng()];
				//map[i];
			} },
		[&]() {
			return map.size();
		});
}

void run_benchmark_remove_100M(auto& map)
{
	auto rng = sfc64(SEED);
	run_benchmark(
		std::format("remove 100M"), 0, [&]() {
			for (int i = 0; i < 100'000'000; ++i)
			{
				map.erase(rng());
			} },
		[&]() {
			return map.size();
		});
}

void run_benchmark_clear_100M(auto& map)
{
	run_benchmark(
		std::format("clear 100M"), 0, [&]() { map.clear(); },
		[&]() {
			return map.size();
		});
}

void run_benchmark_std_find_T100(int element, int attempts, uint64_t expected)
{
	auto num_found = 0;
	auto std_map   = std::unordered_map<uint64_t, uint64_t>();
	auto rng	   = sfc64(SEED);
	auto rng2	   = sfc64(SEED2);
	for (int i = 0; i < element; ++i)
	{
		std_map[rng()] = rng2();
	}

	run_benchmark(
		std::format("stl find {} (T 100% F 0%) ({} attempts)", element, attempts), expected, [&]() {
			auto it_num = attempts / element;
			for (auto i = 0; i < it_num; ++i) {
				rng = sfc64(SEED);
				for (auto i = 0; i < element; ++i)
				{
					auto it = std_map.find(rng());
					if (it != std_map.end()) {
						num_found += it->second;
					}
				}
			} },
		[&]() {
			return num_found;
		});
}

void run_benchmark_std_find_T75(int element, int attempts, uint64_t expected)
{
	auto num_found = 0;
	auto std_map   = std::unordered_map<uint64_t, uint64_t>();
	auto rng	   = sfc64(SEED);
	auto rng2	   = sfc64(SEED2);
	for (int i = 0; i < element; ++i)
	{
		std_map[rng()] = rng2();
	}

	run_benchmark(
		std::format("stl find {} (T 75% F 25%) ({} attempts)", element, attempts), expected, [&]() {
			auto it_num = attempts / element;
			auto it_num_2 = element >> 2;
			for (auto i = 0; i < it_num; ++i) 
			{
				rng	 = sfc64(SEED);
				rng2 = sfc64(SEED2);
				for (auto i = 0; i < it_num_2; ++i)
				{
					auto it_0 = std_map.find(rng());
					if (it_0 != std_map.end()) 
					{
						num_found += it_0->second;
					}

					auto it_1 = std_map.find(rng());
					if (it_1 != std_map.end())
					{
						num_found += it_1->second;
					}

					auto it_2 = std_map.find(rng());
					if (it_2 != std_map.end())
					{
						num_found += it_2->second;
					}

					auto it_3 = std_map.find(rng2());
					if (it_3 != std_map.end())
					{
						num_found += it_3->second;
					}
				}
			} },
		[&]() {
			return num_found;
		});
}

void run_benchmark_std_find_T50(int element, int attempts, uint64_t expected)
{
	auto num_found = 0;
	auto std_map   = std::unordered_map<uint64_t, uint64_t>();
	auto rng	   = sfc64(SEED);
	auto rng2	   = sfc64(SEED2);
	for (int i = 0; i < element; ++i)
	{
		std_map[rng()] = rng2();
	}

	run_benchmark(
		std::format("stl find {} (T 50% F 50%) ({} attempts)", element, attempts), expected, [&]() {
			auto it_num = attempts / element;
			auto it_num_2 = element >> 2;
			for (auto i = 0; i < it_num; ++i) 
			{
				rng	 = sfc64(SEED);
				rng2 = sfc64(SEED2);
				for (auto i = 0; i < it_num_2; ++i)
				{
					auto it_0 = std_map.find(rng());
					if (it_0 != std_map.end()) 
					{
						num_found += it_0->second;
					}

					auto it_1 = std_map.find(rng());
					if (it_1 != std_map.end())
					{
						num_found += it_1->second;
					}

					auto it_2 = std_map.find(rng2());
					if (it_2 != std_map.end())
					{
						num_found += it_2->second;
					}

					auto it_3 = std_map.find(rng2());
					if (it_3 != std_map.end())
					{
						num_found += it_3->second;
					}
				}
			} },
		[&]() {
			return num_found;
		});
}

void run_benchmark_std_find_T25(int element, int attempts, uint64_t expected)
{
	auto num_found = 0;
	auto std_map   = std::unordered_map<uint64_t, uint64_t>();
	auto rng	   = sfc64(SEED);
	auto rng2	   = sfc64(SEED2);
	for (int i = 0; i < element; ++i)
	{
		std_map[rng()] = rng2();
	}

	run_benchmark(
		std::format("stl find {} (T 25% F 75%) ({} attempts)", element, attempts), expected, [&]() {
			auto it_num = attempts / element;
			auto it_num_2 = element >> 2;
			for (auto i = 0; i < it_num; ++i) 
			{
				rng	 = sfc64(SEED);
				rng2 = sfc64(SEED2);
				for (auto i = 0; i < it_num_2; ++i)
				{
					auto it_0 = std_map.find(rng());
					if (it_0 != std_map.end()) 
					{
						num_found += it_0->second;
					}

					auto it_1 = std_map.find(rng2());
					if (it_1 != std_map.end())
					{
						num_found += it_1->second;
					}

					auto it_2 = std_map.find(rng2());
					if (it_2 != std_map.end())
					{
						num_found += it_2->second;
					}

					auto it_3 = std_map.find(rng2());
					if (it_3 != std_map.end())
					{
						num_found += it_3->second;
					}
				}
			} },
		[&]() {
			return num_found;
		});
}

void run_benchmark_std_find_T0(int element, int attempts, uint64_t expected)
{
	auto num_found = 0;
	auto std_map   = std::unordered_map<uint64_t, uint64_t>();
	auto rng	   = sfc64(SEED);
	auto rng2	   = sfc64(SEED2);
	for (int i = 0; i < element; ++i)
	{
		std_map[rng()] = rng2();
	}

	run_benchmark(
		std::format("stl find {} (T 0% F 100%) ({} attempts)", element, attempts), expected, [&]() {
			auto it_num = attempts / element;
			for (auto i = 0; i < it_num; ++i) {
				rng	 = sfc64(SEED);
				for (auto i = 0; i < element; ++i)
				{
					auto it = std_map.find(rng2());
					if (it != std_map.end()) {
						num_found += it->second;
					}
				}
			} },
		[&]() {
			return num_found;
		});
}

void run_benchmark_rbh_find_T100(int element, int attempts, uint64_t expected)
{
	auto num_found = 0;
	auto rbh_map   = robin_hood_hashmap();
	auto rng	   = sfc64(SEED);
	auto rng2	   = sfc64(SEED2);
	for (int i = 0; i < element; ++i)
	{
		rbh_map[rng()] = rng2();
	}

	run_benchmark(
		std::format("rbh find {} (T 100% F 0%) ({} attempts)", element, attempts), expected, [&]() {
			auto it_num = attempts / element;
			for (auto i = 0; i < it_num; ++i) {
				rng	 = sfc64(SEED);
				for (auto i = 0; i < element; ++i)
				{
					auto res = rbh_map.find(rng());
					if (res != nullptr) {
						num_found += *res;
					}
				}
			} },
		[&]() {
			return num_found;
		});
}

void run_benchmark_rbh_find_T75(int element, int attempts, uint64_t expected)
{
	auto num_found = 0;
	auto rbh_map   = robin_hood_hashmap();
	auto rng	   = sfc64(SEED);
	auto rng2	   = sfc64(SEED2);
	for (int i = 0; i < element; ++i)
	{
		rbh_map[rng()] = rng2();
	}

	run_benchmark(
		std::format("rbh find {} (T 75% F 25%) ({} attempts)", element, attempts), expected, [&]() {
			auto it_num = attempts / element;
			auto it_num_2 = element >> 2;
			for (auto i = 0; i < it_num; ++i) 
			{
				rng	 = sfc64(SEED);
				rng2 = sfc64(SEED2);
				for (auto i = 0; i < it_num_2; ++i)
				{
					auto it_0 = rbh_map.find(rng());
					if (it_0 != nullptr) 
					{
						num_found += *it_0;
					}

					auto it_1 = rbh_map.find(rng());
					if (it_1 != nullptr)
					{
						num_found += *it_1;
					}

					auto it_2 = rbh_map.find(rng());
					if (it_2 != nullptr)
					{
						num_found += *it_2;
					}

					auto it_3 = rbh_map.find(rng2());
					if (it_3 != nullptr)
					{
						num_found += *it_3;
					}
				}
			} },
		[&]() {
			return num_found;
		});
}

void run_benchmark_rbh_find_T50(int element, int attempts, uint64_t expected)
{
	auto num_found = 0;
	auto rbh_map   = robin_hood_hashmap();
	auto rng	   = sfc64(SEED);
	auto rng2	   = sfc64(SEED2);
	for (int i = 0; i < element; ++i)
	{
		rbh_map[rng()] = rng2();
	}

	run_benchmark(
		std::format("rbh find {} (T 50% F 50%) ({} attempts)", element, attempts), expected, [&]() {
			auto it_num = attempts / element;
			auto it_num_2 = element >> 2;
			for (auto i = 0; i < it_num; ++i) 
			{
				rng	 = sfc64(SEED);
				rng2 = sfc64(SEED2);
				for (auto i = 0; i < it_num_2; ++i)
				{
					auto it_0 = rbh_map.find(rng());
					if (it_0 != nullptr) 
					{
						num_found += *it_0;
					}

					auto it_1 = rbh_map.find(rng());
					if (it_1 != nullptr)
					{
						num_found += *it_1;
					}

					auto it_2 = rbh_map.find(rng2());
					if (it_2 != nullptr)
					{
						num_found += *it_2;
					}

					auto it_3 = rbh_map.find(rng2());
					if (it_3 != nullptr)
					{
						num_found += *it_3;
					}
				}
			} },
		[&]() {
			return num_found;
		});
}

void run_benchmark_rbh_find_T25(int element, int attempts, uint64_t expected)
{
	auto num_found = 0;
	auto rbh_map   = robin_hood_hashmap();
	auto rng	   = sfc64(SEED);
	auto rng2	   = sfc64(SEED2);
	for (int i = 0; i < element; ++i)
	{
		rbh_map[rng()] = rng2();
	}

	run_benchmark(
		std::format("rbh find {} (T 25% F 75%) ({} attempts)", element, attempts), expected, [&]() {
			auto it_num = attempts / element;
			auto it_num_2 = element >> 2;
			for (auto i = 0; i < it_num; ++i) 
			{
				rng	 = sfc64(SEED);
				rng2 = sfc64(SEED2);
				for (auto i = 0; i < it_num_2; ++i)
				{
					auto it_0 = rbh_map.find(rng());
					if (it_0 != nullptr) 
					{
						num_found += *it_0;
					}

					auto it_1 = rbh_map.find(rng2());
					if (it_1 != nullptr)
					{
						num_found += *it_1;
					}

					auto it_2 = rbh_map.find(rng2());
					if (it_2 != nullptr)
					{
						num_found += *it_2;
					}

					auto it_3 = rbh_map.find(rng2());
					if (it_3 != nullptr)
					{
						num_found += *it_3;
					}
				}
			} },
		[&]() {
			return num_found;
		});
}

void run_benchmark_rbh_find_T0(int element, int attempts, uint64_t expected)
{
	auto num_found = 0;
	auto rbh_map   = robin_hood_hashmap();
	auto rng	   = sfc64(SEED);
	auto rng2	   = sfc64(SEED2);
	for (int i = 0; i < element; ++i)
	{
		rbh_map[rng()] = rng2();
	}

	run_benchmark(
		std::format("rbh find {} (T 0% F 100%) ({} attempts)", element, attempts), expected, [&]() {
			auto it_num = attempts / element;
			for (auto i = 0; i < it_num; ++i) {
				rng	 = sfc64(SEED);
				for (auto i = 0; i < element; ++i)
				{
					auto it = rbh_map.find(rng2());
					if (it != nullptr) {
						num_found += *it;
					}
				}
			} },
		[&]() {
			return num_found;
		});
}

#define RUN_100M

#define RUN_FIND

void write_csv()
{
	auto csv_file_name = "result.txt";
	auto idx		   = 0;
	clear_file(csv_file_name);


#ifdef RUN_100M
	// insert_100M
	append_to_file(csv_file_name, "stl");
	append_to_file(csv_file_name, std::format(",{}\n", results[idx]));
	append_to_file(csv_file_name, "rbh");
	append_to_file(csv_file_name, std::format(",{}\n", results[idx + 3]));
	// remove_100M
	append_to_file(csv_file_name, "stl");
	append_to_file(csv_file_name, std::format(",{}\n", results[idx + 1]));
	append_to_file(csv_file_name, "rbh");
	append_to_file(csv_file_name, std::format(",{}\n", results[idx + 4]));
	// clear_100M
	append_to_file(csv_file_name, "stl");
	append_to_file(csv_file_name, std::format(",{}\n", results[idx + 2]));
	append_to_file(csv_file_name, "rbh");
	append_to_file(csv_file_name, std::format(",{}\n", results[idx + 5]));

	idx += 6;
#endif

#ifdef RUN_FIND
	for (auto i = 0; i < 5; ++i)
	{
		append_to_file(csv_file_name, ",100,1000,10000,100000,1000000\n");
		append_to_file(csv_file_name, "stl");

		append_to_file(csv_file_name, std::format(",{}", results[idx]));
		append_to_file(csv_file_name, std::format(",{}", results[idx + 10]));
		append_to_file(csv_file_name, std::format(",{}", results[idx + 20]));
		append_to_file(csv_file_name, std::format(",{}", results[idx + 30]));
		append_to_file(csv_file_name, std::format(",{}\n", results[idx + 40]));

		idx += 1;
		append_to_file(csv_file_name, "rbh");
		append_to_file(csv_file_name, std::format(",{}", results[idx]));
		append_to_file(csv_file_name, std::format(",{}", results[idx + 5]));
		append_to_file(csv_file_name, std::format(",{}", results[idx + 10]));
		append_to_file(csv_file_name, std::format(",{}", results[idx + 15]));
		append_to_file(csv_file_name, std::format(",{}\n", results[idx + 20]));

		idx += 1;
	}
#endif
}

int main(int argc, char** argv)
{
	clear_file("result.txt");


#ifdef RUN_100M
	{
		auto std_map = std::unordered_map<uint64_t, uint64_t>();
		run_benchmark_insert_100M(std_map);
		{
			auto std_copied_map = std_map;
			run_benchmark_remove_100M(std_copied_map);
		}

		run_benchmark_clear_100M(std_map);
	}
	{
		auto robin_hood_map = robin_hood_hashmap();
		run_benchmark_insert_100M(robin_hood_map);
		{
			auto rbh_copied_map = robin_hood_map;
			run_benchmark_remove_100M(rbh_copied_map);
		}

		run_benchmark_clear_100M(robin_hood_map);
	}

#endif


// find
#ifdef RUN_FIND
	auto arr = std::initializer_list {
		std::tuple { 100, 500'000'000, std::tuple { (uint64_t)(-823732416), (uint64_t)(-2007611776), (uint64_t)(-1800003584), 491358464ull, 0ull } },
		std::tuple { 1'000, 500'000'000, std::tuple { (uint64_t)(-684335648), (uint64_t)(-1702198016), (uint64_t)(-279168224), 1358626944ull, 0ull } },
		std::tuple { 10'000, 500'000'000, std::tuple { (uint64_t)(-1988223392), (uint64_t)(-1609478832), (uint64_t)(-1417668720), 285206672ull, 0ull } },
		std::tuple { 100'000, 500'000'000, std::tuple { (uint64_t)(-3083696), (uint64_t)(1880945216), (uint64_t)(148945576), (uint64_t)(-56941704), 0ull } },
		std::tuple { 1'000'000, 500'000'000, std::tuple { (uint64_t)(659887280), (uint64_t)(-186988160), (uint64_t)(446733096), (uint64_t)(-910845912), 0ull } }
	};

	for (auto [element, attempts, expected_tpl] : arr)
	{
		run_benchmark_std_find_T100(element, attempts, std::get<0>(expected_tpl));
		run_benchmark_rbh_find_T100(element, attempts, std::get<0>(expected_tpl));

		run_benchmark_std_find_T75(element, attempts, std::get<1>(expected_tpl));
		run_benchmark_rbh_find_T75(element, attempts, std::get<1>(expected_tpl));

		run_benchmark_std_find_T50(element, attempts, std::get<2>(expected_tpl));
		run_benchmark_rbh_find_T50(element, attempts, std::get<2>(expected_tpl));

		run_benchmark_std_find_T25(element, attempts, std::get<3>(expected_tpl));
		run_benchmark_rbh_find_T25(element, attempts, std::get<3>(expected_tpl));

		run_benchmark_std_find_T0(element, attempts, std::get<4>(expected_tpl));
		run_benchmark_rbh_find_T0(element, attempts, std::get<4>(expected_tpl));
	}
#endif

	write_csv();
	return 0;
}

int main(int, char**);