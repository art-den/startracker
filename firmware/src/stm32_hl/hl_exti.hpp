#pragma once

#include "hl_device.hpp"
#include <stdint.h>

namespace hl {

inline void exti_configure_channel(unsigned line, bool enable_event, bool enable_interrupt, bool rising_edge, bool falling_edge)
{
	const uint32_t mask = (1 << line);

	auto conf_register = [mask] (volatile uint32_t &reg, bool bit)
	{
		uint32_t etmp = reg;
		etmp &= ~mask;
		if (bit) etmp |= mask;
		reg = etmp;
	};

	conf_register(EXTI->EMR, enable_event);
	conf_register(EXTI->IMR, enable_interrupt);
	conf_register(EXTI->RTSR, rising_edge);
	conf_register(EXTI->FTSR, falling_edge);
}

inline bool exti_get_pending_bit(unsigned line)
{
	return (EXTI->PR & (1 << line)) != 0;
}

inline void exti_clear_pending_bit(unsigned line)
{
	EXTI->PR = (1 << line);
	EXTI->PR;
	__DSB();
}

} // namespace hl
