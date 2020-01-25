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

template <typename U>
inline Vec3<U> cross(const Vec3<U>& u, const Vec3<U>& v)
{
    // use Sarrus's rule (or cofactor expansion) to find cross product which can be expressed as
    // formal determinant
    return Vec3<U>(u.y*v.z - u.z*v.y,
                   u.z*v.x - u.x*v.z,
                   u.x*v.y - u.y*v.x);
}

template <typename U>
inline U dot(const Vec3<U>& u, const Vec3<U>& v)
{
    return u.x*v.x + u.y*v.y + u.z*v.z;
}

template <typename T>
inline T Vec3<T>::length() const
{
    return std::sqrt(x*x + y*y + z*z);
}

template <typename T>
inline T Vec3<T>::squaredLength() const
{
    return x*x + y*y + z*z;
}

template <typename T>
inline void Vec3<T>::normalize()
{
    // intentionally avoided dereference (*)
    T length = std::sqrt(x*x + y*y + z*z);
    x /= length;
    y /= length;
    z /= length;
}
