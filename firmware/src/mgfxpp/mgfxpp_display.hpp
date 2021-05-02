#pragma once

#include <string.h>

#include "mgfxpp_defs.hpp"
#include "mgfxpp_types.hpp"

namespace mgfxpp {

using DispCrd = unsigned;

struct DisplayRect
{
	DispCrd left = 0;
	DispCrd top = 0;
	DispCrd right = 0;
	DispCrd bottom = 0;

	DisplayRect() = default;

	DisplayRect(DispCrd left, DispCrd top, DispCrd right, DispCrd bottom) :
		left(left),
		top(top),
		right(right),
		bottom(bottom)
	{}

	void set(DispCrd left, DispCrd top, DispCrd right, DispCrd bottom)
	{
		this->left = left;
		this->top = top;
		this->right = right;
		this->bottom = bottom;
	}
};

enum class DisplayRotation
{
	NS,
	WE,
	SN,
	EW
};

using DrawFun = void (*) (const void *data);

// Display interface

extern const DisplayRect& display_get_clip_rect();
extern DispCrd display_get_width();
extern DispCrd display_get_height();
extern void display_set_pixel(DispCrd x, DispCrd y, Color color);
extern void display_fill_rect(const DisplayRect &rect, Color color);
extern void display_draw(DrawFun fun, const void *data);

template <typename Fun>
void display_draw(const Fun &fun)
{
	auto lambda = [] (const void *data)
	{
		(*static_cast<const Fun*>(data))();
	};

	display_draw(lambda, &fun);
}

#if defined(MGFXPP_COLOR)

inline uint16_t rgb_to_565(Color color)
{
	uint16_t r16 = (color.r >> 3);
	uint16_t g16 = (color.g >> 2);
	uint16_t b16 = (color.b >> 3);
	return (r16 << 11) | (g16 << 5) | b16;
}

inline Color rgb_from_565(uint16_t value)
{
	return Color {
		(uint8_t)((value >> 10) & 0b011111),
		(uint8_t)((value >> 5)  & 0b111111),
		(uint8_t)(value         & 0b011111)
	};
}

template <unsigned Width, unsigned Height>
class InMemoryDisplay565
{
public:
	static constexpr DispCrd width = Width;
	static constexpr DispCrd height = Height;

	void set_memory_address(uint16_t *memory)
	{
		memory_ = memory;
	}

	uint16_t* get_memory_address() const
	{
		return memory_;
	}

	void set_pixel(DispCrd x, DispCrd y, Color color)
	{
		set_pixel_impl(x, y, rgb_to_565(color));
	}

	void fill_rect(const DisplayRect &rect, Color color)
	{
		const uint16_t color_value = rgb_to_565(color);
		for (auto y = rect.top; y <= rect.bottom; y++)
			for (auto x = rect.left; x <= rect.right; x++)
				set_pixel_impl(x, y, color_value);
	}

	Color get_pixel(DispCrd x, DispCrd y) const
	{
		auto pos = x + y * Width;
		if (pos >= Width*Height) return {};
		return rgb_from_565(pos);
	}

	void clear()
	{
		memset(memory_, 0, Width * Height * sizeof(memory_[0]));
	}

private:
	void set_pixel_impl(DispCrd x, DispCrd y, uint16_t value)
	{
		auto pos = x + y * Width;
		if (pos >= Width*Height) return;
		memory_[pos] = value;
	}

	uint16_t *memory_ = nullptr;
};

template <typename Display, unsigned PartHeight>
class PartBufferedDisplay565
{
public:
	static constexpr DispCrd width = Display::width;
	static constexpr DispCrd height = Display::height;

	DisplayRect clip_rect;

	PartBufferedDisplay565()
	{
		display_.set_memory_address(buffer1_);
	}

	const uint16_t* get_buffer() const
	{
		return display_.get_memory_address();
	}

	void set_pixel(DispCrd x, DispCrd y, Color color)
	{
		display_.set_pixel(x, y - offset_y_, color);
	}

	void fill_rect(const DisplayRect &rect, Color color)
	{
		display_.fill_rect(
			{ rect.left, rect.top - offset_y_, rect.right, rect.bottom - offset_y_ },
			color
		);
	}

