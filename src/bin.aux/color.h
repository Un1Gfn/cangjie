#pragma once

#include <assert.h>
#include <stdint.h>

#define ColorCairo CAIRO_FORMAT_ARGB32

// #define ColorFB XXX

typedef uint32_t Color;

#define ColorBits 32

#define ColorBytes 4

static_assert(ColorBytes==sizeof(Color));
