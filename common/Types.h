#pragma once

#include "Platform.h"

SR_NAMESPACE_START

struct SR_MEM_ALIGN(8) Vec2
{
    union
    {
        float s;
        float t;
    };
    union
    {
        float u;
        float v;
    };
    float x;
    float y;
};

struct SR_MEM_ALIGN(16) Vec3
{
    union
    {
        float r;
        float g;
        float b;
    };
    float x;
    float y;
    float z;
};

struct SR_MEM_ALIGN(16) Vec4
{
    union
    {
        float r;
        float g;
        float b;
        float a;
    };
    float x;
    float y;
    float z;
    float w;
};

struct Vertex_v1
{
    Vec3 pos;     // position
    Vec3 normal;     // normal
    Vec2 texcoord;     // texture coordinate
};

SR_NAMESPACE_END
