// Hashmap_Benchmarks.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// #pragma comment(lib, "benchmark.lib")
#pragma comment(lib, "Shlwapi.lib")
#ifdef _DEBUG
	#pragma comment(lib, "RobinHood_Hashmap_Debug.lib")
#else
	#pragma comment(lib, "RobinHood_Hashmap.lib")
#endif

#include <unordered_map>
#include <random>
#include <ranges>
#include <iostream>
#include <chrono>
#include <numeric>
#include <cassert>
#include <fstream>
#include <filesystem>
// #include "benchmark/benchmark.h"
#include "RobinHood_Hashmap.h"


static constexpr uint64_t SEED = 19990827;
//
// bool undeducible_false()
//{
//	// It took mathematicians more than 3 centuries to prove Fermat's
//	// last theorem in its most general form. Hardly that knowledge
//	// has been put into compilers (or the compiler will try hard
//	// enough to check all one million possible combinations below).
//
//	// Caveat: avoid integer overflow (Fermat's theorem
//	//         doesn't hold for modulo arithmetic)
//	std::uint32_t a = std::clock() % 100 + 1;
//	std::uint32_t b = std::rand() % 100 + 1;
//	std::uint32_t c = reinterpret_cast<std::uintptr_t>(&a) % 100 + 1;
//
//	return a * a * a + b * b * b == c * c * c;
// }
//
// static bool always_false = undeducible_false();
//// volatile char* nextLocationToClobber();
// volatile char* clobberingPtr;
//
//// void clobber()
////{
////	if (always_false)
////	{
////		// This will never be executed, but the compiler
////		// cannot know about it.
////		clobberingPtr  = nextLocationToClobber();
////		*clobberingPtr = *clobberingPtr;
////	}
//// }
//
// #ifdef _MSC_VER
//	#pragma optimize("", off)
//
// template <typename T>
// inline void escape(T* p)
//{
//	*reinterpret_cast<volatile char*>(p) = *reinterpret_cast<const volatile char*>(p);	  // thanks, @milleniumbug
//}
//
//	#pragma optimize("", on)
// #endif
//
// static uint64_t check_sum = 0;
//
// static void BASE_100(benchmark::State& state)
//{
//	// Perform setup here
//	std::mt19937							mersenne(SEED);
//	std::uniform_int_distribution<uint64_t> dis(0, 0xffff'ffff'ffff'ffff);
//	std::unordered_map<uint64_t, uint64_t>	map;
//
//	for (auto i : std::views::iota(0, 10000))
//	{
//		auto rand = dis(mersenne);
//		map[rand] = rand;
//	}
//
//	for (auto _ : state)
//	{
//
//
//		// This code gets timed
//
//		check_sum += dis(mersenne);
//		// escape(&rand);
//	}
//}
//
// static void stl_find_100(benchmark::State& state)
//{
//	// Perform setup here
//	std::mt19937							mersenne(SEED);
//	std::uniform_int_distribution<uint64_t> dis(0, 0xffff'ffff'ffff'ffff);
//	std::unordered_map<uint64_t, uint64_t>	map;
//
//	for (auto i : std::views::iota(0, 10000))
//	{
//		auto rand = dis(mersenne);
//		map[rand] = rand;
//	}
//
//	for (auto _ : state)
//	{
//
//
//		// This code gets timed
//
//
//		check_sum += map[dis(mersenne)];
//		// escape(&rand);
//	}
//}
//
// static void BASE_ROBIN_HOOD_100(benchmark::State& state)
//{
//	// Perform setup here
//
//	std::mt19937							mersenne(SEED);
//	std::uniform_int_distribution<uint64_t> dis(0, 0xffff'ffff'ffff'ffff);
//	robin_hood_hashmap						map;
//
//	for (auto i : std::views::iota(0, 10000))
//	{
//		auto rand = dis(mersenne);
//
//		map[rand] = rand;
//	}
//	for (auto _ : state)
//	{
//		auto find_key = dis(mersenne);
//		benchmark::DoNotOptimize(map._buckets);
//		map[find_key] = find_key;
//		benchmark::ClobberMemory();
//	}
//}
//
// static void BM_STL_UNORDERED_MAP_FIND_100(benchmark::State& state)
//{
//	// Perform setup here
//	std::mt19937							mersenne(SEED);
//	std::uniform_int_distribution<uint64_t> dis(0, 0xffff'ffff'ffff'ffff);
//	std::unordered_map<uint64_t, uint64_t>	map;
//
//	for (auto i : std::views::iota(0, 100))
//	{
//		auto rand = dis(mersenne);
//		map[rand] = rand;
//	}
//
//	for (auto _ : state)
//	{
//		// This code gets timed
//		auto		  rand = dis(mersenne);
//		volatile auto res  = map.find(rand);
//	}
//}
//
// static void BM_STL_UNORDERED_MAP_FIND_NO_OPT_100(benchmark::State& state)
//{
//	// Perform setup here
//
//
//	for (auto _ : state)
//	{
//		std::mt19937							mersenne(SEED);
//		std::uniform_int_distribution<uint64_t> dis(0, 0xffff'ffff'ffff'ffff);
//		std::unordered_map<uint64_t, uint64_t>	map;
//
//		for (auto i : std::views::iota(0, 10000))
//		{
//			auto rand = dis(mersenne);
//			map[rand] = rand;
//		}
//
//		// This code gets timed
//
//		// benchmark::DoNotOptimize(&res);
//		check_sum += map.find(dis(mersenne))->first;
//
//		// benchmark::ClobberMemory();
//	}
//}
//
// static void BM_vector_push_back(benchmark::State& state)
//{
//	for (auto _ : state)
//	{
//		std::vector<int> v;
//		v.reserve(1);
//		auto data = v.data();			   // Allow v.data() to be clobbered. Pass as non-const
//		benchmark::DoNotOptimize(data);	   // lvalue to avoid undesired compiler optimizations
//		v.push_back(42);
//		benchmark::ClobberMemory();		   // Force 42 to be written to memory.
//	}
//}
//
// static void BM_vector_base(benchmark::State& state)
//{
//	for (auto _ : state)
//	{
//		std::vector<int> v;
//		v.reserve(1);
//		auto data = v.data();			   // Allow v.data() to be clobbered. Pass as non-const
//		benchmark::DoNotOptimize(data);	   // lvalue to avoid undesired compiler optimizations
//		benchmark::ClobberMemory();		   // Force 42 to be written to memory.
//	}
//}