	void draw(DrawFun fun, const void *data)
	{
		for (offset_y_ = 0;;)
		{
			clip_rect.set(0, offset_y_, width-1, offset_y_ + PartHeight - 1);

			if (clip_rect.bottom >= height)
				clip_rect.bottom = height - 1;

			fun(data);

			if (offset_y_ != 0)
				Display::finish_fill_data16();

			Display::start_fill_data16_to_rect(clip_rect, display_.get_memory_address(), true);

			display_.set_memory_address((buffer1_ == display_.get_memory_address()) ? buffer2_ : buffer1_);
			display_.clear();

			offset_y_ += PartHeight;

			if (offset_y_ >= height) break;
		}

		Display::finish_fill_data16();
	}

private:
	unsigned offset_y_ = 0;
	alignas(8) uint16_t buffer1_[width * PartHeight] = {0};
	alignas(8) uint16_t buffer2_[width * PartHeight] = {0};

	InMemoryDisplay565<width, PartHeight> display_;
};

#elif defined(MGFXPP_MONO)

enum class BwDisplayColor
{
	Set,
	Clear,
	Transparent,
	Inverse
};

inline BwDisplayColor mono_color_to_bw_display_color(Color color, unsigned x, unsigned y)
{
	switch (color)
	{
	case Color::Tansparent:
		return BwDisplayColor::Transparent;

	case Color::Inverse:
		return BwDisplayColor::Inverse;

	case Color::Black:
		return BwDisplayColor::Set;

	case Color::White:
		return BwDisplayColor::Clear;

	case Color::DarkGray:
		return ((x + ((y << 1) & 3)) == 2) ? BwDisplayColor::Clear : BwDisplayColor::Set;

	case Color::Gray:
		return ((x + y) & 1) ? BwDisplayColor::Clear : BwDisplayColor::Set;

	case Color::LiteGray:
		return ((x + ((y << 1) & 3)) == 0) ? BwDisplayColor::Set : BwDisplayColor::Clear;
	}

	return BwDisplayColor::Transparent;
}

template <typename Display>
class MonoBufferedDisplay
{
public:
	static constexpr unsigned width = Display::get_width();
	static constexpr unsigned height = Display::get_height();

	static void set_pixel(DispCrd x, DispCrd y, Color value)
	{
		if (value == Color::Tansparent) return;

		DispCrd xx = 0;
		DispCrd yy = 0;

		switch (rot_)
		{
		case DisplayRotation::NS:
			xx = x;
			yy = y;
			break;

		case DisplayRotation::SN:
			xx = width - x - 1;
			yy = height - y - 1;
			break;

		case DisplayRotation::EW:
			xx = width - y - 1;
			yy = x;
			break;

		case DisplayRotation::WE:
			xx = y;
			yy = height - x - 1;
			break;

		}

		if ((xx >= width) || (yy >= height)) return;

		const unsigned offset = xx + (yy / 8) * width;
		while (offset >= BufferSize) {} // assert

		PixelBlock* const pixel_data_ptr = data_ + offset;
		const uint8_t mask = 1 << (yy & 0x7);
		uint8_t pixel_data = pixel_data_ptr->data;

		switch (mono_color_to_bw_display_color(value, xx, yy))
		{
		case BwDisplayColor::Set:
			pixel_data |= mask;
			break;

		case BwDisplayColor::Clear:
			pixel_data &= ~mask;
			break;

		case BwDisplayColor::Inverse:
			pixel_data ^= mask;
			break;

		default:
		case BwDisplayColor::Transparent:
			return;
		}

		if (pixel_data != pixel_data_ptr->data)
		{
			pixel_data_ptr->data = pixel_data;
			is_changed_ = true;
		}
	}

	static DispCrd get_width()
	{
		return
			(rot_ == DisplayRotation::NS) || (rot_ == DisplayRotation::SN)
			? width
			: height;
	}

	static DispCrd get_height()
	{
		return
			(rot_ == DisplayRotation::NS) || (rot_ == DisplayRotation::SN)
			? height
			: width;
	}

	static const DisplayRect& get_clip_rect()
	{
		return clip_rect_;
	}

