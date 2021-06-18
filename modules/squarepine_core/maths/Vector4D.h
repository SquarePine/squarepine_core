/** A four-coordinate vector. */
template<typename ValueType>
class Vector4D final
{
public:
    //==============================================================================
    /** This underlying value type for the Vector4D instance. */
    using Type = ValueType;

    //==============================================================================
    /** */
    Vector4D() noexcept = default;
    /** */
    Vector4D (Type xValue, Type yValue, Type zValue, Type wValue) noexcept  : x (xValue), y (yValue), z (zValue), w (wValue) { }
    /** */
    Vector4D (const Vector4D& other) noexcept                               : x (other.x), y (other.y), z (other.z), w (other.w) { }
    /** */
    ~Vector4D() noexcept = default;

    //==============================================================================
    /** @returns a vector that lies along the X axis. */
    static Vector4D xAxis() noexcept                            { return { (Type) 1, 0, 0, 0 }; }
    /** @returns a vector that lies along the Y axis. */
    static Vector4D yAxis() noexcept                            { return { 0, (Type) 1, 0, 0 }; }
    /** @returns a vector that lies along the Z axis. */
    static Vector4D zAxis() noexcept                            { return { 0, 0, (Type) 1, 0 }; }
    /** @returns a vector that lies along the W axis. */
    static Vector4D wAxis() noexcept                            { return { 0, 0, 0, (Type) 1 }; }

    //==============================================================================
    /** @returns */
    Type length() const noexcept                                { return (Type) std::sqrt ((double) lengthSquared()); }
    /** @returns */
    Type lengthSquared() const noexcept                         { return square (x) + square (y) + square (z) + square (w); }
    /** @returns */
    Vector4D normalised() const noexcept                        { return *this / length(); }

    //==============================================================================
    /** @returns the dot-product of these two vectors. */
    Type dotProduct (const Vector4D& other) const noexcept      { return x * other.x + y * other.y + z * other.z + w * other.w; }

    //==============================================================================
    /** */
    Vector4D& operator= (const Vector4D& other) noexcept        { x = other.x; y = other.y; z = other.z; w = other.w; return *this; }

    /** */
    Vector4D& operator++()                                      { ++x; ++y; ++z; ++w; return *this; }
    /** */
    Vector4D& operator--()                                      { --x; --y; --z; --w; return *this; }
    /** */
    Vector4D operator++ (int)                                   { auto v = *this; ++*this; return v; }
    /** */
    Vector4D operator-- (int)                                   { auto v = *this; --*this; return v; }

    /** */
    Vector4D& operator+= (const Vector4D& other) noexcept       { x += other.x; y += other.y; z += other.z; w += other.w; return *this; }
    /** */
    Vector4D& operator-= (const Vector4D& other) noexcept       { x -= other.x; y -= other.y; z -= other.z; w -= other.w; return *this; }
    /** */
    Vector4D& operator*= (Type scaleFactor) noexcept            { x *= scaleFactor; y *= scaleFactor; z *= scaleFactor; w *= scaleFactor; return *this; }
    /** */
    Vector4D& operator/= (Type scaleFactor) noexcept            { x /= scaleFactor; y /= scaleFactor; z /= scaleFactor; w /= scaleFactor; return *this; }

    /** */
    Vector4D operator+ (const Vector4D& other) const noexcept   { return { x + other.x, y + other.y, z + other.z, w + other.w }; }
    /** */
    Vector4D operator- (const Vector4D& other) const noexcept   { return { x - other.x, y - other.y, z - other.z, w - other.w }; }
    /** */
    Vector4D operator* (Type scaleFactor) const noexcept        { return { x * scaleFactor, y * scaleFactor, z * scaleFactor, w * scaleFactor }; }
    /** */
    Vector4D operator/ (Type scaleFactor) const noexcept        { return { x / scaleFactor, y / scaleFactor, z / scaleFactor, w / scaleFactor }; }
    /** */
    Vector4D operator-() const noexcept                         { return { -x, -y, -z, -w }; }

    /** */
    bool operator== (const Vector4D& other) const noexcept      { return approximatelyEqual (x, other.x) && approximatelyEqual (y, other.y)
                                                                      && approximatelyEqual (z, other.z) && approximatelyEqual (w, other.w); }
    /** */
    bool operator!= (const Vector4D& other) const noexcept      { return ! operator== (other); }
    /** */
    bool operator< (const Vector4D& other) const noexcept       { return x < other.x && y < other.y && z < other.z && w < other.w; }
    /** */
    bool operator<= (const Vector4D& other) const noexcept      { return x <= other.x && y <= other.y && z <= other.z && w <= other.w; }
    /** */
    bool operator> (const Vector4D& other) const noexcept       { return x > other.x && y > other.y && z > other.z && w > other.w; }
    /** */
    bool operator>= (const Vector4D& other) const noexcept      { return x >= other.x && y >= other.y && z >= other.z && w >= other.w; }

    //==============================================================================
    Type x {}, y {}, z {}, w {};
};
