#pragma once

#include "Platform.h"

#include <iostream>
#include <atomic>
#include <cstring>

SR_NAMESPACE_START

#if defined(NDEBUG)
#define LOG(fmt, ...)
#define LOG_WHITE(fmt, ...)
#define LOG_RED(fmt, ...)
#define LOG_GREEN(fmt, ...)
#define LOG_BLUE(fmt, ...)
#define LOG_MAGENTA(fmt, ...)
#define LOG_CYAN(fmt, ...)
#define LOG_YELLOW(fmt, ...)
#define LOGE(fmt, ...)
#define LOGE_WHITE(fmt, ...)
#define LOGE_RED(fmt, ...)
#define LOGE_GREEN(fmt, ...)
#define LOGE_BLUE(fmt, ...)
#define LOGE_MAGENTA(fmt, ...)
#define LOGE_CYAN(fmt, ...)
#define LOGE_YELLOW(fmt, ...)
#define LOGA(fmt, ...)
#define LOGA_WHITE(fmt, ...)
#define LOGA_RED(fmt, ...)
#define LOGA_GREEN(fmt, ...)
#define LOGA_BLUE(fmt, ...)
#define LOGA_MAGENTA(fmt, ...)
#define LOGA_CYAN(fmt, ...)
#define LOGA_YELLOW(fmt, ...)
#define LOGAE_WHITE(fmt, ...)
#define LOGAE_RED(fmt, ...)
#define LOGAE_GREEN(fmt, ...)
#define LOGAE_BLUE(fmt, ...)
#define LOGAE_MAGENTA(fmt, ...)
#define LOGAE_CYAN(fmt, ...)
#define LOGAE_YELLOW(fmt, ...)
#else
#define LOG(fmt, ...) sr::Logger::Log(fmt, ##__VA_ARGS__)
#define LOG_WHITE(fmt, ...)  sr::Logger::Log(sr::Logger::TextColor::WHITE, fmt, ##__VA_ARGS__)
#define LOG_RED(fmt, ...)  sr::Logger::Log(sr::Logger::TextColor::RED, fmt, ##__VA_ARGS__)
#define LOG_GREEN(fmt, ...)  sr::Logger::Log(sr::Logger::TextColor::GREEN, fmt, ##__VA_ARGS__)
#define LOG_BLUE(fmt, ...)  sr::Logger::Log(sr::Logger::TextColor::BLUE, fmt, ##__VA_ARGS__)
#define LOG_MAGENTA(fmt, ...)  sr::Logger::Log(sr::Logger::TextColor::MAGENTA, fmt, ##__VA_ARGS__)
#define LOG_CYAN(fmt, ...)  sr::Logger::Log(sr::Logger::TextColor::CYAN, fmt, ##__VA_ARGS__)
#define LOG_YELLOW(fmt, ...)  sr::Logger::Log(sr::Logger::TextColor::YELLOW, fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) sr::Logger::LogE(fmt, ##__VA_ARGS__)
#define LOGE_WHITE(fmt, ...)  sr::Logger::LogE(sr::Logger::TextColor::WHITE, fmt, ##__VA_ARGS__)
#define LOGE_RED(fmt, ...)  sr::Logger::LogE(sr::Logger::TextColor::RED, fmt, ##__VA_ARGS__)
#define LOGE_GREEN(fmt, ...)  sr::Logger::LogE(sr::Logger::TextColor::GREEN, fmt, ##__VA_ARGS__)
#define LOGE_BLUE(fmt, ...)  sr::Logger::LogE(sr::Logger::TextColor::BLUE, fmt, ##__VA_ARGS__)
#define LOGE_MAGENTA(fmt, ...)  sr::Logger::LogE(sr::Logger::TextColor::MAGENTA, fmt, ##__VA_ARGS__)
#define LOGE_CYAN(fmt, ...)  sr::Logger::LogE(sr::Logger::TextColor::CYAN, fmt, ##__VA_ARGS__)
#define LOGE_YELLOW(fmt, ...)  sr::Logger::LogE(sr::Logger::TextColor::YELLOW, fmt, ##__VA_ARGS__)
#define LOGA(fmt, ...) sr::Logger::LogA(fmt, ##__VA_ARGS__)
#define LOGA_WHITE(fmt, ...)  sr::Logger::LogA(sr::Logger::TextColor::WHITE, fmt, ##__VA_ARGS__)
#define LOGA_RED(fmt, ...)  sr::Logger::LogA(sr::Logger::TextColor::RED, fmt, ##__VA_ARGS__)
#define LOGA_GREEN(fmt, ...)  sr::Logger::LogA(sr::Logger::TextColor::GREEN, fmt, ##__VA_ARGS__)
#define LOGA_BLUE(fmt, ...)  sr::Logger::LogA(sr::Logger::TextColor::BLUE, fmt, ##__VA_ARGS__)
#define LOGA_MAGENTA(fmt, ...)  sr::Logger::LogA(sr::Logger::TextColor::MAGENTA, fmt, ##__VA_ARGS__)
#define LOGA_CYAN(fmt, ...)  sr::Logger::LogA(sr::Logger::TextColor::CYAN, fmt, ##__VA_ARGS__)
#define LOGA_YELLOW(fmt, ...)  sr::Logger::LogA(sr::Logger::TextColor::YELLOW, fmt, ##__VA_ARGS__)
#define LOGAE_WHITE(fmt, ...)  sr::Logger::LogAE(sr::Logger::TextColor::WHITE, fmt, ##__VA_ARGS__)
#define LOGAE_RED(fmt, ...)  sr::Logger::LogAE(sr::Logger::TextColor::RED, fmt, ##__VA_ARGS__)
#define LOGAE_GREEN(fmt, ...)  sr::Logger::LogAE(sr::Logger::TextColor::GREEN, fmt, ##__VA_ARGS__)
#define LOGAE_BLUE(fmt, ...)  sr::Logger::LogAE(sr::Logger::TextColor::BLUE, fmt, ##__VA_ARGS__)
#define LOGAE_MAGENTA(fmt, ...)  sr::Logger::LogAE(sr::Logger::TextColor::MAGENTA, fmt, ##__VA_ARGS__)
#define LOGAE_CYAN(fmt, ...)  sr::Logger::LogAE(sr::Logger::TextColor::CYAN, fmt, ##__VA_ARGS__)
#define LOGAE_YELLOW(fmt, ...)  sr::Logger::LogAE(sr::Logger::TextColor::YELLOW, fmt, ##__VA_ARGS__)
#endif

