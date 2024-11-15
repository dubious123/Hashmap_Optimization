#include <stdint.h>

struct robin_hood_hashmap
{
	struct bucket;
	struct key_value_pair;

	robin_hood_hashmap(uint32_t capacity = (1 << 2));

	~robin_hood_hashmap();

	robin_hood_hashmap(const robin_hood_hashmap& other);

	robin_hood_hashmap& operator=(const robin_hood_hashmap& other);

	robin_hood_hashmap(robin_hood_hashmap&& other) noexcept;

	robin_hood_hashmap& operator=(robin_hood_hashmap&& other) noexcept;


	void clear();

	uint32_t size() const;

	void insert(uint64_t key, uint64_t value);

	uint64_t* find(uint64_t key) const;

	uint64_t erase(uint64_t key);

	uint64_t& operator[](uint64_t key);

	void _resize();

	void _insert(const uint64_t& key, const uint64_t& value);

	using header_type = uint16_t;

	struct bucket
	{
		uint64_t key;
		uint64_t value;
		uint32_t home;
		uint32_t offset;	// distance + 1

		bool empty() const;
	};

	struct key_value_pair
	{
		uint64_t key;
		uint64_t value;
	};

	uint32_t _count;
	uint32_t _capacity;
	uint32_t _max_offset;
	uint32_t _bit_mask;
	void*	 _memory;
};
