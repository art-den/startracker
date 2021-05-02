#pragma once

#include <stdint.h>

#include "../mgfxpp_display.hpp"
#include "../mgfxpp_types.hpp"

namespace mgfxpp {

template <typename conn, typename delay>
struct st75256_display
{
	static constexpr unsigned get_width()
	{
		return 256;
	}

	static constexpr unsigned get_height()
	{
		return 128;
	}

	static void set_contrast(uint16_t contrast = 330)
	{
		conn::send_command(0x30);
		conn::send_command(0x81);                 //EV control

		conn::send_data(contrast & 0b111111);     //VPR[5-0]
		conn::send_data((contrast >> 6) & 0b111); //VPR[8-6]
	}

	static void init_1bit_mode(uint16_t contrast = 330)
	{
		conn::reset(false);
		delay::delay_ms(1);
		conn::reset(true);
		delay::delay_ms(50);

		conn::send_command(0x30);   //EXT=0
		conn::send_command(0x94);   //Sleep out
		conn::send_command(0x31);   //EXT=1

		conn::send_command(0xD7);   //Autoread disable
		conn::send_data(0X9F);      //

		conn::send_command(0x32);   //Analog SET
		conn::send_data(0x00);        //OSC Frequency adjustment
		conn::send_data(0x01);        //Frequency on booster capacitors- >6KHz
		conn::send_data(0x02);        //Bias=1/12

		conn::send_command(0x51);  // Booster Level x10
		conn::send_data(0xFB);

		conn::send_command(0x20);   // Gray Level
		conn::send_data(0x01);
		conn::send_data(0x03);
		conn::send_data(0x05);
		conn::send_data(0x07);
		conn::send_data(0x09);
		conn::send_data(0x0b);
		conn::send_data(0x0d);
		conn::send_data(0x10);
		conn::send_data(0x11);
		conn::send_data(0x13);
		conn::send_data(0x15);
		conn::send_data(0x17);
		conn::send_data(0x19);
		conn::send_data(0x1b);
		conn::send_data(0x1d);
		conn::send_data(0x1f);

		conn::send_command(0x30);   //EXT=0

		conn::send_command(0b1100); // DO = 1

		conn::send_command(0x75);   //Page Address setting
		conn::send_data(0X00);      // XS=0
		conn::send_data(0X14);      // XE=159 0x28

		conn::send_command(0x15);

		conn::send_data(0X00);      // XS=0
		conn::send_data(0Xff);      // XE=256

		conn::send_command(0xBC);    //Data scan direction
		conn::send_data(0b00);        //MX.MY=Normal

		conn::send_command(0xCA);
		conn::send_data(0X00);         //
		conn::send_data(0X9F);         //Duty=160
		conn::send_data(0X20);         //Nline=off

		conn::send_command(0xF0);    //Display Mode
		conn::send_data(0X10);         //10=Monochrome Mode,11=4Gray

		set_contrast(contrast);

		conn::send_command(0x20);    //Power control
		conn::send_data(0x0B);       //D0=regulator ; D1=follower ; D3=booste,  on:1 off:0

		delay::delay_ms(1);
		conn::send_command(0xAF);    //Display on
	}

	static void set_window(uint8_t start_col, uint8_t start_row, uint8_t end_col, uint8_t end_row)
	{
		conn::send_command(0x15);
		conn::send_data(start_col);
		conn::send_data(end_col);

		conn::send_command(0x75);
		conn::send_data(start_row+4);
		conn::send_data(end_row+4);

		conn::send_command(0x30);
		conn::send_command(0x5c);
	}

	static void set_pos(unsigned col, unsigned row)
	{
		set_window(col, row, 255, 0x28);
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
