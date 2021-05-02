#pragma once
#include <stdint.h>

#include "mgfxpp_display.hpp"

namespace mgfxpp {

template <typename conn, typename delay>
struct st7789_display
{
	static constexpr DispCrd width = 240;
	static constexpr DispCrd height = 240;

	static constexpr uint8_t ST7789_SWRESET = 0x01;
	static constexpr uint8_t ST7789_MADCTL = 0x36;
	static constexpr uint8_t ST7789_FRMCTR2 = 0xB2;
	static constexpr uint8_t ST7789_COLMOD = 0x3A;
	static constexpr uint8_t ST7789_GCTRL = 0xB7;
	static constexpr uint8_t ST7789_VCOMS = 0xBB;
	static constexpr uint8_t ST7789_LCMCTRL = 0xC0;
	static constexpr uint8_t ST7789_VDVVRHEN = 0xC2;
	static constexpr uint8_t ST7789_VRHS = 0xC3;
	static constexpr uint8_t ST7789_VDVS = 0xC4;
	static constexpr uint8_t ST7789_FRCTRL2 = 0xC6;
	static constexpr uint8_t ST7789_GMCTRP1 = 0xE0;
	static constexpr uint8_t ST7789_GMCTRN1 = 0xE1;
	static constexpr uint8_t ST7789_INVOFF = 0x20;
	static constexpr uint8_t ST7789_INVON = 0x21;
	static constexpr uint8_t ST7789_SLPOUT = 0x11;
	static constexpr uint8_t ST7789_DISPON = 0x29;
	static constexpr uint8_t ST7789_CASET = 0x2A;
	static constexpr uint8_t ST7789_RASET = 0x2B;
	static constexpr uint8_t ST7789_RAMWR = 0x2C;

	static constexpr uint8_t ST7789_MADCTL_MY = 0x80;
	static constexpr uint8_t ST7789_MADCTL_MX = 0x40;
	static constexpr uint8_t ST7789_MADCTL_MV = 0x20;
	static constexpr uint8_t ST7789_MADCTL_ML = 0x10;
	static constexpr uint8_t ST7789_MADCTL_BGR = 0x08;
	static constexpr uint8_t ST7789_MADCTL_RGB = 0x00;

	static void send_command(uint8_t command)
	{
		conn::select_device();
		conn::send_command(command);
		conn::release_device();
	}

	static void send_data(uint8_t data)
	{
		conn::select_device();
		conn::send_data(data);
		conn::release_device();
	}

	static void init()
	{
		conn::reset(false);
		delay::delay_ms(1);
		conn::reset(true);
		delay::delay_ms(50);

		// init code from
		// https://github.com/pimoroni/st7789-python/blob/master/library/ST7789/__init__.py

		send_command(ST7789_SWRESET);    // Software reset
		delay::delay_ms(150);               // delay 150 ms

		send_command(ST7789_MADCTL);
		send_data(ST7789_MADCTL_RGB);

		send_command(ST7789_FRMCTR2);    // Frame rate ctrl - idle mode
		send_data(0x0C);
		send_data(0x0C);
		send_data(0x00);
		send_data(0x33);
		send_data(0x33);

		send_command(ST7789_COLMOD);
		send_data(0x05);

		send_command(ST7789_GCTRL);
		send_data(0x14);

		send_command(ST7789_VCOMS);
		send_data(0x37);

		send_command(ST7789_LCMCTRL);    // Power control
		send_data(0x2C);

		send_command(ST7789_VDVVRHEN);   // Power control
		send_data(0x01);

		send_command(ST7789_VRHS);       // Power control
		send_data(0x12);

		send_command(ST7789_VDVS);       // Power control
		send_data(0x20);

		send_command(0xD0);
		send_data(0xA4);
		send_data(0xA1);

		send_command(ST7789_FRCTRL2);
		send_data(0x0F);

		send_command(ST7789_GMCTRP1);    // Set Gamma
		send_data(0xD0);
		send_data(0x04);
		send_data(0x0D);
		send_data(0x11);
		send_data(0x13);
		send_data(0x2B);
		send_data(0x3F);
		send_data(0x54);
		send_data(0x4C);
		send_data(0x18);
		send_data(0x0D);
		send_data(0x0B);
		send_data(0x1F);
		send_data(0x23);

		send_command(ST7789_GMCTRN1);    // Set Gamma
		send_data(0xD0);
		send_data(0x04);
		send_data(0x0C);
		send_data(0x11);
		send_data(0x13);
		send_data(0x2C);
		send_data(0x3F);
		send_data(0x44);
		send_data(0x51);
		send_data(0x2F);
		send_data(0x1F);
		send_data(0x1F);
		send_data(0x20);
		send_data(0x23);

		send_command(ST7789_INVON);

		send_command(ST7789_SLPOUT);

		send_command(ST7789_DISPON);     // Display on
		delay::delay_ms(100);               // 100 ms
	}

