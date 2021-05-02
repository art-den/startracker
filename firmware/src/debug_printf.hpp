#pragma once

#include <string.h>

#include "mgfxpp/micro_format.hpp"
#include "hardware.hpp"

/* debug_printf ans UART stuff */

template <typename ... Args>
size_t debug_printf(const char* format, const Args& ... args)
{
	auto uart_format_callback = [](auto, char character)
	{
		send_debug_uart_char(character);
        return true;
	};
	return mf::format(uart_format_callback, nullptr, format, args...);
}
