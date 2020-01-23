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
