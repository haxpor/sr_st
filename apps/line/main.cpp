///
/// line - line5 from https://github.com/ssloy/tinyrenderer/wiki/Lesson-1:-Bresenham%E2%80%99s-Line-Drawing-Algorithm
#include "SR_Common.h"
#include <cmath>
#include <algorithm>

// define line implementation to choose 1-7
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

    const float kDError = std::abs(dy / static_cast<float>(dx));      // divide by dx in the equation, it doesn't have the consequence as we just want to check the sign. This will leave 0.5 term. dy can be negative, but dx is always positive as we already swap greater value to be at the end
    float error = 0.0f;
    int y = start.y;

    for (int x=start.x; x <= end.x; ++x)
    {
        if (steep)
            fb.set(y, x, color);        // if we swapped the range, then we still need to set pixel at correct position
        else
            fb.set(x, y, color);
        error += kDError;
        if (error > 0.5f)
        {
            // decide to go up or down
            y += end.y > start.y ? 1 : -1;
            error -= 1.0f;              // in each time of either choosing higher up y-position, error will raise for -1.0f as we already optimized out dx term
        }
    }
}

/// other previous implmentations cost ~15ms but this one reduce it to 0ms as profiled
/// integer operation is faster than floating-point operation
static void line5(sr::Vec2i start, sr::Vec2i end, sr::FrameBuffer& fb, unsigned int color)
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

    const int kDError = std::abs(dy) * 2;      // multiply by 2 for difference terms to eliminate 0.5 and to deal with floating point in the code, it doesn't have the consequence as we just want to check the sign. This will leave 0.5 term
    const int kIncrementError = 2*dx;
    int error = 0;
    int y = start.y;

    for (int x=start.x; x <= end.x; ++x)
    {
        if (steep)
            fb.set(y, x, color);        // if we swapped the range, then we still need to set pixel at correct position
        else
            fb.set(x, y, color);
        error += kDError;
        if (error > dx)
        {
            // decide to go up or down
            y += end.y > start.y ? 1 : -1;
            error -= kIncrementError;              // in each time of either choosing higher up y-position, error will raise for -2*dx as we already optimized out dx term
        }
    }
}

/// not cover all cases yet i.e. starting point is greater than the ending point
/// line6 - from https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
/// note: this one should not be used in real-case as it only covers for a partial range, it's for starting point
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

/// 
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

int main()
{
    sr::FrameBuffer fb(100, 100);

    sr::Profile::start();
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
#elif LINE_IMPL == 4
    line4(sr::Vec2i(13, 20), sr::Vec2i(80, 40), fb, sr::makeColorARGB(255, 255, 255));
    line4(sr::Vec2i(20, 13), sr::Vec2i(40, 80), fb, sr::makeColorARGB(255, 0, 0));
    line4(sr::Vec2i(80, 40), sr::Vec2i(13, 20), fb, sr::makeColorARGB(255, 0, 0));
#elif LINE_IMPL == 5
    line5(sr::Vec2i(13, 20), sr::Vec2i(80, 40), fb, sr::makeColorARGB(255, 255, 255));
    line5(sr::Vec2i(20, 13), sr::Vec2i(40, 80), fb, sr::makeColorARGB(255, 0, 0));
    line5(sr::Vec2i(80, 40), sr::Vec2i(13, 20), fb, sr::makeColorARGB(255, 0, 0));
#elif LINE_IMPL == 6
    line6(sr::Vec2i(13, 20), sr::Vec2i(80, 40), fb, sr::makeColorARGB(255, 255, 255));
    line6(sr::Vec2i(20, 13), sr::Vec2i(40, 80), fb, sr::makeColorARGB(255, 0, 0));
    line6(sr::Vec2i(80, 40), sr::Vec2i(13, 20), fb, sr::makeColorARGB(255, 0, 0));
#elif LINE_IMPL == 7
    line7(sr::Vec2i(13, 20), sr::Vec2i(80, 40), fb, sr::makeColorARGB(255, 255, 255));
    line7(sr::Vec2i(20, 13), sr::Vec2i(40, 80), fb, sr::makeColorARGB(255, 0, 0));
    line7(sr::Vec2i(80, 40), sr::Vec2i(13, 20), fb, sr::makeColorARGB(255, 0, 0));
#endif
    sr::Profile::endAndPrint();

    sr::TGAImage::write24("out.tga", fb.getFrameBuffer(), fb.getWidth(), fb.getHeight());
    return 0;
}
