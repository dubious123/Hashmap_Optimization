#include "RobinHood_Hashmap.h"
#include <memory>
#include <cassert>
#include <cmath>
#include <immintrin.h>

#define UINT_SIZE sizeof(header_type) + sizeof(key_value_pair)

#define MEM_BLOCK_SIZE (sizeof(__m256i) + (sizeof(key_value_pair) << 5))

__m256i get_first_needle(uint16_t home_idx, uint64_t hash)
{
	auto upper	= (16 - home_idx) & 0xf;
	auto lower	= hash & 0x7fe | 0x01;
	auto needle = (upper << 11) | lower;
	auto m		= _mm256_set_epi8(15 << 3, 0, 14 << 3, 0, 13 << 3, 0, 12 << 3, 0, 11 << 3, 0, 10 << 3, 0, 9 << 3, 0, 8 << 3, 0, 7 << 3, 0, 6 << 3, 0, 5 << 3, 0, 4 << 3, 0, 3 << 3, 0, 2 << 3, 0, 1 << 3, 0, 0 << 3, 0);
	m			= _mm256_add_epi16(m, _mm256_set1_epi16(needle));
	return _mm256_and_si256(m, _mm256_set1_epi64x(0x7fff7fff7fff7fff));
}

__m256i get_second_needle(uint16_t home_idx, uint64_t hash)
{
	auto upper	= (16 - home_idx) & 0xf;
	auto lower	= hash & 0x7fe;
	auto needle = (upper << 11) | lower;
	auto m		= _mm256_set_epi8(15 << 3, 0, 14 << 3, 0, 13 << 3, 0, 12 << 3, 0, 11 << 3, 0, 10 << 3, 0, 9 << 3, 0, 8 << 3, 0, 7 << 3, 0, 6 << 3, 0, 5 << 3, 0, 4 << 3, 0, 3 << 3, 0, 2 << 3, 0, 1 << 3, 0, 0 << 3, 0);
	m			= _mm256_add_epi16(m, _mm256_set1_epi16(needle));
	return _mm256_or_si256(m, _mm256_set1_epi64x(0x8001800180018001));
}

bool is_empty(uint16_t header)
{
	return (header & 0x1) == 0;
}

uint32_t get_first_byte_index(const __m256i& v)
{
	auto mask = _mm256_movemask_epi8(v);
	return _tzcnt_u32(mask);
}

robin_hood_hashmap::key_value_pair* key_value_entry_ptr(auto nth_mem_block, auto idx)
{
	return (key_value_pair*)((uint8_t*)_memory + nth_mem_block * MEM_BLOCK_SIZE) + idx;
}

robin_hood_hashmap::key_value_pair* find_key_value_pair(__m256i&& needle_cmp_res, auto nth_header_block)
{
	while (_mm256_testz_si256(needle_cmp_res, needle_cmp_res) != 1)
	{
		auto distance_from_home = get_first_byte_index(needle_cmp_res);

		auto idx = +(nth_header_block << 4);

		auto it = key_value_entry_ptr(nth_header_block, idx);

		if (key == it->key)
		{
			return it;
		}

		*(uint16_t*)(((uint8_t*)&needle_cmp_res) + idx) = 0;
	}

	return nullptr;
}

