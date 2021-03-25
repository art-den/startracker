#pragma once

#include <stdint.h>

namespace hl {
namespace detailed {

template <uint32_t bitmask>
struct BitIndexCalc
{
	static_assert(((bitmask & 1) == 0) || (bitmask == 1), "Wrong bitmask! Bitmask must contains only one 1 bit!");
	enum { result = BitIndexCalc<(bitmask >> 1)>::result + 1 };
};

template <>
struct BitIndexCalc<1>
{
	enum { result = 0 };
};

template <> struct BitIndexCalc<0> {};

} // namespace detailed

template <uint32_t addr, uint32_t Bitmask>
struct BBAddrCalc
{
	enum
	{
		bb_pireph_base = 0x42000000,
		pireph = bb_pireph_base + addr * 32 + (detailed::BitIndexCalc<Bitmask>::result * 4)
	};
};


} // namespace hl