	static void set_rotation(DisplayRotation rot)
	{
		rot_ = rot;
	}

	static void draw(DrawFun fun, const void *data)
	{
		clip_rect_.right = get_width() - 1;
		clip_rect_.bottom = get_height() - 1;
		fun(data);
		write_to_display();
	}

private:
	struct PixelBlock
	{
		uint8_t data;
		uint8_t prev;
	};

	static constexpr unsigned BufferSize = width * ((height + 7) / 8);

	inline static PixelBlock data_[BufferSize] = {};
	inline static bool is_changed_ = false;
	inline static bool first_time_ = true;
	inline static DisplayRect clip_rect_ = DisplayRect{0, 0, width - 1, height - 1};
	inline static DisplayRotation rot_ = DisplayRotation::NS;

	static void write_to_display()
	{
		if (!is_changed_ && !first_time_) return;

		auto *data = data_;
		for (unsigned y = 0; y < (height + 7) / 8; y++)
		{
			bool y_changed = true;
			unsigned disp_x = width;
			uint8_t last_change_counter = 0;
			for (unsigned x = 0; x < width; x++)
			{
				if ((data->data != data->prev) || first_time_ || (last_change_counter != 0))
				{
					if ((x != disp_x) || y_changed)
					{
						Display::set_pos(x, y);
						last_change_counter = 25;
						y_changed = false;
					}
					Display::write_data(data->data);
					data->prev = data->data;
					disp_x = x + 1;
				};
				data++;
				if (last_change_counter)
					--last_change_counter;
			}
		}

		Display::flush();

		is_changed_ = false;
		first_time_ = false;
	}
};

#endif

void display_fill_rect_default(const DisplayRect &rect, Color color);


} // namespace mgfxpp


#define MGFXPP_DISPLAY_DIRECT_IMPL(DISPLAY)                          \
                                                                     \
static const mgfxpp::DisplayRect clip_rect {                         \
	0, 0,                                                            \
	DISPLAY::width-1, DISPLAY::height-1                              \
};                                                                   \
                                                                     \
const mgfxpp::DisplayRect& mgfxpp::display_clip_rect = clip_rect;    \
                                                                     \
mgfxpp::DispCrd mgfxpp::display_get_width()                          \
{                                                                    \
	return DISPLAY::width;                                           \
}                                                                    \
                                                                     \
mgfxpp::DispCrd mgfxpp::display_get_height()                         \
{                                                                    \
	return DISPLAY::height;                                          \
}                                                                    \
                                                                     \
void mgfxpp::display_set_pixel(DispCrd x, DispCrd y, Color color)    \
{                                                                    \
	DISPLAY::set_pixel(x, y, color);                                 \
}                                                                    \
                                                                     \
void mgfxpp::display_fill_rect(const DisplayRect &rect, Color color) \
{                                                                    \
	DISPLAY::fill_rect(rect, color);                                 \
}                                                                    \
                                                                     \
void mgfxpp::display_draw(DrawFun fun, const void *data)             \
{                                                                    \
	fun(data);                                                       \
}


// Buffered display implementation

#define MGFXPP_DISPLAY_BUFFERED_IMPL(BUFFER)                \
                                                             \
namespace mgfxpp {                                           \
                                                             \
DispCrd display_get_width()                                  \
{                                                            \
	return BUFFER::get_width();                              \
}                                                            \
                                                             \
DispCrd display_get_height()                                 \
{                                                            \
	return BUFFER::get_height();                             \
}                                                            \
                                                             \
const DisplayRect& display_get_clip_rect()                   \
{                                                            \
	return BUFFER::get_clip_rect();                          \
}                                                            \
                                                             \
void display_set_pixel(DispCrd x, DispCrd y, Color color)    \
{                                                            \
	BUFFER::set_pixel(x, y, color);                          \
}                                                            \
                                                             \
void display_fill_rect(const DisplayRect &rect, Color color) \
{                                                            \
	display_fill_rect_default(rect, color);                  \
}                                                            \
                                                             \
void display_draw(DrawFun fun, const void *data)             \
{                                                            \
	BUFFER::draw(fun, data);                                 \
}                                                            \
                                                             \
} // namespace mgfxpp
