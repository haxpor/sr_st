#pragma once

#include "Platform.h"

#include <vector>

SR_NAMESPACE_START

class FrameBuffer
{
public:
    FrameBuffer(int width, int height)
        : width(width)
        , height(height)
    {
        frameBuffer.resize(width * height, 0x0);
    }

    ///
    /// Set pixel value at target position
    inline void set(int x, int y, unsigned int color)
    {
        frameBuffer[x + y*width] = color;
    }

    ///
    /// Get pixel value at target position
    inline unsigned int get(int x, int y) const
    {
        return frameBuffer[x + y*width];
    }

private:
    int width;
    int height;

    // 32-bit pixel format ARGB
    std::vector<unsigned int> frameBuffer;
};

SR_NAMESPACE_END
