namespace svg
{

//==============================================================================
Path Parse::parseSVGPath (const String& svgPath, const Environment& environment)
{
    SVGState state (nullptr, {}, environment);
    Path p;
    state.parsePathString (p, svgPath);
    return p;
}

std::unique_ptr<Drawable> Parse::parse (const XmlElement& svgDocument, const Environment& environment)
{
    if (! svgDocument.hasTagNameIgnoringNamespace ("svg"))
        return {};

    SVGState state (&svgDocument, {}, environment);
    return state.parseSVGElement (XmlPath (&svgDocument));
}

std::unique_ptr<Drawable> Parse::parse (const File& svgFile, const Environment& environment)
{
    if (auto xml = parseXMLIfTagMatches (svgFile, "svg"))
        return parse (*xml, environment);

    return {};
}

std::unique_ptr<Drawable> Parse::parse (const void* data, int numBytes, const Environment& environment)
{
    if (auto svg = parseXMLIfTagMatches (String::createStringFromData (data, numBytes), "svg"))
        return parse (*svg, environment);

    return {};
}


//==============================================================================
bool isIdentifierChar (juce_wchar c)
{
    return CharacterFunctions::isLetter (c) || c == '-';
}

//==============================================================================
inline bool isNone (const String& s) noexcept
{
    return s.equalsIgnoreCase ("none");
}

//==============================================================================
inline void setCommonAttributes (Drawable& d, const XmlPath& xml)
{
    const auto compID = xml->getStringAttribute ("id");
    d.setName (compID);
    d.setComponentID (compID);

    if (isNone (xml->getStringAttribute ("display")))
        d.setVisible (false);
}

//==============================================================================
inline String getLink (const XmlPath& xml)
{
    auto link = xml->getStringAttribute ("href").trim();

    if (link.isEmpty())
        link = xml->getStringAttribute ("xlink:href").trim();

    return link;
}

inline String getLinkedID (const XmlPath& xml)
{
    const auto link = getLink (xml);

    if (link.startsWithChar ('#'))
        return link.substring (1);

    return {};
}

//==============================================================================
inline bool pathContainsClosedSubPath (const Path& path) noexcept
{
    for (Path::Iterator iter (path); iter.next();)
        if (iter.elementType == Path::Iterator::closePath)
            return true;

    return false;
}

//==============================================================================
inline String parseURL (const String& str)
{
    if (str.startsWithIgnoreCase ("url"))
        return str
                .fromFirstOccurrenceOf ("(", false, false)
                .upToLastOccurrenceOf (")", false, false)
                .unquoted()
                .substring (1) //To remove the '#'
                .trim();

    return {};
}

//==============================================================================
inline int parsePlacementFlags (const String& align) noexcept
{
    if (align.isEmpty())
        return 0;

    if (isNone (align))
        return RectanglePlacement::stretchToFit;

    return (align.containsIgnoreCase ("slice") ? RectanglePlacement::fillDestination : 0)
         | (align.containsIgnoreCase ("xMin") ? RectanglePlacement::xLeft
                                              : (align.containsIgnoreCase ("xMax") ? RectanglePlacement::xRight
                                                                                   : RectanglePlacement::xMid))
         | (align.containsIgnoreCase ("yMin") ? RectanglePlacement::yTop
                                              : (align.containsIgnoreCase ("yMax") ? RectanglePlacement::yBottom
                                                                                   : RectanglePlacement::yMid));
}

//==============================================================================
inline String getAttributeFromStyleList (const String& list, StringRef attributeName, const String& defaultValue)
{
    int i = 0;

    for (;;)
    {
        i = list.indexOf (i, attributeName);

        if (i < 0)
            break;

        if ((i == 0 || (i > 0 && ! isIdentifierChar (list [i - 1])))
            && ! isIdentifierChar (list [i + attributeName.length()]))
        {
            i = list.indexOfChar (i, ':');

            if (i < 0)
                break;

            auto end = list.indexOfChar (i, ';');

            if (end < 0)
                end = 0x7ffff;

            return list.substring (i + 1, end).trim();
        }

        ++i;
    }

    return defaultValue;
}

//==============================================================================
inline bool isStartOfNumber (juce_wchar c) noexcept
{
    return CharacterFunctions::isDigit (c) || c == '-' || c == '+';
}

//==============================================================================
inline bool parseNextNumber (String::CharPointerType& text, String& value, bool allowUnits)
{
    auto s = text;

    while (s.isWhitespace() || *s == ',')
        ++s;

    auto start = s;

    if (isStartOfNumber (*s))
        ++s;

    while (s.isDigit())
        ++s;

    if (*s == '.')
    {
        ++s;

        while (s.isDigit())
            ++s;
    }

    if ((*s == 'e' || *s == 'E') && isStartOfNumber (s[1]))
    {
        s += 2;

        while (s.isDigit())
            ++s;
    }

    if (allowUnits)
        while (s.isLetter())
            ++s;

    if (s == start)
    {
        text = s;
        return false;
    }

    value = String (start, s);

    while (s.isWhitespace() || *s == ',')
        ++s;

    text = s;
    return true;
}

//==============================================================================
inline bool parseNextFlag (String::CharPointerType& text, bool& value)
{
    while (text.isWhitespace() || *text == ',')
        ++text;

    if (*text != '0' && *text != '1')
        return false;

    value = *(text++) != '0';

    while (text.isWhitespace() || *text == ',')
        ++text;

    return true;
}

//==============================================================================
inline PathStrokeType::JointStyle getJointStyle (const String& join) noexcept
{
    if (join.equalsIgnoreCase ("round"))  return PathStrokeType::JointStyle::curved;
    if (join.equalsIgnoreCase ("bevel"))  return PathStrokeType::JointStyle::beveled;

    return PathStrokeType::JointStyle::mitered;
}

inline PathStrokeType::EndCapStyle getEndCapStyle (const String& cap) noexcept
{
    if (cap.equalsIgnoreCase ("round"))   return PathStrokeType::EndCapStyle::rounded;
    if (cap.equalsIgnoreCase ("square"))  return PathStrokeType::EndCapStyle::square;

    return PathStrokeType::EndCapStyle::butt;
}

//==============================================================================
inline float parseSafeFloat (const String& s) noexcept
{
    const auto n = s.getFloatValue();
    return (std::isnan (n) || std::isinf (n)) ? 0.0f : n;
}

//==============================================================================
inline String::CharPointerType findStyleItem (String::CharPointerType source, String::CharPointerType name)
{
    const auto nameLength = (int) name.length();

    while (! source.isEmpty())
    {
        if (source.getAndAdvance() == '.'
            && CharacterFunctions::compareIgnoreCaseUpTo (source, name, nameLength) == 0)
        {
            auto endOfName = (source + nameLength).findEndOfWhitespace();

            if (*endOfName == '{')
                return endOfName;

            if (*endOfName == ',')
                return CharacterFunctions::find (endOfName, (juce_wchar) '{');
        }
    }

    return source;
}

//==============================================================================
inline AffineTransform parseTransform (String t)
{
    AffineTransform result;

    while (t.isNotEmpty())
    {
        StringArray tokens;
        tokens.addTokens (t.fromFirstOccurrenceOf ("(", false, false)
                           .upToFirstOccurrenceOf (")", false, false),
                          ", ", "");

        tokens.removeEmptyStrings (true);

        float numbers[6];

        for (int i = 0; i < numElementsInArray (numbers); ++i)
            numbers[i] = parseSafeFloat (tokens[i]);

        AffineTransform trans;

        if (t.startsWithIgnoreCase ("matrix"))
            trans = AffineTransform (numbers[0], numbers[2], numbers[4], numbers[1], numbers[3], numbers[5]);
        else if (t.startsWithIgnoreCase ("translate"))
            trans = AffineTransform::translation (numbers[0], numbers[1]);
        else if (t.startsWithIgnoreCase ("scale"))
            trans = AffineTransform::scale (numbers[0], numbers[tokens.size() > 1 ? 1 : 0]);
        else if (t.startsWithIgnoreCase ("rotate"))
            trans = AffineTransform::rotation (degreesToRadians (numbers[0]), numbers[1], numbers[2]);
        else if (t.startsWithIgnoreCase ("skewX"))
            trans = AffineTransform::shear (std::tan (degreesToRadians (numbers[0])), 0.0f);
        else if (t.startsWithIgnoreCase ("skewY"))
            trans = AffineTransform::shear (0.0f, std::tan (degreesToRadians (numbers[0])));

        result = trans.followedBy (result);
        t = t.fromFirstOccurrenceOf (")", false, false).trimStart();
    }

    return result;
}

//==============================================================================
inline void endpointToCentreParameters (double x1, double y1,
                                        double x2, double y2,
                                        double angle,
                                        bool largeArc, bool sweep,
                                        double& rx, double& ry,
                                        double& centreX, double& centreY,
                                        double& startAngle, double& deltaAngle) noexcept
{
    const auto midX = (x1 - x2) * 0.5;
    const auto midY = (y1 - y2) * 0.5;

    const auto cosAngle = std::cos (angle);
    const auto sinAngle = std::sin (angle);
    const auto xp = cosAngle * midX + sinAngle * midY;
    const auto yp = cosAngle * midY - sinAngle * midX;
    const auto xp2 = xp * xp;
    const auto yp2 = yp * yp;

    const auto rx2 = rx * rx;
    const auto ry2 = ry * ry;

    const auto s = (xp2 / rx2) + (yp2 / ry2);
    auto c = 0.0;

    if (s <= 1.0)
    {
        c = std::sqrt (jmax (0.0, ((rx2 * ry2) - (rx2 * yp2) - (ry2 * xp2)) / ((rx2 * yp2) + (ry2 * xp2))));

        if (largeArc == sweep)
            c = -c;
    }
    else
    {
        const auto s2 = std::sqrt (s);
        rx *= s2;
        ry *= s2;
    }

    const auto cpx = ((rx * yp) / ry) * c;
    const auto cpy = ((-ry * xp) / rx) * c;

    centreX = ((x1 + x2) * 0.5) + (cosAngle * cpx) - (sinAngle * cpy);
    centreY = ((y1 + y2) * 0.5) + (sinAngle * cpx) + (cosAngle * cpy);

    const auto ux = (xp - cpx) / rx;
    const auto uy = (yp - cpy) / ry;
    const auto vx = (-xp - cpx) / rx;
    const auto vy = (-yp - cpy) / ry;

    const auto length = juce_hypot (ux, uy);

    startAngle = std::acos (jlimit (-1.0, 1.0, ux / length));

    if (uy < 0)
        startAngle = -startAngle;

    startAngle += MathConstants<double>::halfPi;

    deltaAngle = std::acos (jlimit (-1.0, 1.0, ((ux * vx) + (uy * vy)) / (length * juce_hypot (vx, vy))));

    if ((ux * vy) - (uy * vx) < 0)
        deltaAngle = -deltaAngle;

    if (sweep)
    {
        if (deltaAngle < 0)
            deltaAngle += MathConstants<double>::twoPi;
    }
    else
    {
        if (deltaAngle > 0)
            deltaAngle -= MathConstants<double>::twoPi;
    }

    deltaAngle = std::fmod (deltaAngle, MathConstants<double>::twoPi);
}

//==============================================================================
inline void assertOnUnsupportedTags (const XmlElement& topLevel)
{
    ignoreUnused (topLevel);

   #if JUCE_DEBUG
    // NB: If you hit any assertions here, the SVG XML data contains unsupported tags!
    for (auto* c = topLevel.getFirstChildElement(); c != nullptr; c = c->getNextElement())
    {
        const auto& tag = c->getTagName();

        for (const auto& tagName : { "html", "audio", "video", "iframe", "canvas", "animate" })
            if (tag.containsIgnoreCase (tagName))
                jassertfalse;
    }
   #endif
}

inline StringPairArray scanForMetadata (const XmlElement& topLevel)
{
    StringPairArray metadata;

    for (const auto& tag : { tags::title, tags::desc, tags::metadata })
        if (auto* t = topLevel.getNextElementWithTagName (tag))
            metadata.set (tag, t->getAllSubText());

    return metadata;
}

//==============================================================================
SVGState::SVGState (const XmlElement* topLevel, const File& svgFile, const Environment& env) :
    originalFile (svgFile),
    topLevelXml (topLevel, nullptr),
    environment (env)
{
    jassert (topLevel != nullptr);
    assertOnUnsupportedTags (*topLevel);
    metadata = scanForMetadata (*topLevel);
}

SVGState::SVGState (const SVGState& other) :
    originalFile (other.originalFile),
    topLevelXml (other.topLevelXml),
    environment (other.environment),
    metadata (other.metadata),
    width (other.width),
    height (other.height),
    viewBoxW (other.viewBoxW),
    viewBoxH (other.viewBoxH),
    transform (other.transform),
    cssStyleText (other.cssStyleText)
{
}

SVGState::SVGState (SVGState&& other) :
    originalFile (other.originalFile),
    topLevelXml (other.topLevelXml),
    environment (other.environment),
    metadata (other.metadata),
    width (other.width),
    height (other.height),
    viewBoxW (other.viewBoxW),
    viewBoxH (other.viewBoxH),
    transform (other.transform),
    cssStyleText (other.cssStyleText)
{
}

SVGState& SVGState::operator= (const SVGState& other)
{
    if (this != &other)
    {
        originalFile = other.originalFile;
        topLevelXml = other.topLevelXml;
        environment = other.environment;
        metadata = other.metadata;
        width = other.width;
        height = other.height;
        viewBoxW = other.viewBoxW;
        viewBoxH = other.viewBoxH;
        transform = other.transform;
        cssStyleText = other.cssStyleText;
    }

    return *this;
}

SVGState& SVGState::operator= (SVGState&& other)
{
    if (this != &other)
    {
        originalFile = std::move (other.originalFile);
        topLevelXml = std::move (other.topLevelXml);
        environment = std::move (other.environment);
        metadata = std::move (other.metadata);
        width = std::move (other.width);
        height = std::move (other.height);
        viewBoxW = std::move (other.viewBoxW);
        viewBoxH = std::move (other.viewBoxH);
        transform = std::move (other.transform);
        cssStyleText = std::move (other.cssStyleText);
    }

    return *this;
}

//==============================================================================
std::unique_ptr<Drawable> SVGState::parseSVGElement (const XmlPath& xml)
{
    auto* drawable = new DrawableComposite();
    setCommonAttributes (*drawable, xml);

    SVGState newState (*this);

    if (xml->hasAttribute ("transform"))
        newState.addTransform (xml);

    newState.width  = getCoordLength (xml->getStringAttribute ("width",  String (newState.width)),  viewBoxW);
    newState.height = getCoordLength (xml->getStringAttribute ("height", String (newState.height)), viewBoxH);

    if (newState.width <= 0) newState.width = 100;
    if (newState.height <= 0) newState.height = 100;

    Point<float> viewboxXY;

    if (xml->hasAttribute ("viewBox"))
    {
        auto viewBoxAtt = xml->getStringAttribute ("viewBox");
        auto viewParams = viewBoxAtt.getCharPointer();
        Point<float> vwh;

        if (parseCoords (viewParams, viewboxXY, true)
            && parseCoords (viewParams, vwh, true)
            && vwh.x > 0.0f
            && vwh.y > 0.0f)
        {
            newState.viewBoxW = vwh.x;
            newState.viewBoxH = vwh.y;

            const auto placementFlags = parsePlacementFlags (xml->getStringAttribute ("preserveAspectRatio").trim());

            if (placementFlags != 0)
                newState.transform = RectanglePlacement (placementFlags)
                                        .getTransformToFit ({ viewboxXY.x, viewboxXY.y, vwh.x, vwh.y },
                                                            { newState.width, newState.height })
                                        .followedBy (newState.transform);
        }
    }
    else
    {
        if (viewBoxW == 0.0f)
            newState.viewBoxW = newState.width;

        if (viewBoxH == 0.0f)
            newState.viewBoxH = newState.height;
    }

    newState.parseSubElements (xml, *drawable);

    drawable->setContentArea ({ viewboxXY.x, viewboxXY.y, newState.viewBoxW, newState.viewBoxH });
    drawable->resetBoundingBoxToContentArea();

    return std::unique_ptr<Drawable> (drawable);
}

//==============================================================================
void SVGState::parsePathString (Path& path, const String& pathString) const
{
    auto d = pathString.getCharPointer().findEndOfWhitespace();

    Point<float> subpathStart, last, last2, p1, p2, p3;
    juce_wchar currentCommand = 0, previousCommand = 0;
    bool isRelative = true;
    bool carryOn = true;

    while (! d.isEmpty())
    {
        if (CharPointer_ASCII ("MmLlHhVvCcSsQqTtAaZz").indexOf (*d) >= 0)
        {
            currentCommand = d.getAndAdvance();
            isRelative = currentCommand >= 'a';
        }

        switch (currentCommand)
        {
        case 'M':
        case 'm':
        case 'L':
        case 'l':
            if (parseCoordsOrSkip (d, p1, false))
            {
                if (isRelative)
                    p1 += last;

                if (currentCommand == 'M' || currentCommand == 'm')
                {
                    subpathStart = p1;
                    path.startNewSubPath (p1);
                    currentCommand = 'l';
                }
                else
                {
                    path.lineTo (p1);
                }

                last2 = last = p1;
            }
            break;

        case 'H':
        case 'h':
            if (parseCoord (d, p1.x, false, true))
            {
                if (isRelative)
                    p1.x += last.x;

                path.lineTo (p1.x, last.y);

                last2.x = last.x;
                last.x = p1.x;
            }
            else
            {
                ++d;
            }
            break;

        case 'V':
        case 'v':
            if (parseCoord (d, p1.y, false, false))
            {
                if (isRelative)
                    p1.y += last.y;

                path.lineTo (last.x, p1.y);

                last2.y = last.y;
                last.y = p1.y;
            }
            else
            {
                ++d;
            }
            break;

        case 'C':
        case 'c':
            if (parseCoordsOrSkip (d, p1, false)
                && parseCoordsOrSkip (d, p2, false)
                && parseCoordsOrSkip (d, p3, false))
            {
                if (isRelative)
                {
                    p1 += last;
                    p2 += last;
                    p3 += last;
                }

                path.cubicTo (p1, p2, p3);

                last2 = p2;
                last = p3;
            }
            break;

        case 'S':
        case 's':
            if (parseCoordsOrSkip (d, p1, false)
                && parseCoordsOrSkip (d, p3, false))
            {
                if (isRelative)
                {
                    p1 += last;
                    p3 += last;
                }

                p2 = last;

                if (CharPointer_ASCII ("CcSs").indexOf (previousCommand) >= 0)
                    p2 += (last - last2);

                path.cubicTo (p2, p1, p3);

                last2 = p1;
                last = p3;
            }
            break;

        case 'Q':
        case 'q':
            if (parseCoordsOrSkip (d, p1, false)
                && parseCoordsOrSkip (d, p2, false))
            {
                if (isRelative)
                {
                    p1 += last;
                    p2 += last;
                }

                path.quadraticTo (p1, p2);

                last2 = p1;
                last = p2;
            }
            break;

        case 'T':
        case 't':
            if (parseCoordsOrSkip (d, p1, false))
            {
                if (isRelative)
                    p1 += last;

                p2 = last;

                if (CharPointer_ASCII ("QqTt").indexOf (previousCommand) >= 0)
                    p2 += (last - last2);

                path.quadraticTo (p2, p1);

                last2 = p2;
                last = p1;
            }
            break;

        case 'A':
        case 'a':
            if (parseCoordsOrSkip (d, p1, false))
            {
                String num;
                bool flagValue = false;

                if (parseNextNumber (d, num, false))
                {
                    const auto angle = degreesToRadians (parseSafeFloat (num));

                    if (parseNextFlag (d, flagValue))
                    {
                        const auto largeArc = flagValue;

                        if (parseNextFlag (d, flagValue))
                        {
                            const auto sweep = flagValue;

                            if (parseCoordsOrSkip (d, p2, false))
                            {
                                if (isRelative)
                                    p2 += last;

                                if (last != p2)
                                {
                                    double centreX = 0.0, centreY = 0.0, startAngle = 0.0, deltaAngle = 0.0;
                                    double rx = p1.x, ry = p1.y;

                                    endpointToCentreParameters (last.x, last.y, p2.x, p2.y,
                                                                angle, largeArc, sweep,
                                                                rx, ry, centreX, centreY,
                                                                startAngle, deltaAngle);

                                    path.addCentredArc ((float) centreX, (float) centreY,
                                                        (float) rx, (float) ry,
                                                        angle, (float) startAngle, (float) (startAngle + deltaAngle),
                                                        false);

                                    path.lineTo (p2);
                                }

                                last2 = last;
                                last = p2;
                            }
                        }
                    }
                }
            }

            break;

        case 'Z':
        case 'z':
            path.closeSubPath();
            last = last2 = subpathStart;
            d = d.findEndOfWhitespace();
            currentCommand = 'M';
            break;

        default:
            carryOn = false;
            break;
        }

        if (! carryOn)
            break;

        previousCommand = currentCommand;
    }

    // paths that finish back at their start position often seem to be
    // left without a 'z', so need to be closed explicitly..
    if (path.getCurrentPosition() == subpathStart)
        path.closeSubPath();
}

//==============================================================================
void SVGState::parseSubElements (const XmlPath& xml, DrawableComposite& parentDrawable, bool shouldParseClip)
{
    for (auto* e : xml->getChildIterator())
    {
        const auto child = xml.getChild (e);

        if (auto* drawable = parseSubElement (child))
        {
            parentDrawable.addChildComponent (drawable);

            if (! isNone (getStyleAttribute (child, "display")))
                drawable->setVisible (true);

            if (shouldParseClip)
                parseClipPath (child, *drawable);
        }
    }
}

Drawable* SVGState::parseSubElement (const XmlPath& xml)
{
    {
        Path path;
        if (parsePathElement (xml, path))
            return parseShape (xml, path);
    }

    const auto tag = xml->getTagNameWithoutNamespace();

    if (tag == "g")             { return parseGroupElement (xml, true); }
    else if (tag == "svg")      { return parseSVGElement (xml).release(); }
    else if (tag == "text")     { return parseText (xml, true); }
    else if (tag == "image")    { return parseImage (xml, true); }
    else if (tag == "switch")   { return parseSwitch (xml); }
    else if (tag == "a")        { return parseLinkElement (xml); }
    else if (tag == "use")      { return parseUseOther (xml); }
    else if (tag == "style")    { parseCSSStyle (xml); }
    else if (tag == "defs")     { parseDefs (xml); }

    return nullptr;
}

bool SVGState::parsePathElement (const XmlPath& xml, Path& path) const
{
    const auto tag = xml->getTagNameWithoutNamespace();

    if (tag == "path")              { parsePath (xml, path);           return true; }
    else if (tag == "rect")         { parseRect (xml, path);           return true; }
    else if (tag == "circle")       { parseCircle (xml, path);         return true; }
    else if (tag == "ellipse")      { parseEllipse (xml, path);        return true; }
    else if (tag == "line")         { parseLine (xml, path);           return true; }
    else if (tag == "polyline")     { parsePolygon (xml, true, path);  return true; }
    else if (tag == "polygon")      { parsePolygon (xml, false, path); return true; }
    else if (tag == "use")          { return parseUsePath (xml, path); }

    return false;
}

DrawableComposite* SVGState::parseSwitch (const XmlPath& xml)
{
    if (auto* group = xml->getChildByName ("g"))
        return parseGroupElement (xml.getChild (group), true);

    return nullptr;
}

DrawableComposite* SVGState::parseGroupElement (const XmlPath& xml, bool shouldParseTransform)
{
    if (shouldParseTransform && xml->hasAttribute ("transform"))
    {
        SVGState newState (*this);
        newState.addTransform (xml);

        return newState.parseGroupElement (xml, false);
    }

    auto* drawable = new DrawableComposite();
    setCommonAttributes (*drawable, xml);
    parseSubElements (xml, *drawable);

    drawable->resetContentAreaAndBoundingBoxToFitChildren();
    return drawable;
}

DrawableComposite* SVGState::parseLinkElement (const XmlPath& xml)
{
    return parseGroupElement (xml, true); // TODO: support for making this clickable
}

//==============================================================================
void SVGState::parsePath (const XmlPath& xml, Path& path) const
{
    parsePathString (path, xml->getStringAttribute ("d"));

    if (getStyleAttribute (xml, "fill-rule").trim().equalsIgnoreCase ("evenodd"))
        path.setUsingNonZeroWinding (false);
}

void SVGState::parseRect (const XmlPath& xml, Path& rect) const
{
    const bool hasRX = xml->hasAttribute ("rx");
    const bool hasRY = xml->hasAttribute ("ry");

    if (hasRX || hasRY)
    {
        auto rx = getCoordLength (xml, "rx", viewBoxW);
        auto ry = getCoordLength (xml, "ry", viewBoxH);

        if (! hasRX)
            rx = ry;
        else if (! hasRY)
            ry = rx;

        rect.addRoundedRectangle (getCoordLength (xml, "x", viewBoxW),
                                  getCoordLength (xml, "y", viewBoxH),
                                  getCoordLength (xml, "width", viewBoxW),
                                  getCoordLength (xml, "height", viewBoxH),
                                  rx, ry);
    }
    else
    {
        rect.addRectangle (getCoordLength (xml, "x", viewBoxW),
                           getCoordLength (xml, "y", viewBoxH),
                           getCoordLength (xml, "width", viewBoxW),
                           getCoordLength (xml, "height", viewBoxH));
    }
}

void SVGState::parseCircle (const XmlPath& xml, Path& circle) const
{
    const auto cx = getCoordLength (xml, "cx", viewBoxW);
    const auto cy = getCoordLength (xml, "cy", viewBoxH);
    const auto radius = getCoordLength (xml, "r", viewBoxW);

    circle.addEllipse (cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);
}

void SVGState::parseEllipse (const XmlPath& xml, Path& ellipse) const
{
    const auto cx = getCoordLength (xml, "cx", viewBoxW);
    const auto cy = getCoordLength (xml, "cy", viewBoxH);
    const auto radiusX = getCoordLength (xml, "rx", viewBoxW);
    const auto radiusY = getCoordLength (xml, "ry", viewBoxH);

    ellipse.addEllipse (cx - radiusX, cy - radiusY, radiusX * 2.0f, radiusY * 2.0f);
}

void SVGState::parseLine (const XmlPath& xml, Path& line) const
{
    const auto x1 = getCoordLength (xml, "x1", viewBoxW);
    const auto y1 = getCoordLength (xml, "y1", viewBoxH);
    const auto x2 = getCoordLength (xml, "x2", viewBoxW);
    const auto y2 = getCoordLength (xml, "y2", viewBoxH);

    line.startNewSubPath (x1, y1);
    line.lineTo (x2, y2);
}

void SVGState::parsePolygon (const XmlPath& xml, bool isPolyline, Path& path) const
{
    const auto pointsAtt = xml->getStringAttribute ("points");
    auto points = pointsAtt.getCharPointer();
    Point<float> p;

    if (parseCoords (points, p, true))
    {
        Point<float> first (p), last;

        path.startNewSubPath (first);

        while (parseCoords (points, p, true))
        {
            last = p;
            path.lineTo (p);
        }

        if (! isPolyline || first == last)
            path.closeSubPath();
    }
}

bool SVGState::parseUsePath (const XmlPath& xml, Path& path) const
{
    const auto linkedID = getLinkedID (xml);

    if (linkedID.isNotEmpty())
    {
        UsePathOp op = { this, &path };
        return topLevelXml.applyOperationToChildWithID (linkedID, op);
    }

    return false;
}

Drawable* SVGState::parseUseOther (const XmlPath& xml) const
{
    if (auto* drawableText  = parseText (xml, false))    return drawableText;
    if (auto* drawableImage = parseImage (xml, false))   return drawableImage;

    return nullptr;
}

//==============================================================================
Drawable* SVGState::parseShape (const XmlPath& xml, Path& path,
                                bool shouldParseTransform,
                                AffineTransform* additonalTransform) const
{
    if (shouldParseTransform && xml->hasAttribute ("transform"))
    {
        SVGState newState (*this);
        newState.addTransform (xml);

        return newState.parseShape (xml, path, false, additonalTransform);
    }

    auto* dp = new DrawablePath();
    setCommonAttributes (*dp, xml);
    dp->setFill (Colours::black);

    path.applyTransform (transform);

    if (additonalTransform != nullptr)
        path.applyTransform (*additonalTransform);

    dp->setPath (path);

    dp->setFill (getPathFillType (path, xml, "fill",
                                  getStyleAttribute (xml, "fill-opacity"),
                                  getStyleAttribute (xml, "opacity"),
                                  pathContainsClosedSubPath (path)
                                    ? Colours::black
                                    : Colours::transparentBlack));

    const auto strokeType = getStyleAttribute (xml, "stroke");

    if (strokeType.isNotEmpty() && ! isNone (strokeType))
    {
        dp->setStrokeFill (getPathFillType (path, xml, "stroke",
                                            getStyleAttribute (xml, "stroke-opacity"),
                                            getStyleAttribute (xml, "opacity"),
                                            Colours::black));

        dp->setStrokeType (getStrokeFor (xml));
    }

    const auto strokeDashArray = getStyleAttribute (xml, "stroke-dasharray");

    if (strokeDashArray.isNotEmpty())
        parseDashArray (strokeDashArray, *dp);

    return dp;
}

void SVGState::parseDashArray (const String& dashList, DrawablePath& dp) const
{
    if (dashList.equalsIgnoreCase ("null") || isNone (dashList))
        return;

    Array<float> dashLengths;

    for (auto t = dashList.getCharPointer();;)
    {
        auto value = 0.0f;
        if (! parseCoord (t, value, true, true))
            break;

        dashLengths.add (value);

        t = t.findEndOfWhitespace();

        if (*t == ',')
            ++t;
    }

    const auto numDashs = dashLengths.size();
    if (numDashs > 0)
    {
        auto* dashes = dashLengths.getRawDataPointer();

        for (int i = 0; i < numDashs; ++i)
        {
            if (dashes[i] <= 0) // NB: SVG uses zero-length dashes to mean a dotted line.
            {
                if (numDashs == 1)
                    return;

                dashes[i] = environment.nonZeroLength;

                const auto pairedIndex = i ^ 1;

                if (isPositiveAndBelow (pairedIndex, numDashs)
                        && dashes[pairedIndex] > environment.nonZeroLength)
                    dashes[pairedIndex] -= environment.nonZeroLength;
            }
        }

        dp.setDashLengths (dashLengths);
    }
}

bool SVGState::parseClipPath (const XmlPath& xml, Drawable& d)
{
    auto clipPath = getStyleAttribute (xml, "clip-path");

    if (clipPath.isEmpty())
        clipPath = getStyleAttribute (xml, "mask");

    if (clipPath.isNotEmpty())
    {
        const auto urlID = parseURL (clipPath);

        if (urlID.isNotEmpty())
        {
            GetClipPathOp op = { this, &d };
            return topLevelXml.applyOperationToChildWithID (urlID, op);
        }
    }

    return false;
}

bool SVGState::applyClipPath (Drawable& target, const XmlPath& xmlPath)
{
    if (xmlPath->hasTagNameIgnoringNamespace ("clipPath")
        || xmlPath->hasTagNameIgnoringNamespace ("mask"))
    {
        auto drawableClipPath = std::make_unique<DrawableComposite>();

        parseSubElements (xmlPath, *drawableClipPath, false);

        if (drawableClipPath->getNumChildComponents() > 0)
        {
            setCommonAttributes (*drawableClipPath, xmlPath);
            target.setClipPath (std::move (drawableClipPath));
            return true;
        }
    }

    return false;
}

bool SVGState::addGradientStopsIn (ColourGradient& cg, const XmlPath& fillXml) const
{
    bool result = false;

    if (fillXml.xml != nullptr)
    {
        for (auto* e : fillXml->getChildWithTagNameIterator ("stop"))
        {
            auto col = parseColour (fillXml.getChild (e), "stop-color", Colours::black);

            const auto opacity = getStyleAttribute (fillXml.getChild (e), "stop-opacity", "1");
            col = col.withMultipliedAlpha (jlimit (0.0f, 1.0f, parseSafeFloat (opacity)));

            auto offset = parseSafeFloat (e->getStringAttribute ("offset"));

            if (e->getStringAttribute ("offset").containsChar ('%'))
                offset *= 0.01f;

            cg.addColour (jlimit (0.0f, 1.0f, offset), col);
            result = true;
        }
    }

    return result;
}

//==============================================================================
FillType SVGState::getGradientFillType (const XmlPath& fillXml,
                                        const Path& path,
                                        float opacity) const
{
    ColourGradient gradient;

    {
        const auto linkedID = getLinkedID (fillXml);

        if (linkedID.isNotEmpty())
        {
            SetGradientStopsOp op = { this, &gradient, };
            topLevelXml.applyOperationToChildWithID (linkedID, op);
        }
    }

    addGradientStopsIn (gradient, fillXml);

    const auto numColours = gradient.getNumColours();
    if (numColours > 0)
    {
        if (gradient.getColourPosition (0) > 0)
            gradient.addColour (0.0, gradient.getColour (0));

        if (gradient.getColourPosition (numColours - 1) < 1.0)
            gradient.addColour (1.0, gradient.getColour (numColours - 1));
    }
    else
    {
        gradient.addColour (0.0, Colours::black);
        gradient.addColour (1.0, Colours::black);
    }

    if (opacity < 1.0f)
        gradient.multiplyOpacity (opacity);

    jassert (gradient.getNumColours() > 0);

    gradient.isRadial = fillXml->hasTagNameIgnoringNamespace ("radialGradient");

    auto gradientWidth = viewBoxW;
    auto gradientHeight = viewBoxH;
    auto dx = 0.0f;
    auto dy = 0.0f;

    const bool userSpace = fillXml->getStringAttribute ("gradientUnits").equalsIgnoreCase ("userSpaceOnUse");

    if (! userSpace)
    {
        auto bounds = path.getBounds();
        dx = bounds.getX();
        dy = bounds.getY();
        gradientWidth = bounds.getWidth();
        gradientHeight = bounds.getHeight();
    }

    if (gradient.isRadial)
    {
        if (userSpace)
            gradient.point1.setXY (dx + getCoordLength (fillXml->getStringAttribute ("cx", "50%"), gradientWidth),
                                   dy + getCoordLength (fillXml->getStringAttribute ("cy", "50%"), gradientHeight));
        else
            gradient.point1.setXY (dx + gradientWidth  * getCoordLength (fillXml->getStringAttribute ("cx", "50%"), 1.0f),
                                   dy + gradientHeight * getCoordLength (fillXml->getStringAttribute ("cy", "50%"), 1.0f));

        const auto radius = getCoordLength (fillXml->getStringAttribute ("r", "50%"), gradientWidth);
        gradient.point2 = gradient.point1 + Point<float> (radius, 0.0f);

        // TODO xxx (the fx, fy focal point isn't handled properly here..)
    }
    else
    {
        if (userSpace)
        {
            gradient.point1.setXY (dx + getCoordLength (fillXml->getStringAttribute ("x1", "0%"), gradientWidth),
                                   dy + getCoordLength (fillXml->getStringAttribute ("y1", "0%"), gradientHeight));

            gradient.point2.setXY (dx + getCoordLength (fillXml->getStringAttribute ("x2", "100%"), gradientWidth),
                                   dy + getCoordLength (fillXml->getStringAttribute ("y2", "0%"), gradientHeight));
        }
        else
        {
            gradient.point1.setXY (dx + gradientWidth  * getCoordLength (fillXml->getStringAttribute ("x1", "0%"), 1.0f),
                                   dy + gradientHeight * getCoordLength (fillXml->getStringAttribute ("y1", "0%"), 1.0f));

            gradient.point2.setXY (dx + gradientWidth  * getCoordLength (fillXml->getStringAttribute ("x2", "100%"), 1.0f),
                                   dy + gradientHeight * getCoordLength (fillXml->getStringAttribute ("y2", "0%"), 1.0f));
        }

        if (gradient.point1 == gradient.point2)
            return gradient.getColour (gradient.getNumColours() - 1);
    }

    FillType type (gradient);

    const auto gradientTransform = parseTransform (fillXml->getStringAttribute ("gradientTransform"));

    if (gradient.isRadial)
    {
        type.transform = gradientTransform;
    }
    else
    {
        // Transform the perpendicular vector into the new coordinate space for the gradient.
        // This vector is now the slope of the linear gradient as it should appear in the new coord space
        const auto perpendicular = Point<float> (gradient.point2.y - gradient.point1.y,
                                                    gradient.point1.x - gradient.point2.x)
                                    .transformedBy (gradientTransform.withAbsoluteTranslation (0, 0));

        const auto newGradPoint1 = gradient.point1.transformedBy (gradientTransform);
        const auto newGradPoint2 = gradient.point2.transformedBy (gradientTransform);

        // Project the transformed gradient vector onto the transformed slope of the linear
        // gradient as it should appear in the new coordinate space
        const auto scale = perpendicular.getDotProduct (newGradPoint2 - newGradPoint1)
                           / perpendicular.getDotProduct (perpendicular);

        type.gradient->point1 = newGradPoint1;
        type.gradient->point2 = newGradPoint2 - perpendicular * scale;
    }

    return type;
}

FillType SVGState::getPathFillType (const Path& path, const XmlPath& xml, StringRef fillAttribute,
                                    const String& fillOpacity, const String& overallOpacity, Colour defaultColour) const
{
    auto opacity = 1.0f;

    if (overallOpacity.isNotEmpty())
        opacity = jlimit (0.0f, 1.0f, parseSafeFloat (overallOpacity));

    if (fillOpacity.isNotEmpty())
        opacity *= jlimit (0.0f, 1.0f, parseSafeFloat (fillOpacity));

    const auto fill = getStyleAttribute (xml, fillAttribute);
    const auto urlID = parseURL (fill);

    if (urlID.isNotEmpty())
    {
        GetFillTypeOp op = { this, &path, opacity, FillType() };

        if (topLevelXml.applyOperationToChildWithID (urlID, op))
            return op.fillType;
    }

    if (isNone (fill))
        return Colours::transparentBlack;

    return parseColour (xml, fillAttribute, defaultColour).withMultipliedAlpha (opacity);
}

float SVGState::getStrokeWidth (const String& strokeWidth) const noexcept
{
    const auto transformScale = std::sqrt (std::abs (transform.getDeterminant()));
    return transformScale * getCoordLength (strokeWidth, viewBoxW);
}

PathStrokeType SVGState::getStrokeFor (const XmlPath& xml) const
{
    return
    {
        getStrokeWidth (getStyleAttribute (xml, "stroke-width", "1")),
        getJointStyle (getStyleAttribute (xml, "stroke-linejoin")),
        getEndCapStyle (getStyleAttribute (xml, "stroke-linecap"))
    };
}

//==============================================================================
Drawable* SVGState::useText (const XmlPath& xml) const
{
    auto translation = AffineTransform::translation (parseSafeFloat (xml->getStringAttribute ("x")),
                                                     parseSafeFloat (xml->getStringAttribute ("y")));

    UseTextOp op = { this, &translation, nullptr };

    const auto linkedID = getLinkedID (xml);

    if (linkedID.isNotEmpty())
        topLevelXml.applyOperationToChildWithID (linkedID, op);

    return op.target;
}

Drawable* SVGState::parseText (const XmlPath& xml, bool shouldParseTransform,
                               AffineTransform* additonalTransform) const
{
    if (shouldParseTransform && xml->hasAttribute ("transform"))
    {
        SVGState newState (*this);
        newState.addTransform (xml);

        return newState.parseText (xml, false, additonalTransform);
    }

    if (xml->hasTagName ("use"))
        return useText (xml);

    if (! xml->hasTagName ("text") && ! xml->hasTagNameIgnoringNamespace ("tspan"))
        return nullptr;

    Array<float> xCoords, yCoords, dxCoords, dyCoords;

    getCoordList (xCoords,  getInheritedAttribute (xml, "x"),  true, true);
    getCoordList (yCoords,  getInheritedAttribute (xml, "y"),  true, false);
    getCoordList (dxCoords, getInheritedAttribute (xml, "dx"), true, true);
    getCoordList (dyCoords, getInheritedAttribute (xml, "dy"), true, false);

    const auto font = getFont (xml);
    const auto anchorStr = getStyleAttribute (xml, "text-anchor");

    auto* dc = new DrawableComposite();
    setCommonAttributes (*dc, xml);

    for (auto* e : xml->getChildIterator())
    {
        if (e->isTextElement())
        {
            const auto text = e->getText();

            auto* dt = new DrawableText();
            dc->addAndMakeVisible (dt);

            dt->setText (text);
            dt->setFont (font, true);

            if (additonalTransform != nullptr)
                dt->setTransform (transform.followedBy (*additonalTransform));
            else
                dt->setTransform (transform);

            dt->setColour (parseColour (xml, "fill", Colours::black)
                            .withMultipliedAlpha (parseSafeFloat (getStyleAttribute (xml, "fill-opacity", "1"))));

            Rectangle<float> bounds (xCoords[0], yCoords[0] - font.getAscent(),
                                     font.getStringWidthFloat (text), font.getHeight());

            if (anchorStr == "middle")
                bounds.setX (bounds.getX() - bounds.getWidth() / 2.0f);
            else if (anchorStr == "end")
                bounds.setX (bounds.getX() - bounds.getWidth());

            dt->setBoundingBox (bounds);
        }
        else if (e->hasTagNameIgnoringNamespace ("tspan"))
        {
            dc->addAndMakeVisible (parseText (xml.getChild (e), true));
        }
    }

    return dc;
}

Font SVGState::getFont (const XmlPath& xml) const
{
    Font f;
    const auto family = getStyleAttribute (xml, "font-family").unquoted().trim();

    if (family.isNotEmpty())
        f.setTypefaceName (family);

    if (getStyleAttribute (xml, "font-style").containsIgnoreCase ("italic"))
        f.setItalic (true);

    if (getStyleAttribute (xml, "font-weight").containsIgnoreCase ("bold"))
        f.setBold (true);

    return f.withPointHeight (getCoordLength (getStyleAttribute (xml, "font-size", "15"), 1.0f));
}

//==============================================================================
Drawable* SVGState::useImage (const XmlPath& xml) const
{
    auto translation = AffineTransform::translation (parseSafeFloat (xml->getStringAttribute ("x")),
                                                     parseSafeFloat (xml->getStringAttribute ("y")));

    UseImageOp op = { this, &translation, nullptr };

    const auto linkedID = getLinkedID (xml);

    if (linkedID.isNotEmpty())
        topLevelXml.applyOperationToChildWithID (linkedID, op);

    return op.target;
}

Drawable* SVGState::parseImage (const XmlPath& xml, bool shouldParseTransform,
                                AffineTransform* additionalTransform) const
{
    if (shouldParseTransform && xml->hasAttribute ("transform"))
    {
        SVGState newState (*this);
        newState.addTransform (xml);

        return newState.parseImage (xml, false, additionalTransform);
    }

    if (xml->hasTagName ("use"))
        return useImage (xml);

    if (! xml->hasTagName ("image"))
        return nullptr;

    const auto link = getLink (xml);

    std::unique_ptr<InputStream> inputStream;
    MemoryOutputStream imageStream;
    bool isDataSVG = false;

    if (link.startsWith ("data:"))
    {
        const auto indexOfComma = link.indexOf (",");
        const auto format = link.substring (5, indexOfComma).trim();
        const auto indexOfSemi = format.indexOf (";");

        if (format.substring (indexOfSemi + 1).trim().equalsIgnoreCase ("base64"))
        {
            const auto mime = format.substring (0, indexOfSemi).trim();
            isDataSVG = mime.equalsIgnoreCase ("image/svg+xml");

            if (isDataSVG
                || mime.equalsIgnoreCase ("image/bmp")
                || mime.equalsIgnoreCase ("image/gif")
                || mime.equalsIgnoreCase ("image/ief")
                || mime.equalsIgnoreCase ("image/jpeg")
                || mime.equalsIgnoreCase ("image/pipeg")
                || mime.equalsIgnoreCase ("image/png")
                || mime.equalsIgnoreCase ("image/tga")
                || mime.equalsIgnoreCase ("image/tiff")
                || mime.equalsIgnoreCase ("image/vnd.microsoft.icon")
                || mime.equalsIgnoreCase ("image/webp")
                || mime.equalsIgnoreCase ("image/x-icon")
                || mime.equalsIgnoreCase ("image/x-portable-anymap")
                || mime.equalsIgnoreCase ("image/x-portable-bitmap")
                || mime.equalsIgnoreCase ("image/x-portable-graymap")
                || mime.equalsIgnoreCase ("image/x-rgb")
                || mime.equalsIgnoreCase ("image/x-xbitmap")
                || mime.equalsIgnoreCase ("image/x-xpixmap"))
            {
                const auto base64text = link.substring (indexOfComma + 1).removeCharacters ("\t\n\r ");

                if (Base64::convertFromBase64 (imageStream, base64text))
                    inputStream.reset (new MemoryInputStream (imageStream.getData(), imageStream.getDataSize(), false));
            }
        }
    }
    else
    {
        const auto linkedFile = originalFile.getParentDirectory().getChildFile (link);

        isDataSVG = linkedFile.hasFileExtension ("svg");

        if (linkedFile.existsAsFile())
        {
            inputStream = linkedFile.createInputStream();
        }
        else
        {
            Logger::writeToLog ("Missing linked file in SVG!\n\n\t" + linkedFile.getFullPathName());
            jassertfalse;
        }
    }

    if (inputStream != nullptr)
    {
        const auto placementFlags = parsePlacementFlags (xml->getStringAttribute ("preserveAspectRatio").trim());

        if (isDataSVG)
        {
            const auto pathString = inputStream->readEntireStreamAsString();
            const auto path = Parse::parseSVGPath (pathString, environment);

            auto* dp = new DrawablePath();
            setCommonAttributes (*dp, xml);

            const auto pathBounds = path.getBounds();

            Rectangle<float> imageBounds (parseSafeFloat (xml->getStringAttribute ("x")),
                                          parseSafeFloat (xml->getStringAttribute ("y")),
                                          parseSafeFloat (xml->getStringAttribute ("width",  String (pathBounds.getWidth()))),
                                          parseSafeFloat (xml->getStringAttribute ("height", String (pathBounds.getHeight()))));

            dp->setPath (path);
            dp->setTransformToFit (imageBounds, placementFlags);

            auto trans = dp->getTransform().followedBy (transform);
            if (additionalTransform != nullptr)
                trans = trans.followedBy (*additionalTransform);

            dp->setTransform (trans);

            return dp;
        }

        const auto image = ImageFileFormat::loadFrom (*inputStream);
        if (! image.isValid())
            return nullptr;

        auto* di = new DrawableImage();

        setCommonAttributes (*di, xml);

        Rectangle<float> imageBounds (parseSafeFloat (xml->getStringAttribute ("x")),
                                      parseSafeFloat (xml->getStringAttribute ("y")),
                                      parseSafeFloat (xml->getStringAttribute ("width",  String (image.getWidth()))),
                                      parseSafeFloat (xml->getStringAttribute ("height", String (image.getHeight()))));

        di->setImage (image.rescaled ((int) imageBounds.getWidth(), (int) imageBounds.getHeight()));

        di->setTransformToFit (imageBounds, placementFlags);

        auto trans = di->getTransform().followedBy (transform);
        if (additionalTransform != nullptr)
            trans = trans.followedBy (*additionalTransform);

        di->setTransform (trans);

        return di;
    }

    return nullptr;
}

//==============================================================================
void SVGState::addTransform (const XmlPath& xml)
{
    transform = parseTransform (xml->getStringAttribute ("transform"))
                    .followedBy (transform);
}

//==============================================================================
bool SVGState::parseCoord (String::CharPointerType& s, float& value, bool allowUnits, bool isX) const
{
    String number;

    if (! parseNextNumber (s, number, allowUnits))
    {
        value = 0.0f;
        return false;
    }

    value = getCoordLength (number, isX ? viewBoxW : viewBoxH);
    return true;
}

bool SVGState::parseCoords (String::CharPointerType& s, Point<float>& p, bool allowUnits) const
{
    return parseCoord (s, p.x, allowUnits, true)
        && parseCoord (s, p.y, allowUnits, false);
}

bool SVGState::parseCoordsOrSkip (String::CharPointerType& s, Point<float>& p, bool allowUnits) const
{
    if (parseCoords (s, p, allowUnits))
        return true;

    if (! s.isEmpty())
        ++s;

    return false;
}

float SVGState::getCoordLength (const String& s, float sizeForProportions) const noexcept
{
    auto n = parseSafeFloat (s);
    const auto len = s.length();

    if (len > 2)
    {
        const auto n1 = s[len - 2];
        const auto n2 = s[len - 1];

        if (n1 == 'i' && n2 == 'n')         n *= environment.dpi;
        else if (n1 == 'm' && n2 == 'm')    n *= environment.dpi / 25.4f;
        else if (n1 == 'c' && n2 == 'm')    n *= environment.dpi / 2.54f;
        else if (n1 == 'p' && n2 == 'c')    n *= 15.0f;
        else if (n2 == '%')                 n *= 0.01f * sizeForProportions;
    }

    return n;
}

float SVGState::getCoordLength (const XmlPath& xml, const char* attName, const float sizeForProportions) const noexcept
{
    return getCoordLength (xml->getStringAttribute (attName), sizeForProportions);
}

void SVGState::getCoordList (Array<float>& coords, const String& list, bool allowUnits, bool isX) const
{
    auto text = list.getCharPointer();
    auto value = 0.0f;

    while (parseCoord (text, value, allowUnits, isX))
        coords.add (value);

    coords.minimiseStorageOverheads();
}

//==============================================================================
void SVGState::parseMask (const XmlPath&)
{
    jassertfalse; // @todo TODO xxx
}

void SVGState::parseMarker (const XmlPath&)
{
    jassertfalse; // @todo TODO xxx
}

void SVGState::parsePattern (const XmlPath&)
{
    jassertfalse; // @todo TODO xxx
}

void SVGState::parseFilter (const XmlPath&)
{
    jassertfalse; // @todo TODO xxx
}

void SVGState::parseCSSStyle (const XmlPath& xml)
{
    cssStyleText = xml->getAllSubText() + "\n" + cssStyleText;
}

void SVGState::parseDefs (const XmlPath& xml)
{
    if (auto* style = xml->getChildByName ("style"))
        parseCSSStyle (xml.getChild (style));

    if (auto* mask = xml->getChildByName ("mask"))
        parseMask (xml.getChild (mask));

    if (auto* marker = xml->getChildByName ("marker"))
        parseMarker (xml.getChild (marker));

    if (auto* pattern = xml->getChildByName ("pattern"))
        parsePattern (xml.getChild (pattern));

    if (auto* filter = xml->getChildByName ("filter"))
        parseFilter (xml.getChild (filter));
}

String SVGState::getStyleAttribute (const XmlPath& xml, StringRef attributeName, const String& defaultValue) const
{
    if (xml->hasAttribute (attributeName))
        return xml->getStringAttribute (attributeName, defaultValue);

    const auto styleAtt = xml->getStringAttribute ("style").trim();

    if (styleAtt.isNotEmpty())
    {
        const auto value = getAttributeFromStyleList (styleAtt, attributeName, {});

        if (value.isNotEmpty())
            return value;
    }
    else if (xml->hasAttribute ("class"))
    {
        for (auto i = cssStyleText.getCharPointer();;)
        {
            const auto openBrace = findStyleItem (i, xml->getStringAttribute ("class").getCharPointer());

            if (openBrace.isEmpty())
                break;

            const auto closeBrace = CharacterFunctions::find (openBrace, (juce_wchar) '}');

            if (closeBrace.isEmpty())
                break;

            const auto value = getAttributeFromStyleList (String (openBrace + 1, closeBrace),
                                                            attributeName, defaultValue);
            if (value.isNotEmpty())
                return value;

            i = closeBrace + 1;
        }
    }

    if (xml.parent != nullptr)
        return getStyleAttribute (*xml.parent, attributeName, defaultValue);

    return defaultValue;
}

String SVGState::getInheritedAttribute (const XmlPath& xml, StringRef attributeName) const
{
    if (xml->hasAttribute (attributeName))
        return xml->getStringAttribute (attributeName);

    if (xml.parent != nullptr)
        return getInheritedAttribute (*xml.parent, attributeName);

    return {};
}

//==============================================================================
Colour SVGState::parseColour (const XmlPath& xml, StringRef attributeName, const Colour defaultColour) const
{
    auto text = getStyleAttribute (xml, attributeName);

    if (text.startsWithChar ('#'))
    {
        uint32 hex[8] = { 0 };
        hex[6] = hex[7] = 15;

        int numChars = 0;
        auto s = text.getCharPointer();

        while (numChars < 8)
        {
            const auto hexValue = CharacterFunctions::getHexDigitValue (*++s);

            if (hexValue >= 0)
                hex[numChars++] = (uint32) hexValue;
            else
                break;
        }

        if (numChars <= 3)
            return Colour ((uint8) (hex[0] * 0x11),
                           (uint8) (hex[1] * 0x11),
                           (uint8) (hex[2] * 0x11));

        return Colour ((uint8) ((hex[0] << 4) + hex[1]),
                       (uint8) ((hex[2] << 4) + hex[3]),
                       (uint8) ((hex[4] << 4) + hex[5]),
                       (uint8) ((hex[6] << 4) + hex[7]));
    }

    if (text.startsWith ("rgb") || text.startsWith ("hsl"))
    {
        auto tokens = [&text]
        {
            const auto openBracket = text.indexOfChar ('(');
            const auto closeBracket = text.indexOfChar (openBracket, ')');

            StringArray arr;

            if (openBracket >= 3 && closeBracket > openBracket)
            {
                arr.addTokens (text.substring (openBracket + 1, closeBracket), ",", "");
                arr.trim();
                arr.removeEmptyStrings();
            }

            return arr;
        }();

        auto alpha = [&tokens, &text]
        {
            if ((text.startsWith ("rgba") || text.startsWith ("hsla")) && tokens.size() == 4)
                return parseSafeFloat (tokens[3]);

            return 1.0f;
        }();

        if (text.startsWith ("hsl"))
            return Colour::fromHSL (parseSafeFloat (tokens[0]) / 360.0f,
                                    parseSafeFloat (tokens[1]) / 100.0f,
                                    parseSafeFloat (tokens[2]) / 100.0f,
                                    alpha);

        if (tokens[0].containsChar ('%'))
            return Colour ((uint8) roundToInt (2.55f * parseSafeFloat (tokens[0])),
                           (uint8) roundToInt (2.55f * parseSafeFloat (tokens[1])),
                           (uint8) roundToInt (2.55f * parseSafeFloat (tokens[2])),
                           alpha);

        return Colour ((uint8) tokens[0].getIntValue(),
                       (uint8) tokens[1].getIntValue(),
                       (uint8) tokens[2].getIntValue(),
                       alpha);
    }

    if (text == "inherit")
    {
        for (const auto* p = xml.parent; p != nullptr; p = p->parent)
            if (getStyleAttribute (*p, attributeName).isNotEmpty())
                return parseColour (*p, attributeName, defaultColour);
    }

    if (text == "currentColor")
        return environment.currentColour;

    return Colours::findColourForName (text, defaultColour);
}

//==============================================================================

} //namespace svg
