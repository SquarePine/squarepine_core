/** An interpolated spline curve. */
template<typename ValueType,
         typename TypeOfCriticalSectionToUse = DummyCriticalSection,
         int minimumAllocatedSize = 0>
struct Spline final
{
    //==============================================================================
    /** */
    using Type = ValueType;
    /** */
    using Point = Point<Type>;
    /** */
    using ArrayType = Array<Point, TypeOfCriticalSectionToUse, minimumAllocatedSize>;

    //==============================================================================
    /** */
    Spline() = default;
    /** */
    Spline (const Spline&) = default;
    /** */
    Spline (Spline&&) = default;
    /** */
    ~Spline() = default;

    //==============================================================================
    /** */
    Spline& operator= (const Spline&) = default;
    /** */
    Spline& operator= (Spline&&) = default;

    //==============================================================================
    /** */
    int getNumPoints() const noexcept { return points.size(); }

    /** */
    void clear() { points.clear(); }
    /** */
    void clearQuick() { points.clearQuick(); }

    /** Reduces the amount of storage being used for storing the points. */
    void minimiseStorageOverheads() { points.minimiseStorageOverheads(); }

    /** Increases the array's internal storage to hold a minimum number of elements.

        Calling this before adding a large known number of elements means that
        the array won't have to keep dynamically resizing itself as the elements
        are added, and it'll therefore be more efficient.
    */
    void ensureStorageAllocated (int minNumElements)
    {
        points.ensureStorageAllocated (minNumElements);
    }

    /** @returns true if the array is empty, false otherwise. */
    bool isEmpty() const noexcept { return points.isEmpty(); }

    /** */
    Point operator[] (int index) const { return points[index]; }

    /** */
    Point getPoint (int index) const noexcept { return points[index]; }
    /** */
    Point getPointUnchecked (int index) const noexcept { return points.getUnchecked (index); }
    /** */
    Point& getPointReference (int index) noexcept { return points.getReference (index); }
    /** */
    const Point& getPointReference (int index) const noexcept { return points.getReference (index); }

    //==============================================================================
    /** */
    void addPoint (ValueType x, ValueType y)
    {
        auto i = getNumPoints();

        while (--i >= 0)
        {
            auto& p = points.getReference (i);

            if (p.x <= x)
            {
                if (p.x == x)
                {
                    p.y = y;
                    return;
                }

                break;
            }
        }

        points.insert (++i, { x, y });
    }

    /** */
    void addPoint (const Point& p)
    {
        addPoint (p.x, p.y);
    }

    /** */
    void removePoint (int index) { points.remove (index); }

    /** */
    void removePoint (const Point& p) { points.removeAllInstancesOf (p); }

    //==============================================================================
    /** */
    ValueType getY (ValueType x) const noexcept
    {
        const auto num = getNumPoints();

        auto i = num;
        while (--i >= 0)
            if (points.getReference (i).x <= x)
                break;

        if (i >= num - 1)
            return points.getLast().y;

        if (i < 0)
            return points.getFirst().y;

        const auto p0 = points.getReference (jmax (0, i - 1));
        const auto p1 = points.getReference (i);
        const auto p2 = points.getReference (i + 1);
        const auto p3 = points.getReference (jmin (num - 1, i + 2));

        const auto alpha = (x - p1.x) / (p2.x - p1.x);
        const auto alpha2 = square (alpha);
        const auto a0 = p3.y - p2.y - p0.y + p1.y;

        return (a0 * alpha * alpha2)
             + ((p0.y - p1.y - a0) * alpha2)
             + ((p2.y - p0.y) * alpha)
             + p1.y;
    }

    //==============================================================================
    /** @returns a pointer to the first element in the array.
        This method is provided for compatibility with standard C++ iteration mechanisms.
    */
    Point* begin() noexcept { return points.begin(); }

    /** @returns a pointer to the first element in the array.
        This method is provided for compatibility with standard C++ iteration mechanisms.
    */
    const Point* begin() const noexcept  { return points.begin(); }

    /** @returns a pointer to the element which follows the last element in the array.
        This method is provided for compatibility with standard C++ iteration mechanisms.
    */
    Point* end() noexcept { return points.begin(); }

    /** @returns a pointer to the element which follows the last element in the array.
        This method is provided for compatibility with standard C++ iteration mechanisms.
    */
    const Point* end() const noexcept { return points.begin(); }

    /** @returns a pointer to the first element in the array.
        This method is provided for compatibility with the standard C++ containers.
    */
    Point* data() noexcept { return points.begin(); }

    /** @returns a pointer to the first element in the array.
        This method is provided for compatibility with the standard C++ containers.
    */
    const Point* data() const noexcept { return points.begin(); }

private:
    //==============================================================================
    ArrayType points;
};
