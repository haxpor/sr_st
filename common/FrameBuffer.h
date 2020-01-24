#pragma once

#include "Platform.h"

#include <vector>

SR_NAMESPACE_START

class FrameBuffer
{
public:
    typedef unsigned int type;
    typedef const unsigned int* const_pointer;

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
        if (x < 0 || x >= width || y < 0 || y >= height)
            return;
        frameBuffer[x + y*width] = color;
    }

    ///
    /// Get pixel value at target position
    inline unsigned int get(int x, int y) const
    {
        return frameBuffer[x + y*width];
    }

    inline unsigned int* getFrameBuffer()
    {
        return &frameBuffer[0];
    }

    inline const unsigned int* getFrameBuffer() const
    {
        return &frameBuffer[0];
    }

    inline unsigned int operator[](int i) const
    {
        return frameBuffer[i];
    }

    inline unsigned int& operator[](int i)
    {
        return frameBuffer[i];
    }

    inline int getWidth() const { return width; }
    inline int getHeight() const { return height; }

private:
    int width;
    int height;

    // 32-bit pixel format ARGB
    std::vector<unsigned int> frameBuffer;
};

SR_NAMESPACE_END
