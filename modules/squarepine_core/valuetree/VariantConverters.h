//==============================================================================
namespace juce
{
    template<typename Type>
    var fromEnum (Type value)
    {
        return static_cast<int> (static_cast<std::underlying_type_t<Type>> (value));
    }

    template<typename Type>
    Type toEnum (const var& var)
    {
        return static_cast<Type> (static_cast<std::underlying_type_t<Type>> ((int) var));
    }

    //==============================================================================
    /** */
    template<typename Type, int expectedNumElements>
    struct ArrayConverter final
    {
        using ArrayType = juce::Array<Type>;

        enum
        {
            sizeOfType = (int) sizeof (Type),
            numElements = expectedNumElements
        };

        static ArrayType from (const juce::var& v)
        {
            if (auto* arr = v.getArray())
            {
                if (arr->size() == numElements)
                {
                    ArrayType dest;
                    dest.ensureStorageAllocated (numElements);

                    for (const auto& value : *arr)
                        dest.add (static_cast<Type> (value));

                    return dest;
                }
                else
                {
                    jassertfalse;
                }
            }
            else
            {
                jassertfalse;
            }

            return {};
        }

        static juce::var to (const Type (&data) [numElements])
        {
            juce::Array<juce::var> dest;
            dest.ensureStorageAllocated (numElements);

            for (const auto& v : data)
                dest.add (v);

            return dest;
        }
    };

    //==============================================================================

   #if JUCE_MODULE_AVAILABLE_juce_core

    //==============================================================================
    /** */
    template<>
    struct VariantConverter<Uuid> final
    {
        /** */
        static Uuid fromVar (const var& v)  { return v.toString(); }
        /** */
        static var toVar (const Uuid& uuid) { return uuid.toString(); }
    };

    //==============================================================================
    /** */
    template<>
    struct VariantConverter<IPAddress> final
    {
        /** */
        static IPAddress fromVar (const var& v) { return IPAddress (v.toString()); }

        /** */
        static var toVar (const IPAddress& ip)  { return ip.toString(); }
    };

    //==============================================================================
    /** */
    template<>
    struct VariantConverter<MACAddress> final
    {
        /** */
        static MACAddress fromVar (const var& v)    { return MACAddress (v.toString()); }
        /** */
        static var toVar (const MACAddress& mac)    { return mac.toString(); }
    };

    //==============================================================================
    /** */
    template<>
    struct VariantConverter<URL> final
    {
        /** */
        static URL fromVar (const var& v) noexcept  { return v.toString(); }
        /** */
        static var toVar (const URL& url)           { return url.toString (true); }
    };

    //==============================================================================
    /** */
    template<>
    struct VariantConverter<File> final
    {
        /** */
        static File fromVar (const var& v) noexcept { return v.toString(); }
        /** */
        static var toVar (const File& f) noexcept   { return f.getFullPathName(); }
    };

    //==============================================================================
    /** */
    template<>
    struct VariantConverter<Time> final
    {
        /** */
        static Time fromVar (const var& v) noexcept { return Time (static_cast<int64> (v)); }
        /** */
        static var toVar (const Time& t)            { return t.toMilliseconds(); }
    };

    //==============================================================================
    /** */
    template<>
    struct VariantConverter<RelativeTime> final
    {
        /** */
        static RelativeTime fromVar (const var& v) noexcept { return RelativeTime (static_cast<double> (v)); }
        /** */
        static var toVar (const RelativeTime& rt)           { return rt.inSeconds(); }
    };

   #endif //JUCE_MODULE_AVAILABLE_juce_core

    //==============================================================================
   #if JUCE_MODULE_AVAILABLE_juce_graphics