// Register the function as a benchmark
// BENCHMARK(BASE_100);
// BENCHMARK(BM_STL_UNORDERED_MAP_FIND_100);
// BENCHMARK(BM_STL_UNORDERED_MAP_FIND_NO_OPT_100);
// BENCHMARK(BM_vector_base);
// BENCHMARK(BM_vector_push_back);
// BENCHMARK(BASE_100);
// BENCHMARK(stl_find_100);
// BENCHMARK(BASE_ROBIN_HOOD_100);

// BENCHMARK(BM_STL_UNORDERED_MAP_INIT_100);
//  Run the benchmark
// BENCHMARK_MAIN();

static constexpr auto it_amount = 1'000'000;

//{element_count, it_amount} => first * seconde = 10'000'000;

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
auto run_benchmark(auto benchmark_name, auto it_num, auto lambda)
{
	std::cout << "running " << benchmark_name << std::endl;
	auto begin = std::chrono::high_resolution_clock::now();
	lambda();
	auto end	  = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
	std::cout << "duration : " << duration << std::endl;
	std::cout << "mean : " << duration / it_num << std::endl;
	std::cout << "==========================" << std::endl;


	append_to_file(std::string("result.txt"), std::format("{} : | total_duration : {}, iteration num : {}, mean : {} \n", benchmark_name, duration, it_num, duration / (float)it_num));
	return duration;
}