uint16_t make_header(const uint16_t distance_from_home, const uint64_t& key, uint8_t empty)
{
	assert(empty == 0 or empty == 1);
	return (distance_from_home << 11) | key & (0x0000'0000'0000'07fe) | empty;
}

robin_hood_hashmap::robin_hood_hashmap(uint32_t capacity) : _count(0), _capacity(capacity), _max_offset(std::log2(capacity)), _bit_mask(((1ull << _max_offset) - 1))
{
	_memory = calloc(capacity, UINT_SIZE);
}

robin_hood_hashmap::~robin_hood_hashmap()
{
	free(_memory);
}

robin_hood_hashmap::robin_hood_hashmap(const robin_hood_hashmap& other)
{
	_count		= other._count;
	_capacity	= other._capacity;
	_max_offset = other._max_offset;
	_bit_mask	= other._bit_mask;
	_memory		= calloc(_capacity, UINT_SIZE);
	memcpy(_memory, other._memory, _capacity * UINT_SIZE);	  // todo
}

robin_hood_hashmap& robin_hood_hashmap::operator=(const robin_hood_hashmap& other)
{
	if (this != &other)
	{
		realloc(_memory, other._capacity * UINT_SIZE);	  // todo
		memcpy(_memory, other._memory, other._capacity * UINT_SIZE);

		_count		= other._count;
		_capacity	= other._capacity;
		_max_offset = other._max_offset;
		_bit_mask	= other._bit_mask;
	}

	return *this;
}

robin_hood_hashmap::robin_hood_hashmap(robin_hood_hashmap&& other) noexcept
{
	_count		  = other._count;
	_capacity	  = other._capacity;
	_max_offset	  = other._max_offset;
	_bit_mask	  = other._bit_mask;
	_memory		  = other._memory;
	other._memory = nullptr;
}

robin_hood_hashmap& robin_hood_hashmap::operator=(robin_hood_hashmap&& other) noexcept
{
	if (this != &other)
	{
		realloc(_memory, other._capacity * UINT_SIZE);	  // todo
		memcpy(_memory, other._memory, other._capacity * UINT_SIZE);

		_count		= other._count;
		_capacity	= other._capacity;
		_max_offset = other._max_offset;
		_bit_mask	= other._bit_mask;

		other._memory = nullptr;
	}

	return *this;
}

void robin_hood_hashmap::clear()
{
	memset(_memory, 0, _capacity * UINT_SIZE);
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

	_insert(key, value);
	++_count;
}

uint64_t* robin_hood_hashmap::find(uint64_t key) const
{
	auto  home				   = key & _bit_mask;
	auto  home_index		   = home & 0x1f;
	auto  nth_header_block	   = home & 0xffff'ffff'ffff'ff70;
	auto* header_block_address = (__m256i*)((uint8_t*)_memory + nth_header_block * MEM_BLOCK_SIZE);

	auto res_ptr = find_key_value_pair(_mm256_cmpeq_epi16(*(__m256i*)(header_block_address), get_first_needle(home_index, key)), nth_header_block);

	if (res_ptr == nullptr)
	{
		return &find_key_value_pair(_mm256_cmpeq_epi16(*(__m256i*)(header_block_address), get_second_needle(home_index, key)), nth_header_block)->value;
	}

	return &res_ptr->value;
}

uint64_t robin_hood_hashmap::erase(uint64_t key)
{
	auto  home				   = key & _bit_mask;
	auto  home_index		   = home & 0x1f;
	auto  nth_header_block	   = home & 0xffff'ffff'ffff'ff70;
	auto* header_block_address = (__m256i*)((uint8_t*)_memory + nth_header_block * MEM_BLOCK_SIZE);

	auto res_ptr = find_key_value_pair(_mm256_cmpeq_epi16(*(__m256i*)(header_block_address), get_first_needle(home_index, key)), nth_header_block);

	if (res_ptr)
	{
		*((uint16_t*)header_block_address + home_index) = (uint16_t)0;
		--_count;

		return res_ptr->value;
	}


	res_ptr = find_key_value_pair(_mm256_cmpeq_epi16(*(__m256i*)(header_block_address), get_second_needle(home_index, key)), nth_header_block);

	if (res_ptr)
	{
		*((uint16_t*)header_block_address + home_index) = (uint16_t)0;
		--_count;

		return res_ptr->value;
	}


	return -1;
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
	auto  _backup_capacity = _capacity;
	auto* _backup_memory   = _memory;
	auto  memory_block_num = _capacity >> 5;	// capacity / 32

	_capacity	= _capacity << 1;				// todo
	_max_offset = std::log2(_capacity);
	_bit_mask	= (1 << _max_offset) - 1;
	_memory		= calloc(_capacity, UINT_SIZE);


	for (int i = 0; i < memory_block_num; ++i)
	{
		auto* header_block_address = (__m256i*)((uint8_t*)_memory + i * MEM_BLOCK_SIZE);
		auto  cmp_res			   = _mm256_and_epi32(*header_block_address, _mm256_set1_epi16(1));
		auto  distance_from_home   = get_first_byte_index(cmp_res);

		while (distance_from_home < 32)
		{
			auto* pair_ptr = key_value_entry_ptr(i, distance_from_home);
			_insert(pair_ptr->key, pair_ptr->value);
			*((uint16_t*)header_block_address + distance_from_home) = (uint16_t)0;
		}
	}

	free(_backup_memory);
}

void robin_hood_hashmap::_insert(const uint64_t& key, const uint64_t& value)
{
	auto* val_ptr = robin_hood_hashmap::find(key);
	if (val_ptr != nullptr)
	{
		*val_ptr	   = value;
		*(val_ptr - 1) = key;
		return;
	}


	auto  home				   = key & _bit_mask;
	auto  home_index		   = home & 0x1f;
	auto  nth_header_block	   = home & 0xffff'ffff'ffff'ff70;
	auto* header_block_address = (__m256i*)((uint8_t*)_memory + nth_header_block * MEM_BLOCK_SIZE);
	auto  cmp_res			   = _mm256_and_epi32(*header_block_address, _mm256_set1_epi16(1));
	auto  distance_from_home   = get_first_byte_index(cmp_res);

	if (distance_from_home == 32)
	{
		_resize();
		home				 = key & _bit_mask;
		home_index			 = home & 0x1f;
		nth_header_block	 = home & 0xffff'ffff'ffff'ff70;
		header_block_address = (__m256i*)((uint8_t*)_memory + nth_header_block * MEM_BLOCK_SIZE);
		cmp_res				 = _mm256_and_epi32(*header_block_address, _mm256_set1_epi16(1));
	}


	*(uint16_t*)((uint8_t*)(header_block_address) + distance_from_home) = make_header(distance_from_home, key, 1);

	auto* key_value_ptr	 = (key_value_pair*)(header_block_address + 1) + distance_from_home;
	key_value_ptr->key	 = key;
	key_value_ptr->value = value;
}

bool robin_hood_hashmap::bucket::empty() const
{
	return offset == 0;
}