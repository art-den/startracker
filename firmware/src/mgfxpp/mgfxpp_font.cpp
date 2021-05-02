#include "mgfxpp_font.hpp"

#include <stddef.h>

namespace mgfxpp {

const Character* font_find_charaster(const Font& font, uint32_t char_code)
{
	if (font.flags & FONT_FLAG_CHARS_SORTED)
	{
		const Character* begin = font.characters;
		const Character* end = font.characters + font.charaster_count - 1;

		if (char_code < begin->char_code) return nullptr;
		if (char_code > end->char_code) return nullptr;

		while (begin != end)
		{
			if (begin->char_code == char_code) return begin;
			if (end->char_code == char_code) return end;
			ptrdiff_t dist = end - begin;
			const Character* mid = begin + (dist + 1) / 2;
			if (mid->char_code == char_code) return mid;
			if ((mid == begin) || (mid == end)) return nullptr;

			if ((begin->char_code < char_code) && (char_code < mid->char_code))
				end = mid;
			else
				begin = mid;
		}
	}
	else
	{
		for (auto* chr = font.characters; chr->char_code != 0; chr++)
		{
			if (chr->char_code == char_code)
				return chr;
		}
	}

	return nullptr;
}

void font_get_symbol_data(const Font& font, const Character* font_char, SymbolData &result)
{
	if (font.flags & FONT_FLAG_ONLY_BITMAP_WIDTH)
	{
		result.bmp_width = font_char->data[0];
		result.width = result.bmp_width + font.spacing;
		result.bmp_height = font.height;
		result.bmp_top = 0;
		result.spacing = font.spacing;
		result.bitmap = font_char->data + 1;
	}
	else
	{
		result.width = font_char->data[0];
		result.bmp_width = font_char->data[1];
		result.bmp_height = font_char->data[2];
		result.bmp_top = font_char->data[3];
		result.spacing = result.width - result.bmp_width;
		result.bitmap = font_char->data + 4;
	}
}


} // namespace mgfxpp

