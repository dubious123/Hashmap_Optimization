#include <stdint.h>

struct robin_hood_hashmap
{
	struct bucket;

	uint32_t _count;
	uint32_t _capacity;
	uint32_t _max_offset;
	bucket*	 _buckets;

	robin_hood_hashmap(uint32_t capacity = (1 << 2)) /* : _count(0), _capacity(capacity), _max_offset(_capacity / 4 + 1) {}*/;

	~robin_hood_hashmap();
	void clear();

	int32_t count() const;

	void	  insert(uint64_t key, uint64_t value);
	uint64_t* find(uint64_t key);

	uint64_t erase(uint64_t key);

	uint64_t& operator[](uint64_t key);

	void _resize();

	void _insert(const bucket&& bkt);

	struct bucket
	{
		uint64_t key;
		uint64_t value;
		uint32_t home;
		uint32_t offset;	// distance + 1

		bool empty() const;
	};
};
