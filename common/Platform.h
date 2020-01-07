#pragma once

#define SR_NAMESPACE_START namespace sr {
#define SR_NAMESPACE_END }

#define SR_NAMESPACE_USING using namespace sr;

#define SR_MEM_ALIGN(bytes) __attribute__((aligned(bytes)))
