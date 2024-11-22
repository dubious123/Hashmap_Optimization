// Robin_Hood_Hashmap.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#if _DEBUG
	#pragma comment(lib, "RobinHood_Hashmap_debug.lib")
#else
	#pragma comment(lib, "RobinHood_Hashmap.lib")
#endif

#include <iostream>
#include <cassert>
#include <random>
#include <ranges>

#include <unordered_map>
#include <unordered_set>

#include "RobinHood_Hashmap.h"

#define LOG(message, ...) std::cout << std::format(message __VA_OPT__(, ) __VA_ARGS__) << std::endl;

// 1. count
// 2. clear
// 3. find
// 4. insert
// 5. erase

// test scenario
// A. deterministic
//	1. add 100, 1'000, 10'000, 100'000, 1'000'000, 10'000'000, 100'000'000, 1'000,000,000 distinct items
//		1. check count
//		2. ?
//	2. copy
//	3. find all
//	4. check for []
//		1. existed item
//		2. non existed item
//	4. delete all items
//		1. check correct return
//		2. check count == 0
//	5. check clear for copied
//
//
//	B. non_deterministic (display seed for debug)
//	 1. 5 operations (insert(add) (20%), erase(20%), find(20%), [](insert 20%), [](find 20%)) 10'000'000 : add 40 remove 20 find 40
//	 2. 5 operations (insert(add) (15%), erase(15%), find(10%), [](insert 5%), [](find 5%), erase(40%))  10'000'000 : add 20, remove 40, find 40
//
//

// static void debug_log(auto message, auto... args)
//{
//	std::cout << std::format(message, args...) << std::endl;
// }

// test insert
//	1. check count == ref_map::size()
//	2. check map[key] == value
//
// bool static test_insert(auto key, auto value, auto& test_map, std::unordered_map<decltype(key), decltype(value)>& ref_map) ?? why not
template <typename K, typename V>
bool static test_count(auto& test_map, std::unordered_map<K, V>& ref_map)
{
	return test_map.size() == ref_map.size();
}

// test insert
// assume count is valie
//	1. check count == ref_map::size()
//	2. check map[key] == value
//
// bool static test_insert(auto key, auto value, auto& test_map, std::unordered_map<decltype(key), decltype(value)>& ref_map) ?? why not
template <typename K, typename V>
bool static test_insert(K key, V value, auto& test_map, std::unordered_map<K, V>& ref_map)
{
	assert(ref_map.contains(key) == false);
	bool res = true;

	ref_map[key] = value;
	test_map.insert(key, value);

	res &= (test_map[key] == value);
	res &= test_map.size() == ref_map.size();

	return res;
}

// test erase
// assume count is valid
// assuem find is valid
// assume ref_map contains key
//  1. check value == ref_map[key]
//	1. check count == ref_map::size()
//	2. check map.find(key) fail
template <typename K, typename V>
bool static test_erase(K key, auto& test_map, std::unordered_map<K, V>& ref_map)
{
	assert(ref_map.contains(key) == true);
	bool res		= true;
	auto ref_value	= ref_map[key];
	auto ptr		= test_map.find(key);
	auto test_value = test_map.erase(key);

	ref_map.erase(key);

	res &= test_value == ref_value;
	assert(res);
	res &= test_map.size() == ref_map.size();
	assert(res);
	res &= test_map.find(test_value) == nullptr;	// assume find is valid
	assert(res);

	return res;
}

// test find
// assume count is valid
//	1. if exist
//		1. check value != nullptr
//		1. check value == ref_map[value]
//		2. check count == ref_map.size()
//
//	2. if not exist
//		1. check value == nullptr
//		2. check count == ref_map.size()
//
template <typename K, typename V>
bool static test_find(K key, auto& test_map, std::unordered_map<K, V>& ref_map)
{
	auto  res		  = true;
	auto  exist		  = ref_map.contains(key);
	auto* test_pvalue = test_map.find(key);

	res &= exist == (test_pvalue != nullptr);
	if (res == false)
	{
		return false;
	}

	if (exist)
	{
		res &= *test_pvalue == ref_map[key];
	}

	res &= test_map.size() == ref_map.size();
	return res;
}

// test update
// assume count, find are valid
// 1. check without update
//	1. check value == ref_map[key]
//	2. check count == ref_map.size()
// 2. check with outdate
//	1. check find(key) == ref_map.find(key)
//	2. check count == ref_map.size()
template <typename K, typename V>
bool static test_index_operator(K key, V value, auto& test_map, std::unordered_map<K, V>& ref_map)
{
	auto res = true;

	auto  ref_value	  = ref_map[key];
	auto  test_value  = test_map[key];
	auto* test_pvalue = (V*)nullptr;

	res &= test_value == ref_value;
	res &= test_map.size() == ref_map.size();

	ref_map[key]  = value;
	test_map[key] = value;

	test_pvalue = test_map.find(key);

	res &= test_pvalue != nullptr and ref_map[key] == *test_pvalue;
	return res;
}

