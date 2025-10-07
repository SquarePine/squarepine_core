namespace svg
{

//==============================================================================
struct Environment
{
    float dpi = 96.0f;
    float nonZeroLength = 0.001f;
    Colour currentColour = Colours::black;
};

//==============================================================================
/** */
class Parse
{
public:
    /** */
    static Path parseSVGPath (const String& svgPath, const Environment& environment = {});

    /** */
    static std::unique_ptr<Drawable> parse (const XmlElement& svgDocument, const Environment& environment = {});

    /** */
    static std::unique_ptr<Drawable> parse (const File& svgDocument, const Environment& environment = {});

    /** */
    static std::unique_ptr<Drawable> parse (const void* data, int numBytes, const Environment& environment = {});

private:
    Parse() = delete;
    JUCE_DECLARE_NON_COPYABLE (Parse)
};

//==============================================================================
struct XmlPath
{
    XmlPath (const XmlElement* e, const XmlPath* p = nullptr) noexcept
        : xml (e), parent (p)  {}

    const XmlElement& operator*() const noexcept            { jassert (xml != nullptr); return *xml; }
    const XmlElement* operator->() const noexcept           { return xml; }
    XmlPath getChild (const XmlElement* e) const noexcept   { return XmlPath (e, this); }

    template<typename OperationType>
    bool applyOperationToChildWithID (const String& id, OperationType& op) const
    {
        for (auto* e : xml->getChildIterator())
        {
            XmlPath child (e, this);

            if (e->compareAttribute ("id", id) && ! child->hasTagName ("defs"))
                return op (child);

            if (child.applyOperationToChildWithID (id, op))
                return true;
        }

        return false;
    }

    const XmlElement* xml;
    const XmlPath* parent;
};

//==============================================================================
/** */
bool isIdentifierChar (juce_wchar c);

/** */
bool isNone (const String& s) noexcept;

/** */
void setCommonAttributes (Drawable& d, const XmlPath& xml);

/** */
String getLinkedID (const XmlPath& xml);

/** */
bool pathContainsClosedSubPath (const Path& path) noexcept;

/** */
String parseURL (const String& str);

/** */
int parsePlacementFlags (const String& align) noexcept;

/** */
String getAttributeFromStyleList (const String& list, StringRef attributeName, const String& defaultValue);

/** */
bool isStartOfNumber (juce_wchar c) noexcept;

/** */
bool parseNextNumber (String::CharPointerType& text, String& value, bool allowUnits);

/** */
bool parseNextFlag (String::CharPointerType& text, bool& value);

/** */
PathStrokeType::JointStyle getJointStyle (const String& join) noexcept;

/** */
PathStrokeType::EndCapStyle getEndCapStyle (const String& cap) noexcept;

/** */
float parseSafeFloat (const String& s) noexcept;

/** */
String::CharPointerType findStyleItem (String::CharPointerType source, String::CharPointerType name);

/** */
AffineTransform parseTransform (String t);

/** */
void endpointToCentreParameters (double x1, double y1,
                                 double x2, double y2,
                                 double angle,
                                 bool largeArc, bool sweep,
                                 double& rx, double& ry,
                                 double& centreX, double& centreY,
                                 double& startAngle, double& deltaAngle) noexcept;

//==============================================================================
namespace tags
{
    #undef SVG_CREATE_METADATA_TAG

    #define SVG_CREATE_METADATA_TAG(name) \
        static const char* const name = #name;

    SVG_CREATE_METADATA_TAG (title)
    SVG_CREATE_METADATA_TAG (desc)
    SVG_CREATE_METADATA_TAG (metadata)

    #undef SVG_CREATE_METADATA_TAG
}

//==============================================================================
/** */
class SVGState final
{
public:
    /** */
    explicit SVGState (const XmlElement* topLevel,
                       const File& svgFile = {},
                       const Environment& env = {});

    /** */
    SVGState (const SVGState& other);

    /** */
    SVGState (SVGState&& other);

    /** */
    SVGState& operator= (const SVGState& other);

    /** */
    SVGState& operator= (SVGState&& other);

    /** */
    ~SVGState() = default;

    //==============================================================================
    /** */
    std::unique_ptr<Drawable> parseSVGElement (const XmlPath& xml);

    /** */
    void parsePathString (Path& path, const String& pathString) const;

    /** */
    const StringPairArray& getMetadata() const noexcept { return metadata; }

private:
    //==============================================================================
    struct UsePathOp final
    {
        const SVGState* state;
        Path* targetPath;

        bool operator() (const XmlPath& xmlPath) const
        {
            return state->parsePathElement (xmlPath, *targetPath);
        }
    };

    struct UseTextOp final
    {
        const SVGState* state;
        AffineTransform* transform;
        Drawable* target;

        bool operator() (const XmlPath& xmlPath)
        {
            target = state->parseText (xmlPath, true, transform);
            return target != nullptr;
        }
    };

    struct UseImageOp final
    {
        const SVGState* state;
        AffineTransform* transform;
        Drawable* target;

        bool operator() (const XmlPath& xmlPath)
        {
            target = state->parseImage (xmlPath, true, transform);
            return target != nullptr;
        }
    };

    struct GetClipPathOp final
    {
        SVGState* state;
        Drawable* target;

        bool operator() (const XmlPath& xmlPath)
        {
            return state->applyClipPath (*target, xmlPath);
        }
    };

    struct SetGradientStopsOp final
    {
        const SVGState* state;
        ColourGradient* gradient;

        bool operator() (const XmlPath& xml) const
        {
            return state->addGradientStopsIn (*gradient, xml);
        }
    };

    struct GetFillTypeOp final
    {
        const SVGState* state;
        const Path* path;
        float opacity;
        FillType fillType;

        bool operator() (const XmlPath& xml)
        {
            if (xml->hasTagNameIgnoringNamespace ("linearGradient")
                || xml->hasTagNameIgnoringNamespace ("radialGradient"))
            {
                fillType = state->getGradientFillType (xml, *path, opacity);
                return true;
            }

            return false;
        }
    };

    //==============================================================================
    File originalFile;
    XmlPath topLevelXml;
    Environment environment;
    StringPairArray metadata;

    float width = 512.0f, height = 512.0f, viewBoxW = 0.0f, viewBoxH = 0.0f;
    AffineTransform transform;
    String cssStyleText;

    //==============================================================================
    void parseSubElements (const XmlPath& xml, DrawableComposite& parentDrawable, bool shouldParseClip = true);
    Drawable* parseSubElement (const XmlPath& xml);
    bool parsePathElement (const XmlPath& xml, Path& path) const;

    DrawableComposite* parseSwitch (const XmlPath& xml);
    DrawableComposite* parseGroupElement (const XmlPath& xml, bool shouldParseTransform);
    DrawableComposite* parseLinkElement (const XmlPath& xml);

    //==============================================================================
    void parsePath (const XmlPath& xml, Path& path) const;
    void parseRect (const XmlPath& xml, Path& rect) const;
    void parseCircle (const XmlPath& xml, Path& circle) const;
    void parseEllipse (const XmlPath& xml, Path& ellipse) const;
    void parseLine (const XmlPath& xml, Path& line) const;
    void parsePolygon (const XmlPath& xml, bool isPolyline, Path& path) const;
    bool parseUsePath (const XmlPath& xml, Path& path) const;
    Drawable* parseUseOther (const XmlPath& xml) const;
    Drawable* parseShape (const XmlPath& xml, Path& path,
                          bool shouldParseTransform = true,
                          AffineTransform* additonalTransform = nullptr) const;

    void parseDashArray (const String& dashList, DrawablePath& dp) const;
    bool parseClipPath (const XmlPath& xml, Drawable& d);
    bool applyClipPath (Drawable& target, const XmlPath& xmlPath);

    bool addGradientStopsIn (ColourGradient& cg, const XmlPath& fillXml) const;
    FillType getGradientFillType (const XmlPath& fillXml, const Path& path, float opacity) const;

    FillType getPathFillType (const Path& path, const XmlPath& xml, StringRef fillAttribute,
                              const String& fillOpacity, const String& overallOpacity, Colour defaultColour) const;

    float getStrokeWidth (const String& strokeWidth) const noexcept;

    PathStrokeType getStrokeFor (const XmlPath& xml) const;

    //==============================================================================
    Drawable* useText (const XmlPath& xml) const;
    Drawable* parseText (const XmlPath& xml, bool shouldParseTransform,
                         AffineTransform* additonalTransform = nullptr) const;

    Font getFont (const XmlPath& xml) const;

    //==============================================================================
    Drawable* useImage (const XmlPath& xml) const;

    Drawable* parseImage (const XmlPath& xml, bool shouldParseTransform,
                          AffineTransform* additionalTransform = nullptr) const;

    //==============================================================================
    void addTransform (const XmlPath& xml);

    //==============================================================================
    bool parseCoord (String::CharPointerType& s, float& value, bool allowUnits, bool isX) const;
    bool parseCoords (String::CharPointerType& s, juce::Point<float>& p, bool allowUnits) const;
    bool parseCoordsOrSkip (String::CharPointerType& s, juce::Point<float>& p, bool allowUnits) const;

    float getCoordLength (const String& s, float sizeForProportions) const noexcept;
    float getCoordLength (const XmlPath& xml, const char* attName, const float sizeForProportions) const noexcept;
    void getCoordList (Array<float>& coords, const String& list, bool allowUnits, bool isX) const;

    void parseMask (const XmlPath& xml);
    void parseMarker (const XmlPath& xml);
    void parsePattern (const XmlPath& xml);
    void parseFilter (const XmlPath& xml);
    void parseCSSStyle (const XmlPath& xml);
    void parseDefs (const XmlPath& xml);

    String getStyleAttribute (const XmlPath& xml, StringRef attributeName, const String& defaultValue = String()) const;
    String getInheritedAttribute (const XmlPath& xml, StringRef attributeName) const;

    //==============================================================================
    Colour parseColour (const XmlPath& xml, StringRef attributeName, Colour defaultColour) const;
};

//==============================================================================

} // namespace svg
