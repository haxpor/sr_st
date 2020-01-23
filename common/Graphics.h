#pragma once

#include "Platform.h"
#include "Types.h"
#include "FrameBuffer.h"
#include <algorithm>

SR_NAMESPACE_START

///
/// Rasterization of line
void line(sr::Vec2i start, sr::Vec2i end, sr::FrameBuffer& fb, sr::Color32i color);

///
/// Rasterization of triangle
void triangle(sr::Vec2i t0, sr::Vec2i t1, sr::Vec2i t2, sr::FrameBuffer& fb, sr::Color32i color);

SR_NAMESPACE_END
