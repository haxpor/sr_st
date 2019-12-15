#pragma once

#include "Platform.h"

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
};

SR_NAMESPACE_END
