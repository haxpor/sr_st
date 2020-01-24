#pragma once

#include "Platform.h"
#include "Types.h"
#include <algorithm>

SR_NAMESPACE_START

///
/// Make color in format ARGB
inline constexpr unsigned int makeColorARGB(unsigned char a, unsigned char r, unsigned char g, unsigned char b)
{
    return (a << 24) | (r << 16) | (g << 8) | b;
}

///
/// Make color in format ARGB
inline constexpr unsigned int makeColorARGB(unsigned char r, unsigned char g, unsigned char b)
{
    return (0xFF << 24) | (r << 16) | (g << 8) | b;
}

///
/// Make color in format ARGB as packed single integer type.
inline constexpr unsigned int makeColorPacked(unsigned char a, unsigned char r, unsigned char g, unsigned char b)
{
    return (a << 24) | (r << 16) | (g << 8) | b;
}

///
/// Make color in format ARGB as packed single integer type.
inline constexpr unsigned int makeColorPacked(unsigned char r, unsigned char g, unsigned char b)
{
    return (0xFF << 24) | (r << 16) | (g << 8) | b;
}

///
/// Make color in format ARGB as packed single integer type with R/G/B component all have value of `v`
/// with opaque value of A compnent.
inline constexpr unsigned int makeColorPacked(unsigned char v)
{
    return (0xFF << 24) | (v << 16) | (v << 8) | v;
}

///
/// Make color in format ARGB with R/G/B components all have value of `v` with opaque value of A component.
/// Return as type sr::Color32i.
inline sr::Color32i makeColor32i(unsigned char v)
{
    return sr::Color32i(v, v, v);
}

///
/// Make color in format ARGB
/// Return as type sr::Color32i
inline sr::Color32i makeColor32i(unsigned char r, unsigned char g, unsigned char b)
{
    return sr::Color32i(r, g, b);
}

SR_NAMESPACE_END