// test clear
// assume count are valid
template <typename K, typename V>
bool static test_clear(auto& test_map, std::unordered_map<K, V>& ref_map)
{
	auto res = true;
	ref_map.clear();
	test_map.clear();
	res &= test_map.size() == ref_map.size();
	return res;
}

// test scenario
// A. deterministic
//	1. add 100, 1'000, 10'000, 100'000, 1'000'000, 10'000'000, 100'000'000, 1'000,000,000 distinct items
//		1. check count
//		2. ?
//	2. copy
//	3. find all
//	4. check for []
//		1. existed item (all items)
//		2. non existed item -> (1000 times)
//	4. delete all items
//		1. check correct return
//		2. check count == 0
//	5. check clear for copied
//
//
//	B. non_deterministic (display seed for debug)
//	 1. 5 operations (insert(add) (20%), erase(20%), find(20%), [](insert 20%), [](find 20%)) 1'000'000 : add 40 remove 20 find 40
//	 2. 5 operations (insert(add) (10%), erase(40%), find(20%), [](insert 10%), [](find 20%))  1'000'000 : add 20, remove 40, find 40

static auto find_unique_key(auto& gen, auto& dist, auto& ref_map)
{
	while (true)
	{
		auto key = dist(gen);
		if (ref_map.contains(key) == false)
		{
			return key;
		}
	}
}

static auto extract_random_from_set(auto& gen, auto& dist, auto& ref_set)
{
	auto rand = dist(gen);
	auto res  = rand;
	if (ref_set.contains(rand))
	{
		ref_set.erase(rand);
		return res;
	}
	else
	{
		auto pair = ref_set.insert(rand);
		auto it	  = pair.first;
		if (++it == ref_set.end())
		{
			res = *(ref_set.begin());
		}
		else
		{
			res = *it;
		}

		ref_set.erase(rand);
		return res;
	}
}

static void fill(auto it_num, auto& gen, auto& dist, auto& test_map, auto& ref_map)
{
	while (ref_map.size() < it_num)
	{
		auto key	 = find_unique_key(gen, dist, ref_map);
		auto value	 = dist(gen);
		ref_map[key] = value;
		test_map.insert(key, value);

		// auto key = dist(gen);
		// if (ref_map.contains(key) == false)
		//{
		//	auto value	 = dist(gen);
		//	ref_map[key] = value;
		//	test_map.insert(key, value);
		// }
	}
}