    //==============================================================================
    #undef CREATE_VAR_CONV_POINT
    #define CREATE_VAR_CONV_POINT(Type) \
        template<> \
        struct VariantConverter<Point<Type>> final \
        { \
            using Converter = ArrayConverter<Type, 2>; \
            \
            static Point<Type> fromVar (const var& v) \
            { \
                const auto d = Converter::from (v); \
                return { d[0], d[1] }; \
            } \
            \
            static var toVar (const Point<Type>& p) \
            { \
                return Converter::to ({ p.x, p.y }); \
            } \
        };

    /** */
    CREATE_VAR_CONV_POINT (int)
    /** */
    CREATE_VAR_CONV_POINT (int64)
    /** */
    CREATE_VAR_CONV_POINT (float)
    /** */
    CREATE_VAR_CONV_POINT (double)

    #undef CREATE_VAR_CONV_POINT

    //==============================================================================
    #undef CREATE_VAR_CONV_RECT
    #define CREATE_VAR_CONV_RECT(Type) \
        template<> \
        struct VariantConverter<Rectangle<Type>> final \
        { \
            using Converter = ArrayConverter<Type, 4>; \
            \
            static Rectangle<Type> fromVar (const var& v) \
            { \
                const auto d = Converter::from (v); \
                return { d[0], d[1], d[2], d[3] }; \
            } \
            \
            static var toVar (const Rectangle<Type>& r) \
            { \
                return Converter::to ({ r.getX(), r.getY(), r.getWidth(), r.getHeight() }); \
            } \
        };

    /** */
    CREATE_VAR_CONV_RECT (int)
    /** */
    CREATE_VAR_CONV_RECT (int64)
    /** */
    CREATE_VAR_CONV_RECT (float)
    /** */
    CREATE_VAR_CONV_RECT (double)

    #undef CREATE_VAR_CONV_RECT

    //==============================================================================
    #undef CREATE_VAR_CONV_LINE
    #define CREATE_VAR_CONV_LINE(Type) \
        template<> \
        struct VariantConverter<Line<Type>> final \
        { \
            using Converter = ArrayConverter<Type, 4>; \
            \
            static Line<Type> fromVar (const var& v) \
            { \
                const auto d = Converter::from (v); \
                return { { d[0], d[1] }, { d[2], d[3] } }; \
            } \
            \
            static var toVar (const Line<Type>& r) \
            { \
                return Converter::to ({ r.getStartX(), r.getStartY(), r.getEndX(), r.getEndX() }); \
            } \
        };

    /** */
    CREATE_VAR_CONV_LINE (int)
    /** */
    CREATE_VAR_CONV_LINE (int64)
    /** */
    CREATE_VAR_CONV_LINE (float)
    /** */
    CREATE_VAR_CONV_LINE (double)

    #undef CREATE_VAR_CONV_RECT

    //==============================================================================
    #undef CREATE_VAR_CONV_RANGE
    #define CREATE_VAR_CONV_RANGE(Type) \
        template<> \
        struct VariantConverter<Range<Type>> final \
        { \
            using Converter = ArrayConverter<Type, 2>; \
            \
            static Range<Type> fromVar (const var& v) \
            { \
                const auto d = Converter::from (v); \
                return Range<Type> (d[0], d[1]); \
            } \
            \
            static var toVar (const Range<Type>& r) \
            { \
                return Converter::to ({ r.getStart(), r.getEnd() }); \
            } \
        };

    /** */
    CREATE_VAR_CONV_RANGE (int)
    /** */
    CREATE_VAR_CONV_RANGE (int64)
    /** */
    CREATE_VAR_CONV_RANGE (float)
    /** */
    CREATE_VAR_CONV_RANGE (double)

    #undef CREATE_VAR_CONV_RANGE

    //==============================================================================
    #undef CREATE_VAR_CONV_BORDERSIZE
    #define CREATE_VAR_CONV_BORDERSIZE(Type) \
        template<> \
        struct VariantConverter<BorderSize<Type>> final \
        { \
            using Converter = ArrayConverter<Type, 4>; \
            \
            static BorderSize<Type> fromVar (const var& v) \
            { \
                const auto d = Converter::from (v); \
                return { d[0], d[1], d[2], d[3] }; \
            } \
            \
            static var toVar (const BorderSize<Type>& b) \
            { \
                return Converter::to ({ b.getTop(), b.getLeft(), b.getBottom(), b.getRight() }); \
            } \
        };

