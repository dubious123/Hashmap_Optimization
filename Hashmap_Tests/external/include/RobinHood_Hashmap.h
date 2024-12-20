#include <stdint.h>
#include <immintrin.h>
#include <xhash>
#include <memory>

template <typename _Kty, typename _Vty, typename _Hasher = std::hash<_Vty>>
struct robin_hood_hashmap
{
	struct key_value_pair;

	using key_type	  = _Kty;
	using value_type  = _Vty;
	using header_type = uint16_t;
	using hasher	  = _Hasher;

	constexpr static const auto UINT_SIZE	   = sizeof(header_type) + sizeof(key_value_pair);
	constexpr static const auto MEM_BLOCK_SIZE = ((sizeof(__m256i) << 1) + (sizeof(key_value_pair) << 5));

	uint32_t _count;
	uint32_t _capacity;
	uint32_t _max_offset;
	uint32_t _bit_mask;
	void*	 _memory;

	robin_hood_hashmap(uint32_t capacity = (1 << 5)) : _count(0), _capacity(capacity)
	{
		_max_offset = (log2(capacity));
		_bit_mask	= (((1ull << _max_offset) - 1));
		_memory		= calloc(capacity, UINT_SIZE);
	}

	~robin_hood_hashmap()
	{
		free(_memory);
	}

	robin_hood_hashmap(const robin_hood_hashmap& other)
	{
		_count		= other._count;
		_capacity	= other._capacity;
		_max_offset = other._max_offset;
		_bit_mask	= other._bit_mask;
		_memory		= calloc(_capacity, UINT_SIZE);
		memcpy(_memory, other._memory, (uint64_t)_capacity * UINT_SIZE);	// todo
	}

	robin_hood_hashmap& operator=(const robin_hood_hashmap& other)
	{
		if (this != &other)
		{
			realloc(_memory, (uint64_t)other._capacity * UINT_SIZE);	// todo
			memcpy(_memory, other._memory, (uint64_t)other._capacity * UINT_SIZE);

			_count		= other._count;
			_capacity	= other._capacity;
			_max_offset = other._max_offset;
			_bit_mask	= other._bit_mask;
		}

		return *this;
	}

	robin_hood_hashmap(robin_hood_hashmap&& other) noexcept
	{
		_count		  = other._count;
		_capacity	  = other._capacity;
		_max_offset	  = other._max_offset;
		_bit_mask	  = other._bit_mask;
		_memory		  = other._memory;
		other._memory = nullptr;
	}

	robin_hood_hashmap& operator=(robin_hood_hashmap&& other) noexcept
	{
		if (this != &other)
		{
			realloc(_memory, (uint64_t)other._capacity * UINT_SIZE);	// todo
			memcpy(_memory, other._memory, (uint64_t)other._capacity * UINT_SIZE);

			_count		= other._count;
			_capacity	= other._capacity;
			_max_offset = other._max_offset;
			_bit_mask	= other._bit_mask;

			other._memory = nullptr;
		}

		return *this;
	}

	void clear()
	{
		memset(_memory, 0, _capacity * UINT_SIZE);
		_count = 0;
	}

	uint32_t size() const
	{
		return _count;
	}

