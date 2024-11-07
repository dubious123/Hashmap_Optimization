#include "RobinHood_Hashmap.h"
#include <memory>
#include <cassert>
#include <cmath>

robin_hood_hashmap::robin_hood_hashmap(uint32_t capacity) : _count(0), _capacity(capacity), _max_offset(std::log2(capacity)), _bit_mask(((1ull << _max_offset) - 1))
{
	_buckets = (bucket*)calloc(capacity, sizeof(bucket));
}

robin_hood_hashmap::~robin_hood_hashmap()
{
	free(_buckets);
}

robin_hood_hashmap::robin_hood_hashmap(const robin_hood_hashmap& other)
{
	_count		= other._count;
	_capacity	= other._capacity;
	_max_offset = other._max_offset;
	_bit_mask	= other._bit_mask;
	_buckets	= (bucket*)calloc(_capacity, sizeof(bucket));
	memcpy(_buckets, other._buckets, _capacity * sizeof(bucket));	 // todo
}

robin_hood_hashmap& robin_hood_hashmap::operator=(const robin_hood_hashmap& other)
{
	if (this != &other)
	{
		realloc(_buckets, other._capacity * sizeof(bucket));	// todo
		memcpy(_buckets, other._buckets, other._capacity * sizeof(bucket));

		_count		= other._count;
		_capacity	= other._capacity;
		_max_offset = other._max_offset;
		_bit_mask	= other._bit_mask;
	}

	return *this;
}

robin_hood_hashmap::robin_hood_hashmap(robin_hood_hashmap&& other) noexcept
{
	_count		   = other._count;
	_capacity	   = other._capacity;
	_max_offset	   = other._max_offset;
	_bit_mask	   = other._bit_mask;
	_buckets	   = other._buckets;
	other._buckets = nullptr;
}

robin_hood_hashmap& robin_hood_hashmap::operator=(robin_hood_hashmap&& other) noexcept
{
	if (this != &other)
	{
		realloc(_buckets, other._capacity * sizeof(bucket));	// todo
		memcpy(_buckets, other._buckets, other._capacity * sizeof(bucket));

		_count		= other._count;
		_capacity	= other._capacity;
		_max_offset = other._max_offset;
		_bit_mask	= other._bit_mask;

		other._buckets = nullptr;
	}

	return *this;
}

void robin_hood_hashmap::clear()
{
	memset(_buckets, 0, sizeof(bucket) * _capacity);
	_count = 0;
}

uint32_t robin_hood_hashmap::size() const
{
	return _count;
}

void robin_hood_hashmap::insert(uint64_t key, uint64_t value)
{
	if (_count == _capacity)
	{
		_resize();
	}

	_insert({ .key = key, .value = value, .home = (uint32_t)(key & _bit_mask), .offset = 1 });
	++_count;
}

uint64_t* robin_hood_hashmap::find(uint64_t key) const
{
	auto home	= key & _bit_mask;
	auto offset = 1;
	auto idx	= (home + offset - 1) & _bit_mask;

	for (idx = (home + offset - 1) & _bit_mask; offset <= _capacity; idx = (idx + 1) & _bit_mask)	 // todo
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

uint64_t robin_hood_hashmap::erase(uint64_t key)
{
	auto home	= key & _bit_mask;
	auto offset = 1;
	auto idx	= (home + offset - 1) & _bit_mask;
	auto result = (decltype(key))0;
	// find
	for (idx = (home + offset - 1) & _bit_mask; offset <= _capacity; idx = (idx + 1) & _bit_mask)	 // todo
	{
		if (_buckets[idx].offset < offset)															 // bucket is empty or found other 'home'
		{
			return -1;
		}

		if (_buckets[idx].key == key)
		{
			result = _buckets[idx].value;
			break;
		}

		++offset;
	}

	for (;; idx = (idx + 1) & _bit_mask)
	{
		auto& next_bucket = _buckets[(idx + 1) & _bit_mask];
		if (next_bucket.offset < 2)	   // next bucket is empty or home
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

uint64_t& robin_hood_hashmap::operator[](uint64_t key)	  // todo
{
	auto* res = find(key);
	if (res == nullptr)
	{
		insert(key, 0);
	}

	return *find(key);
}

void robin_hood_hashmap::_resize()
{
	// auto  _backup_count	   = _count;
	auto  _backup_capacity = _capacity;
	auto* _backup_buckets  = _buckets;

	//_count		= 0;
	_capacity	= _capacity << 1;	 // todo
	_max_offset = std::log2(_capacity);
	_bit_mask	= (1 << _max_offset) - 1;
	_buckets	= (bucket*)calloc(_capacity, sizeof(bucket));

	for (int i = 0; i < _backup_capacity; ++i)
	{
		if (_backup_buckets[i].empty() == false)
		{
			_insert({ .key = _backup_buckets[i].key, .value = _backup_buckets[i].value, .home = (uint32_t)((_backup_buckets[i].key & _bit_mask)), .offset = 1 });
		}
	}

	free(_backup_buckets);
}

void robin_hood_hashmap::_insert(const bucket&& bkt)
{
	if (bkt.offset > _max_offset)
	{
		_resize();
		_insert({ .key = bkt.key, .value = bkt.value, .home = (uint32_t)(bkt.key & _bit_mask), .offset = 1 });
		return;
	}


	auto idx = (bkt.home + bkt.offset - 1) & _bit_mask;
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

bool robin_hood_hashmap::bucket::empty() const
{
	return offset == 0;
}