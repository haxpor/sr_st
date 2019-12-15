#pragma once

#include "Platform.h"

SR_NAMESPACE_START

///
/// Make color in format ARGB
/// This will truncate input parameter to 8-bit then pack into unsigned int for color
static constexpr unsigned int makeColorARGB(unsigned char a, unsigned char r, unsigned char g, unsigned char b)
{
    return (a << 24) | (r << 16) | (g << 8) | b;
}

///
/// Make color in format ARGB
/// This will truncate input parameter to 8-bit then pack into unsigned int for color
static constexpr unsigned int makeColorARGB(unsigned char r, unsigned char g, unsigned char b)
{
    return (r << 16) | (g << 8) | b;
}

SR_NAMESPACE_END
