#pragma once

#include "Platform.h"

SR_NAMESPACE_START

// for GCC, this is to suppress the warning about using anonymous unnamed struct/union
#if defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

template <typename T>
struct SR_MEM_ALIGN(8) Vec2
{
    union
    {
        struct
        {
            T s;
            T t;
        };
        struct
        {
            T u;
            T v;
        };
        struct
        {
            T x;
            T y;
        };
    };

    Vec2();
    Vec2(T x_, T y_);
    Vec2(const Vec2<T>& other);

    // opted-in to declare free function as "friend" here then we have to again define template argument
    // although U is the same as T here but we declare it with different name.
    //
    // Another option (which we don't use it here) is to declare free functions outside of the scope
    // of the Vec2 struct, still we have to define template argument but we can use T as name instead.
    template <typename U>
    friend Vec2<U> operator-(const Vec2<U>& a);
    template <typename U>
    friend Vec2<U> operator-(const Vec2<U>& a, const Vec2<U>& b);
    template <typename U>
    friend Vec2<U> operator+(const Vec2<U>& a);
    template <typename U>
    friend Vec2<U> operator+(const Vec2<U>& a, const Vec2<U>& b);
    template <typename U>
    friend Vec2<U> operator*(const Vec2<U>& a, const Vec2<U>& b);
    template <typename U>
    friend Vec2<U> operator/(const Vec2<U>& a, const Vec2<U>& b);
    template <typename U, typename S>
    friend Vec2<U> operator*(S s, const Vec2<U>& a);
    template <typename U, typename S>
    friend Vec2<U> operator*(const Vec2<U>& a, S s);
    template <typename U, typename S>
    friend Vec2<U> operator/(S s, const Vec2<U>& a);
    template <typename U, typename S>
    friend Vec2<U> operator/(const Vec2<U>& a, S s);

    Vec2<T>& operator--();
    Vec2<T> operator--(int);
    Vec2<T>& operator++();
    Vec2<T> operator++(int);
};

template <typename T>
struct SR_MEM_ALIGN(16) Vec3
{
    union
    {
        struct
        {
            T b;
            T g;
            T r;
        };
        struct
        {
            T x;
            T y;
            T z;
        };
    };

    Vec3<T>();
    Vec3<T>(T x_, T y_, T z_);
    Vec3<T>(const Vec3<T>& other);

    template <typename U>
    friend Vec3<U> operator-(const Vec3<U>& a);
    template <typename U>
    friend Vec3<U> operator-(const Vec3<U>& a, const Vec3<U>& b);
    template <typename U>
    friend Vec3<U> operator+(const Vec3<U>& a);
    template <typename U>
    friend Vec3<U> operator+(const Vec3<U>& a, const Vec3<U>& b);
    template <typename U>
    friend Vec3<U> operator*(const Vec3<U>& a, const Vec3<U>& b);
    template <typename U>
    friend Vec3<U> operator/(const Vec3<U>& a, const Vec3<U>& b);
    template <typename U, typename S>
    friend Vec3<U> operator*(S s, const Vec3<U>& a);
    template <typename U, typename S>
    friend Vec3<U> operator*(const Vec3<U>& a, S s);
    template <typename U, typename S>
    friend Vec3<U> operator/(S s, const Vec3<U>& a);
    template <typename U, typename S>
    friend Vec3<U> operator/(const Vec3<U>& a, S s);

    Vec3<T>& operator--();
    Vec3<T> operator--(int);
    Vec3<T>& operator++();
    Vec3<T> operator++(int);
};

template <typename T>
struct SR_MEM_ALIGN(16) Vec4
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

    Vec4<T>();
    Vec4<T>(T x_, T y_, T z_, T w_);
    Vec4<T>(const Vec4<T>& other);

    template <typename U>
    friend Vec4<U> operator-(const Vec4<U>& a);
    template <typename U>
    friend Vec4<U> operator-(const Vec4<U>& a, const Vec4<U>& b);
    template <typename U>
    friend Vec4<U> operator+(const Vec4<U>& a);
    template <typename U>
    friend Vec4<U> operator+(const Vec4<U>& a, const Vec4<U>& b);
    template <typename U>
    friend Vec4<U> operator*(const Vec4<U>& a, const Vec4<U>& b);
    template <typename U>
    friend Vec4<U> operator/(const Vec4<U>& a, const Vec4<U>& b);
    template <typename U, typename S>
    friend Vec4<U> operator*(S s, const Vec4<U>& a);
    template <typename U, typename S>
    friend Vec4<U> operator*(const Vec4<U>& a, S s);
    template <typename U, typename S>
    friend Vec4<U> operator/(S s, const Vec4<U>& a);
    template <typename U, typename S>
    friend Vec4<U> operator/(const Vec4<U>& a, S s);

    Vec4<T>& operator--();
    Vec4<T> operator--(int);
    Vec4<T>& operator++();
    Vec4<T> operator++(int);
};

// Type defnitions for convenient usage
typedef Vec2<int> Vec2i;
typedef Vec2<float> Vec2f;
typedef Vec2<double> Vec2d;

typedef Vec3<int> Vec3i;
typedef Vec3<float> Vec3f;
typedef Vec3<double> Vec3d;

typedef Vec4<int> Vec4i;
typedef Vec4<float> Vec4f;
typedef Vec4<double> Vec4d;

struct Vertex_v1
{
    Vec3f pos;          // position
    Vec3f normal;       // normal
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

#include "Types_Vec2.inl"
#include "Types_Vec3.inl"
#include "Types_Vec4.inl"

#if defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic pop
#endif

SR_NAMESPACE_END