void temp_stl_unordered_map_find(int count)
{
	auto exec_count		= 0ull;
	auto sum_for_no_opt = 0ul;
	auto total_duration = std::chrono::nanoseconds(0);

	std::mt19937							mersenne(SEED);
	std::uniform_int_distribution<uint64_t> dis(0, 0xffff'ffff'ffff'ffff);
	std::unordered_map<uint64_t, uint64_t>	map;


	for (int i : std::views::iota(1, count))
	{
		auto rand = dis(mersenne);
		map[rand] = rand;
	}


	for (int i : std::views::iota(1, it_amount))
	{
		auto rand = dis(mersenne);
		if (map.contains(rand))
		{
			// prevent insert
			auto start		= std::chrono::system_clock::now();
			sum_for_no_opt += map[dis(mersenne)];
			auto end		= std::chrono::system_clock::now();
			total_duration += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
			++exec_count;
		}
	}

	auto mean_time = total_duration / exec_count;

	std::cout << "it_count : " << count << std::endl;
	std::cout << "elapsed_time : " << total_duration.count() << "ns, mean_time : " << mean_time.count() << "ns" << std::endl;


	if (sum_for_no_opt == map[0])	 // unlikely
	{
		std::cout << sum_for_no_opt << std::endl;
	}
	else
	{
	}
}

// todo -> 빼기를 어떻게?
void temp_stl_unordered_map_insert_extract(int iteration)
{
	auto exec_count		= 0;
	auto sum_for_no_opt = 0ul;
	auto total_duration = std::chrono::nanoseconds(0);

	std::mt19937							mersenne(SEED);
	std::uniform_int_distribution<uint64_t> dis(0, 0xffff'ffff'ffff'ffff);
	std::unordered_map<uint64_t, uint64_t>	map;
	for (int i : std::views::iota(1, iteration))
	{
		auto rand = dis(mersenne);
		map[rand] = rand;
	}

	for (int i : std::views::iota(1, it_amount))
	{
		auto rand = dis(mersenne);
		if (map.contains(rand) == false)
		{
			map[rand] = rand;
		}

		if (rand % 2 == 0)	  // insert
		{
			map.extract(map.at(rand));

			auto start		= std::chrono::system_clock::now();
			sum_for_no_opt += map[rand];
			auto end		= std::chrono::system_clock::now();

			total_duration += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
		}
		else	// extract
		{
			auto start		= std::chrono::system_clock::now();
			sum_for_no_opt += map.extract(rand).key();
			auto end		= std::chrono::system_clock::now();

			total_duration += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
		}
		++exec_count;
	}

	auto mean_time = total_duration / exec_count;

	std::cout << "it_count : " << iteration << std::endl;
	std::cout << "elapsed_time : " << total_duration.count() << "ns, mean_time : " << mean_time.count() << "ns" << std::endl;


	if (sum_for_no_opt == map[0])	 // unlikely
	{
		std::cout << sum_for_no_opt << std::endl;
	}
	else
	{
	}
}

void temp_stl_robin_hood_hash_map_find(int count)
{
	auto exec_count		= 0;
	auto sum_for_no_opt = 0ul;
	auto total_duration = std::chrono::nanoseconds(0);

	std::mt19937							mersenne(19990827);
	std::uniform_int_distribution<uint64_t> dis(0, 0xffff'ffff'ffff'ffff);
	robin_hood_hashmap						map;


	for (int i : std::views::iota(1, count))
	{
		auto rand = dis(mersenne);
		map[rand] = rand;
	}


	for (int i : std::views::iota(1, it_amount))
	{
		auto rand = dis(mersenne);
		if (map.find(rand) != nullptr)
		{
			// prevent insert
			auto start		= std::chrono::system_clock::now();
			sum_for_no_opt += map[dis(mersenne)];
			auto end		= std::chrono::system_clock::now();
			total_duration += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
			++exec_count;
		}
	}

	auto mean_time = total_duration / exec_count;

	std::cout << "it_count : " << count << std::endl;
	std::cout << "elapsed_time : " << total_duration.count() << "ns, mean_time : " << mean_time.count() << "ns" << std::endl;


	if (sum_for_no_opt == map[0])	 // unlikely
	{
		std::cout << sum_for_no_opt << std::endl;
	}
	else
	{
	}
}

void temp_stl_robin_hood_hash_map_insert_extract(int iteration)
{
	auto exec_count		= 0;
	auto sum_for_no_opt = 0ul;
	auto total_duration = std::chrono::nanoseconds(0);

	std::mt19937					   mersenne(SEED);
	std::uniform_int_distribution<int> dis(0, iteration);
	robin_hood_hashmap				   map;
	for (int i : std::views::iota(1, iteration))
	{
		auto rand = dis(mersenne);
		map[rand] = rand;
	}

	for (int i : std::views::iota(1, it_amount))
	{
		auto rand = dis(mersenne);
		if (map.find(rand) == nullptr)
		{
			map[rand] = rand;
		}

		if (rand % 2 == 0)	  // insert
		{
			map.erase(rand);

			auto start		= std::chrono::system_clock::now();
			sum_for_no_opt += map[rand];
			auto end		= std::chrono::system_clock::now();

			total_duration += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
		}
		else									  // extract
		{
			auto start		= std::chrono::system_clock::now();
			sum_for_no_opt += map.erase(rand);	  // map.extract(rand).key();
			auto end		= std::chrono::system_clock::now();

			total_duration += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
		}
		++exec_count;
	}

	auto mean_time = total_duration / exec_count;

	std::cout << "it_count : " << iteration << std::endl;
	std::cout << "elapsed_time : " << total_duration.count() << "ns, mean_time : " << mean_time.count() << "ns" << std::endl;


	if (sum_for_no_opt == map[0])	 // unlikely
	{
		std::cout << sum_for_no_opt << std::endl;
	}
	else
	{
	}
}

void run_std_unordered_map_benchmark()
{
	// 1. init
	// 2. copy
	// 3. remove all
	// 4. insert/remove (7 : 3)
	// 5. insert/remove (3 : 7)
	// 6. find (contain 100)
	// 7. find (false 100)
	auto pair_arr = std::initializer_list<std::pair<int, int>> { { 100, 100'000 }, { 1'000, 10'000 }, { 10'000, 1'000 }, { 100'000, 100 }, { 1'000'000, 10 } };
	// auto pair_arr = std::initializer_list<std::pair<int, int>> { { 100, 1 }, { 1'000, 1 }, { 10'000, 1 }, { 100'000, 1 }, { 1'000'000, 1 } };

	auto seed = 19990827;
	auto gen  = std::mt19937(seed);
	auto dist = std::uniform_int_distribution<uint64_t>(0, 0xffff'ffff'ffff'ffff);

	std::ranges::for_each(pair_arr, [&](const auto& p) {
		auto test_pass					 = true;
		const auto& [elem_count, it_num] = p;
		std::unordered_map<uint64_t, uint64_t> map;
		std::cout << std::format("benchmark start, element count : {}, iteration amount : {}", elem_count, it_num) << std::endl;
		// init
		run_benchmark(std::format("init unordered_map"), it_num, [&]() {
			std::ranges::for_each(std::views::iota(0, it_num), [&](auto _) {
				map = std::unordered_map<uint64_t, uint64_t>();
				while (map.size() < elem_count)
				{
					auto key = dist(gen);
					if (map.contains(key) == false)
					{
						map[key] = key;
					}
				}
			});
		});
		// copy
		auto key_vec = std::ranges::to<std::vector>(map | std::ranges::views::keys);
		// auto value_sum = std::accumulate(key_vec.begin(), key_vec.end(), 0ull);
		auto sum = (size_t)0;

		auto duration = run_benchmark(
			std::format("remove all element"), it_num * map.size(), [&, map]() {
				std::ranges::for_each(std::views::iota(0, it_num), [&, map](auto _) {
					std::ranges::for_each(key_vec, [&, map](auto k) mutable {
						sum += map.erase(k);
					});
				});
			});

		test_pass &= sum == map.size() * it_num;

		auto value_sum = (size_t)0;

		sum = (size_t)0;
		run_benchmark(std::format("find true"), it_num * map.size(), [&, map]() {
			std::ranges::for_each(std::views::iota(0, it_num), [&, map](auto _) {
				std::ranges::for_each(key_vec, [&, map](auto k) mutable {
					auto idx	= dist(gen) % key_vec.size();
					auto key	= key_vec[idx];
					auto value	= map.find(key);
					sum		   += value->second;
					value_sum  += key;
				});
			});
		});
		test_pass &= sum == value_sum;


		std::cout << "init_false_key_vec" << std::endl;
		auto false_key_vec = std::vector<uint64_t>();
		while (false_key_vec.size() < it_num * map.size())
		{
			auto rand = dist(gen);
			if (map.contains(rand) == false)
			{
				false_key_vec.push_back(rand);
			}
		}
		std::cout << "done" << std::endl;

		sum = (size_t)0;
		run_benchmark(std::format("find false"), it_num * map.size(), [&, map]() {
			std::ranges::for_each(false_key_vec, [&, map](auto k) mutable {
				auto it = map.find(k);
				if (it == map.end())
				{
					++sum;
				}
			});
		});

		test_pass &= sum == false_key_vec.size();


		// auto insert_duration = std::chrono::nanoseconds();
		// auto erase_duration	 = std::chrono::nanoseconds();
		// auto find_duration	 = std::chrono::nanoseconds();
		// sum					 = 0;
		// value_sum			 = 0;
		// for (int i = 0; i < it_num * map.size(); ++i)
		//{
		//	auto rand = dist(gen) % 10;

		//	if (rand < 8)
		//	{
		//		auto idx	= dist(gen) % key_vec.size();
		//		auto key	= key_vec[idx];
		//		auto value	= map.find(key);
		//		sum		   += value->second;
		//		value_sum  += key;
		//	}
		//	else if (rand < 9)
		//	{
		//	}
		//	else
		//	{
		//	}
		//}

		std::cout << (test_pass ? "test_passed" : "test_failled") << std::endl;


		//  remove all
	});
}

#include <unordered_set>

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

int main(int argc, char** argv)
{
	// char  arg0_default[] = "benchmark";
	// char* args_default	 = arg0_default;
	// if (!argv)
	//{

	//	argc = 1;
	//	argv = &args_default;
	//}
	//::benchmark::Initialize(&argc, argv);
	// if (::benchmark::ReportUnrecognizedArguments(argc, argv)) return 1;
	//::benchmark::RunSpecifiedBenchmarks();
	//::benchmark::Shutdown();

	// std::cout << check_sum << std::endl;


	// std::cout << "\ntemp_stl_unordered_map find\n"
	//		  << std::endl;
	// temp_stl_unordered_map_find(100);
	// temp_stl_unordered_map_find(1000);
	// temp_stl_unordered_map_find(10'000);
	// temp_stl_unordered_map_find(100'000);
	// temp_stl_unordered_map_find(1'000'000);

	// std::cout << "\ntemp_stl_unordered_map insert_extract\n"
	//		  << std::endl;

	// temp_stl_unordered_map_insert_extract(100);
	// temp_stl_unordered_map_insert_extract(1000);
	// temp_stl_unordered_map_insert_extract(10'000);
	// temp_stl_unordered_map_insert_extract(100'000);
	// temp_stl_unordered_map_insert_extract(1'000'000);

	// std::cout << "\ntemp_stl_robin_hood_hash_map find\n"
	//		  << std::endl;
	// temp_stl_robin_hood_hash_map_find(100);
	// temp_stl_robin_hood_hash_map_find(1000);
	// temp_stl_robin_hood_hash_map_find(10'000);
	// temp_stl_robin_hood_hash_map_find(100'000);
	// temp_stl_robin_hood_hash_map_find(1'000'000);

	// std::cout << "\ntemp_stl_robin_hood_hash_map insert_extract\n"
	//		  << std::endl;

	// temp_stl_robin_hood_hash_map_insert_extract(100);
	// temp_stl_robin_hood_hash_map_insert_extract(1000);
	// temp_stl_robin_hood_hash_map_insert_extract(10'000);
	// temp_stl_robin_hood_hash_map_insert_extract(100'000);
	// temp_stl_robin_hood_hash_map_insert_extract(1'000'000);


	// run_std_unordered_map_benchmark();
	// run_robin_hood_hashmap_benchmark();
	clear_file("result.txt");
	auto seed			   = 19990827;
	auto gen			   = std::mt19937(seed);
	auto dist			   = std::uniform_int_distribution<uint64_t>(0, 0xffff'ffff'ffff'ffff);
	auto insert_erase_dist = std::uniform_int_distribution<int>(0, 1);
	auto _4_op_dist		   = std::uniform_int_distribution<int>(0, 9);

	auto std_map		  = std::unordered_map<uint64_t, uint64_t>();
	auto rh_map			  = robin_hood_hashmap();
	auto key_set		  = generate_unique_set(dist, gen, 1'000'000);
	auto key_vec		  = key_set | std::ranges::to<std::vector>();
	auto false_key_set	  = generate_false_key_set(dist, gen, 1'000'000, key_set);
	auto false_key_vec	  = false_key_set | std::ranges::to<std::vector>();
	auto insert_erase_vec = std::vector<uint8_t>();
	auto _4_op_vec		  = std::vector<uint8_t>();

	std::ranges::for_each(std::ranges::views::iota(0, 1'000'000), [&](auto _) {
		insert_erase_vec.push_back(insert_erase_dist(gen));
	});

	std::ranges::for_each(std::ranges::views::iota(0, 1'000'000), [&](auto _) {
		auto op = _4_op_dist(gen);
		if (op < 4)
		{
			_4_op_vec.push_back(0);
		}
		else if (op < 8)
		{
			_4_op_vec.push_back(1);
		}
		else if (op < 9)
		{
			_4_op_vec.push_back(2);
		}
		else
		{
			_4_op_vec.push_back(3);
		}
	});


	bool test_pass = true;

	// insert test
	auto it_arr = { 100, 1'000, 10'000, 100'000, 1'000'000 };
	std::ranges::for_each(it_arr, [&](auto it_num) {
		// init
		{
			run_benchmark(std::format("std insert test it_num : {}", it_num), it_num, [&]() {
				std::ranges::for_each(key_vec | std::views::take(it_num), [&](auto key) {
					// std_map.insert(key, key);
					// std_map[key] = key;
					std_map.insert({ key, key });
				});
			});

			run_benchmark(std::format("rh insert test it_num : {}", it_num), it_num, [&]() {
				std::ranges::for_each(key_vec | std::views::take(it_num), [&](auto key) {
					rh_map.insert(key, key);
				});
			});
		}

		// find true
		{
			auto sum_ans  = std::accumulate(key_vec.begin(), key_vec.begin() + it_num, 0ull);
			auto test_sum = 0ull;
			run_benchmark(std::format("std find true test it_num : {}", it_num), it_num, [&]() {
				std::ranges::for_each(key_vec | std::views::take(it_num), [&](auto key) {
					auto pair  = std_map.find(key);
					test_sum  += pair->second;
				});
			});
			test_pass &= test_sum == sum_ans;

			test_sum = 0ull;
			run_benchmark(std::format("rh find true test it_num : {}", it_num), it_num, [&]() {
				std::ranges::for_each(key_vec | std::views::take(it_num), [&](auto key) {
					test_sum += *rh_map.find(key);
				});
			});
			test_pass &= test_sum == sum_ans;
		}

		// find false
		{
			auto false_count = 0;
			run_benchmark(std::format("std find false test it_num : {}", it_num), it_num, [&]() {
				std::ranges::for_each(false_key_vec | std::views::take(it_num), [&](auto key) {
					auto it		 = std_map.find(key);
					false_count += it == std_map.end() ? 1 : 0;
				});
			});
			test_pass &= false_count == it_num;

			false_count = 0;
			run_benchmark(std::format("rh find false test it_num : {}", it_num), it_num, [&]() {
				std::ranges::for_each(false_key_vec | std::views::take(it_num), [&](auto key) {
					false_count += rh_map.find(key) == nullptr ? 1 : 0;
				});
			});
			test_pass &= false_count == it_num;

			assert(test_pass);
		}

		// 50% insert 50% erase
		{
			auto std_map_copied = std_map;
			auto rh_map_copied	= rh_map;
			auto insert_sum		= 0ull;
			auto erase_sum		= 0ull;
			auto insert_idx		= 0;
			auto erase_idx		= 0;
			enum op
			{
				insert,
				erase
			};

			run_benchmark(std::format("std 50% insert 50% erase it_num : {}", it_num), it_num, [&]() {
				std::ranges::for_each(insert_erase_vec | std::views::take(it_num), [&](auto op) {
					switch (op)
					{
					case insert:
					{
						auto key	= false_key_vec[insert_idx];
						auto pair	= std_map_copied.insert({ key, key });
						insert_sum += pair.second ? 1 : 0;

						++insert_idx;
						break;
					}
					case erase:
					{
						auto key   = key_vec[erase_idx];
						erase_sum += std_map_copied.erase(key);
						++erase_idx;
						break;
					}
					default:
						break;
					}
				});
			});

			test_pass &= insert_sum == insert_idx;
			test_pass &= erase_sum == erase_idx;

			insert_sum = 0ull;
			erase_sum  = 0ull;
			insert_idx = 0;
			erase_idx  = 0;

			auto erase_sum_ans = 0ull;

			run_benchmark(std::format("rh 50% insert 50% erase it_num : {}", it_num), it_num, [&]() {
				std::ranges::for_each(insert_erase_vec | std::views::take(it_num), [&](auto op) {
					switch (op)
					{
					case insert:
					{
						auto key = false_key_vec[insert_idx];
						rh_map_copied.insert(key, key);
						/*insert_sum += pair.second ? 1 : 0;*/
						++insert_sum;
						++insert_idx;
						break;
					}
					case erase:
					{
						auto key	   = key_vec[erase_idx];
						erase_sum	  += rh_map_copied.erase(key);
						erase_sum_ans += key;

						++erase_idx;
						break;
					}
					default:
						break;
					}
				});
			});

			test_pass &= insert_sum == insert_idx;
			test_pass &= erase_sum == erase_sum_ans;

			assert(test_pass);
		}

		// 10% insert 10% erase 40% true find 40% false find
		{
			auto std_map_copied = std_map;
			auto rh_map_copied	= rh_map;
			auto false_key_idx	= 0;
			auto true_key_idx	= 0;

			auto false_find_count = 0;
			auto false_find_sum	  = 0;
			auto true_find_count  = 0;
			auto true_find_sum	  = 0;
			auto insert_count	  = 0;
			auto insert_sum		  = 0;
			auto erase_count	  = 0;
			auto erase_sum		  = 0ull;
			enum op
			{
				false_find,
				true_find,
				insert,
				erase
			};

			run_benchmark(std::format("std 10% insert 10% erase 40% false find 40% true find it_num : {}", it_num), it_num, [&]() {
				std::ranges::for_each(_4_op_vec | std::views::take(it_num), [&](auto op) {
					switch (op)
					{
					case false_find:
					{
						auto key		= false_key_vec[false_key_idx];
						auto it			= std_map_copied.find(key);
						false_find_sum += it == std_map_copied.end() ? 1 : 0;

						++false_key_idx;
						++false_find_count;
						break;
					}
					case true_find:
					{
						auto key	   = key_vec[true_key_idx];
						auto it		   = std_map_copied.find(key);
						true_find_sum += it == std_map_copied.end() ? 0 : 1;

						++true_key_idx;
						++true_find_count;
						break;
					}
					case insert:
					{
						auto key	= false_key_vec[false_key_idx];
						auto pair	= std_map_copied.insert({ key, key });
						insert_sum += pair.second ? 1 : 0;

						++false_key_idx;
						++insert_count;
						break;
					}
					case erase:
					{
						auto key   = key_vec[true_key_idx];
						erase_sum += std_map_copied.erase(key);
						++true_key_idx;
						++erase_count;
						break;
					}
					default:
						break;
					}
				});
			});


			test_pass &= false_find_count == false_find_sum;
			test_pass &= true_find_count == true_find_sum;
			test_pass &= insert_count == insert_sum;
			test_pass &= erase_count == erase_sum;

			false_key_idx = 0;
			true_key_idx  = 0;

			false_find_count = 0;
			false_find_sum	 = 0;
			true_find_count	 = 0;
			true_find_sum	 = 0;
			insert_count	 = 0;
			insert_sum		 = 0;
			erase_count		 = 0;
			erase_sum		 = 0ull;

			auto erase_sum_ans = 0ull;

			run_benchmark(std::format("rh 10% insert 10% erase 40% false find 40% true find it_num : {}", it_num), it_num, [&]() {
				std::ranges::for_each(_4_op_vec | std::views::take(it_num), [&](auto op) {
					switch (op)
					{
					case false_find:
					{
						auto  key		= false_key_vec[false_key_idx];
						auto* ptr		= rh_map_copied.find(key);
						false_find_sum += ptr == nullptr ? 1 : 0;

						++false_key_idx;
						++false_find_count;
						break;
					}
					case true_find:
					{
						auto key	   = key_vec[true_key_idx];
						auto ptr	   = rh_map_copied.find(key);
						true_find_sum += ptr == nullptr ? 0 : 1;

						++true_key_idx;
						++true_find_count;
						break;
					}
					case insert:
					{
						auto key = false_key_vec[false_key_idx];
						rh_map_copied.insert(key, key);
						// insert_sum += pair.second ? 1 : 0;
						++insert_sum;
						++false_key_idx;
						++insert_count;
						break;
					}
					case erase:
					{
						auto key   = key_vec[true_key_idx];
						erase_sum += rh_map_copied.erase(key);
						++true_key_idx;
						++erase_count;
						erase_sum_ans += key;
						break;
					}
					default:
						break;
					}
				});
			});

			test_pass &= false_find_count == false_find_sum;
			test_pass &= true_find_count == true_find_sum;
			test_pass &= insert_count == insert_sum;
			// test_pass &= erase_count == erase_sum;
			test_pass &= erase_sum_ans == erase_sum;

			test_pass &= it_num == (false_find_count + true_find_count + insert_count + erase_count);


			assert(test_pass);
		}

		// remove all
		{
			auto map_size = std_map.size();
			auto sum_ans  = std::accumulate(key_vec.begin(), key_vec.begin() + map_size, 0ull);
			auto test_sum = 0ull;
			run_benchmark(std::format("std remove all test it_num : {}", map_size), map_size, [&]() {
				std::ranges::for_each(key_vec | std::views::take(map_size), [&](auto key) {
					test_sum += std_map.erase(key);
				});
			});
			test_pass &= test_sum == it_num;

			test_sum = 0ull;
			run_benchmark(std::format("rh remove all test it_num : {}", map_size), map_size, [&]() {
				std::ranges::for_each(key_vec | std::views::take(map_size), [&](auto key) {
					test_sum += rh_map.erase(key);
				});
			});
			test_pass &= test_sum == sum_ans;
			test_pass &= (std_map.size() == 0 and rh_map.count() == 0);

			assert(test_pass);
		}


		// auto sum_ans  = std::accumulate(key_vec.begin(), key_vec.begin() + it_num, 0ull);
		// auto test_sum = 0ull;
		// run_benchmark(std::format("std find false test it_num : {}", it_num), it_num, [&]() {
		//	std::ranges::for_each(key_vec | std::views::take(it_num), [&](auto key) {
		//		test_sum += std_map[key];
		//	});
		// });
		// test_pass &= test_sum == sum_ans;

		// test_sum = 0ull;
		// run_benchmark(std::format("rh find false test it_num : {}", it_num), it_num, [&]() {
		//	std::ranges::for_each(key_vec | std::views::take(it_num), [&](auto key) {
		//		test_sum += *rh_map.find(key);
		//	});
		// });
		// test_pass &= test_sum == sum_ans;
	});


	std::cout << (test_pass ? "sum test pass" : "test failed") << std::endl;
	// run_benchmark("std ")


	return 0;
}

int main(int, char**)

	// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
	// Debug program: F5 or Debug > Start Debugging menu#pragma comment ( lib, "Shlwapi.lib" )

	// Tips for Getting Started:
	//   1. Use the Solution Explorer window to add/manage files
	//   2. Use the Team Explorer window to connect to source control
	//   3. Use the Output window to see build output and other messages
	//   4. Use the Error List window to view errors
	//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
	//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
	;