#pragma once

#include <stdint.h>

namespace mgfxpp {

template <uint8_t Address, typename I2C, unsigned I2CTimeOut, typename DelayT>
class Display_I2C_Common_Connection
{
public:
	static bool send_command(uint8_t cmd)
	{
		bool ok = flush_buffer();
		if (!ok) return false;

		ok = I2C::start_transfer(Address, 2, I2CTimeOut, DelayT::time_out_delay);
		if (!ok) return false;

		ok = I2C::send_data_and_wait(0, false, I2CTimeOut, DelayT::time_out_delay);
		if (!ok) return false;

		ok = I2C::send_data_and_wait(cmd, true, I2CTimeOut, DelayT::time_out_delay);
		if (!ok) return false;

		I2C::generate_stop();

		return true;
	}

	static bool send_data(uint8_t data)
	{
		if (buf_len_ == BufSize)
			flush_buffer();

		buffer_[buf_len_] = data;
		buf_len_++;

		return true;
	}

	static void finish_send_data()
	{
		flush_buffer();
	}

private:
	static constexpr unsigned BufSize = 32;
	inline static char buffer_[BufSize] = {};
	inline static uint8_t buf_len_ = 0;

	static bool flush_buffer()
	{
		if (buf_len_ == 0) return true;

		bool ok = I2C::start_transfer(Address, buf_len_+1, I2CTimeOut, DelayT::time_out_delay);
		if (!ok) return false;

		ok = I2C::send_data_and_wait(0x40, false, I2CTimeOut, DelayT::time_out_delay);
		if (!ok) return false;

		for (const char *ptr = buffer_; buf_len_ != 0; buf_len_--, ptr++)
		{
			ok = I2C::send_data_and_wait(*ptr, buf_len_ == 1, I2CTimeOut, DelayT::time_out_delay);
			if (!ok) return false;
		}

		I2C::generate_stop();

		return true;
	}
};

} // namespace mgfxpp
