/// inline-definition
/// (header inclusion guard should be done already via its parent header Types.h

#include "Platform.h"
SR_NAMESPACE_USING

template <typename T>
inline Vec3<T>::Vec3()
    : x(0)
    , y(0)
    , z(0)
{
}

template <typename T>
inline Vec3<T>::Vec3(T x_, T y_, T z_)
    : x(x_)
    , y(y_)
    , z(z_)
{
}

template <typename T>
inline Vec3<T>::Vec3(const Vec3<T>& other)
    : x(other.x)
    , y(other.y)
    , z(other.z)
{
}

template <typename U>
inline Vec3<U> operator-(const Vec3<U>& a)
{
    return Vec3<U>(-a.x,
                   -a.y,
                   -a.z);
}

template <typename U>
inline Vec3<U> operator-(const Vec3<U>& a, const Vec3<U>& b)
{
    return Vec3<U>(a.x - b.x,
                   a.y - b.y,
                   a.z - b.z);
}

template <typename U>
inline Vec3<U> operator+(const Vec3<U>& a)
{
    return Vec3<U>(a);
}

template <typename U>
inline Vec3<U> operator+(const Vec3<U>& a, const Vec3<U>& b)
{
    return Vec3<U>(a.x + b.x,
                   a.y + b.y,
                   a.z + b.z);
}

template <typename U>
inline Vec3<U> operator*(const Vec3<U>& a, const Vec3<U>& b)
{
    return Vec3<U>(a.x * b.x,
                   a.y * b.y,
                   a.z * b.z);
}

template <typename U>
inline Vec3<U> operator/(const Vec3<U>& a, const Vec3<U>& b)
{
    return Vec3<U>(a.x / b.x,
                   a.y / b.y,
                   a.z / b.z);
}

template <typename U, typename S>
inline Vec3<U> operator*(S s, const Vec3<U>& a)
{
    return Vec3<U>(s * a.x,
                   s * a.y,
                   s * a.z);
}

template <typename U, typename S>
inline Vec3<U> operator*(const Vec3<U>& a, S s)
{
    return Vec3<U>(a.x * s,
                   a.y * s,
                   a.z * s);
}

template <typename U, typename S>
inline Vec3<U> operator/(S s, const Vec3<U>& a)
{
    return Vec3<U>(s / a.x,
                   s / a.y,
                   s / a.z);
}

template <typename U, typename S>
inline Vec3<U> operator/(const Vec3<U>& a, S s)
{
    return Vec3<U>(a.x / s,
                   a.y / s,
                   a.z / s);
}

// prefix decrement
template <typename T>
inline Vec3<T>& Vec3<T>::operator--()
{
    --x;
    --y;
    --z;
    return *this;
}

// postfix decrement
template <typename T>
inline Vec3<T> Vec3<T>::operator--(int)
{
    Vec3<T> ret(*this);
    operator--();   // call prefix
    return ret;
}

// prefix increment
template <typename T>
inline Vec3<T>& Vec3<T>::operator++()
{
    ++x;
    ++y;
    ++z;
    return *this;
}

// postfix increment
template <typename T>
inline Vec3<T> Vec3<T>::operator++(int)
{
    Vec3<T> ret(*this);
    operator++();
    return ret;
}

template <typename T>
inline Vec3<T> Vec3<T>::cross(const Vec3<T>& v) const
{
    // use Sarrus's rule (or cofactor expansion) to find cross product which can be expressed as
    // formal determinant
    return Vec3<T>(y*v.z - z*v.y,
                   z*v.x - x*v.z,
                   x*v.y - y*v.x);
}
