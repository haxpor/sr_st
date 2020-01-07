/// inline-definition
/// (header inclusion guard should be done already via its parent header Types.h

#include "Platform.h"
SR_NAMESPACE_USING

template <typename T>
inline Vec2<T>::Vec2()
    : x(0)
    , y(0)
{
}

template <typename T>
inline Vec2<T>::Vec2(T x_, T y_)
    : x(x_)
    , y(y_)
{
}

template <typename T>
inline Vec2<T>::Vec2(const Vec2<T>& other)
    : x(other.x)
    , y(other.y)
{
}

template <typename U>
inline Vec2<U> operator-(const Vec2<U>& a)
{
    return Vec2<U>(-a.x,
                   -a.y);
}

template <typename U>
inline Vec2<U> operator-(const Vec2<U>& a, const Vec2<U>& b)
{
    return Vec2<U>(a.x - b.x,
                   a.y - b.y);
}

template <typename U>
inline Vec2<U> operator+(const Vec2<U>& a)
{
    return Vec2<U>(a);
}

template <typename U>
inline Vec2<U> operator+(const Vec2<U>& a, const Vec2<U>& b)
{
    return Vec2<U>(a.x + b.x,
                   a.y + b.y);
}

template <typename U>
inline Vec2<U> operator*(const Vec2<U>& a, const Vec2<U>& b)
{
    return Vec2<U>(a.x * b.x,
                   a.y * b.y);
}

template <typename U>
inline Vec2<U> operator/(const Vec2<U>& a, const Vec2<U>& b)
{
    return Vec2<U>(a.x / b.x,
                   a.y / b.y);
}

template <typename U, typename S>
inline Vec2<U> operator*(S s, const Vec2<U>& a)
{
    return Vec2<U>(s * a.x,
                   s * a.y);
}

template <typename U, typename S>
inline Vec2<U> operator*(const Vec2<U>& a, S s)
{
    return Vec2<U>(a.x * s,
                   a.y * s);
}

template <typename U, typename S>
inline Vec2<U> operator/(S s, const Vec2<U>& a)
{
    return Vec2<U>(s / a.x,
                   s / a.y);
}

template <typename U, typename S>
inline Vec2<U> operator/(const Vec2<U>& a, S s)
{
    return Vec2<U>(a.x / s,
                   a.y / s);
}

// prefix decrement
template <typename T>
inline Vec2<T>& Vec2<T>::operator--()
{
    --x;
    --y;
    return *this;
}

// postfix decrement
template <typename T>
inline Vec2<T> Vec2<T>::operator--(int)
{
    Vec2<T> ret(*this);
    operator--();   // call prefix
    return ret;
}

// prefix increment
template <typename T>
inline Vec2<T>& Vec2<T>::operator++()
{
    ++x;
    ++y;
    return *this;
}

// postfix increment
template <typename T>
inline Vec2<T> Vec2<T>::operator++(int)
{
    Vec2<T> ret(*this);
    operator++();
    return ret;
}
