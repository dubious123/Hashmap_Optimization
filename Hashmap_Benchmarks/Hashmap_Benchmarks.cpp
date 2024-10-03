// Hashmap_Benchmarks.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#pragma comment(lib, "benchmark.lib")
#pragma comment(lib, "Shlwapi.lib")

#include <unordered_map>
#include <random>
#include <ranges>
#include <iostream>
#include <chrono>

#include "benchmark/benchmark.h"
#include "Robin_Hood_Hashmap.h"


static constexpr uint64_t SEED = 19990827;

bool undeducible_false()
{
	// It took mathematicians more than 3 centuries to prove Fermat's
	// last theorem in its most general form. Hardly that knowledge
	// has been put into compilers (or the compiler will try hard
	// enough to check all one million possible combinations below).

	// Caveat: avoid integer overflow (Fermat's theorem
	//         doesn't hold for modulo arithmetic)
	std::uint32_t a = std::clock() % 100 + 1;
	std::uint32_t b = std::rand() % 100 + 1;
	std::uint32_t c = reinterpret_cast<std::uintptr_t>(&a) % 100 + 1;

	return a * a * a + b * b * b == c * c * c;
}

static bool always_false = undeducible_false();
// volatile char* nextLocationToClobber();
volatile char* clobberingPtr;

// void clobber()
//{
//	if (always_false)
//	{
//		// This will never be executed, but the compiler
//		// cannot know about it.
//		clobberingPtr  = nextLocationToClobber();
//		*clobberingPtr = *clobberingPtr;
//	}
// }

#ifdef _MSC_VER
	#pragma optimize("", off)

template <typename T>
inline void escape(T* p)
{
	*reinterpret_cast<volatile char*>(p) = *reinterpret_cast<const volatile char*>(p);	  // thanks, @milleniumbug
}

	#pragma optimize("", on)
#endif

static uint64_t check_sum = 0;

static void BASE_100(benchmark::State& state)
{
	// Perform setup here
	std::mt19937						   mersenne(SEED);
	std::uniform_int_distribution<int>	   dis(0, 10000);
	std::unordered_map<uint64_t, uint64_t> map;

	for (auto i : std::views::iota(0, 10000))
	{
		auto rand = dis(mersenne);
		map[rand] = rand;
	}

	for (auto _ : state)
	{


		// This code gets timed

		check_sum += dis(mersenne);
		// escape(&rand);
	}
}

static void stl_find_100(benchmark::State& state)
{
	// Perform setup here
	std::mt19937						   mersenne(SEED);
	std::uniform_int_distribution<int>	   dis(0, 10000);
	std::unordered_map<uint64_t, uint64_t> map;

	for (auto i : std::views::iota(0, 10000))
	{
		auto rand = dis(mersenne);
		map[rand] = rand;
	}

	for (auto _ : state)
	{


		// This code gets timed


		check_sum += map[dis(mersenne)];
		// escape(&rand);
	}
}

static void BASE_ROBIN_HOOD_100(benchmark::State& state)
{
	// Perform setup here

	std::mt19937					   mersenne(SEED);
	std::uniform_int_distribution<int> dis(0, 10000);
	robin_hood_hashmap				   map;

	for (auto i : std::views::iota(0, 10000))
	{
		auto rand = dis(mersenne);

		map[rand] = rand;
	}
	for (auto _ : state)
	{
		auto find_key = dis(mersenne);
		benchmark::DoNotOptimize(map._buckets);
		map[find_key] = find_key;
		benchmark::ClobberMemory();
	}
}

static void BM_STL_UNORDERED_MAP_FIND_100(benchmark::State& state)
{
	// Perform setup here
	std::mt19937						   mersenne(SEED);
	std::uniform_int_distribution<int>	   dis(0, 100);
	std::unordered_map<uint64_t, uint64_t> map;

	for (auto i : std::views::iota(0, 100))
	{
		auto rand = dis(mersenne);
		map[rand] = rand;
	}

	for (auto _ : state)
	{
		// This code gets timed
		auto		  rand = dis(mersenne);
		volatile auto res  = map.find(rand);
	}
}

static void BM_STL_UNORDERED_MAP_FIND_NO_OPT_100(benchmark::State& state)
{
	// Perform setup here


	for (auto _ : state)
	{
		std::mt19937						   mersenne(SEED);
		std::uniform_int_distribution<int>	   dis(0, 10000);
		std::unordered_map<uint64_t, uint64_t> map;

		for (auto i : std::views::iota(0, 10000))
		{
			auto rand = dis(mersenne);
			map[rand] = rand;
		}

		// This code gets timed

		// benchmark::DoNotOptimize(&res);
		check_sum += map.find(dis(mersenne))->first;

		// benchmark::ClobberMemory();
	}
}

