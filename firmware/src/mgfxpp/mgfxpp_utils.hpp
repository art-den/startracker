#pragma once

namespace mgfxpp {

template <typename T>
void swap(T &v1, T &v2)
{
	T tmp = v1;
	v1 = v2;
	v2 = tmp;
}

template <typename T>
T abs(const T value)
{
	return (value < 0) ? -value : value;
}

template <typename T>
T max(const T v1, const T v2)
{
	return (v1 > v2) ? v1 : v2;
}

template <typename T>
T min(const T v1, const T v2)
{
	return (v1 < v2) ? v1 : v2;
}

#define mgfxpp_assert(cond) if (!(cond)) for (;;) {}

} // namespace mgfxpp