void run_test_robinhood_det()
{
	auto	   seed	   = 19990827;
	auto	   gen	   = std::mt19937(seed);
	auto	   dist	   = std::uniform_int_distribution<uint64_t>(0, 0xffff'ffff'ffff'ffff);
	const auto it_nums = { 33, 1'000, 10'000, 100'000, 1'000'000 /*, 1'000'000'000 */ /*, 10'000'000*/ /*, 100'000'000, */ };
	LOG("deterministic test start, seed : {}", seed)
	std::ranges::for_each(it_nums, [&](auto it_num) {
		LOG("it_num : {} test start", it_num)

		auto ref_map  = std::unordered_map<uint64_t, uint64_t>();
		auto test_map = robin_hood_hashmap();
		auto res	  = true;
		fill(it_num, gen, dist, test_map, ref_map);

		auto copied_test_map = test_map;
		auto copied_ref_map	 = ref_map;
		LOG("init done");

		res &= ref_map.size() == it_num;
		assert(res);
		res &= test_count(test_map, ref_map);
		assert(res);


		for (auto key : ref_map | std::ranges::views::keys)
		{
			res &= test_find(key, test_map, ref_map);
			assert(res);
			res &= test_index_operator(key, ref_map[key] >> 1, test_map, ref_map);
			assert(res);
		}

		LOG("find test pass");

		for (auto i = 0; i < 1000;)
		{
			auto key = dist(gen);
			if (ref_map.contains(key) == false)
			{
				auto value	= dist(gen);
				res		   &= test_index_operator(key, value, test_map, ref_map);
				assert(res);
				++i;
			}
		}

		LOG("insert test pass");

		// auto keys = ;
		for (auto key : ref_map
							| std::ranges::views::keys
							| std::ranges::to<std::vector>())
		{
			res &= test_erase(key, test_map, ref_map);
			assert(res);
		}

		LOG("erase test pass");

		res &= test_clear(test_map, ref_map);
		assert(res);
		LOG("clear test pass");
	});
}

enum operation
{
	insert,
	erase,
	find,
	index_find,
	index_insert
};

void run_test_robinhood_rand()
{
	// deterministic
	auto rd				  = std::random_device();
	auto seed			  = rd();
	auto gen			  = std::mt19937(seed);
	auto operation_dice_1 = std::uniform_int_distribution<int>(0, 5 - 1);
	auto operation_dice_2 = std::uniform_int_distribution<int>(0, 10 - 1);
	auto dist			  = std::uniform_int_distribution<uint64_t>(0, 0xffff'ffff'ffff'ffff);
	auto ref_map		  = std::unordered_map<uint64_t, uint64_t>();
	auto test_map		  = robin_hood_hashmap();
	auto key_set		  = std::unordered_set<uint64_t>();
	LOG("random test start, seed : {}", seed);
	LOG("Scenario 1 begin");
	std::ranges::for_each(std::views::iota(0, 1'000'000), [&](auto _) {
		auto op = ref_map.empty() ? insert : (operation)operation_dice_1(gen);
		switch (op)
		{
		case insert:
		{
			auto key   = dist(gen);
			auto value = dist(gen);
			test_insert(key, value, test_map, ref_map);
			key_set.insert(key);
			break;
		}
		case erase:
		{
			if (ref_map.empty())
			{
				return;
			}
			auto key = extract_random_from_set(gen, dist, key_set);
			test_erase(key, test_map, ref_map);
			key_set.erase(key);
			break;
		}
		case find:
		{
			auto key = dist(gen);
			test_find(key, test_map, ref_map);
			if (ref_map.contains(key))
			{
				key_set.insert(key);
			}
			break;
		}
		case index_find:
		{
			auto key   = extract_random_from_set(gen, dist, key_set);
			auto value = dist(gen);
			test_index_operator(key, value, test_map, ref_map);
			break;
		}

		case index_insert:
		{
			auto key   = find_unique_key(gen, dist, ref_map);
			auto value = dist(gen);
			test_index_operator(key, value, test_map, ref_map);
			key_set.insert(key);
			break;
		}

		default:
			break;
		}
	});

	test_count(test_map, ref_map);

	LOG("Scenario 1 pass");
	LOG("Scenario 2 begin");

	std::ranges::for_each(std::views::iota(0, 1'000'000), [&](auto _) {
		// 5 operations(insert(add)(10 %), erase(40 %), find(20 %), [](insert 10 %), [](find 20 %)) 1'000'000 : add 20, remove 40, find 40

		auto op_arr = { insert,
						erase, erase, erase, erase,
						find, find,
						index_find, index_find,
						index_insert };

		auto op = ref_map.empty() ? insert : *(op_arr.begin() + operation_dice_2(gen));

		switch (op)
		{
		case insert:
		{
			auto key   = dist(gen);
			auto value = dist(gen);
			test_insert(key, value, test_map, ref_map);
			key_set.insert(key);
			break;
		}
		case erase:
		{
			if (ref_map.empty())
			{
				return;
			}
			auto key = extract_random_from_set(gen, dist, key_set);
			test_erase(key, test_map, ref_map);
			key_set.erase(key);
			break;
		}
		case find:
		{
			auto key = dist(gen);
			test_find(key, test_map, ref_map);
			if (ref_map.contains(key))
			{
				key_set.insert(key);
			}
			break;
		}
		case index_find:
		{

			auto key   = extract_random_from_set(gen, dist, key_set);
			auto value = dist(gen);
			assert(ref_map.contains(key));
			test_index_operator(key, value, test_map, ref_map);
			assert(ref_map.size() == key_set.size());
			break;
		}

		case index_insert:
		{
			auto key   = find_unique_key(gen, dist, ref_map);
			auto value = dist(gen);
			test_index_operator(key, value, test_map, ref_map);
			key_set.insert(key);
			break;
		}
		default:
			break;
		}

		assert(ref_map.size() == key_set.size());
	});

	test_count(test_map, ref_map);
	test_clear(test_map, ref_map);
	LOG("Scenario 2 pass");
}

struct em
{
	em(uint64_t a) : sa(log2(a))
	{
	}

	uint64_t sa;
};

int main()
{
	std::cout << "Hello World!\n";
	auto sss = em(0);

	robin_hood_hashmap					   map;
	std::unordered_map<uint64_t, uint64_t> ref_map;

	assert(test_count(map, ref_map));
	assert(test_insert(10ull, 20ull, map, ref_map));
	assert(test_clear(map, ref_map));
	assert(test_insert(10ull, 20ull, map, ref_map));
	assert(test_count(map, ref_map));
	assert(test_find(10ull, map, ref_map));
	assert(test_count(map, ref_map));
	assert(test_find(20ull, map, ref_map));
	assert(test_count(map, ref_map));
	assert(test_erase(10ull, map, ref_map));
	assert(test_count(map, ref_map));

	assert(test_index_operator(5ull, 10ull, map, ref_map));
	assert(test_count(map, ref_map));
	assert(test_index_operator(5ull, 20ull, map, ref_map));
	assert(test_count(map, ref_map));


	map.clear();

	for (int i = 0; i < 16; ++i)
	{
		map[1 << i] = i;
	}

	auto i = map.erase(1 << 16);
	i	   = map.erase(256);


	run_test_robinhood_det();
	run_test_robinhood_rand();

	// auto map2 = robin_hood_hashmap();
	// for (uint64_t i = 0; i < 100000000; ++i)
	//{
	//	map2[i] = i;
	// }

	// map2.clear();
	//   auto check_sum = 0;

	// for (int i : std::views::iota(1, 100'000))
	//{
	//	auto rand = dis(mersenne);
	//	map[rand] = rand;

	//	check_sum += map[rand];
	//}
	// std::cout << check_sum;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started:
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
