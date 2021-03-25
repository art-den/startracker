#pragma once

#include <stdint.h>
#include <stddef.h>

#if defined (HL_STM32L1XX)
	#include <stm32l1xx.h>
	#define HL_BB_SUPPORT 1
#elif defined (HL_STM32F1XX)
	#include <stm32f1xx.h>
	#define HL_BB_SUPPORT 1
#elif defined (HL_STM32F3XX)
	#include <stm32f3xx.h>
	#define HL_BB_SUPPORT 0
#elif defined (HL_STM32F4XX)
	#include <stm32f4xx.h>
	#define HL_BB_SUPPORT 1
#elif defined (HL_STM32F0XX)
	#include <stm32f0xx.h>
	#define HL_BB_SUPPORT 0
#else
	#error "You must define HL_STM32L1XX or HL_STM32F3XX"
#endif

#if HL_BB_SUPPORT==1
	#include "hl_bb.hpp"
#endif

#if defined(__GNUC__)
#define HL_ALWAYS_INLINE __attribute__((__always_inline__)) inline
#else
#define HL_ALWAYS_INLINE inline
#endif

#define HL_UI32REG(ADDR) (*(volatile uint32_t*)(ADDR))

namespace hl {

template <class Helper>
class Device
{
public:
	static void clock_on()
	{
		HL_UI32REG(Helper::ClockRegister) |= Helper::RccBit;
		__DSB();
		__DSB();
		__NOP();
		__NOP();
	}

	static void clock_off()
	{
		HL_UI32REG(Helper::ClockRegister) &= ~Helper::RccBit;
		__DSB();
		__DSB();
		__NOP();
		__NOP();
	}

	static bool is_clock_on()
	{
		return HL_UI32REG(Helper::ClockRegister) & Helper::RccBit;
	}

	static void reset()
	{
		HL_UI32REG(Helper::ResetRegister) |= Helper::RccBit;
		__DSB();
		__DSB();
		__NOP();
		__NOP();
		HL_UI32REG(Helper::ResetRegister) &= ~Helper::RccBit;
		__DSB();
		__DSB();
		__NOP();
		__NOP();
	}
};

inline void set_value_by_mask(volatile uint32_t &reg, const uint32_t mask, const uint32_t value)
{
	uint32_t tmp = reg;
	tmp &= ~mask;
	tmp |= (value & mask);
	reg = tmp;
}

inline void set_value_by_mask(volatile uint16_t &reg, const uint16_t mask, const uint16_t value)
{
	uint16_t tmp = reg;
	tmp &= ~mask;
	tmp |= (value & mask);
	reg = tmp;
}

template <typename T>
void set_value_by_mask(uintptr_t reg_ptr, const T mask, const T value)
{
	T tmp = *(volatile T*)reg_ptr;
	tmp &= ~mask;
	tmp |= (value & mask);
	*(volatile T*)reg_ptr = tmp;
}

template <typename T>
T get_value_by_mask(uintptr_t reg_ptr, const T mask)
{
	return *(volatile T*)reg_ptr & mask;
}


template <uintptr_t reg_addr, uint32_t bit_mask>
void set_periph_reg_bit()
{
#if HL_BB_SUPPORT==1
	HL_UI32REG((BBAddrCalc<reg_addr, bit_mask>::pireph)) = 1;
#else
	HL_UI32REG(reg_addr) |= bit_mask;
#endif
}

template <uintptr_t reg_addr, uint32_t bit_mask>
void clear_periph_reg_bit()
{
#if HL_BB_SUPPORT==1
	HL_UI32REG((BBAddrCalc<reg_addr, bit_mask>::pireph)) = 0;
#else
	HL_UI32REG(reg_addr) &= ~bit_mask;
#endif
}

template <uintptr_t reg_addr, uint32_t bit_mask>
bool get_periph_reg_bit()
{
#if HL_BB_SUPPORT==1
	return HL_UI32REG((BBAddrCalc<reg_addr, bit_mask>::pireph));
#else
	return HL_UI32REG(reg_addr) & bit_mask;
#endif
}

} // namespace hl
