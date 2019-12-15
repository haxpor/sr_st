///
/// line - line5 from https://github.com/ssloy/tinyrenderer/wiki/Lesson-1:-Bresenham%E2%80%99s-Line-Drawing-Algorithm
#include "SR_Common.h"
#include <cmath>
#include <algorithm>

// define line implementation to choose 1-4
#define LINE_IMPL 7

// Floating-point operation (multiplication, and rounding), and the choice of constant stepping value
// are things needed to be improved.
static void line(const sr::Vec2i& start, const sr::Vec2i& end, sr::FrameBuffer& fb, unsigned int color)
{
    for (float t=0.0f; t<1.0f; t+=0.01f)
    {
        int x = start.x + (end.x-start.x) * t;
        int y = start.y + (end.y-start.y) * t;
        fb.set(x, y, color);
    }
}

// more independent computed value of t, and used for both lerping for resultant x, and y
static void line2(sr::Vec2i start, sr::Vec2i end, sr::FrameBuffer& fb, unsigned int color)
{
    for (int x=start.x; x <= end.x; ++x)
    {
        float t = (x - start.x) / static_cast<float>(end.x - start.x); 
        int y = start.y + (end.y - start.y)*t;      //the same as `int y = start.y * (1.0f - t) + end.y*t;`
        fb.set(x, y, color);
    }
}

static void line3(sr::Vec2i start, sr::Vec2i end, sr::FrameBuffer& fb, unsigned int color)
{
    bool steep = false;
    if (std::abs(start.x - end.x) < std::abs(start.y - end.y))          // choose range that has more width
    {
        std::swap(start.x, start.y);
        std::swap(end.x, end.y);
        steep = true;
    }
    if (start.x > end.x)            // make it left-to-right if left is greater than right
    {
        std::swap(start.x, end.x);
        std::swap(start.y, end.y);
    }
    for (int x=start.x; x <= end.x; ++x)
    {
        float t = (x - start.x) / static_cast<float>(end.x - start.x);
        int y = start.y + (end.y - start.y)*t;
        if (steep)
            fb.set(y, x, color);        // if we swapped the range, then we still need to set pixel at correct position
        else
            fb.set(x, y, color);
    }
}

static void line4(sr::Vec2i start, sr::Vec2i end, sr::FrameBuffer& fb, unsigned int color)
{
    bool steep = false;
    if (std::abs(start.x - end.x) < std::abs(start.y - end.y))          // choose range that has more width
    {
        std::swap(start.x, start.y);
        std::swap(end.x, end.y);
        steep = true;
    }
    if (start.x > end.x)            // make it left-to-right if left is greater than right
    {
        std::swap(start.x, end.x);
        std::swap(start.y, end.y);
    }
    const int dx = end.x - start.x;
    const int dy = end.y - start.y;

    for (int x=start.x; x <= end.x; ++x)
    {
        float t = (x - start.x) / static_cast<float>(end.x - start.x);
        int y = start.y + (end.y - start.y)*t;
        if (steep)
            fb.set(y, x, color);        // if we swapped the range, then we still need to set pixel at correct position
        else
            fb.set(x, y, color);
    }
}

/// not cover all cases yet i.e. starting point is greater than the ending point
/// line6 - from https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
static void line6(sr::Vec2i start, sr::Vec2i end, sr::FrameBuffer& fb, unsigned int color)
{
    const int dx = end.x - start.x;
    const int dy = end.y - start.y;
    int D = 2*dy - dx;      // only determine the sign, then multiply by 2 has no side-effect, as initial value has 1/2, we multiply by 2
                            // then we need to keep multiply by 2 for the less of the operation for D's related terms
    int y = start.y;

    for (int x = start.x; x <= end.x; ++x)
    {
        fb.set(x, y, color);
        if (D > 0)
        {
            ++y;
            D = D - 2*dx;
        }
        D = D + 2*dy;
    }
}

static void line7(sr::Vec2i start, sr::Vec2i end, sr::FrameBuffer& fb, unsigned int color)
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
    int D = 2*dy - dx;      // only determine the sign, then multiply by 2 has no side-effect, as initial value has 1/2, we multiply by 2
                            // then we need to keep multiply by 2 for the less of the operation for D's related terms
    int y = start.y;

    for (int x = start.x; x <= end.x; ++x)
    {
        if (steep)
            fb.set(y, x, color);
        else
            fb.set(x, y, color);

        if (D > 0)
        {
            ++y;
            D = D - 2*dx;
        }
        D = D + 2*dy;
    }
}

int main()
{
    sr::FrameBuffer fb(100, 100);

#if LINE_IMPL == 1
    line(sr::Vec2i(13, 20), sr::Vec2i(80, 40), fb, sr::makeColorARGB(255, 255, 255));
#elif LINE_IMPL == 2
    // this will produce no 3rd line
    // 2nd line is not smooth enough
    line2(sr::Vec2i(13, 20), sr::Vec2i(80, 40), fb, sr::makeColorARGB(255, 255, 255));
    line2(sr::Vec2i(20, 13), sr::Vec2i(40, 80), fb, sr::makeColorARGB(255, 0, 0));
    line2(sr::Vec2i(80, 40), sr::Vec2i(13, 20), fb, sr::makeColorARGB(255, 0, 0));
#elif LINE_IMPL == 3
    line3(sr::Vec2i(13, 20), sr::Vec2i(80, 40), fb, sr::makeColorARGB(255, 255, 255));
    line3(sr::Vec2i(20, 13), sr::Vec2i(40, 80), fb, sr::makeColorARGB(255, 0, 0));
    line3(sr::Vec2i(80, 40), sr::Vec2i(13, 20), fb, sr::makeColorARGB(255, 0, 0));
#elif LINE_IMPL == 6
    line6(sr::Vec2i(13, 20), sr::Vec2i(80, 40), fb, sr::makeColorARGB(255, 255, 255));
    line6(sr::Vec2i(80, 40), sr::Vec2i(13, 20), fb, sr::makeColorARGB(255, 0, 0));      // replace white line, but it should not work
#elif LINE_IMPL == 7
    line7(sr::Vec2i(13, 20), sr::Vec2i(80, 40), fb, sr::makeColorARGB(255, 255, 255));
    line7(sr::Vec2i(80, 40), sr::Vec2i(13, 20), fb, sr::makeColorARGB(255, 0, 0));      // replace one line
#endif

    sr::TGAImage::write24("out.tga", fb.getFrameBuffer(), fb.getWidth(), fb.getHeight());
    return 0;
}
