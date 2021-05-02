#pragma once
#include <stdint.h>

#include "mgfxpp_display.hpp"

namespace mgfxpp {

template <typename conn, typename delay>
struct ili9341_display
{
	static constexpr DispCrd width = 240;
	static constexpr DispCrd height = 320;

	static constexpr uint8_t ILI9341_NOP = 0x00;
	static constexpr uint8_t ILI9341_SWRESET = 0x01;
	static constexpr uint8_t ILI9341_RDDID = 0x04;
	static constexpr uint8_t ILI9341_RDDST = 0x09;

	static constexpr uint8_t ILI9341_SLPIN = 0x10;
	static constexpr uint8_t ILI9341_SLPOUT = 0x11;

	static constexpr uint8_t ILI9341_PTLON = 0x12;
	static constexpr uint8_t ILI9341_NORON = 0x13;

	static constexpr uint8_t ILI9341_RDMODE = 0x0A;
	static constexpr uint8_t ILI9341_RDMADCTL = 0x0B;
	static constexpr uint8_t ILI9341_RDPIXFMT = 0x0C;
	static constexpr uint8_t ILI9341_RDIMGFMT = 0x0A;
	static constexpr uint8_t ILI9341_RDSELFDIAG = 0x0F;

	static constexpr uint8_t ILI9341_INVOFF = 0x20;
	static constexpr uint8_t ILI9341_INVON = 0x21;
	static constexpr uint8_t ILI9341_GAMMASET = 0x26;
	static constexpr uint8_t ILI9341_DISPOFF = 0x28;
	static constexpr uint8_t ILI9341_DISPON = 0x29;

	static constexpr uint8_t ILI9341_CASET = 0x2A;
	static constexpr uint8_t ILI9341_PASET = 0x2B;
	static constexpr uint8_t ILI9341_RAMWR = 0x2C;
	static constexpr uint8_t ILI9341_RAMRD = 0x2E;

	static constexpr uint8_t ILI9341_PTLAR = 0x30;
	static constexpr uint8_t ILI9341_MADCTL = 0x36;
	static constexpr uint8_t ILI9341_PIXFMT = 0x3A;

	static constexpr uint8_t ILI9341_FRMCTR1 = 0xB1;
	static constexpr uint8_t ILI9341_FRMCTR2 = 0xB2;
	static constexpr uint8_t ILI9341_FRMCTR3 = 0xB3;
	static constexpr uint8_t ILI9341_INVCTR = 0xB4;
	static constexpr uint8_t ILI9341_DFUNCTR = 0xB6;

	static constexpr uint8_t ILI9341_PWCTR1 = 0xC0;
	static constexpr uint8_t ILI9341_PWCTR2 = 0xC1;
	static constexpr uint8_t ILI9341_PWCTR3 = 0xC2;
	static constexpr uint8_t ILI9341_PWCTR4 = 0xC3;
	static constexpr uint8_t ILI9341_PWCTR5 = 0xC4;
	static constexpr uint8_t ILI9341_VMCTR1 = 0xC5;
	static constexpr uint8_t ILI9341_VMCTR2 = 0xC7;

	static constexpr uint8_t ILI9341_RDID1 = 0xDA;
	static constexpr uint8_t ILI9341_RDID2 = 0xDB;
	static constexpr uint8_t ILI9341_RDID3 = 0xDC;
	static constexpr uint8_t ILI9341_RDID4 = 0xDD;

	static constexpr uint8_t ILI9341_GMCTRP1 = 0xE0;
	static constexpr uint8_t ILI9341_GMCTRN1 = 0xE1;

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

	static uint8_t read_reg(uint8_t reg, uint8_t offset)
	{
		send_command(0xD9);
		send_data(0x10 + offset);
		conn::select_device();
		conn::send_command(reg);
		auto result = conn::send_data(0);
		conn::release_device();
		return result;
	}

