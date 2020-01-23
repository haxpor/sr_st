#pragma once

#include "Platform.h"
#include "Types.h"
#include "MathUtil.h"
#include <algorithm>

SR_NAMESPACE_START

///
/// Make color in format ARGB
static constexpr unsigned int makeColorARGB(unsigned char a, unsigned char r, unsigned char g, unsigned char b)
{
    return (a << 24) | (r << 16) | (g << 8) | b;
}

///
/// Make color in format ARGB
static constexpr unsigned int makeColorARGB(unsigned char r, unsigned char g, unsigned char b)
{
    return (0xFF << 24) | (r << 16) | (g << 8) | b;
}

///
/// Make color in format ARGB as packed single integer type.
static constexpr unsigned int makeColorPacked(unsigned char a, unsigned char r, unsigned char g, unsigned char b)
{
    return (a << 24) | (r << 16) | (g << 8) | b;
}

///
/// Make color in format ARGB as packed single integer type.
static constexpr unsigned int makeColorPacked(unsigned char r, unsigned char g, unsigned char b)
{
    return (0xFF << 24) | (r << 16) | (g << 8) | b;
}

///
/// Make color in format ARGB
/// Return as type sr::Color32i
static sr::Color32i makeColor32i(unsigned char r, unsigned char g, unsigned char b)
{
    return sr::Color32i(r, g, b);
}

///
/// Line rasterization function
static void line(sr::Vec2i start, sr::Vec2i end, sr::FrameBuffer& fb, unsigned int color)
{
    bool steep = false;
    if (std::abs(start.x - end.x) < std::abs(start.y - end.y))          // choose range that has more width
    {
        std::swap(start.x, start.y);
        std::swap(end.x, end.y);
        steep = true;
    }
    if (start.x > end.x)
    {
        std::swap(start.x, end.x);
        std::swap(start.y, end.y);
    }
    const int dx = end.x - start.x;
    const int dy = end.y - start.y;
    const int D = std::abs(2*dy);       // base difference constant on no changing of y, thus it's still at the same level
    int error = 0;
    const int errorIncrement = -2*dx;   // now dx is on the right side, and 2 is multiplied as initially we multiplied by 2 so the less should be the same
    int y = start.y;

    const int yStep = end.y > start.y ? 1 : -1;

    for (int x = start.x; x <= end.x; ++x)
    {
        if (steep)
            fb.set(y, x, color);
        else
            fb.set(x, y, color);

        error += D; 
        if (error > dx)                 // dx switched to the left side of the equation thus it's now positive
        {
            y += yStep;
            error += errorIncrement;
        }
    }
}

///
/// Optimized rasterizing of triangle routine.
static void triangle(sr::Vec2i t0, sr::Vec2i t1, sr::Vec2i t2, sr::FrameBuffer& fb, sr::Color32i color)
{
    const int maxW = fb.getWidth() - 1;
    const int maxH = fb.getHeight() - 1;

    // find the bounding box for the triangle
    sr::Vec2i bbMin(maxW, maxH);
    sr::Vec2i bbMax(0, 0);

    // find minimum x/y
    bbMin.x = std::max(0, std::min(bbMin.x, t0.x));
    bbMin.x = std::max(0, std::min(bbMin.x, t1.x));
    bbMin.x = std::max(0, std::min(bbMin.x, t2.x));

    bbMin.y = std::max(0, std::min(bbMin.y, t0.y));
    bbMin.y = std::max(0, std::min(bbMin.y, t1.y));
    bbMin.y = std::max(0, std::min(bbMin.y, t2.y));

    // find maximum x/y
    bbMax.x = std::min(maxW, std::max(bbMax.x, t0.x));
    bbMax.x = std::min(maxW, std::max(bbMax.x, t1.x));
    bbMax.x = std::min(maxW, std::max(bbMax.x, t2.x));

    bbMax.y = std::min(maxH, std::max(bbMax.y, t0.y));
    bbMax.y = std::min(maxH, std::max(bbMax.y, t1.y));
    bbMax.y = std::min(maxH, std::max(bbMax.y, t2.y));

    sr::Vec2i p;
    for (p.x = bbMin.x; p.x<=bbMax.x; ++p.x)
    {
        for (p.y = bbMin.y; p.y<=bbMax.y; ++p.y)
        {
            sr::Vec3f bcScreen = sr::MathUtil::barycentric(t0, t1, t2, p);
            if (bcScreen.x < 0.0f || bcScreen.y < 0.0f || bcScreen.z < 0.0f)
                continue;
            fb.set(p.x, p.y, color.packed);
        }
    } 
}

SR_NAMESPACE_END
