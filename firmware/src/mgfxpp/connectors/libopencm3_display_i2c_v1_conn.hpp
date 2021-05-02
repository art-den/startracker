#pragma once

#include <stdint.h>

#include <libopencm3/stm32/i2c.h>

namespace mgfxpp {

template <uintptr_t i2c, uint8_t addr, uint32_t time_out = 0>
class LibOpenCM3_Display_I2C_V1_Conn
{
public:
	static bool send_command(uint8_t cmd)
	{
		bool ok = flush_buffer();
		if (!ok) return false;

		uint32_t timer = time_out;
		while ((I2C_SR2(i2c) & I2C_SR2_BUSY)) if (timer && (--timer == 0)) return false;

		i2c_send_start(i2c);
		timer = time_out;
		while (!(I2C_SR1(i2c) & I2C_SR1_SB)) if (timer && (--timer == 0)) return false;
		timer = time_out;
		while (!(I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))) if (timer && (--timer == 0)) return false;

		i2c_send_7bit_address(i2c, addr/2, I2C_WRITE);
		timer = time_out;
		while (!(I2C_SR1(i2c) & I2C_SR1_ADDR)) if (timer && (--timer == 0)) return false;
		I2C_SR1(i2c);
		I2C_SR2(i2c);

		i2c_send_data(i2c, 0);
		timer = time_out;
		while (!(I2C_SR1(i2c) & I2C_SR1_BTF))
			if (timer && (--timer == 0)) return false;

		i2c_send_data(i2c, cmd);
		timer = time_out;
		while (!(I2C_SR1(i2c) & (I2C_SR1_BTF | I2C_SR1_TxE)))
			if (timer && (--timer == 0)) return false;

		i2c_send_stop(i2c);

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
	inline static uint8_t buffer_[BufSize] = {};
	inline static uint8_t buf_len_ = 0;

	static bool flush_buffer()
	{
		if (buf_len_ == 0) return true;

		uint32_t timer = time_out;
		while ((I2C_SR2(i2c) & I2C_SR2_BUSY)) if (timer && (--timer == 0)) return false;

		i2c_send_start(i2c);
		timer = time_out;
		while (!(I2C_SR1(i2c) & I2C_SR1_SB)) if (timer && (--timer == 0)) return false;
		timer = time_out;
		while (!(I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))) if (timer && (--timer == 0)) return false;

		i2c_send_7bit_address(i2c, addr/2, I2C_WRITE);
		timer = time_out;
		while (!(I2C_SR1(i2c) & I2C_SR1_ADDR)) if (timer && (--timer == 0)) return false;
		I2C_SR1(i2c);
		I2C_SR2(i2c);

		i2c_send_data(i2c, 0x40);
		timer = time_out;
		while (!(I2C_SR1(i2c) & I2C_SR1_BTF)) if (timer && (--timer == 0)) return false;

		for (const auto *ptr = buffer_; buf_len_ != 0; buf_len_--, ptr++)
		{
			i2c_send_data(i2c, *ptr);
			timer = time_out;
			while (!(I2C_SR1(i2c) & I2C_SR1_BTF)) if (timer && (--timer == 0)) return false;
		}

		timer = time_out;
		while (!(I2C_SR1(i2c) & I2C_SR1_TxE)) if (timer && (--timer == 0)) return false;

		i2c_send_stop(i2c);

		return true;
	}

};

} // namespace mgfxpp