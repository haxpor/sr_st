/// inline-definition
/// (header inclusion guard should be done already via its parent header Types.h


inline Vec2i::Vec2i()
    : x(0)
    , y(0)
{
}

inline Vec2i::Vec2i(int x_, int y_)
    : x(x_)
    , y(y_)
{
}

inline Vec2i::Vec2i(const Vec2i& other)
    : x(other.x)
    , y(other.y)
{
}

inline Vec2i operator-(const Vec2i& a)
{
    return Vec2i(-a.x,
                 -a.y);
}

inline Vec2i operator-(const Vec2i& a, const Vec2i& b)
{
    return Vec2i(a.x - b.x,
                 a.y - b.y);
}

inline Vec2i operator+(const Vec2i& a)
{
    return Vec2i(a);
}

inline Vec2i operator+(const Vec2i& a, const Vec2i& b)
{
    return Vec2i(a.x + b.x,
                 a.y + b.y);
}

inline Vec2i operator*(const Vec2i& a, const Vec2i& b)
{
    return Vec2i(a.x * b.x,
                 a.y * b.y);
}

inline Vec2i operator/(const Vec2i& a, const Vec2i& b)
{
    return Vec2i(a.x / b.x,
                 a.y / b.y);
}

template <typename T>
inline Vec2i operator*(T s, const Vec2i& a)
{
    return Vec2i(s * a.x,
                 s * a.y);
}

template <typename T>
inline Vec2i operator*(const Vec2i& a, T s)
{
    return Vec2i(a.x * s,
                 a.y * s);
}

template <typename T>
inline Vec2i operator/(T s, const Vec2i& a)
{
    return Vec2i(s / a.x,
                 s / a.y);
}

template <typename T>
inline Vec2i operator/(const Vec2i& a, T s)
{
    return Vec2i(a.x / s,
                 a.y / s);
}

// prefix decrement
inline Vec2i& Vec2i::operator--()
{
    --x;
    --y;
    return *this;
}

// postfix decrement
inline Vec2i Vec2i::operator--(int)
{
    Vec2i ret(*this);
    operator--();   // call prefix
    return ret;
}

// prefix increment
inline Vec2i& Vec2i::operator++()
{
    ++x;
    ++y;
    return *this;
}

// postfix increment
inline Vec2i Vec2i::operator++(int)
{
    Vec2i ret(*this);
    operator++();
    return ret;
}
