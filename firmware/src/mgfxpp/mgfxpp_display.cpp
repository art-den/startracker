#include "mgfxpp_display.hpp"
#include "mgfxpp_utils.hpp"

namespace mgfxpp {

void display_fill_rect_default(const DisplayRect &rect, Color color)
{
	for (auto x = rect.left; x <= rect.right; x++)
		for (auto y = rect.top; y <= rect.bottom; y++)
			display_set_pixel(x, y, color);
}

} // namespace mgfxpp