class Logger
{
public:
    enum class TextColor
    {
        WHITE,
        RED,
        GREEN,
        BLUE,
        MAGENTA,
        CYAN,
        YELLOW
    };

    static const int MAX_BUFFER_SIZE = 1024;
    
public:

    ///
    /// Log to standard output.
    /// This is not thread-safe.
    template<typename... Args>
    inline static void Log(const char* fmt, Args&&... args)
    {
        LogFS(stdout, fmt, std::forward<Args>(args)...);
    }

    ///
    /// Log to standard output with color.
    /// This is not thread-safe.
    template<typename... Args>
    inline static void Log(TextColor tc, const char* fmt, Args&&... args)
    {
        LogFS(stdout, tc, fmt, std::forward<Args>(args)...);
    }

    ///
    /// Log to standard error output.
    /// This is not thread-safe.
    template<typename... Args>
    inline static void LogE(const char* fmt, Args&&... args)
    {
        LogFS(stderr, fmt, std::forward<Args>(args)...);
    }

    ///
    /// Log to standard error output with color.
    /// This is not thread-safe.
    template<typename... Args>
    inline static void LogE(TextColor tc, const char* fmt, Args&&... args)
    {
        LogFS(stderr, tc, fmt, std::forward<Args>(args)...);
    }

    ///
    /// Log to file stream output.
    /// This is not thread-safe.
    template <typename... Args>
    inline static void LogFS(std::FILE* fs, const char* fmt, Args&&... args)
    {
        std::fprintf(fs, fmt, std::forward<Args>(args)...);
        std::fflush(fs);
    }