static void BM_vector_push_back(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::vector<int> v;
		v.reserve(1);
		auto data = v.data();			   // Allow v.data() to be clobbered. Pass as non-const
		benchmark::DoNotOptimize(data);	   // lvalue to avoid undesired compiler optimizations
		v.push_back(42);
		benchmark::ClobberMemory();		   // Force 42 to be written to memory.
	}
}

static void BM_vector_base(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::vector<int> v;
		v.reserve(1);
		auto data = v.data();			   // Allow v.data() to be clobbered. Pass as non-const
		benchmark::DoNotOptimize(data);	   // lvalue to avoid undesired compiler optimizations
		benchmark::ClobberMemory();		   // Force 42 to be written to memory.
	}
}

										   // Register the function as a benchmark
// BENCHMARK(BASE_100);
// BENCHMARK(BM_STL_UNORDERED_MAP_FIND_100);
// BENCHMARK(BM_STL_UNORDERED_MAP_FIND_NO_OPT_100);
// BENCHMARK(BM_vector_base);
BENCHMARK(BM_vector_push_back);
BENCHMARK(BASE_100);
BENCHMARK(stl_find_100);
BENCHMARK(BASE_ROBIN_HOOD_100);

// BENCHMARK(BM_STL_UNORDERED_MAP_INIT_100);
//  Run the benchmark
// BENCHMARK_MAIN();

static constexpr auto it_amount = 100000;

void temp_stl_unordered_map_find(int count)
{
	auto exec_count		= 0;
	auto sum_for_no_opt = 0ul;
	auto total_duration = std::chrono::nanoseconds(0);

	std::mt19937						   mersenne(SEED);
	std::uniform_int_distribution<int>	   dis(0, count);
	std::unordered_map<uint64_t, uint64_t> map;


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

	std::mt19937						   mersenne(SEED);
	std::uniform_int_distribution<int>	   dis(0, iteration);
	std::unordered_map<uint64_t, uint64_t> map;
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

	std::mt19937					   mersenne(SEED);
	std::uniform_int_distribution<int> dis(0, count);
	robin_hood_hashmap				   map;


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

int main(int argc, char** argv)
{
	char  arg0_default[] = "benchmark";
	char* args_default	 = arg0_default;
	if (!argv)
	{

		argc = 1;
		argv = &args_default;
	}
	::benchmark::Initialize(&argc, argv);
	if (::benchmark::ReportUnrecognizedArguments(argc, argv)) return 1;
	::benchmark::RunSpecifiedBenchmarks();
	::benchmark::Shutdown();

	std::cout << check_sum << std::endl;


	std::cout << "\ntemp_stl_unordered_map find\n"
			  << std::endl;
	temp_stl_unordered_map_find(100);
	temp_stl_unordered_map_find(1000);
	temp_stl_unordered_map_find(10'000);
	temp_stl_unordered_map_find(100'000);
	temp_stl_unordered_map_find(1'000'000);

	std::cout << "\ntemp_stl_unordered_map insert_extract\n"
			  << std::endl;

	temp_stl_unordered_map_insert_extract(100);
	temp_stl_unordered_map_insert_extract(1000);
	temp_stl_unordered_map_insert_extract(10'000);
	temp_stl_unordered_map_insert_extract(100'000);
	temp_stl_unordered_map_insert_extract(1'000'000);

	std::cout << "\ntemp_stl_robin_hood_hash_map find\n"
			  << std::endl;
	temp_stl_robin_hood_hash_map_find(100);
	temp_stl_robin_hood_hash_map_find(1000);
	temp_stl_robin_hood_hash_map_find(10'000);
	// temp_stl_robin_hood_hash_map_find(100'000);
	//   temp_stl_robin_hood_hash_map_find(1'000'000);

	std::cout << "\ntemp_stl_robin_hood_hash_map insert_extract\n"
			  << std::endl;

	temp_stl_robin_hood_hash_map_insert_extract(100);
	temp_stl_robin_hood_hash_map_insert_extract(1000);
	temp_stl_robin_hood_hash_map_insert_extract(10'000);
	// temp_stl_robin_hood_hash_map_insert_extract(100'000);
	//   temp_stl_robin_hood_hash_map_insert_extract(1'000'000);


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