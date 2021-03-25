#pragma once

#include "stm32_hl/hl_usart.hpp"
#include "stm32_hl/hl_gpio.hpp"
#include "micro_format/micro_format.hpp"

using DbgUsart = hl::Usart1;
using TxPin = hl::PA9;
using TxPinGndPin = hl::PA8;

template <typename ... Args>
size_t debug_printf(const char* format, const Args& ... args)
{
	auto uart_format_callback = [](auto, char character)
	{
		DbgUsart::clear_tc_flag();
		DbgUsart::send_data((unsigned char)character);
		while (!DbgUsart::get_tc_flag()) { }
		DbgUsart::clear_tc_flag();
	        return true;
	};
	return mf::format(uart_format_callback, nullptr, format, args...);
}

inline void init_debug_printf(unsigned sys_clock_freq)
{
	DbgUsart::clock_on();
	DbgUsart::reset();

	DbgUsart::set_stop_bits(hl::StopBits::_1);
	DbgUsart::set_baud_rate(sys_clock_freq / 115200);

	DbgUsart::enable_transmitter();
	DbgUsart::enable();

	TxPin::conf_alt_push_pull();
	TxPinGndPin::conf_out_push_pull();
	TxPinGndPin::off();
}