	static uint32_t init()
	{
		conn::reset(false);
		delay::delay_ms(1);
		conn::reset(true);
		delay::delay_ms(50);

		uint32_t display_code =
			(read_reg(0xD3, 1) << 16) |
			(read_reg(0xD3, 2) << 8) |
			read_reg(0xD3, 3);

		if (display_code != 0x9341)
			return display_code;

		send_command(0xEF);
		send_data(0x03);
		send_data(0x80);
		send_data(0x02);

		send_command(0xCF);
		send_data(0x00);
		send_data(0XC1);
		send_data(0X30);

		send_command(0xED);
		send_data(0x64);
		send_data(0x03);
		send_data(0X12);
		send_data(0X81);

		send_command(0xE8);
		send_data(0x85);
		send_data(0x00);
		send_data(0x78);

		send_command(0xCB);
		send_data(0x39);
		send_data(0x2C);
		send_data(0x00);
		send_data(0x34);
		send_data(0x02);

		send_command(0xF7);
		send_data(0x20);

		send_command(0xEA);
		send_data(0x00);
		send_data(0x00);

		send_command(ILI9341_PWCTR1);    //Power control
		send_data(0x23);   //VRH[5:0]

		send_command(ILI9341_PWCTR2);    //Power control
		send_data(0x10);   //SAP[2:0];BT[3:0]

		send_command(ILI9341_VMCTR1);    //VCM control
		send_data(0x3e); //
		send_data(0x28);

		send_command(ILI9341_VMCTR2);    //VCM control2
		send_data(0x86);  //--

		send_command(ILI9341_MADCTL);    // Memory Access Control
		send_data(0x48);

		send_command(ILI9341_PIXFMT);
		send_data(0x55);

		send_command(ILI9341_FRMCTR1);
		send_data(0x00);
		send_data(0x18);

		send_command(ILI9341_DFUNCTR);    // Display Function Control
		send_data(0x08);
		send_data(0x82);
		send_data(0x27);

		send_command(0xF2);    // 3Gamma Function Disable
		send_data(0x00);

		send_command(ILI9341_GAMMASET);    //Gamma curve selected
		send_data(0x01);

		send_command(ILI9341_GMCTRP1);    //Set Gamma
		send_data(0x0F);
		send_data(0x31);
		send_data(0x2B);
		send_data(0x0C);
		send_data(0x0E);
		send_data(0x08);
		send_data(0x4E);
		send_data(0xF1);
		send_data(0x37);
		send_data(0x07);
		send_data(0x10);
		send_data(0x03);
		send_data(0x0E);
		send_data(0x09);
		send_data(0x00);

		send_command(ILI9341_GMCTRN1);    //Set Gamma
		send_data(0x00);
		send_data(0x0E);
		send_data(0x14);
		send_data(0x03);
		send_data(0x11);
		send_data(0x07);
		send_data(0x31);
		send_data(0xC1);
		send_data(0x48);
		send_data(0x08);
		send_data(0x0F);
		send_data(0x0C);
		send_data(0x31);
		send_data(0x36);
		send_data(0x0F);

		send_command(ILI9341_SLPOUT); //Exit Sleep

		delay::delay_ms(120);

		send_command(ILI9341_DISPON); //Display on

		return display_code;
	}

	static void set_window(const DisplayRect &rect)
	{
		send_command(ILI9341_CASET);
		send_data(rect.left >> 8);
		send_data(rect.left & 0xFF);
		send_data(rect.right >> 8);
		send_data(rect.right & 0xFF);

		send_command(ILI9341_PASET);
		send_data(rect.top >> 8);
		send_data(rect.top & 0xFF);
		send_data(rect.bottom >> 8);
		send_data(rect.bottom & 0xFF);
	}

	static void set_pixel(DispCrd x, DispCrd y, Color color)
	{
		set_window(DisplayRect(x, y, x, y));
		send_command(ILI9341_RAMWR);
		conn::select_device();
		uint16_t data565 = rgb_to_565(color);
		conn::start_send_data16(&data565, 1, false);
		conn::finish_send_data();
		conn::release_device();
	}

	static void fill_rect(const DisplayRect &rect, Color color)
	{
		set_window(rect);
		send_command(ILI9341_RAMWR);
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
		send_command(ILI9341_RAMWR);
		unsigned count = (rect.right - rect.left + 1) * (rect.bottom - rect.top + 1);
		conn::select_device();
		conn::start_send_data16(data_ptr, count, increment_data_ptr);
	}

	static void finish_fill_data16()
	{
		conn::finish_send_data();
		conn::release_device();
	}
};

} // namespace mgfxpp


