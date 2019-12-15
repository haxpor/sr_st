#pragma once

#include "Platform.h"

SR_NAMESPACE_START

struct SR_MEM_ALIGN(8) Vec2i
{
    int x;
    int y;

    Vec2i()
        : x(0)
        , y(0)
    {
    }

    Vec2i(int x_, int y_)
        : x(x_)
        , y(y_)
    {
    }
};

struct SR_MEM_ALIGN(8) Vec2f
{
    union
    {
        struct
        {
            float s;
            float t;
        };
        struct
        {
            float u;
            float v;
        };
        struct
        {
            float x;
            float y;
        };
    };

    Vec2f()
        : x(0.0f)
        , y(0.0f)
    {
    }

    Vec2f(float x_, float y_)
        : x(x_)
        , y(y_)
    {
    }
};

struct SR_MEM_ALIGN(16) Vec3i
{
    union
    {
        struct
        {
            int b;
            int g;
            int r;
        };
        struct
        {
            int x;
            int y;
            int z;
        };
    };

    Vec3i()
        : x(0)
        , y(0)
        , z(0)
    {
    }

    Vec3i(int x_, int y_, int z_)
        : x(x_)
        , y(y_)
        , z(z_)
    {
    }
};

struct SR_MEM_ALIGN(16) Vec3f
{
    union
    {
        struct
        {
            float b;
            float g;
            float r;
        };
        struct
        {
            float x;
            float y;
            float z;
        };
    };

    Vec3f()
        : x(0.0f)
        , y(0.0f)
        , z(0.0f)
    {
    }

    Vec3f(float x_, float y_, float z_)
        : x(x_)
        , y(y_)
        , z(z_)
    {
    }
};

struct SR_MEM_ALIGN(16) Vec4i
{
    union
    {
        struct
        {
            int b;
            int g;
            int r;
            int a;
        };
        struct
        {
            int x;
            int y;
            int z;
            int w;
        };
    };

    Vec4i()
        : x(0)
        , y(0)
        , z(0)
        , w(0)
    {
    }

    Vec4i(int x_, int y_, int z_, int w_)
        : x(x_)
        , y(y_)
        , z(z_)
        , w(w_)
    {
    }
};

struct SR_MEM_ALIGN(16) Vec4f
{
    union
    {
        struct
        {
            float b;
            float g;
            float r;
            float a;
        };
        struct
        {
            float x;
            float y;
            float z;
            float w;
        };
    };

    Vec4f()
        : x(0.0f)
        , y(0.0f)
        , z(0.0f)
        , w(0.0f)
    {
    }

    Vec4f(float x_, float y_, float z_, float w_)
        : x(x_)
        , y(y_)
        , z(z_)
        , w(w_)
    {
    }
};

struct Vertex_v1
{
    Vec3f pos;     // position
    Vec3f normal;     // normal
    Vec2f texcoord;     // texture coordinate
};

/// format 32-bit ARGB
struct SR_MEM_ALIGN(16) Color32i
{
    union
    {
        struct
        {
            unsigned char b;
            unsigned char g;
            unsigned char r;
            unsigned char a;
        };
        unsigned int packed;
    };

    Color32i()
        : packed(0)
    {
    }

    Color32i(unsigned char r_, unsigned char g_, unsigned char b_, unsigned char a_)
        : b(b_)
        , g(g_)
        , r(r_)
        , a(a_)
    {
    }

    Color32i(unsigned char r_, unsigned char g_, unsigned char b_)
        : b(b_)
        , g(g_)
        , r(r_)
        , a(255)
    {
    }

};

/// aliased name of floating-point component color32 to Vec4
typedef Vec4f Color32f;


SR_NAMESPACE_END
