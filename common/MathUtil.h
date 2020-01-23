#pragma once

#include "Platform.h"
#include "Types.h"

#include <cstdlib>
#include <ctime>

SR_NAMESPACE_START

class MathUtil
{
public:
    /// Initialize MathUtil
    /// only call once
    static void init(std::time_t seed=std::time(nullptr))
    {
        std::srand(seed);
    }

    ///
    /// Random integer number in range [0,max]
    static inline int randInt(int max)
    {
        return std::rand() % (max+1);
    }

    ///
    /// Random integer number in range [min, max]
    static inline int randInt2(int min, int max)
    {
        return std::rand() % (max+1-min) + min;
    }

    ///
    /// Random floating-point number in range [0.0, 1.0]
    static inline float randFloat()
    {
        return std::rand() / static_cast<float>(RAND_MAX);
    }

    ///
    /// Random floating-point number in range [0.0, max]
    static inline float randFloat(float max)
    {
        return std::rand() / static_cast<float>(RAND_MAX) * max;
    }

    ///
    /// Random floating-point number in range [min, max]
    static inline float randFloat2(float min, float max)
    {
        return static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX) + 1.0f) * (max+1-min) + min;
    }

    ///
    /// Barycentric
    ///
    /// \param p1 A position in 2d of a triangle
    /// \param p2 B position in 2d of a triangle
    /// \param p3 C position in 2d of a triangle
    /// \param p A target position to get the barycentric coordindate from
    /// \return sr::Vec3f of barycentric coordinate.
    static inline sr::Vec3f barycentric(const sr::Vec2i& p1, const sr::Vec2i& p2, const sr::Vec2i& p3, const sr::Vec2i& p)
    {
        // ref: https://en.wikipedia.org/wiki/Barycentric_coordinate_system at "Conversion between
        // barycentric and Cartesian coordinates" section in the form of T x alpha = r - r3.
        // In short, it drills down to finding inverse 2x2 matrix.
        float det = (p2.y - p3.y) * (p1.x - p3.x) - (p3.x - p2.x) * (p3.y - p1.y);
        float l1 = ((p2.y - p3.y)*(p.x - p3.x) + (p3.x - p2.x)*(p.y - p3.y)) / det;
        float l2 = ((p3.y - p1.y)*(p.x - p3.x) + (p1.x - p3.x)*(p.y - p3.y)) / det;
        float l3 = 1.0f - l1 - l2;
        return sr::Vec3f(l1, l2, l3);
    }
};

SR_NAMESPACE_END
