/// inline-definition
/// (header inclusion guard should be done already via its parent header Types.h

inline Vec2f::Vec2f()
    : x(0.0f)
    , y(0.0f)
{
}

inline Vec2f::Vec2f(float x_, float y_)
    : x(x_)
    , y(y_)
{
}

inline Vec2f::Vec2f(const Vec2f& other)
    : x(other.x)
    , y(other.y)
{
}

inline Vec2f operator-(const Vec2f& a)
{
    return Vec2f(-a.x,
                 -a.y);
}

inline Vec2f operator-(const Vec2f& a, const Vec2f& b)
{
    return Vec2f(a.x - b.x,
                 a.y - b.y);
}

inline Vec2f operator+(const Vec2f& a)
{
    return Vec2f(a);
}

inline Vec2f operator+(const Vec2f& a, const Vec2f& b)
{
    return Vec2f(a.x + b.x,
                 a.y + b.y);
}

inline Vec2f operator*(const Vec2f& a, const Vec2f& b)
{
    return Vec2f(a.x * b.x,
                 a.y * b.y);
}

inline Vec2f operator/(const Vec2f& a, const Vec2f& b)
{
    return Vec2f(a.x / b.x,
                 a.y / b.y);
}

template <typename T>
inline Vec2f operator*(T s, const Vec2f& a)
{
    return Vec2f(s * a.x,
                 s * a.y);
}

template <typename T>
inline Vec2f operator*(const Vec2f& a, T s)
{
    return Vec2f(a.x * s,
                 a.y * s);
}

template <typename T>
inline Vec2f operator/(T s, const Vec2f& a)
{
    return Vec2f(s / a.x,
                 s / a.y);
}

template <typename T>
inline Vec2f operator/(const Vec2f& a, T s)
{
    return Vec2f(a.x / s,
                 a.y / s);
}

inline Vec2f& Vec2f::operator--()
{
    --x;
    --y;
    return *this;
}

inline Vec2f Vec2f::operator--(int)
{
    Vec2f ret(*this);
    operator--();
    return ret;
}

inline Vec2f& Vec2f::operator++()
{
    ++x;
    --y;
    return *this;
}

inline Vec2f Vec2f::operator++(int)
{
    Vec2f ret(*this);
    operator++();
    return ret;
}
