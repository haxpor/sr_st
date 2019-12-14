#include "Logger.h"

SR_NAMESPACE_START

std::atomic_flag Logger::lock = ATOMIC_FLAG_INIT;
char Logger::fmtBuffer[MAX_BUFFER_SIZE];

SR_NAMESPACE_END