    /** */
    CREATE_VAR_CONV_BORDERSIZE (int)
    /** */
    CREATE_VAR_CONV_BORDERSIZE (int64)
    /** */
    CREATE_VAR_CONV_BORDERSIZE (float)
    /** */
    CREATE_VAR_CONV_BORDERSIZE (double)

    #undef CREATE_VAR_CONV_BORDERSIZE

    //==============================================================================
    #undef CREATE_VAR_CONV_PARALLELOGRAM
    #define CREATE_VAR_CONV_PARALLELOGRAM(Type) \
        template<> \
        struct VariantConverter<Parallelogram<Type>> final \
        { \
            using Converter = ArrayConverter<Type, 6>; \
            \
            static Parallelogram<Type> fromVar (const var& v) \
            { \
                const auto d = Converter::from (v); \
                return { { d[0], d[1] }, { d[2], d[3] }, { d[4], d[5] } }; \
            } \
            \
            static var toVar (const Parallelogram<Type>& p) \
            { \
                return Converter::to ({ p.topLeft.x, p.topLeft.y, p.topRight.x, p.topRight.y, p.bottomLeft.x, p.bottomLeft.y }); \
            } \
        };

    /** */
    CREATE_VAR_CONV_PARALLELOGRAM (int)
    /** */
    CREATE_VAR_CONV_PARALLELOGRAM (int64)
    /** */
    CREATE_VAR_CONV_PARALLELOGRAM (float)
    /** */
    CREATE_VAR_CONV_PARALLELOGRAM (double)

    #undef CREATE_VAR_CONV_PARALLELOGRAM

    //==============================================================================
    /** */
    template<>
    struct VariantConverter<Font> final
    {
        /** */
        static Font fromVar (const var& v)
        {
            auto s = StringArray::fromLines (v.toString());
            s.trim();
            jassert (s.size() == 5);

            Font f;
            f.setTypefaceName (s[0]);
            f.setStyleFlags (s[1].getIntValue());
            f.setHeight (s[2].getFloatValue());
            f.setHorizontalScale (s[3].getFloatValue());
            f.setExtraKerningFactor (s[4].getFloatValue());
            return f;
        }

        /** */
        static var toVar (const Font& f)
        {
            String s;
            s
                << f.getTypefaceName() << newLine
                << f.getStyleFlags() << newLine
                << f.getHeight() << newLine
                << f.getHorizontalScale() << newLine
                << f.getExtraKerningFactor();

            return std::move (s);
        }
    };

    //==============================================================================
    /** */
    template<>
    struct VariantConverter<Colour> final
    {
        /** */
        static Colour fromVar (const var& v) noexcept   { return Colour::fromString (v.toString()); }
        /** */
        static var toVar (Colour c)                     { return c.toString(); }
    };

    //==============================================================================
    /** */
    template<>
    struct VariantConverter<RectanglePlacement> final
    {
        /** */
        static RectanglePlacement fromVar (const var& v) noexcept   { return static_cast<int> (v); }
        /** */
        static var toVar (RectanglePlacement p)                     { return p.getFlags(); }
    };

    //==============================================================================
    /** */
    template<>
    struct VariantConverter<Justification> final
    {
        /** */
        static Justification fromVar (const var& v) noexcept    { return static_cast<int> (v); }
        /** */
        static var toVar (Justification p)                      { return p.getFlags(); }
    };

    //==============================================================================
    /** */
    template<>
    struct VariantConverter<Image> final
    {
        /** */
        static Image fromVar (const var& v) noexcept
        {
            if (! v.isObject())
                return {};

            Image result;

            // @todo load image pixel data from base64...

            return result;
        }

