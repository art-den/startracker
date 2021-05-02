#include "mgfxpp_draw.hpp"
#include "mgfxpp_display.hpp"
#include "mgfxpp_utils.hpp"

namespace mgfxpp {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Misc utilites */

Rect get_display_rect()
{
	return Rect{ 0, 0, (Crd)display_get_width()-1, (Crd)display_get_height()-1 };
}

bool is_rect_visible(const Rect &rect, const DisplayRect &clip_rect)
{
	if ((rect.left < (Crd)clip_rect.left) && (rect.right < (Crd)clip_rect.left)) return false;
	if ((rect.left > (Crd)clip_rect.right) && (rect.right > (Crd)clip_rect.right)) return false;
	if ((rect.top < (Crd)clip_rect.top) && (rect.bottom < (Crd)clip_rect.top)) return false;
	if ((rect.top > (Crd)clip_rect.bottom) && (rect.bottom > (Crd)clip_rect.bottom)) return false;
	return true;
}

bool is_point_visible(Crd x, Crd y, const DisplayRect &clip_rect)
{
	if (x < (Crd)clip_rect.left) return false;
	if (x > (Crd)clip_rect.right) return false;
	if (y < (Crd)clip_rect.top) return false;
	if (y > (Crd)clip_rect.bottom) return false;
	return true;
}

static DisplayRect get_visible_rect(const Rect &rect, const DisplayRect &clip_rect)
{
	return DisplayRect {
		(DispCrd)max(rect.left, (Crd)clip_rect.left),
		(DispCrd)max(rect.top, (Crd)clip_rect.top),
		(DispCrd)min(rect.right, (Crd)clip_rect.right),
		(DispCrd)min(rect.bottom, (Crd)clip_rect.bottom)
	};
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Color */

#if defined(MGFXPP_COLOR)

enum class Anisotropy
{
	None,
	Vertical,
	Horizontal,
	Both
};

struct ColorProvider
{
	virtual Color get(Crd x, Crd y) const = 0;
	virtual Anisotropy get_anisotropy() const = 0;
	virtual void init(Crd x1, Crd y1, Crd x2, Crd y2) = 0;
};

class SolidColorProvider : public ColorProvider
{
public:
	SolidColorProvider(const Color &color) :
		color_(color)
	{}

	Color get(Crd x, Crd y) const override
	{
		return color_;
	}

	Anisotropy get_anisotropy() const override
	{
		return Anisotropy::None;
	}

	void init(Crd x1, Crd y1, Crd x2, Crd y2) override {}

	void set(Color color)
	{
		color_ = color;
	}

private:
	Color color_;
};

class GradientProvider : public ColorProvider
{
public:
	Color get(Crd x, Crd y) const override
	{
		if (div_crd_ == 0)
			return color1_;

		auto crd = is_vertical_ ? y : x;

		if (crd == prev_crd_)
			return prev_result_;

		int tmp1 = (crd - crd1_);

		int r = tmp1 * diff_r_ / div_crd_ + color1_.r;
		int g = tmp1 * diff_g_ / div_crd_ + color1_.g;
		int b = tmp1 * diff_b_ / div_crd_ + color1_.b;

		if (r < 0) r = 0;
		if (r > 255) r = 255;
		if (g < 0) g = 0;
		if (g > 255) g = 255;
		if (b < 0) b = 0;
		if (b > 255) b = 255;

		prev_result_ = Color(r, g, b);
		prev_crd_ = crd;

		return prev_result_;
	}

	Anisotropy get_anisotropy() const override
	{
		return is_vertical_ ? Anisotropy::Vertical : Anisotropy::Horizontal;
	}

	void init(Crd x1, Crd y1, Crd x2, Crd y2) override
	{
		crd1_ = is_vertical_ ? y1 : x1;
		div_crd_ = is_vertical_ ? (y2 - y1) : (x2 - x1);
		prev_crd_ = -1;
		prev_result_ = {};
	}

	void set(Color color1, Color color2, bool is_vertical)
	{
		color1_ = color1;
		is_vertical_ = is_vertical;
		diff_r_ = (int)color2.r - (int)color1.r;
		diff_g_ = (int)color2.g - (int)color1.g;
		diff_b_ = (int)color2.b - (int)color1.b;
	}

private:
	Color color1_;
	int diff_r_ = 0;
	int diff_g_ = 0;
	int diff_b_ = 0;
	Crd crd1_ = 0;
	int div_crd_ = 0;
	bool is_vertical_ = false;
	mutable Color prev_result_;
	mutable Crd prev_crd_ = -1;
};

static SolidColorProvider solid_fg_color = Color(255, 255, 255);
static GradientProvider fg_gradient;

static SolidColorProvider solid_bg_color = Color(0, 0, 0);
static GradientProvider bg_gradient;

static ColorProvider* fg = &solid_fg_color;
static ColorProvider* bg = &solid_bg_color;

void set_fg_color(Color color)
{
	solid_fg_color.set(color);
	fg = &solid_fg_color;
}

void set_fg_transparent()
{
	fg = nullptr;
}

void set_bg_transparent()
{
	bg = nullptr;
}

void set_fg_vert_grad(Color color1, Color color2)
{
	fg_gradient.set(color1, color2, true);
	fg = &fg_gradient;
}

void set_fg_horiz_grad(Color color1, Color color2)
{
	fg_gradient.set(color1, color2, false);
	fg = &fg_gradient;
}

void set_bg_color(Color color)
{
	solid_bg_color.set(color);
	bg = &solid_bg_color;
}

void set_bg_vert_grad(Color color1, Color color2)
{
	bg_gradient.set(color1, color2, true);
	bg = &bg_gradient;
}

void set_bg_horiz_grad(Color color1, Color color2)
{
	bg_gradient.set(color1, color2, false);
	bg = &bg_gradient;
}

static void set_fg_pixel(Crd x, Crd y, const DisplayRect &clip_rect)
{
	if (!fg) return;
	if (!is_point_visible(x, y, clip_rect)) return;
	display_set_pixel(x, y, fg->get(x, y));
}

static void set_bg_pixel(Crd x, Crd y, const DisplayRect &clip_rect)
{
	if (!bg) return;
	if (!is_point_visible(x, y, clip_rect)) return;
	display_set_pixel(x, y, bg->get(x, y));
}

static void before_draw_figure(const Rect &rect)
{
	if (bg) bg->init(rect.left, rect.top, rect.right, rect.bottom);
	if (fg) fg->init(rect.left, rect.top, rect.right, rect.bottom);
}

#elif defined(MGFXPP_MONO)

static Color fg = Color::PixelOn;
static Color bg = Color::PixelOff;

void set_fg_color(Color color)
{
	fg = color;
}

void set_bg_color(Color color)
{
	bg = color;
}

void set_fg_transparent()
{
	fg = Color::Tansparent;
}

void set_bg_transparent()
{
	bg = Color::Tansparent;
}


static void before_draw_figure(const Rect &rect) {}

static void set_fg_pixel(Crd x, Crd y, const DisplayRect &clip_rect)
{
	if (fg == Color::Tansparent) return;
	if (!is_point_visible(x, y, clip_rect)) return;
	display_set_pixel(x, y, fg);
}

static void set_bg_pixel(Crd x, Crd y, const DisplayRect &clip_rect)
{
	if (bg == Color::Tansparent) return;
	if (!is_point_visible(x, y, clip_rect)) return;
	display_set_pixel(x, y, bg);
}

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Drawing */

void set_pixel(Crd x, Crd y, Color color)
{
	if (!is_point_visible(x, y, display_get_clip_rect())) return;
	display_set_pixel(x, y, color);
}

void draw_line_impl(Crd x1, Crd y1, Crd x2, Crd y2)
{
	// check line is visible on screen
	const DisplayRect &disp_clip_rect = display_get_clip_rect();
	if (!is_rect_visible(Rect(x1, y1, x2, y2), disp_clip_rect)) return;

	// simple cases

	if ((x1 == x2) && (y1 == y2))
		set_fg_pixel(x1, y1, disp_clip_rect);

	// draw line

	else
	{
		unsigned horiz_dist = abs(x1 - x2);
		unsigned vert_dist = abs(y1 - y2);
		Crd* crd1 = nullptr;
		Crd* crd2 = nullptr;
		unsigned cnt = 0;
		Crd crd1_incr = 0;
		Crd crd2_incr = 0;
		Crd d = 0;
		Crd d_incr = 0;
		Crd d_decr= 0;

		if (horiz_dist > vert_dist)
		{
			crd1 = &x1;
			crd2 = &y1;
			cnt = horiz_dist + 1;
			crd1_incr = (x2 > x1) ? 1 : -1;
			crd2_incr = (y2 > y1) ? 1 : -1;
			d = 2 * vert_dist - horiz_dist;
			d_decr = 2 * horiz_dist;
			d_incr = 2 * vert_dist;
		}
		else
		{
			crd1 = &y1;
			crd2 = &x1;
			cnt = vert_dist + 1;
			crd1_incr = (y2 > y1) ? 1 : -1;
			crd2_incr = (x2 > x1) ? 1 : -1;

			d = 2 * horiz_dist - vert_dist;
			d_decr = 2 * vert_dist;
			d_incr = 2 * horiz_dist;
		}

		while (cnt)
		{
			set_fg_pixel(x1, y1, disp_clip_rect);

			*crd1 += crd1_incr;

			if (d > 0)
			{
				d -= d_decr;
				*crd2 += crd2_incr;
			}

			d += d_incr;

			--cnt;
		}
	}
}

void draw_line(Crd x1, Crd y1, Crd x2, Crd y2)
{
	before_draw_figure({ min(x1, x2), min(y1, y2), max(x1, x2), max(y1, y2) });
	draw_line_impl(x1, y1, x2, y2);
}

static void fill_rect_impl(const DisplayRect &rect)
{
#if defined(MGFXPP_COLOR)
	if (!bg) return;

	switch (bg->get_anisotropy())
	{
	case Anisotropy::None:
		display_fill_rect(rect, bg->get(rect.left, rect.top));
		break;

	case Anisotropy::Vertical:
		for (auto y = rect.top; y <= rect.bottom; y++)
		{
			auto color = bg->get(rect.left, y);
			display_fill_rect(DisplayRect(rect.left, y, rect.right, y), color);
		}
		break;

	case Anisotropy::Horizontal:
		for (auto x = rect.left; x <= rect.right; x++)
		{
			auto color = bg->get(x, rect.top);
			display_fill_rect(DisplayRect(x, rect.top, x, rect.bottom), color);
		}
		break;

	case Anisotropy::Both:
		for (auto x = rect.left; x <= rect.right; x++)
			for (auto y = rect.top; y <= rect.bottom; y++)
				display_set_pixel(x, y, bg->get(x, y));
		break;
	}

#elif defined(MGFXPP_MONO)
	if (bg == Color::Tansparent) return;
	display_fill_rect(rect, bg);
#endif
}

void fill_rect(const Rect& rect)
{
	const DisplayRect &disp_clip_rect = display_get_clip_rect();
	if (!is_rect_visible(rect, disp_clip_rect)) return;
	before_draw_figure(rect);
	fill_rect_impl(get_visible_rect(rect, disp_clip_rect));
}


void fill_whole_display()
{
	fill_rect(Rect(0, 0, display_get_width()-1, display_get_height()-1));
}


static void draw_rect_impl(const Rect& rect)
{
	draw_line_impl(rect.left, rect.top, rect.left, rect.bottom);
	draw_line_impl(rect.left, rect.top, rect.right, rect.top);
	draw_line_impl(rect.right, rect.top, rect.right, rect.bottom);
	draw_line_impl(rect.left, rect.bottom, rect.right, rect.bottom);
}


void draw_rect(const Rect& rect)
{
	const DisplayRect &disp_clip_rect = display_get_clip_rect();
	if (!is_rect_visible(rect, disp_clip_rect)) return;
	before_draw_figure(rect);
	draw_rect_impl(rect);
}


void draw_and_fill_rect(const Rect& rect)
{
	const DisplayRect &disp_clip_rect = display_get_clip_rect();
	if (!is_rect_visible(rect, disp_clip_rect)) return;

	before_draw_figure(rect);

	draw_rect_impl(rect);

	Rect rect_to_fill(
		rect.left+1,
		rect.top+1,
		rect.right-1,
		rect.bottom-1
	);

	if ((rect_to_fill.left >= rect_to_fill.right) || (rect_to_fill.top >= rect_to_fill.bottom)) return;

	fill_rect_impl(get_visible_rect(rect_to_fill, disp_clip_rect));
}

static void fill_ellipse_impl(Crd left, Crd top, Crd right, Crd bottom)
{
	const DisplayRect& display_clip_rect = display_get_clip_rect();
	const Crd left_lim = max((Crd)display_clip_rect.left, left);
	const Crd right_lim = min((Crd)display_clip_rect.right, right);
	const Crd top_lim = max((Crd)display_clip_rect.top, top);
	const Crd bottom_lim = min((Crd)display_clip_rect.bottom, bottom);

	// all coords bellow x2 for more precise

	const Crd a = right - left;
	const Crd b = bottom - top;
	const Crd cx = right + left - 1;
	const Crd cy = top + bottom - 1;
	const Crd a2 = a * a;
	const Crd b2 = b * b;
	const Crd ab2 = a2 * b2;

	for (Crd y = top_lim; y <= bottom_lim; y++)
	{
		const Crd ey = 2 * y - cy;
		const Crd ey2 = ey * ey;
		const Crd ey2a2 = ey2 * a2;

		int x1 = -1;
		int x2 = -1;

		bool x1_defined = false;
		for (Crd x = left_lim; x < right_lim; x++)
		{
			const Crd ex = 2 * x - cx;
			const Crd ex2 = ex * ex;
			const Crd ex2b2 = ex2 * b2;

			bool pt_hit_in_ellipse = (ex2b2 + ey2a2) < ab2;

			if (pt_hit_in_ellipse && !x1_defined)
			{
				x1 = x;
				x1_defined = true;
			}
			else if (!pt_hit_in_ellipse && x1_defined)
			{
				x2 = x - 1;
				break;
			}
		}

		if (!x1_defined) continue;

		if (x2 == -1)
			x2 = right-1;

		fill_rect_impl(DisplayRect(x1, y, x2, y));
	}
}

void fill_ellipse(const Rect& rect)
{
	const DisplayRect &disp_clip_rect = display_get_clip_rect();
	if (!is_rect_visible(rect, disp_clip_rect)) return;
	before_draw_figure(rect);
	fill_ellipse_impl(rect.left, rect.top, rect.right, rect.bottom);
}

static void draw_ellipse_impl(Crd left, Crd top, Crd right, Crd bottom)
{
	const Crd a = right - left;
	const Crd b = bottom - top;
	const Crd x = right + left - 1;
	const Crd y = top + bottom - 1;

	const DisplayRect &disp_clip_rect = display_get_clip_rect();

	auto pixel4 = [&](Crd x, Crd y, Crd _x, Crd _y)
	{
		auto x_plus = (x + _x) / 2;
		auto x_minus = (x - _x) / 2;
		auto y_plus = (y + _y) / 2;
		auto y_minus = (y - _y) / 2;

		set_fg_pixel(x_plus, y_plus, disp_clip_rect);
		set_fg_pixel(x_plus, y_minus, disp_clip_rect);
		set_fg_pixel(x_minus, y_minus, disp_clip_rect);
		set_fg_pixel(x_minus, y_plus, disp_clip_rect);
	};

	Crd _x = 0;
	Crd _y = b;
	Crd a_sqr = a * a;
	Crd b_sqr = b * b;

	Crd delta = 4 * b_sqr * ((_x + 1) * (_x + 1)) + a_sqr * ((2 * _y - 1) * (2 * _y - 1)) - 4 * a_sqr * b_sqr;
	while (a_sqr * (2 * _y - 1) > 2 * b_sqr * (_x + 1))
	{
		pixel4(x, y, _x, _y);
		if (delta < 0)
		{
			_x++;
			delta += 4 * b_sqr * (2 * _x + 3);
		}
		else
		{
			_x++;
			delta = delta - 8 * a_sqr * (_y - 1) + 4 * b_sqr * (2 * _x + 3);
			_y--;
		}
	}

	delta = b_sqr * ((2 * _x + 1) * (2 * _x + 1)) + 4 * a_sqr * ((_y + 1) * (_y + 1)) - 4 * a_sqr * b_sqr;
	while (_y + 1 != 0)
	{
		pixel4(x, y, _x, _y);
		if (delta < 0)
		{
			_y--;
			delta += 4 * a_sqr * (2 * _y + 3);
		}
		else
		{
			_y--;
			delta = delta - 8 * b_sqr * (_x + 1) + 4 * a_sqr * (2 * _y + 3);
			_x++;
		}
	}
}

void draw_ellipse(const Rect &rect)
{
	const DisplayRect &disp_clip_rect = display_get_clip_rect();
	if (!is_rect_visible(rect, disp_clip_rect)) return;
	before_draw_figure(rect);
	draw_ellipse_impl(rect.left, rect.top, rect.right, rect.bottom);
}

void draw_and_fill_ellipse(const Rect &rect)
{
	const DisplayRect &disp_clip_rect = display_get_clip_rect();
	if (!is_rect_visible(rect, disp_clip_rect)) return;
	before_draw_figure(rect);
	fill_ellipse_impl(rect.left, rect.top, rect.right, rect.bottom);
	draw_ellipse_impl(rect.left, rect.top, rect.right, rect.bottom);
}

void fill_triangle(Crd x1, Crd y1, Crd x2, Crd y2, Crd x3, Crd y3)
{
	const DisplayRect &disp_clip_rect = display_get_clip_rect();

	if (x1 > x2) { swap(x1, x2); swap(y1, y2); }
	if (x2 > x3) { swap(x2, x3); swap(y2, y3); }
	if (x1 > x3) { swap(x1, x3); swap(y1, y3); }

	Crd dx1 = x2 - x1;
	Crd dy1 = y2 - y1;
	Crd dy1_abs = dx1 ? (abs(dy1) % dx1) : 0;
	Crd step1 = y2 > y1 ? 1 : -1;
	Crd int1 = dx1 ? (dy1 / dx1) : 0;
	Crd cnt_y1 = dx1 / 2;
	Crd yy1 = y1;

	Crd dx2 = x3 - x1;
	Crd dy2 = y3 - y1;
	Crd dy2_abs = dx2 ? (abs(dy2) % dx2) : 0;
	Crd step2 = y3 > y1 ? 1 : -1;
	Crd int2 = dx2 ? (dy2 / dx2) : 0;
	Crd cnt_y2 = dx2 / 2;
	Crd yy2 = y1;

	for (Crd x = x1; x <= x3; x++)
	{
		if (x == x2)
		{
			dx1 = x3 - x2;
			if (dx1 == 0) break;
			dy1 = y3 - y2;
			dy1_abs = abs(dy1) % dx1;
			step1 = y3 > y2 ? 1 : -1;
			int1 = dy1 / dx1;
			cnt_y1 = dx1 / 2;
			yy1 = y2;
		}

		Rect rect { x, min(yy1, yy2), x, max(yy1, yy2) };
		if (is_rect_visible(rect, disp_clip_rect))
			fill_rect_impl(get_visible_rect(rect, disp_clip_rect));

		cnt_y1 -= dy1_abs;
		if (cnt_y1 < 0)
		{
			cnt_y1 += dx1;
			yy1 += step1;
		}
		yy1 += int1;

		cnt_y2 -= dy2_abs;
		if (cnt_y2 < 0)
		{
			cnt_y2 += dx2;
			yy2 += step2;
		}
		yy2 += int2;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Text drawing */

static Crd cur_text_x = 0;
static Crd cur_text_y = 0;
static const Font* cur_font = nullptr;

static void print_symbol(const SymbolData& symbol, const DisplayRect &clip_rect)
{
	unsigned y_cnt = (symbol.bmp_height + 7) / 8;

	const uint8_t* data = symbol.bitmap;

	int y = 0;
	for (unsigned i = 0; i < y_cnt; i++)
	{
		for (unsigned j = 0; j < symbol.bmp_width; j++)
		{
			uint8_t row_data = *data;

			for (unsigned k = 0; k < 8; k++)
			{
				if (row_data & 1)
					set_fg_pixel(cur_text_x + j, cur_text_y + y + k + symbol.bmp_top, clip_rect);

				row_data >>= 1;
			}
			data++;
		}

		y += 8;
	}
}

static void print_char(uint32_t chr, const DisplayRect &clip_rect)
{
	if (cur_font == nullptr) return;

	auto* font_chr = font_find_charaster(*cur_font, chr);
	if (!font_chr) return;

	SymbolData symbol_data;

	font_get_symbol_data(*cur_font, font_chr, symbol_data);

	Rect char_rect {
		cur_text_x,
		cur_text_y,
		cur_text_x + symbol_data.width-1,
		cur_text_y + cur_font->height-1
	};

	if (is_rect_visible(char_rect, clip_rect))
		print_symbol(symbol_data, clip_rect);

	cur_text_x += symbol_data.width;
}

struct DisplayCharDest : CharDest
{
	DisplayCharDest(const DisplayRect &clip_rect) :
		clip_rect(clip_rect)
	{}

	void consume(Char chr) override
	{
		print_char(chr, clip_rect);
	}

	const DisplayRect &clip_rect;
};

struct CalcWidthCharDest : CharDest
{
	void consume(Char chr) override
	{
		if (cur_font == nullptr) return;

		auto* font_char = font_find_charaster(*cur_font, chr);
		if (!font_char) return;

		SymbolData symbol_data;
		font_get_symbol_data(*cur_font, font_char, symbol_data);
		result_ += symbol_data.width;
		last_spacing_ = symbol_data.spacing;
	}

	uint32_t get_width() const
	{
		return result_ - last_spacing_;
	}

private:
	uint32_t result_ = 0;
	uint8_t last_spacing_ = 0;
};

void set_current_font(const Font& font)
{
	cur_font = &font;
}

const Font* get_current_font()
{
	return cur_font;
}

void set_text_draw_pos(Crd x, Crd y)
{
	cur_text_x = x;
	cur_text_y = y;
}

void get_text_draw_pos(Crd* x, Crd* y)
{
	if (x) *x = cur_text_x;
	if (y) *y = cur_text_y;
}

void draw_any_text_impl(CharSource& src)
{
	int text_width = get_any_text_width(src);
	if (text_width == 0) return;

	Rect rect {
		cur_text_x,
		cur_text_y,
		cur_text_x + text_width - 1,
		cur_text_y + cur_font->height - 1
	};

	const DisplayRect &disp_clip_rect = display_get_clip_rect();
	if (!is_rect_visible(rect, disp_clip_rect)) return;

	before_draw_figure(rect);
	fill_rect_impl(get_visible_rect(rect, disp_clip_rect));

	DisplayCharDest display { disp_clip_rect };
	src.produce(display);
}

void draw_any_text_rect_impl(const Rect& rect, HorizAlign h_align, Crd h_padding, CharSource& any)
{
	if (cur_font == nullptr) return;

	const DisplayRect &disp_clip_rect = display_get_clip_rect();
	if (!is_rect_visible(rect, disp_clip_rect)) return;

	Crd y = (rect.top + rect.bottom - (Crd)cur_font->height + 1) / 2;
	Crd x = 0;

	fill_rect(rect);

	switch (h_align)
	{
	case HorizAlign::Left:
		x = rect.left + h_padding;
		break;

	case HorizAlign::Center:
		x = (rect.left + rect.right - (int)get_any_text_width(any) + 1) / 2;
		if (x < rect.left) x = rect.left;
		break;

	case HorizAlign::Right:
		x = rect.right - (int)get_any_text_width(any) - h_padding;
		break;
	}

	DisplayCharDest display { disp_clip_rect };
	set_text_draw_pos(x, y);
	any.produce(display);
}

void draw_any_text(CharSource& src)
{
	draw_any_text_impl(src);
}

void draw_any_text_at(Crd x, Crd y, CharSource& src)
{
	set_text_draw_pos(x, y);
	draw_any_text_impl(src);
}

void draw_any_text_rect(const Rect& rect, HorizAlign h_align, Crd h_padding, CharSource& any)
{
	draw_any_text_rect_impl(rect, h_align, h_padding, any);
}

void draw_string(const char* text_utf8)
{
	Utf8CharSource utf8_str(text_utf8);
	draw_any_text_impl(utf8_str);
}

void draw_string_at(Crd x, Crd y, const char* text_utf8)
{
	Utf8CharSource utf8_str(text_utf8);
	set_text_draw_pos(x, y);
	draw_any_text_impl(utf8_str);
}

void draw_string_rect(
	const Rect&  rect,
	HorizAlign   h_align,
	Crd          h_padding,
	const char*  text_utf8)
{
	Utf8CharSource utf8_str(text_utf8);
	draw_any_text_rect_impl(rect, h_align, h_padding, utf8_str);
}

void draw_int(int value, int pt_pos, bool show_plus_sign)
{
	IntCharSource int_char_source(value, pt_pos, show_plus_sign);
	draw_any_text_impl(int_char_source);
}

void draw_int_at(Crd x, Crd y, int value, int pt_pos, bool show_plus_sign)
{
	IntCharSource int_char_source(value, pt_pos, show_plus_sign);
	set_text_draw_pos(x, y);
	draw_any_text_impl(int_char_source);
}

void draw_int_rect(const Rect& rect, HorizAlign h_align, Crd h_padding, int value, int pt_pos, bool show_plus_sign)
{
	IntCharSource int_char_source(value, pt_pos, show_plus_sign);
	draw_any_text_rect_impl(rect, h_align, h_padding, int_char_source);
}

unsigned get_any_text_width(CharSource& src)
{
	CalcWidthCharDest calc_width_dest;
	src.produce(calc_width_dest);
	return calc_width_dest.get_width();
}

unsigned get_string_width(const char* text_utf8)
{
	Utf8CharSource utf8_str(text_utf8);
	return get_any_text_width(utf8_str);
}


} // namespace mgfxpp