    ///
    /// Log to file stream output with color.
    /// This is not thread-safe.
    template <typename... Args>
    inline static void LogFS(std::FILE* fs, TextColor tc, const char* fmt, Args&&... args)
    {
        int fg;

        switch (tc)
        {
        case TextColor::WHITE: fg = 37; break;
        case TextColor::RED: fg = 31; break;
        case TextColor::GREEN: fg = 32; break;
        case TextColor::BLUE: fg = 34; break;
        case TextColor::MAGENTA: fg = 35; break;
        case TextColor::CYAN: fg = 36; break;
        case TextColor::YELLOW: fg = 33; break;
        }

        // modify fmt string
        int bwritten = std::snprintf(fmtBuffer, sizeof(fmtBuffer), "\033[1;%dm", fg);
        if (bwritten < 0) return;
        // copy original fmt not included \n
        std::strncpy(fmtBuffer + bwritten, fmt, strlen(fmt) - 1);
        std::snprintf(fmtBuffer + bwritten + strlen(fmt) - 1, sizeof(fmtBuffer) - (bwritten+strlen(fmt)), "\033[0m\n");

        std::fprintf(fs, fmtBuffer, std::forward<Args>(args)...);
        std::fflush(fs);
    }    

    ///
    /// Log to standard output.
    /// This is thread-safe.
    template <typename... Args>
    inline static void LogA(const char* fmt, Args&&... args)
    {
        LogAFS(stdout, fmt, std::forward<Args>(args)...);
    }

    ///
    /// Log to standard output with specified color.
    /// This is thread-safe.
    template <typename... Args>
    inline static void LogA(TextColor tc, const char* fmt, Args&&... args)
    {
        LogAFS(stdout, tc, fmt, std::forward<Args>(args)...);
    }

    ///
    /// Log to standard error output.
    /// This is thread-safe.
    template <typename... Args>
    inline static void LogAE(const char* fmt, Args&&... args)
    {
        LogAFS(stderr, fmt, std::forward<Args>(args)...);
    }

    ///
    /// Log to standard error output with specified color.
    /// This is thread-safe.
    template <typename... Args>
    inline static void LogAE(TextColor tc, const char* fmt, Args&&... args)
    {
        LogAFS(stderr, tc, fmt, std::forward<Args>(args)...);
    }

    ///
    /// Log to file strem output in thread-safe manner.
    template <typename... Args>
    inline static void LogAFS(std::FILE* fs, const char* fmt, Args&&... args)
    {
        while (lock.test_and_set(std::memory_order_acquire))
            ;

        std::fprintf(fs, fmt, std::forward<Args>(args)...);
        std::fflush(fs);

        lock.clear();
    }

    ///
    /// Log to file stream output with color in thread-safe manner.
    template <typename... Args>
    inline static void LogAFS(std::FILE* fs, TextColor tc, const char* fmt, Args&&... args)
    {
        // this also synchronize access to shared buffer to print out string `buffer`
        while (lock.test_and_set(std::memory_order_acquire))
            ;

        int fg;

        switch (tc)
        {
        case TextColor::WHITE: fg = 37; break;
        case TextColor::RED: fg = 31; break;
        case TextColor::GREEN: fg = 32; break;
        case TextColor::BLUE: fg = 34; break;
        case TextColor::MAGENTA: fg = 35; break;
        case TextColor::CYAN: fg = 36; break;
        case TextColor::YELLOW: fg = 33; break;
        }

        // modify fmt string
        int bwritten = std::snprintf(fmtBuffer, sizeof(fmtBuffer), "\033[1;%dm", fg);
        if (bwritten < 0) return;
        // copy original fmt not included \n
        std::strncpy(fmtBuffer + bwritten, fmt, strlen(fmt) - 1);
        std::snprintf(fmtBuffer + bwritten + strlen(fmt) - 1, sizeof(fmtBuffer) - (bwritten+strlen(fmt)), "\033[0m\n");

        std::fprintf(fs, fmtBuffer, std::forward<Args>(args)...);
        std::fflush(fs);

        lock.clear();
    }

private:
    static std::atomic_flag lock;
    static char fmtBuffer[MAX_BUFFER_SIZE];
};

SR_NAMESPACE_END