        /** */
        static var toVar (const Image& image)
        {
            if (image.isNull())
                return {};

            auto* o = new DynamicObject();
            if (o == nullptr)
                return {};

            const Image::BitmapData bitmapData (image, 0, 0, image.getWidth(), image.getHeight());

            o->setProperty ("format", (int) bitmapData.pixelFormat);
            o->setProperty ("lineStride", bitmapData.lineStride);
            o->setProperty ("width", bitmapData.width);
            o->setProperty ("height", bitmapData.height);

            // @todo write image pixel data as base64...
            o->setProperty ("data", var());

            return o;
        }
    };

   #endif //JUCE_MODULE_AVAILABLE_juce_graphics

    //==============================================================================

   #if JUCE_MODULE_AVAILABLE_juce_opengl

    //==============================================================================
    #undef CREATE_VAR_CONV_VECTOR3D
    #define CREATE_VAR_CONV_VECTOR3D(Type) \
        template<> \
        struct VariantConverter<Vector3D<Type>> final \
        { \
            using Converter = ArrayConverter<Type, 3>; \
            \
            static Vector3D<Type> fromVar (const var& v) \
            { \
                const auto d = Converter::from (v); \
                return { d[0], d[1], d[2] }; \
            } \
            \
            static var toVar (const Vector3D<Type>& v) \
            { \
                return Converter::to ({ v.x, v.y, v.z }); \
            } \
        };

    /** */
    CREATE_VAR_CONV_VECTOR3D (int)
    /** */
    CREATE_VAR_CONV_VECTOR3D (int64)
    /** */
    CREATE_VAR_CONV_VECTOR3D (float)
    /** */
    CREATE_VAR_CONV_VECTOR3D (double)

    #undef CREATE_VAR_CONV_VECTOR3D

    //==============================================================================
    #undef CREATE_VAR_CONV_QUATERNION
    #define CREATE_VAR_CONV_QUATERNION(Type) \
        template<> \
        struct VariantConverter<Quaternion<Type>> final \
        { \
            using Converter = ArrayConverter<Type, 4>; \
            \
            static Quaternion<Type> fromVar (const var& v) \
            { \
                const auto d = Converter::from (v); \
                return { d[0], d[1], d[2], d[3] }; \
            } \
            \
            static var toVar (const Quaternion<Type>& q) \
            { \
                return Converter::to ({ q.vector.x, q.vector.y, q.vector.z, q.scalar }); \
            } \
        };

    /** */
    CREATE_VAR_CONV_QUATERNION (int)
    /** */
    CREATE_VAR_CONV_QUATERNION (int64)
    /** */
    CREATE_VAR_CONV_QUATERNION (float)
    /** */
    CREATE_VAR_CONV_QUATERNION (double)

    #undef CREATE_VAR_CONV_QUATERNION

    //==============================================================================
    #undef CREATE_VAR_CONV_MATRIX3D
    #define CREATE_VAR_CONV_MATRIX3D(Type) \
        template<> \
        struct VariantConverter<Matrix3D<Type>> final \
        { \
            using Converter = ArrayConverter<Type, 16>; \
            \
            static Matrix3D<Type> fromVar (const var& v) \
            { \
                return { Converter::from (v).data() }; \
            } \
            \
            static var toVar (const Matrix3D<Type>& m) \
            { \
                return Converter::to (m.mat); \
            } \
        };

    /** */
    CREATE_VAR_CONV_MATRIX3D (int)
    /** */
    CREATE_VAR_CONV_MATRIX3D (int64)
    /** */
    CREATE_VAR_CONV_MATRIX3D (float)
    /** */
    CREATE_VAR_CONV_MATRIX3D (double)

    #undef CREATE_VAR_CONV_MATRIX3D

    //==============================================================================

   #endif //JUCE_MODULE_AVAILABLE_squarepine_core

} // juce
