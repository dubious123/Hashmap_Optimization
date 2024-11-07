#pragma once

#include <array>
#include <cstdint>
#include <limits>
#include <random>
#include <utility>
#include "__msvc_int128.hpp"

// from https://github.com/martinus/map_benchmark
// this is probably the fastest high quality 64bit random number generator that exists.
// Implements Small Fast Counting v4 RNG from PractRand.
class sfc64
{
  public:
	using result_type = uint64_t;
	using uint128_t	  = std::_Unsigned128;

	// no copy ctors so we don't accidentally get the same random again
	sfc64(const sfc64&)			   = delete;
	sfc64& operator=(const sfc64&) = delete;

	sfc64(sfc64&&)			  = default;
	sfc64& operator=(sfc64&&) = default;

	sfc64(const std::array<uint64_t, 4>& state)
		: m_a(state[0]), m_b(state[1]), m_c(state[2]), m_counter(state[3]) { }

	static constexpr uint64_t(min)()
	{
		return (std::numeric_limits<uint64_t>::min)();
	}

	static constexpr uint64_t(max)()
	{
		return (std::numeric_limits<uint64_t>::max)();
	}

	sfc64()
		: sfc64(UINT64_C(0x853c49e6748fea9b)) { }

	sfc64(uint64_t seed)
		: m_a(seed), m_b(seed), m_c(seed), m_counter(1)
	{
		for (int i = 0; i < 12; ++i)
		{
			operator()();
		}
	}

	void seed()
	{
		*this = sfc64 { std::random_device {}() };
	}

	uint64_t operator()() noexcept
	{
		const auto tmp = m_a + m_b + m_counter++;
		m_a			   = m_b ^ (m_b >> right_shift);
		m_b			   = m_c + (m_c << left_shift);
		m_c			   = rotl(m_c, rotation) + tmp;
		return tmp;
	}

	// this is a bit biased, but for our use case that's not important.
	uint64_t operator()(uint64_t boundExcluded) noexcept
	{
		return static_cast<uint64_t>((static_cast<uint128_t>(operator()()) * static_cast<uint128_t>(boundExcluded)) >> 64u);
	}

	std::array<uint64_t, 4> state() const
	{
		return { m_a, m_b, m_c, m_counter };
	}

	void state(const std::array<uint64_t, 4>& s)
	{
		m_a		  = s[0];
		m_b		  = s[1];
		m_c		  = s[2];
		m_counter = s[3];
	}

  private:
	template <typename T>
	T rotl(const T x, int k)
	{
		return (x << k) | (x >> (8 * sizeof(T) - k));
	}

	static constexpr int rotation	 = 24;
	static constexpr int right_shift = 11;
	static constexpr int left_shift	 = 3;
	uint64_t			 m_a;
	uint64_t			 m_b;
	uint64_t			 m_c;
	uint64_t			 m_counter;
};
