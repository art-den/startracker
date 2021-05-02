#include "gfx.hpp"

#include "hardware.hpp"
#include "mgfxpp/mgfxpp_draw.hpp"
#include "mgfxpp/mgfxpp_display.hpp"
#include "fonts/font_roboto_bold11_en.hpp"


void show_wellcome_screen()
{
	if (!is_display_ok()) return;

	mgfxpp::display_draw([]
	{
		mgfxpp::set_current_font(mgfxpp::font_roboto_bold11_en);
		mgfxpp::display_fill_rect(mgfxpp::display_get_clip_rect(), mgfxpp::Color::PixelOff);
		mgfxpp::draw_string_at(0, 0, "StarTracker v 0.0.1");
	});
}

void show_cur_info(double angle, unsigned seconds_to_dithering, double dither_angle)
{
	if (!is_display_ok()) return;

	mgfxpp::display_draw([&]
	{
		mgfxpp::set_current_font(mgfxpp::font_roboto_bold11_en);
		mgfxpp::display_fill_rect(mgfxpp::display_get_clip_rect(), mgfxpp::Color::PixelOff);

		int y = 0;
		int x = 2 * mgfxpp::display_get_width() / 3;
		mgfxpp::draw_string_at(0, y, "Angle");
		mgfxpp::printf_at(x, y, u8": {:.1}°", angle);
		y += 12 * mgfxpp::get_current_font()->height / 10;

		mgfxpp::draw_string_at(0, y, "Dither time");
		mgfxpp::printf_at(x, y, ": {}:{:02}", seconds_to_dithering / 60U, seconds_to_dithering % 60U);
		y += 12 * mgfxpp::get_current_font()->height / 10;

		mgfxpp::draw_string_at(0, y, "Dither angle");
		mgfxpp::printf_at(x, y, u8": {:.1}°", dither_angle);
	});
}
