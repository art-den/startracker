#pragma once

#include <stdint.h>

namespace mgfxpp {

struct Character
{
	uint32_t char_code;
	const uint8_t *data;
};

constexpr uint8_t FONT_FLAG_ONLY_BITMAP_WIDTH = 1;
constexpr uint8_t FONT_FLAG_CHARS_SORTED      = 2;

struct Font
{
	uint8_t height;
	uint8_t flags;
	uint8_t spacing;
	uint16_t charaster_count;
	const Character* characters;
};

const Character* font_find_charaster(const Font &font, uint32_t char_code);

struct SymbolData
{
	uint8_t bmp_width = 0;
	uint8_t bmp_height = 0;
	uint8_t bmp_top = 0;
	uint8_t width = 0;
	uint8_t spacing = 0;
	const uint8_t* bitmap = nullptr;
};

void font_get_symbol_data(const Font& font, const Character* font_char, SymbolData &result);


} // namespace mgfxpp