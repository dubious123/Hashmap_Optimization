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

#include "RobinHood_Hashmap.h"

int main()
{
	std::cout << "Hello World!\n";

	auto map1 = robin_hood_hashmap();

	map1.insert(0, 0);
	map1.insert(1, 1);
	map1.insert(2, 2);
	map1.insert(3, 3);

	auto map2 = robin_hood_hashmap();

	map2.insert(0, 0);
	map2.insert(3, 3);
	map2.insert(6, 6);
	map2.insert(9, 9);
	map2.insert(10, 10);
	map2.insert(17, 17);
	map2.insert(24, 24);

	assert(*map2.find(0) == 0);
	assert(map2[3] == 3);
	assert(map2[6] == 6);
	assert(map2[9] == 9);
	assert(map2[10] == 10);
	assert(map2[17] == 17);
	assert(map2[24] == 24);

	assert(map2.erase(0) == 0);
	assert(map2.find(0) == nullptr);
	assert(map2[3] == 3);
	assert(map2[6] == 6);
	assert(map2[9] == 9);
	assert(map2[10] == 10);
	assert(map2[17] == 17);
	assert(map2[24] == 24);

	assert(map2.erase(3) == 3);
	assert(map2.find(3) == nullptr);
	assert(map2[6] == 6);
	assert(map2[9] == 9);
	assert(map2[10] == 10);
	assert(map2[17] == 17);
	assert(map2[24] == 24);

	assert(map2.erase(6) == 6);
	assert(map2.find(6) == nullptr);
	assert(map2[9] == 9);
	assert(map2[10] == 10);
	assert(map2[17] == 17);
	assert(map2[24] == 24);

	assert(map2.erase(9) == 9);
	assert(map2.find(9) == nullptr);
	assert(map2[10] == 10);
	assert(map2[17] == 17);
	assert(map2[24] == 24);


	std::mt19937					   mersenne(19990827);
	std::uniform_int_distribution<int> dis(0, 100000);
	robin_hood_hashmap				   map;

	auto check_sum = 0;

	for (int i : std::views::iota(1, 100'000))
	{
		auto rand = dis(mersenne);
		map[rand] = rand;

		check_sum += map[rand];
	}
	std::cout << check_sum;
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
