/// inline-definition
/// (header inclusion guard should be done already via its parent header Types.h

#include "Platform.h"
SR_NAMESPACE_USING

template <typename T>
inline Vec4<T>::Vec4()
    : x(0)
    , y(0)
    , z(0)
    , w(0)
{
}

template <typename T>
inline Vec4<T>::Vec4(T x_, T y_, T z_, T w_)
    : x(x_)
    , y(y_)
    , z(z_)
    , w(w_)
{
}

template <typename T>
inline Vec4<T>::Vec4(const Vec4<T>& other)
    : x(other.x)
    , y(other.y)
    , z(other.z)
    , w(other.w)
{
}

template <typename U>
inline Vec4<U> operator-(const Vec4<U>& a)
{
    return Vec4<U>(-a.x,
                   -a.y,
                   -a.z,
                   -a.w);
}

template <typename U>
inline Vec4<U> operator-(const Vec4<U>& a, const Vec4<U>& b)
{
    return Vec4<U>(a.x - b.x,
                   a.y - b.y,
                   a.z - b.z,
                   a.w - b.w);
}

template <typename U>
inline Vec4<U> operator+(const Vec4<U>& a)
{
    return Vec4<U>(a);
}

template <typename U>
inline Vec4<U> operator+(const Vec4<U>& a, const Vec4<U>& b)
{
    return Vec4<U>(a.x + b.x,
                   a.y + b.y,
                   a.z + b.z,
                   a.w + b.w);
}

template <typename U>
inline Vec4<U> operator*(const Vec4<U>& a, const Vec4<U>& b)
{
    return Vec4<U>(a.x * b.x,
                   a.y * b.y,
                   a.z * b.z,
                   a.w * b.w);
}

template <typename U>
inline Vec4<U> operator/(const Vec4<U>& a, const Vec4<U>& b)
{
    return Vec4<U>(a.x / b.x,
                   a.y / b.y,
                   a.z / b.z,
                   a.w / b.w);
}

template <typename U, typename S>
inline Vec4<U> operator*(S s, const Vec4<U>& a)
{
    return Vec4<U>(s * a.x,
                   s * a.y,
                   s * a.z,
                   s * a.w);
}

template <typename U, typename S>
inline Vec4<U> operator*(const Vec4<U>& a, S s)
{
    return Vec4<U>(a.x * s,
                   a.y * s,
                   a.z * s,
                   a.w * s);
}

template <typename U, typename S>
inline Vec4<U> operator/(S s, const Vec4<U>& a)
{
    return Vec4<U>(s / a.x,
                   s / a.y,
                   s / a.z,
                   s / a.w);
}

template <typename U, typename S>
inline Vec4<U> operator/(const Vec4<U>& a, S s)
{
    return Vec4<U>(a.x / s,
                   a.y / s,
                   a.z / s,
                   a.w / s);
}

// prefix decrement
template <typename T>
inline Vec4<T>& Vec4<T>::operator--()
{
    --x;
    --y;
    --z;
    --w;
    return *this;
}

// postfix decrement
template <typename T>
inline Vec4<T> Vec4<T>::operator--(int)
{
    Vec4<T> ret(*this);
    operator--();   // call prefix
    return ret;
}

// prefix increment
template <typename T>
inline Vec4<T>& Vec4<T>::operator++()
{
    ++x;
    ++y;
    ++z;
    ++w;
    return *this;
}

// postfix increment
template <typename T>
inline Vec4<T> Vec4<T>::operator++(int)
{
    Vec4<T> ret(*this);
    operator++();
    return ret;
}
