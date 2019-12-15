#pragma once

#include "Platform.h"

SR_NAMESPACE_START

///
/// Make color in format ARGB
/// This will truncate input parameter to 8-bit then pack into unsigned int for color
static constexpr unsigned int makeColorARGB(int a, int r, int g, int b)
{
    return ((a&0xFF) << 24) | ((r&0xFF) << 16) | ((g&0xFF) << 8) | (b & 0xFF);
}

///
/// Make color in format ARGB
/// This will truncate input parameter to 8-bit then pack into unsigned int for color
static constexpr unsigned int makeColorARGB(int r, int g, int b)
{
    return ((0xFF) << 24) | ((r&0xFF) << 16) | ((g&0xFF) << 8) | (b & 0xFF);
}

SR_NAMESPACE_END