	static void set_rotation(DisplayRotation rot)
	{
		switch (rot)
		{
		case DisplayRotation::NS:
			send_command(ST7789_MADCTL);
			send_data(ST7789_MADCTL_RGB);
			start_x_ = 0;
			start_y_ = 0;
			break;

		case DisplayRotation::SN:
			send_command(ST7789_MADCTL);
			send_data(ST7789_MADCTL_RGB | ST7789_MADCTL_MX | ST7789_MADCTL_MY);
			start_x_ = 0;
			start_y_ = 320-height;
			break;

		case DisplayRotation::WE:
			send_command(ST7789_MADCTL);
			send_data(ST7789_MADCTL_RGB | ST7789_MADCTL_MV | ST7789_MADCTL_MY | ST7789_MADCTL_ML);
			start_x_ = 320-width;
			start_y_ = 0;
			break;

		case DisplayRotation::EW:
			send_command(ST7789_MADCTL);
			send_data(ST7789_MADCTL_RGB | ST7789_MADCTL_MV | ST7789_MADCTL_MX);
			start_x_ = 0;
			start_y_ = 0;
			break;
		}
	}

	static void set_window(const DisplayRect &rect)
	{
		auto left = rect.left + start_x_;
		auto right = rect.right + start_x_;

		send_command(ST7789_CASET);
		send_data(left >> 8);
		send_data(left & 0xFF);
		send_data(right >> 8);
		send_data(right & 0xFF);

		auto top = rect.top + start_y_;
		auto bottom = rect.bottom + start_y_;

		send_command(ST7789_RASET);
		send_data(top >> 8);
		send_data(top & 0xFF);
		send_data(bottom >> 8);
		send_data(bottom & 0xFF);
	}

	static void set_pixel(DispCrd x, DispCrd y, Color color)
	{
		set_window(DisplayRect(x, y, x, y));
		send_command(ST7789_RAMWR);
		conn::select_device();
		uint16_t data565 = rgb_to_565(color);
		conn::start_send_data16(&data565, 1, false);
		conn::finish_send_data();
		conn::release_device();
	}

	static void fill_rect(const DisplayRect &rect, Color color)
	{
		set_window(rect);
		send_command(ST7789_RAMWR);
		uint16_t data565 = rgb_to_565(color);
		unsigned count = (rect.right - rect.left + 1) * (rect.bottom - rect.top + 1);
		conn::select_device();
		conn::start_send_data16(&data565, count, false);
		conn::finish_send_data();
		conn::release_device();
	}

	static void start_fill_data16_to_rect(
		const DisplayRect &rect,
		const uint16_t    *data_ptr,
		bool              increment_data_ptr)
	{
		set_window(rect);
		send_command(ST7789_RAMWR);
		unsigned count = (rect.right - rect.left + 1) * (rect.bottom - rect.top + 1);
		conn::select_device();
		conn::start_send_data16(data_ptr, count, increment_data_ptr);
	}

	static void finish_fill_data16()
	{
		conn::finish_send_data();
		conn::release_device();
	}

private:
	inline static DispCrd start_x_ = 0;
	inline static DispCrd start_y_ = 0;
};


} // namespace mgfxpp