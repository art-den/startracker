#pragma once

#include <stdint.h>

namespace mgfxpp {

using Crd = int;

struct Rect
{
	Crd left = 0;
	Crd top = 0;
	Crd right = 0;
	Crd bottom = 0;

	Rect() = default;
	Rect(Crd left, Crd top, Crd right, Crd bottom) :
		left(left),
		top(top),
		right(right),
		bottom(bottom)
	{}

	Crd get_height() const
	{
		return bottom - top + 1;
	}

	Crd get_width() const
	{
		return right - left + 1;
	}

};

#if defined(MGFXPP_COLOR)

struct Color
{
	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;

	Color() = default;

	constexpr Color(uint8_t r, uint8_t g, uint8_t b) :
		r(r), g(g), b(b)
	{}
};

static constexpr Color Black      = Color { 0x00, 0x00, 0x00 };
static constexpr Color White      = Color { 0xFF, 0xFF, 0xFF };
static constexpr Color Red        = Color { 0xFF, 0x00, 0x00 };
static constexpr Color DarkRed    = Color { 0x80, 0x00, 0x00 };
static constexpr Color Green      = Color { 0x00, 0xFF, 0x00 };
static constexpr Color DarkGreen  = Color { 0x00, 0x80, 0x00 };
static constexpr Color Blue       = Color { 0x00, 0x00, 0xFF };
static constexpr Color DarkBlue   = Color { 0x00, 0x00, 0x80 };
static constexpr Color Yellow     = Color { 0xFF, 0xFF, 0x00 };
static constexpr Color DarkYellow = Color { 0x80, 0x80, 0x00 };
static constexpr Color Cyan       = Color { 0x00, 0xFF, 0xFF };
static constexpr Color DarkCyan   = Color { 0x00, 0x80, 0x80 };
static constexpr Color Violet     = Color { 0xFF, 0x00, 0xFF };
static constexpr Color DarkViolet = Color { 0x80, 0x00, 0x80 };
static constexpr Color Grey       = Color { 0x80, 0x80, 0x80 };
static constexpr Color Gray       = Color { 0x80, 0x80, 0x80 };
static constexpr Color DarkGray   = Color { 0x40, 0x40, 0x40 };
static constexpr Color LiteGray   = Color { 0xA0, 0xA0, 0xA0 };


#elif defined(MGFXPP_COLOR_16)

enum class Color16 : uint8_t
{
	None            = 0b0001,
	Black           = 0b0000,
	Red             = 0b1000,
	Green           = 0b0100,
	Blue            = 0b0010,
	BrightBit       = 0x0001,
	BrightRed       = Red|BrightBit,
	BrightGreen     = Green|BrightBit,
	BrightBlue      = Blue|BrightBit,
	Yellow          = Red|Green,
	BrightYellow    = Yellow|BrightBit,
	Turquoise       = Green|Blue,
	BrightTurquoise = Turquoise|BrightBit,
	Purple          = Red|Blue,
	BrightPurple    = Purple|BrightBit,
	Grey            = Red|Green|Blue,
	White           = Grey|BrightBit,
	Toggle          = 0b10000
};

#elif defined(MGFXPP_MONO)

enum class Color : uint8_t
{
	Tansparent,
	Inverse,
	Black,
	DarkGray,
	Gray,
	LiteGray,
	White,

	PixelOn = Black,
	PixelOff = White,
};

#endif

enum class ColorIndex : uint8_t
{
	Foreground = 0,
	Background = 1,

	_MaxColorIndex
};


} // namespace mgfxpp

