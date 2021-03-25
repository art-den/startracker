#pragma once

#include <stdint.h>
#include "hl_device.hpp"

namespace hl {

constexpr uintptr_t DWT_CYCCNT_ADDR  = 0xE0001004;
constexpr uintptr_t DWT_CONTROL_ADDR = 0xE0001000;
constexpr uintptr_t SCB_DEMCR_ADDR   = 0xE000EDFC;

inline void dwt_enable()
{
	HL_UI32REG(SCB_DEMCR_ADDR)  |= 0x01000000;
	HL_UI32REG(DWT_CYCCNT_ADDR) = 0;
	HL_UI32REG(DWT_CONTROL_ADDR) |= 1;
}

inline uint32_t dwt_get()
{
	return HL_UI32REG(DWT_CYCCNT_ADDR);
}

inline void dwt_wait_ticks(uint32_t ticks)
{
	int32_t stop = dwt_get()+ticks;
	while ((int32_t)(dwt_get()-stop) < 0) {}
}

inline void dwt_wait_ticks_wfi(uint32_t ticks)
{
	int32_t stop = dwt_get()+ticks;
	while ((int32_t)(dwt_get()-stop) < 0) __WFI();
}


template <unsigned SysFreq>
class Delay
{
public:
	inline static void exec_ms(uint32_t value)
	{
		dwt_wait_ticks(value * (SysFreq/1000));
	}

	inline static void exec_us(uint32_t value)
	{
		dwt_wait_ticks(value * (SysFreq/1000000));
	}

	inline static void exec_ns(uint32_t value)
	{
		dwt_wait_ticks(value * SysFreq / 1000000000);
	}

	inline static void exec_ms_with_sleep(uint32_t value)
	{
		dwt_wait_ticks_wfi(value * (SysFreq/1000));
	}

	inline static void exec_us_with_sleep(uint32_t value)
	{
		dwt_wait_ticks_wfi(value * (SysFreq/1000000));
	}
};


} // namespace hl
