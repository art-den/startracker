#pragma once

#include <stdint.h>
#include <stdarg.h>
#include "micro_format.hpp"

namespace mgfxpp {

using Char = uint32_t;

struct CharDest
{
	virtual void consume(Char chr) = 0;
};

struct CharSource
{
	virtual void produce(CharDest & destination) = 0;
};

struct Utf8CharSource : CharSource
{
	Utf8CharSource(const char* utf8_text) :
		utf8_text_(utf8_text)
	{}

	void produce(CharDest& destination) override;

private:
	const char* utf8_text_;
};

struct IntCharSource : CharSource
{
public:
	IntCharSource(int value, int pt_pos = -1, bool show_plus_sign = false) :
		value_(value),
		pt_pos_(pt_pos),
		show_plus_sign_(show_plus_sign)
	{}

	void produce(CharDest& destination) override;

private:
	const int value_;
	const int pt_pos_;
	const bool show_plus_sign_;
};

template <typename ... Args>
struct PrintfSource : CharSource
{
	PrintfSource(const char* format_utf8, const Args& ... args) :
		format_utf8_(format_utf8),
		args_arr_({args...})
	{}

	void produce(CharDest& destination) override
	{
		bool wide_char_callback(void* data, mf::WideChar character);
		mf::impl::Utf8Receiver utf8_rec { wide_char_callback, &destination, 0, 0, '?', 0 };
		mf::impl::FormatCtx ctx { { mf::impl::utf8_char_callback, &utf8_rec, 0 }, args_arr_, sizeof ... (Args) };
		mf::impl::format_impl(ctx, format_utf8_);
		printf_result_ = utf8_rec.chars_printed;
	}

	int get_printf_result() const
	{
		return printf_result_;
	}

private:
	const char* format_utf8_;
	const mf::impl::FormatArg args_arr_[sizeof ... (Args)];
	int printf_result_ = -1;
};

} // namespace mgfxpp