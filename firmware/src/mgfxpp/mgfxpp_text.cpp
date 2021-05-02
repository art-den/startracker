#include "mgfxpp_text.hpp"

namespace mgfxpp {

bool wide_char_callback(void* data, mf::WideChar character)
{
	auto* dest = (CharDest*)data;
	dest->consume(character);
	return true;
}

void Utf8CharSource::produce(CharDest& destination)
{
	mf::impl::Utf8Receiver utf8_rec{ wide_char_callback, &destination, 0, 0, '?', 0 };
	const char* str = utf8_text_;
	while (str && *str) mf::impl::utf8_char_callback(&utf8_rec, *str++);
}

void IntCharSource::produce(CharDest& destination)
{
	int value = value_;
	int pt_pos = pt_pos_;

	if (value < 0)
	{
		value = -value;
		destination.consume('-');
	}

	if (show_plus_sign_ && (value >= 0))
		destination.consume('+');

	int comp_value = 1000000000;

	if (pt_pos != -1) pt_pos = 9 - pt_pos;

	bool begin = true;
	do
	{
		wchar_t chr = '0';
		while (value >= comp_value)
		{
			value -= comp_value;
			chr++;
		}
		if ((chr != '0') || (pt_pos == 0)) begin = false;
		if (!begin || (comp_value == 1))
			destination.consume(chr);
		comp_value /= 10;
		if ((pt_pos != -1) && (pt_pos-- == 0))
			destination.consume('.');
	} while (comp_value != 0);
}


} // namespace mgfxpp
