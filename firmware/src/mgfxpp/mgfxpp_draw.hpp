#pragma once

#include <stdint.h>
#include "mgfxpp_types.hpp"
#include "mgfxpp_font.hpp"
#include "mgfxpp_text.hpp"

namespace mgfxpp {

struct DisplayRect;

bool is_rect_visible(const Rect &rect, const DisplayRect &clip_rect);
bool is_point_visible(Crd x, Crd y, const DisplayRect &clip_rect);

Rect get_display_rect();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void set_fg_color(Color color);
void set_bg_color(Color color);

void set_fg_transparent();
void set_bg_transparent();

#if defined(MGFXPP_COLOR)

void set_fg_vert_grad(Color color1, Color color2);
void set_fg_horiz_grad(Color color1, Color color2);
void set_bg_vert_grad(Color color1, Color color2);
void set_bg_horiz_grad(Color color1, Color color2);

#endif

/* Drawing */

void set_pixel(Crd x, Crd y, Color color);

void draw_line(Crd x1, Crd y1, Crd x2, Crd y2);

void fill_rect(const Rect &rect);
void fill_whole_display();

void draw_rect(const Rect& rect);

void draw_and_fill_rect(const Rect& rect);


void fill_ellipse(const Rect& rect);
void draw_ellipse(const Rect& rect);
void draw_and_fill_ellipse(const Rect& rect);

void fill_triangle(Crd x1, Crd y1, Crd x2, Crd y2, Crd x3, Crd y3);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct CharSource;

enum class HorizAlign
{
	Left,
	Center,
	Right
};

void set_current_font(const Font& font);
const Font* get_current_font();

void set_text_draw_pos(Crd x, Crd y);
void get_text_draw_pos(Crd *x, Crd *y);


/* Draw any */

void draw_any_text(CharSource& any);
void draw_any_text_at(Crd x, Crd y, CharSource& any);
void draw_any_text_rect(const Rect &rect, HorizAlign h_align, Crd h_padding, CharSource& any);


/* Draw utf8 null-terminarted string  */

void draw_string(const char* text_utf8);
void draw_string_at(Crd x, Crd y, const char* text_utf8);
void draw_string_rect(const Rect& rect, HorizAlign h_align, Crd h_padding, const char* text_utf8);


/* Draw integer value */

void draw_int(int value, int pt_pos = -1, bool show_plus_sign = false);
void draw_int_at(Crd x, Crd y, int value, int pt_pos = -1, bool show_plus_sign = false);
void draw_int_rect(const Rect& rect, HorizAlign h_align, Crd h_padding, int value, int pt_pos = -1, bool show_plus_sign = false);


/* Draw text produced by printf */

template <typename ... Args>
int printf(const char* format_utf8, const Args& ... args)
{
	PrintfSource<Args...> src(format_utf8, args...);
	draw_any_text(src);
	return src.get_printf_result();
}

template <typename ... Args>
int printf_at(Crd x, Crd y, const char* format_utf8, const Args& ... args)
{
	PrintfSource<Args...> src(format_utf8, args...);
	draw_any_text_at(x, y, src);
	return src.get_printf_result();
}

template <typename ... Args>
int printf_rect(const Rect& rect, HorizAlign h_align, Crd h_padding, const char* format_utf8, const Args& ... args)
{
	PrintfSource<Args...> src(format_utf8, args...);
	draw_any_text_rect(rect, h_align, h_padding, src);
	return src.get_printf_result();
}


/* Get text dimensions */

unsigned get_any_text_width(CharSource& src);
unsigned get_string_width(const char* text_utf8);



} // namespace mgfxpp