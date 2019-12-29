#pragma once

#include "Platform.h"
#include "Logger.h"

#include <chrono>

SR_NAMESPACE_START

class Profile
{
public:
    ///
    /// Mark the start time of profiling
    inline static void start()
    {
        gProfile_startTime = std::chrono::steady_clock::now();
    }

    ///
    /// Mark the end time of profiling then printing out to console
    /// This function is non-thread-safe.
    inline static void endAndPrint()
    {
        const auto now = std::chrono::steady_clock::now();
        const auto diff = now - gProfile_startTime;
        LOG(" execution elapsed %lu ms (%lu s)\n",
                std::chrono::duration_cast<std::chrono::milliseconds>(diff).count(),
                std::chrono::duration_cast<std::chrono::seconds>(diff).count());
    }

    ///
    /// Mark the end time of profiling then printing out to console in thread-safe manner.
    inline static void endAndPrintA()
    {
        const auto now = std::chrono::steady_clock::now();
        const auto diff = now - gProfile_startTime;
        LOGA(" execution elapsed %lu ms (%lu s)\n",
                std::chrono::duration_cast<std::chrono::milliseconds>(diff).count(),
                std::chrono::duration_cast<std::chrono::seconds>(diff).count());
    }

    ///
    /// Mark the end time of profiling then return the elapsed time in millisecond.
    ///
    /// Note: std::chrono::microseconds has type long int on 64-bit Linux (Ubuntu 18.04)
    inline static long int end()
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - gProfile_startTime).count();
    }

private:
    static std::chrono::steady_clock::time_point gProfile_startTime;
};

SR_NAMESPACE_END
