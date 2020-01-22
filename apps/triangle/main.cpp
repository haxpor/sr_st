#include "SR_Common.h"
#include <algorithm>

static sr::Color32i white = sr::makeColor32i(255, 255, 255);
static sr::Color32i green = sr::makeColor32i(0, 255, 0);
static sr::Color32i red = sr::makeColor32i(255, 0, 0);

static void triangle(sr::Vec2i t0, sr::Vec2i t1, sr::Vec2i t2, sr::FrameBuffer& fb, unsigned int color)
{
    if (t0.y > t1.y) std::swap(t0, t1);
    if (t0.y > t2.y) std::swap(t0, t2);
    if (t1.y > t2.y) std::swap(t1, t2);

    int totalHeight = t2.y - t0.y;

    // render the bottom half of the triangle
    for (int y = t0.y; y <= t1.y; y++) {
        int segmentHeight = t1.y - t0.y +1;

        float alpha = static_cast<float>(y - t0.y) / totalHeight;
        float beta  = static_cast<float>(y - t0.y) / segmentHeight;

        sr::Vec2i A = t0 + (t2 - t0) * alpha;
        sr::Vec2i B = t0 + (t1 - t0) * beta;

        if (A.x > B.x) std::swap(A, B);

        for (int x = A.x; x <= B.x; ++x)
        {
            fb.set(x, y, color);
        }
    }

    // render the upper half of the triangle
    for (int y = t1.y; y <= t2.y; ++y)
    {
        int segmentHeight = t2.y - t1.y + 1;

        float alpha = static_cast<float>(y - t0.y) / totalHeight;
        float beta = static_cast<float>(y - t1.y) / segmentHeight;

        sr::Vec2i A = t0 + (t2 - t0) * alpha;
        sr::Vec2i B = t1 + (t2 - t1) * beta;

        if (A.x > B.x) std::swap(A, B);

        for (int x = A.x; x <= B.x; ++x)
        {
            fb.set(x, y, color);
        }
    }
}

/// optimized version from triangle
static void triangle2(sr::Vec2i t0, sr::Vec2i t1, sr::Vec2i t2, sr::FrameBuffer& fb, unsigned int color)
{
    if (t0.y == t1.y && t0.y == t2.y) return;

    if (t0.y > t1.y) std::swap(t0, t1);
    if (t0.y > t2.y) std::swap(t0, t2);
    if (t1.y > t2.y) std::swap(t1, t2);

    const int totalHeight = t2.y - t0.y;
    const int deltaFirstHalf = t1.y - t0.y;
    const int deltaSecondHalf = t2.y - t1.y;
    const bool equal = t1.y == t0.y;
    const sr::Vec2i deltaPoint1 = t1 - t0;
    const sr::Vec2i deltaPoint2 = t2 - t1;
    const sr::Vec2i deltaPoint3 = t2 - t0;

    // render the upper half of the triangle
    for (int y=0; y < totalHeight; ++y)
    {
        bool isSecondHalf = y > deltaFirstHalf || equal;
        int segmentHeight = isSecondHalf ? deltaSecondHalf : deltaFirstHalf;

        float alpha = static_cast<float>(y) / totalHeight;
        float beta = static_cast<float>(y - (isSecondHalf ? deltaFirstHalf : 0)) / segmentHeight;

        sr::Vec2i A = t0 + deltaPoint3 * alpha;
        sr::Vec2i B = isSecondHalf ? t1 + (deltaPoint2) * beta : t0 + (deltaPoint1) * beta;

        if (A.x > B.x) std::swap(A, B);

        for (int x = A.x; x <= B.x; ++x)
        {
            fb.set(x, t0.y + y, color);
        }
    }
}

// barycentric calculation which assume it operates on 2d plane.
// It accepts only 2d position but internally will automatically supply 0.0f for 3rd dimension.
// Return barycentric coordinate.
static sr::Vec3f barycentric(const sr::Vec2i& p1, const sr::Vec2i& p2, const sr::Vec2i& p3, const sr::Vec2i& p)
{
    // ref: https://en.wikipedia.org/wiki/Barycentric_coordinate_system at "Conversion between
    // barycentric and Cartesian coordinates" section in the form of T x alpha = r - r3.
    // In short, it drills down to finding inverse 2x2 matrix.
    //
    // Note that z dimension is simplified to be 0.
    float det = (p1.x - p3.x) + (p3.x - p2.x);
    float l1 = ((p.x - p3.x) + (p3.x - p2.x) * p.y) / det;
    float l2 = ((p.x - p3.x) + (p1.x - p3.x) * p.y) / det;
    float l3 = 1.0f - l1 - l2;
    return sr::Vec3f(l1, l2, l3);
}

// More optimize and different approach in rendering triangle.
// Assume that input vertices are within the boundary of framebuffer's resolution.
// This function works with barycentric() function.
static void triangle3(sr::Vec2i t0, sr::Vec2i t1, sr::Vec2i t2, sr::FrameBuffer& fb, sr::Color32i color)
{
    const int maxW = fb.getWidth() - 1;
    const int maxH = fb.getHeight() - 1;

    // find the bounding box for the triangle
    sr::Vec2i bbMin(maxW, maxH);
    sr::Vec2i bbMax(0, 0);

    // find minimum x/y
    bbMin.x = std::max(0, std::min({bbMin.x, t0.x, t1.x, t2.x}));
    bbMin.y = std::max(0, std::min({bbMin.y, t0.y, t1.y, t2.y}));

    // find maximum x/y
    bbMax.x = std::min(maxW, std::max({bbMax.x, t0.x, t1.x, t2.x}));
    bbMax.y = std::max(maxH, std::max({bbMax.y, t0.y, t1.y, t2.y}));

    sr::Vec2i p;
    for (p.x = bbMin.x; p.x<=bbMax.x; ++p.x)
    {
        for (p.y = bbMin.y; p.y<=bbMax.y; ++p.y)
        {
            sr::Vec3f bcScreen = barycentric(t0, t1, t2, p);
            if (bcScreen.x < 0.0f || bcScreen.y < 0.0f || bcScreen.z < 0.0f)
                continue;
            fb.set(p.x, p.y, color.packed);
        }
    } 
}

int main()
{
    sr::FrameBuffer fb(200, 200);

    sr::Vec2i t0[3] = {sr::Vec2i(10, 70), sr::Vec2i(50, 160), sr::Vec2i(70, 80)};
    sr::Vec2i t1[3] = {sr::Vec2i(180, 50), sr::Vec2i(150, 1), sr::Vec2i(70, 180)};
    sr::Vec2i t2[3] = {sr::Vec2i(180, 150), sr::Vec2i(120, 160), sr::Vec2i(130, 180)};

    sr::Profile::start();
    triangle2(t0[0], t0[1], t0[2], fb, red.packed);
    triangle2(t1[0], t1[1], t1[2], fb, white.packed);
    triangle2(t2[0], t2[1], t2[2], fb, green.packed);
    sr::Profile::endAndPrint();

    sr::TGAImage::write24("out.tga", fb);
    return 0;
}
