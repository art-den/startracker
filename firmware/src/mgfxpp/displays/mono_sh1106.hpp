#pragma once

#include <stdint.h>

#include "../mgfxpp_display.hpp"
#include "../mgfxpp_types.hpp"

namespace mgfxpp {

template<typename conn>
struct sh1106_display {

	static constexpr unsigned get_width()
	{
		return 128;
	}

	static constexpr unsigned get_height()
	{
		return 64;
	}

	static bool init(bool rotate180)
	{
		const uint8_t commans[] = {
			0xAE, //display off
			0x20, //Set Memory Addressing Mode
			0x10, //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
			0xB0, //Set Page Start Address for Page Addressing Mode,0-7
			0x00, //---set low column address
			0x10, //---set high column address
			0x40, //--set start line address
			!rotate180 ? (uint8_t)0xA1 : (uint8_t)0xA0, //set segment re-map 0 to 127
			!rotate180 ? (uint8_t)0xC8 : (uint8_t)0xC0, //Set COM Output Scan Direction
			0xA6, //--set normal display
			0xA8, //--set multiplex ratio(1 to 64)
			0x3F, //
			0xA4, //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
			0xD3, //-set display offset
			0x00, //-not offset
			0xD5, //--set display clock divide ratio/oscillator frequency
			0xF0, //--set divide ratio
			0xD9, //--set pre-charge period
			0x22, //
			0xDA, //--set com pins hardware configuration
			0x12,
			0xDB, //--set vcomh
			0x20, //0x20,0.77xVcc
			0x8D, //--set DC-DC enable
			0x14, //
			0xAF //--turn on SSD1306 panel
		};

		for (auto cmd : commans)
		{
			bool ok = conn::send_command(cmd);
			if (!ok) return false;
		}

		conn::finish_send_data();

		return true;
	}

	static void set_brightness(uint8_t brightness)
	{
		conn::send_command(0x81); //--set contrast control register
		conn::send_command(brightness);

		conn::finish_send_data();
	}

	static void set_row(unsigned row)
	{
		conn::send_command(0xB0 | (row & 0xF));
	}

	static void set_col(unsigned col)
	{
		col += 2;
		conn::send_command(0x10 | ((col >> 4) & 0xF));
		conn::send_command(0x00 | (col & 0xF));
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