	static constexpr const uint8_t LogTable256[256] = {
#define LT(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n
		-1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
		LT(4), LT(5), LT(5), LT(6), LT(6), LT(6), LT(6),
		LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7)
	};

	uint32_t log2(uint32_t v)
	{
		uint32_t r = 0, t = 0, tt = 0;	  // r will be lg(v)

		if (tt = v >> 16)
		{
			r = (t = tt >> 8) ? 24 + LogTable256[t] : 16 + LogTable256[tt];
		}
		else
		{
			r = (t = v >> 8) ? 8 + LogTable256[t] : LogTable256[v];
		}

		return r;
	}

	__m256i get_first_needle(uint16_t home_idx, uint64_t hash)
	{
		auto lower = hash & 0x7fe | 0x01;
		auto m	   = _mm256_set_epi16(15 << 11, 14 << 11, 13 << 11, 12 << 11, 11 << 11, 10 << 11, 9 << 11, 8 << 11, 7 << 11, 6 << 11, 5 << 11, 4 << 11, 3 << 11, 2 << 11, 1 << 11, 0 << 11);
		m		   = _mm256_sub_epi16(m, _mm256_set1_epi16(home_idx << 11));
		return _mm256_or_si256(m, _mm256_set1_epi16(lower));
	}

	__m256i get_second_needle(uint16_t home_idx, uint64_t hash)
	{
		auto lower = hash & 0x7fe | 0x01;
		auto m	   = _mm256_set_epi16(31 << 11, 30 << 11, 29 << 11, 28 << 11, 27 << 11, 26 << 11, 25 << 11, 24 << 11, 23 << 11, 22 << 11, 21 << 11, 20 << 11, 19 << 11, 18 << 11, 17 << 11, 16 << 11);
		m		   = _mm256_sub_epi16(m, _mm256_set1_epi16(home_idx << 11));
		return _mm256_or_si256(m, _mm256_set1_epi16(lower));
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

	// value_type* find(key_type key) const;

	// value_type erase(key_type key);

	// value_type& operator[](key_type key);

	// void _resize();

	// void insert(key_type key, value_type value);

	// void _insert_new(key_type key, value_type value);

	// key_value_pair* _key_value_entry_ptr(auto nth_mem_block, auto idx) const;

	// key_value_pair* _find_key_value_pair(auto& key, __m256i&& needle_cmp_res, auto nth_header_block) const;

	// key_value_pair* _find_key_value_pair_second(auto& key, __m256i&& needle_cmp_res, auto nth_header_block) const;

	// uint16_t _make_header(const uint16_t distance_from_home, const uint64_t& key, uint8_t empty) const;

	key_value_pair* _key_value_entry_ptr(auto nth_mem_block, auto idx) const
	{
		return (key_value_pair*)((uint8_t*)_memory + nth_mem_block * MEM_BLOCK_SIZE + 64) + idx;
	}

	key_value_pair* _find_key_value_pair(auto& key, __m256i&& needle_cmp_res, auto nth_header_block) const
	{
		while (_mm256_testz_si256(needle_cmp_res, needle_cmp_res) != 1)
		{
			auto idx = get_first_byte_index(needle_cmp_res) >> 1;

			auto it = _key_value_entry_ptr(nth_header_block, idx);

			if (key == it->key)
			{
				return it;
			}

			*((uint16_t*)&needle_cmp_res + idx) = 0;
		}

		return nullptr;
	}

	key_value_pair* _find_key_value_pair_second(auto& key, __m256i&& needle_cmp_res, auto nth_header_block) const
	{
		while (_mm256_testz_si256(needle_cmp_res, needle_cmp_res) != 1)
		{
			auto idx = (get_first_byte_index(needle_cmp_res) >> 1);

			auto it = _key_value_entry_ptr(nth_header_block, idx + 16);

			if (key == it->key)
			{
				return it;
			}

			*((uint16_t*)&needle_cmp_res + idx) = 0;
		}

		return nullptr;
	}

	uint16_t _make_header(const uint16_t distance_from_home, const uint64_t& key, uint8_t empty) const
	{
		assert(empty == 0 or empty == 1);
		return (distance_from_home << 11) | key & (0x0000'0000'0000'07fe) | empty;
	}

	void insert(key_type key, value_type value)
	{
		auto  hash				   = hasher(key);
		auto  home				   = hash & _bit_mask;
		auto  home_index		   = home & 0x1f;
		auto  nth_header_block	   = home >> 5;
		auto* header_block_address = (__m256i*)((uint8_t*)_memory + nth_header_block * MEM_BLOCK_SIZE);

		// find
		{
			auto res_ptr = _find_key_value_pair(key, _mm256_cmpeq_epi16(*(header_block_address), get_first_needle(home_index, hash)), nth_header_block);

			if (res_ptr != nullptr)
			{
				res_ptr->value = value;
				return;
			}

			res_ptr = _find_key_value_pair_second(key, _mm256_cmpeq_epi16(*(header_block_address + 1), get_second_needle(home_index, hash)), nth_header_block);


			if (res_ptr != nullptr)
			{
				res_ptr->value = value;
				return;
			}
		}

		auto cmp_res   = _mm256_cmpeq_epi16(*header_block_address, _mm256_setzero_si256());
		auto empty_idx = get_first_byte_index(cmp_res) >> 1;

		if (empty_idx == 16)
		{
			// check header 16~31
			cmp_res	  = _mm256_cmpeq_epi16(*(header_block_address + 1), _mm256_setzero_si256());
			empty_idx = 16 + (get_first_byte_index(cmp_res) >> 1);
			if (empty_idx == 32)
			{
				_resize();
				_insert_new(key, value);
				return;
			}
		}

		*((uint16_t*)(header_block_address) + empty_idx) = _make_header(empty_idx - home_index, key, 1);

		auto* key_value_ptr	 = (key_value_pair*)(header_block_address + 2) + empty_idx;
		key_value_ptr->key	 = key;
		key_value_ptr->value = value;
		++_count;
	}

	value_type* find(key_type key) const
	{
		auto  hash				   = hasher(key);
		auto  home				   = hash & _bit_mask;
		auto  home_index		   = home & 0x1f;
		auto  nth_header_block	   = home >> 5;
		auto* header_block_address = (__m256i*)((uint8_t*)_memory + nth_header_block * MEM_BLOCK_SIZE);

		auto res_ptr = _find_key_value_pair(key, _mm256_cmpeq_epi16(*(__m256i*)(header_block_address), get_first_needle(home_index, hash)), nth_header_block);

		if (res_ptr)
		{
			return &res_ptr->value;
		}

		res_ptr = _find_key_value_pair_second(key, _mm256_cmpeq_epi16(*(__m256i*)(header_block_address + 1), get_second_needle(home_index, hash)), nth_header_block);


		if (res_ptr)
		{
			return &res_ptr->value;
		}

		return nullptr;
	}

	value_type erase(key_type key)
	{
		auto  hash				   = hasher(key);
		auto  home				   = hash & _bit_mask;
		auto  home_index		   = home & 0x1f;
		auto  nth_header_block	   = home >> 5;
		auto* header_block_address = (__m256i*)((uint8_t*)_memory + nth_header_block * MEM_BLOCK_SIZE);

		auto res_ptr = _find_key_value_pair(key, _mm256_cmpeq_epi16(*(__m256i*)(header_block_address), get_first_needle(home_index, hash)), nth_header_block);

		if (res_ptr)
		{
			auto idx = res_ptr - (key_value_pair*)(header_block_address + 2);

			*((uint16_t*)header_block_address + idx) = (uint16_t)0;
			--_count;

			return res_ptr->value;
		}


		res_ptr = _find_key_value_pair_second(key, _mm256_cmpeq_epi16(*(__m256i*)(header_block_address + 1), get_second_needle(home_index, hash)), nth_header_block);
		if (res_ptr == nullptr)
		{
			return -1;
		}

		auto idx								 = res_ptr - (key_value_pair*)(header_block_address + 2);
		*((uint16_t*)header_block_address + idx) = (uint16_t)0;
		--_count;

		return res_ptr->value;
	}

	value_type& operator[](key_type key)	// todo
	{
		auto* res = find(key);
		if (res == nullptr)
		{
			insert(key, 0);
		}

		return *find(key);
	}

	void _resize()
	{
		auto  _backup_capacity = _capacity;
		auto* _backup_memory   = _memory;
		auto  _backup_size	   = _count;
		auto  memory_block_num = _capacity >> 5;	// capacity / 32

		_capacity	= _capacity << 1;				// todo
		_max_offset = log2(_capacity);
		_bit_mask	= (1 << _max_offset) - 1;
		_memory		= calloc(_capacity, UINT_SIZE);

		for (int i = 0; i < memory_block_num; ++i)
		{
			auto* header_block_address = (__m256i*)((uint8_t*)_backup_memory + i * MEM_BLOCK_SIZE);
			auto  haystack			   = _mm256_and_epi32(*header_block_address, _mm256_set1_epi16(1));
			auto  cmp_res			   = _mm256_cmpeq_epi16(haystack, _mm256_set1_epi16(1));
			auto  not_empty_idx		   = get_first_byte_index(cmp_res) >> 1;

			while (not_empty_idx < 16)
			{
				auto* pair_ptr = (key_value_pair*)((uint8_t*)_backup_memory + i * MEM_BLOCK_SIZE + 64) + not_empty_idx;
				_insert_new(pair_ptr->key, pair_ptr->value);
				*((uint16_t*)&haystack + not_empty_idx) = (uint16_t)0;

				cmp_res		  = _mm256_cmpeq_epi16(haystack, _mm256_set1_epi16(1));
				not_empty_idx = get_first_byte_index(cmp_res) >> 1;
			}

			haystack	  = _mm256_and_epi32(*(header_block_address + 1), _mm256_set1_epi16(1));
			cmp_res		  = _mm256_cmpeq_epi16(haystack, _mm256_set1_epi16(1));
			not_empty_idx = get_first_byte_index(cmp_res) >> 1;
			while (not_empty_idx < 16)
			{
				auto* pair_ptr = (key_value_pair*)((uint8_t*)_backup_memory + i * MEM_BLOCK_SIZE + 64) + not_empty_idx + 16;
				_insert_new(pair_ptr->key, pair_ptr->value);
				*((uint16_t*)&haystack + not_empty_idx) = (uint16_t)0;

				cmp_res		  = _mm256_cmpeq_epi16(haystack, _mm256_set1_epi16(1));
				not_empty_idx = get_first_byte_index(cmp_res) >> 1;
			}
		}

		_count = _backup_size;
		free(_backup_memory);
	}

	void _insert_new(uint64_t key, uint64_t value)
	{
		auto  hash				   = hasher(key);
		auto  home				   = hash & _bit_mask;
		auto  home_index		   = home & 0x1f;
		auto  nth_header_block	   = home >> 5;
		auto* header_block_address = (__m256i*)((uint8_t*)_memory + nth_header_block * MEM_BLOCK_SIZE);

		auto cmp_res   = _mm256_cmpeq_epi16(*header_block_address, _mm256_setzero_si256());
		auto empty_idx = get_first_byte_index(cmp_res) >> 1;

		if (empty_idx == 16)
		{
			// check header 16~31
			cmp_res	  = _mm256_cmpeq_epi16(*(header_block_address + 1), _mm256_setzero_si256());
			empty_idx = 16 + (get_first_byte_index(cmp_res) >> 1);
			if (empty_idx == 32)
			{
				_resize();
				insert(key, value);
				return;
			}
		}

		*((uint16_t*)(header_block_address) + empty_idx) = _make_header(empty_idx - home_index, hash, 1);

		auto* key_value_ptr	 = (key_value_pair*)(header_block_address + 2) + empty_idx;
		key_value_ptr->key	 = key;
		key_value_ptr->value = value;
		++_count;
	}

	struct key_value_pair
	{
		key_type   key;
		value_type value;
	};
};
