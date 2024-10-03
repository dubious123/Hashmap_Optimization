// Robin_Hood_Hashmap.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <cassert>

class robin_hood_hashmap
{
  public:
	struct bucket
	{
		uint64_t key;
		uint64_t value;
		uint32_t home;
		uint32_t offset;	// distance + 1

		bool empty()
		{
			return offset == 0;
		}
	};

	robin_hood_hashmap(uint32_t capacity = 3) : _count(0), _capacity(capacity), _max_offset(_capacity / 4 + 1)
	{
		//_buckets = (bucket*)malloc(sizeof(bucket) * capacity);
		_buckets = (bucket*)calloc(capacity, sizeof(bucket));
	}

	~robin_hood_hashmap()
	{
		free(_buckets);
	}

	void clear()
	{
		memset(_buckets, 0, sizeof(bucket) * _capacity);
		_count = 0;
	}

	int32_t count() const
	{
		return _count;
	}

	void insert(uint64_t key, uint64_t value)
	{
		if (_count == _capacity)
		{
			_resize();
		}

		_insert({ .key = key, .value = value, .home = (uint32_t)key % _capacity, .offset = 1 });
		++_count;
	}

	uint64_t* find(uint64_t key)
	{
		auto home	= key % _capacity;
		auto offset = 1;
		auto idx	= (home + offset - 1) % _capacity;

		for (idx = (home + offset - 1) % _capacity; offset <= _capacity; idx = (idx + 1) % _capacity)	 // todo
		{
			if (_buckets[idx].offset < offset)															 // bucket is empty or found other 'home'
			{
				return nullptr;
			}

			if (_buckets[idx].key == key)
			{
				return &_buckets[idx].value;
			}

			++offset;
		}

		return nullptr;
	}

	uint64_t erase(uint64_t key)
	{
		auto home	= key % _capacity;
		auto offset = 1;
		auto idx	= (home + offset - 1) % _capacity;
		auto result = 0;
		// find
		for (idx = (home + offset - 1) % _capacity; offset <= _capacity; idx = (idx + 1) % _capacity)	 // todo
		{
			if (_buckets[idx].key == key)
			{
				result = _buckets[idx].value;
				break;
			}

			if (_buckets[idx].offset < offset)	  // bucket is empty or found other 'home'
			{
				return -1;
			}
		}

		for (;; idx = (idx + 1) % _capacity)
		{
			auto& next_bucket = _buckets[(idx + 1) % _capacity];
			if (next_bucket.offset < 2)	   // bucket is empty or home
			{
				_buckets[idx].offset = 0;
				break;
			}

			_buckets[idx] = next_bucket;
			--_buckets[idx].offset;
			assert(_buckets[idx].empty() == false);
		}

		--_count;
		return result;
	}

	uint64_t& operator[](uint64_t key)	  // todo
	{
		auto* res = find(key);
		if (res == nullptr)
		{
			insert(key, 0);
		}

		return *find(key);
	}

  public:
	uint32_t _count;
	uint32_t _capacity;
	uint32_t _max_offset;
	bucket*	 _buckets;

	void _resize()
	{
		// auto  _backup_count	   = _count;
		auto  _backup_capacity = _capacity;
		auto* _backup_buckets  = _buckets;

		//_count		= 0;
		_capacity	= _capacity * 2 + 1;	// todo
		_max_offset = _capacity / 4 + 1;
		_buckets	= (bucket*)calloc(_capacity, sizeof(bucket));

		for (int i = 0; i < _backup_capacity; ++i)
		{
			if (_backup_buckets[i].empty() == false)
			{
				_insert({ .key = _backup_buckets[i].key, .value = _backup_buckets[i].value, .home = (uint32_t)_backup_buckets[i].key % _capacity, .offset = 1 });
			}
		}

		free(_backup_buckets);
	}

	void _insert(const bucket&& bkt)
	{
		if (bkt.offset > _max_offset)
		{
			_resize();
			_insert({ .key = bkt.key, .value = bkt.value, .home = (uint32_t)bkt.key % _capacity, .offset = 1 });
			return;
		}


		auto idx = (bkt.home + bkt.offset - 1) % _capacity;
		assert(bkt.home < _capacity);

		if (_buckets[idx].empty() or _buckets[idx].key == bkt.key)
		{
			_buckets[idx].key	 = bkt.key;
			_buckets[idx].value	 = bkt.value;
			_buckets[idx].offset = bkt.offset;
			_buckets[idx].home	 = bkt.home;
		}
		else if (_buckets[idx].offset >= bkt.offset)
		{
			_insert({ .key = bkt.key, .value = bkt.value, .home = bkt.home, .offset = bkt.offset + 1 });
		}
		else
		{
			const auto backup = _buckets[idx];
			_buckets[idx]	  = bkt;
			_insert({ .key = backup.key, .value = backup.value, .home = backup.home, .offset = backup.offset + 1 });
		}
	}
};

// int main()
//{
//	std::cout << "Hello World!\n";
//
//	auto map1 = robin_hood_hashmap();
//
//	map1.insert(0, 0);
//	map1.insert(1, 1);
//	map1.insert(2, 2);
//	map1.insert(3, 3);
//
//	auto map2 = robin_hood_hashmap();
//
//	map2.insert(0, 0);
//	map2.insert(3, 3);
//	map2.insert(6, 6);
//	map2.insert(9, 9);
//	map2.insert(10, 10);
//	map2.insert(17, 17);
//	map2.insert(24, 24);
//
//	assert(*map2.find(0) == 0);
//	assert(map2[3] == 3);
//	assert(map2[6] == 6);
//	assert(map2[9] == 9);
//	assert(map2[10] == 10);
//	assert(map2[17] == 17);
//	assert(map2[24] == 24);
//
//	assert(map2.erase(0) == 0);
//	assert(map2.find(0) == nullptr);
//	assert(map2[3] == 3);
//	assert(map2[6] == 6);
//	assert(map2[9] == 9);
//	assert(map2[10] == 10);
//	assert(map2[17] == 17);
//	assert(map2[24] == 24);
//
//	assert(map2.erase(3) == 3);
//	assert(map2.find(3) == nullptr);
//	assert(map2[6] == 6);
//	assert(map2[9] == 9);
//	assert(map2[10] == 10);
//	assert(map2[17] == 17);
//	assert(map2[24] == 24);
//
//	assert(map2.erase(6) == 6);
//	assert(map2.find(6) == nullptr);
//	assert(map2[9] == 9);
//	assert(map2[10] == 10);
//	assert(map2[17] == 17);
//	assert(map2[24] == 24);
//
//	assert(map2.erase(9) == 9);
//	assert(map2.find(9) == nullptr);
//	assert(map2[10] == 10);
//	assert(map2[17] == 17);
//	assert(map2[24] == 24);
// }

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started:
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
