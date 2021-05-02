#pragma once

// #define MGFXPP_COLOR
// #define MGFXPP_MONO

#if !defined(MGFXPP_COLOR) and !defined(MGFXPP_MONO)
#error MGFXPP_COLOR or MGFXPP_MONO must be defined
#endif

#if defined(MGFXPP_COLOR) and defined(MGFXPP_MONO)
#error You should only define MGFXPP_COLOR or MGFXPP_MONO not both
#endif