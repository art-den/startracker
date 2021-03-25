#pragma once

#include "hl_device.hpp"
#include <stdint.h>
#include <stddef.h>

namespace hl
{

#ifdef FLASH_ACR_DCEN
inline void flash_enable_data_cache()
{
	set_periph_reg_bit<FLASH_BASE + offsetof(FLASH_TypeDef, ACR), FLASH_ACR_DCEN>();
}

inline void flash_disable_data_cache()
{
	clear_periph_reg_bit<FLASH_BASE + offsetof(FLASH_TypeDef, ACR), FLASH_ACR_DCEN>();
}
#endif

#ifdef FLASH_ACR_ICEN
inline void flash_enable_instructions_cache()
{
	set_periph_reg_bit<FLASH_BASE + offsetof(FLASH_TypeDef, ACR), FLASH_ACR_ICEN>();
}

inline void flash_disable_instructions_cache()
{
	clear_periph_reg_bit<FLASH_BASE + offsetof(FLASH_TypeDef, ACR), FLASH_ACR_ICEN>();
}
#endif

inline void flash_enable_prefetch()
{
#if defined(FLASH_ACR_PRFTEN)
	FLASH->ACR |= FLASH_ACR_PRFTEN;
	while (!(FLASH->ACR & FLASH_ACR_PRFTEN)) {}
#elif defined(FLASH_ACR_PRFTBE)
	FLASH->ACR |= FLASH_ACR_PRFTBE;
	while (!(FLASH->ACR & FLASH_ACR_PRFTBS)) {}
#else
	#error Not implemented
#endif
}

inline void flash_disable_prefetch()
{
#if defined(FLASH_ACR_PRFTEN)
	FLASH->ACR &= ~FLASH_ACR_PRFTEN;
	while (FLASH->ACR & FLASH_ACR_PRFTEN) {}
#elif defined(FLASH_ACR_PRFTBE)
	FLASH->ACR &= ~FLASH_ACR_PRFTBE;
#else
	#error Not implemented
#endif
}

enum class FlashLatency
{
	_0 = 0,
#ifdef FLASH_ACR_LATENCY_0WS
	_1 = FLASH_ACR_LATENCY_1WS,
#endif
#ifdef FLASH_ACR_LATENCY_0WS
	_2 = FLASH_ACR_LATENCY_2WS,
#endif
#ifdef FLASH_ACR_LATENCY_0WS
	_3 = FLASH_ACR_LATENCY_3WS,
#endif
#ifdef FLASH_ACR_LATENCY_0WS
	_4 = FLASH_ACR_LATENCY_4WS,
#endif
#ifdef FLASH_ACR_LATENCY_0WS
	_5 = FLASH_ACR_LATENCY_5WS,
#endif
#ifdef FLASH_ACR_LATENCY_0WS
	_6 = FLASH_ACR_LATENCY_6WS,
#endif
#ifdef FLASH_ACR_LATENCY_0WS
	_7 = FLASH_ACR_LATENCY_7WS,
#endif
#ifdef FLASH_ACR_LATENCY_0
	_1 = FLASH_ACR_LATENCY_0,
#endif
#ifdef FLASH_ACR_LATENCY_1
	_2 = FLASH_ACR_LATENCY_1,
#endif
};

inline void flash_set_latency(FlashLatency latency)
{
	set_value_by_mask(FLASH->ACR, FLASH_ACR_LATENCY, (uint32_t)latency);
	while ((FLASH->ACR&FLASH_ACR_LATENCY) != (uint32_t)latency) {}
}


// power saving during run

#ifdef FLASH_ACR_RUN_PD
inline void flash_enable_power_saving_during_run()
{
	FLASH->ACR |= FLASH_ACR_RUN_PD;
}

inline void flash_disable_power_saving_during_run()
{
	FLASH->ACR &= ~FLASH_ACR_RUN_PD;
}
#endif


// power saving during sleep

#ifdef FLASH_ACR_SLEEP_PD
inline void flash_enable_power_saving_during_sleep()
{
	FLASH->ACR |= FLASH_ACR_SLEEP_PD;
}

inline void flash_disable_power_saving_during_sleep()
{
	FLASH->ACR &= ~FLASH_ACR_SLEEP_PD;
}
#endif


// 64 bit access

#ifdef FLASH_ACR_ACC64
inline void flash_enable_64bit_access()
{
	FLASH->ACR |= FLASH_ACR_ACC64;
	while (!(FLASH->ACR & FLASH_ACR_ACC64)) {}
}

inline void flash_disable_64bit_access()
{
	FLASH->ACR &= ~FLASH_ACR_ACC64;
	while (FLASH->ACR & FLASH_ACR_ACC64) {}
}
#endif

// wait state (latency)

inline void flash_set_set_zero_wait_state()
{
	FLASH->ACR &= ~FLASH_ACR_LATENCY;
	while (FLASH->ACR & FLASH_ACR_LATENCY) {}
}

inline void flash_set_one_wait_state()
{
	FLASH->ACR |= FLASH_ACR_LATENCY;
	while (!(FLASH->ACR & FLASH_ACR_LATENCY)) {}
}


#ifdef HL_STM32L1XX

// EEPROM

constexpr uintptr_t EepromStartAddr = 0x08080000;

inline void eeprom_unlock()
{
	if (FLASH->PECR & FLASH_PECR_PELOCK)
	{
		FLASH->PEKEYR = 0x89ABCDEF;
		FLASH->PEKEYR = 0x02030405;
		while (FLASH->SR & FLASH_SR_BSY) {}
	}
}

inline void eeprom_lock()
{
	FLASH->PECR |= FLASH_PECR_PELOCK;
}

union BytesOfInt16
{
	uint16_t int_value;
	uint8_t  bytes[2];
};

union BytesOfInt32
{
	uint32_t int_value;
	uint8_t bytes[4];
};

inline uint16_t get_uint16_unaligned(uintptr_t src)
{
	if ((src % 2) == 0)
		return *(volatile uint16_t*)src;
	else
	{
		BytesOfInt16 boi;
		boi.bytes[0] = *(volatile uint8_t*)src;
		boi.bytes[1] = *(volatile uint8_t*)(src+1);
		return boi.int_value;
	}
}

inline uint32_t get_uint32_unaligned(uintptr_t src)
{
	if ((src % 4) == 0)
		return *(volatile uint32_t*)src;
	else
	{
		BytesOfInt32 boi;
		boi.bytes[0] = *(volatile uint8_t*)src;
		boi.bytes[1] = *(volatile uint8_t*)(src+1);
		boi.bytes[2] = *(volatile uint8_t*)(src+2);
		boi.bytes[3] = *(volatile uint8_t*)(src+3);
		return boi.int_value;
	}
}

inline void set_uint16_unaligned(uintptr_t ram_dst_addr, uint16_t value)
{
	if ((ram_dst_addr % 2) == 0)
		*(volatile uint16_t*)ram_dst_addr = value;
	else
	{
		BytesOfInt16 boi;
		boi.int_value = value;
		*(volatile uint8_t*)ram_dst_addr     = boi.bytes[0];
		*(volatile uint8_t*)(ram_dst_addr+1) = boi.bytes[1];
	}
}

inline void set_uint32_unaligned(uintptr_t ram_dst_addr, uint32_t value)
{
	if ((ram_dst_addr % 4) == 0)
		*(volatile uint32_t*)ram_dst_addr = value;
	else
	{
		BytesOfInt32 boi;
		boi.int_value = value;
		*(volatile uint8_t*)ram_dst_addr     = boi.bytes[0];
		*(volatile uint8_t*)(ram_dst_addr+1) = boi.bytes[1];
		*(volatile uint8_t*)(ram_dst_addr+2) = boi.bytes[2];
		*(volatile uint8_t*)(ram_dst_addr+3) = boi.bytes[3];
	}
}


inline void eeprom_read8(uintptr_t eeprom_src_addr, uintptr_t ram_dst_addr)
{
	*(volatile uint8_t*)ram_dst_addr = *(volatile uint8_t*)eeprom_src_addr;
}

inline void eeprom_read16(uintptr_t eeprom_src_addr, uintptr_t ram_dst_addr)
{
	set_uint16_unaligned(ram_dst_addr, get_uint16_unaligned(eeprom_src_addr));
}

inline void eeprom_read32(uintptr_t eeprom_src_addr, uintptr_t ram_dst_addr)
{
	set_uint32_unaligned(ram_dst_addr, get_uint32_unaligned(eeprom_src_addr));
}

inline void eeprom_read(uintptr_t eeprom_src_addr, uintptr_t ram_dst_addr, unsigned count)
{
	while (count != 0)
	{
		if (count >= 4)
		{
			eeprom_read32(eeprom_src_addr, ram_dst_addr);
			eeprom_src_addr += 4;
			ram_dst_addr += 4;
			count -= 4;
		}
		else if (count >= 2)
		{
			eeprom_read16(eeprom_src_addr, ram_dst_addr);
			eeprom_src_addr += 2;
			ram_dst_addr += 2;
			count -= 2;
		}
		else
		{
			eeprom_read8(eeprom_src_addr, ram_dst_addr);
			eeprom_src_addr++;
			ram_dst_addr++;
			count--;
		}
	}
}

template <typename T>
void eeprom_write_null_data(uintptr_t eeprom_dst_addr)
{
	volatile uint32_t *addr = (volatile uint32_t*)(eeprom_dst_addr & ~3UL);
	unsigned offset = (unsigned)eeprom_dst_addr & 3UL;
	uint32_t value = *addr;
	uint32_t mask = (T)(~T());
	mask <<= offset * 8;
	value &= ~mask;
	*addr = value;
}


inline void eeprom_write8(uintptr_t eeprom_dst_addr, uintptr_t ram_src_addr)
{
	uint8_t value_to_write = *(volatile uint8_t*)ram_src_addr;
	if (*(volatile uint8_t*)eeprom_dst_addr == value_to_write) return;
	FLASH->PECR |= FLASH_PECR_FTDW;
	if (value_to_write != 0)
		*(volatile uint8_t*)eeprom_dst_addr = value_to_write;
	else
		eeprom_write_null_data<uint8_t>(eeprom_dst_addr);
	while (FLASH->SR & FLASH_SR_BSY) {}
}

inline void eeprom_write16(uintptr_t eeprom_dst_addr, uintptr_t ram_src_addr)
{
	uint16_t value_to_write = get_uint16_unaligned(ram_src_addr);
	if (get_uint16_unaligned(eeprom_dst_addr) == value_to_write) return;
	if ((eeprom_dst_addr % 2) != 0)
	{
		eeprom_write8(eeprom_dst_addr,   ram_src_addr  );
		eeprom_write8(eeprom_dst_addr+1, ram_src_addr+1);
	}
	else
	{
		FLASH->PECR |= FLASH_PECR_FTDW;
		if (value_to_write != 0)
			*(volatile uint16_t*)eeprom_dst_addr = value_to_write;
		else
			eeprom_write_null_data<uint16_t>(eeprom_dst_addr);
		while (FLASH->SR & FLASH_SR_BSY) {}
	}
}

inline void eeprom_write32(uintptr_t eeprom_dst_addr, uintptr_t ram_src_addr)
{
	uint32_t value_to_write = get_uint32_unaligned(ram_src_addr);
	if (get_uint32_unaligned(eeprom_dst_addr) == value_to_write) return;
	if ((eeprom_dst_addr % 4) != 0)
	{
		eeprom_write8(eeprom_dst_addr,   ram_src_addr  );
		eeprom_write8(eeprom_dst_addr+1, ram_src_addr+1);
		eeprom_write8(eeprom_dst_addr+2, ram_src_addr+2);
		eeprom_write8(eeprom_dst_addr+3, ram_src_addr+3);
	}
	else
	{
		FLASH->PECR |= FLASH_PECR_FTDW;
		*(volatile uint32_t*)eeprom_dst_addr = value_to_write;
		while (FLASH->SR & FLASH_SR_BSY) {}
	}
}

inline void eeprom_write(uintptr_t eeprom_dst_addr, uintptr_t ram_src_addr, unsigned count)
{
	while (count != 0)
	{
		if ((count >= 4) && ((eeprom_dst_addr % 4) == 0))
		{
			eeprom_write32(eeprom_dst_addr, ram_src_addr);
			ram_src_addr += 4;
			eeprom_dst_addr += 4;
			count -= 4;
		}
		else if ((count >= 2) && ((eeprom_dst_addr % 2) == 0))
		{
			eeprom_write16(eeprom_dst_addr, ram_src_addr);
			ram_src_addr += 2;
			eeprom_dst_addr += 2;
			count -= 2;
		}
		else
		{
			eeprom_write8(eeprom_dst_addr, ram_src_addr);
			ram_src_addr++;
			eeprom_dst_addr++;
			count--;
		}
	}

}

class EepromUnlocker
{
public:
	EepromUnlocker()
	{
		eeprom_unlock();
	}

	~EepromUnlocker()
	{
		eeprom_lock();
	}
};

#endif // HL_STM32L1XX

} // namespace hl
