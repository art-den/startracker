#pragma once

#include "../mgfxpp_display.hpp"
#include "../mgfxpp_types.hpp"

namespace mgfxpp {

template <typename conn, typename delay>
struct st7567_display
{
	static constexpr unsigned width = 128;
	static constexpr unsigned height = 64;

	static void set_contrast(uint8_t ev_value = 0x1f, uint8_t regulation_ratio = 0b10)
	{
		conn::send_command(0b00100000 | (regulation_ratio & 0b11)); // Regulation Ratio
		conn::send_command(0x81); // EV Select.
		conn::send_command(ev_value & 0b00111111); // Select EV value.
	}

	static void init(uint8_t ev_value = 0x1f, uint8_t regulation_ratio = 0b10)
	{
		conn::reset(false);
		delay::delay_ms(1);
		conn::reset(true);
		delay::delay_ms(50);

		conn::send_command(0xE2); // S/W RESWT
		conn::send_command(0xA3); // LCD bias
		conn::send_command(0xAF); // Display ON
		conn::send_command(0xA0); // segment direction.
		conn::send_command(0xC8); // Common Direction.

		set_contrast();

		conn::send_command(0x2f); // Power control
		conn::send_command(0x40); // Initial display line 40
		conn::send_command(0xB0); // Set page address
		conn::send_command(0x10); // Set coloumn addr  MSB
		conn::send_command(0x00); // Set coloumn addr LSB
		conn::send_command(0xAF); // Display ON
		conn::send_command(0xA4); // A5 .Normal display, all pixels OFF.
		conn::send_command(0xA6); // A7 .Normal display (Inverse Pixel)
	}

	static void set_col(uint8_t col)
	{
		conn::send_command(0x10 | ((col >> 4) & 0xF));
		conn::send_command(0x00 | (col & 0xF));
	}

	static void set_row(uint8_t row)
	{
		conn::send_command(0xB0 | (row & 0xF));
	}

	static void set_pos(unsigned col, unsigned row)
	{
		set_row(row);
		set_col(col);
	}

	static void write_data(uint8_t data)
	{
		conn::send_data(data);
	}

	static void flush()
	{
		conn::finish_send_data();
	}
};

} // namespace mgfxpp
