// -----------------------------------------------------------------------------
// Fully-featured Figma -> JUCE mapping
// - multi-fill (solid, image, linear/radial gradients)
// - gradient stop parsing & gradientTransform -> ColourGradient
// - multi-stroke support with join, cap, dash pattern, dash offset
// - image fills (raster) at device DPI
// - SVG/vector -> Drawable
// - DPI-aware scaling
// Note: Some Figma blend modes are not supported; fallback to NORMAL.
// -----------------------------------------------------------------------------

//==============================================================================
// Paint definition
struct FigmaPaint
{
    enum class Type
    {
        solid,
        image,
        linearGradient,
        radialGradient
    };

    Type type = Type::solid;
    Colour colour;              // solid/stroke color
    float opacity = 1.0f;
    String imageRef;            // imageRef token from Figma
    String blendMode;           // "NORMAL", "MULTIPLY", etc.
    ColourGradient gradient;
    String strokeJoin;          // "MITER", "ROUND", "BEVEL"
    String strokeCap;           // "NONE", "ROUND", "SQUARE"
    Array<float> dashPattern;
    float dashOffset = 0.0f;
};

//==============================================================================
// Text style
struct FigmaTextStyle
{
    String fontFamily;
    float fontSize = 0.0f;
    Colour fillColour;
};

//==============================================================================
// Node definition
struct FigmaNode
{
    String id, name, type, textContent;
    juce::Rectangle<float> bounds;
    Array<FigmaPaint> fills, strokes;
    float strokeWeight = 0.0f;
    FigmaTextStyle textStyle;
    OwnedArray<FigmaNode> children;
};

//==============================================================================
// Figma API client + parser
class FigmaClient
{
public:
    explicit FigmaClient (const String& personalAccessToken) : token (personalAccessToken) {}

    // Fetch raw file JSON
    var fetchFile (const String& fileKey, String* err = nullptr)
    {
        URL url ("https://api.figma.com/v1/files/" + fileKey);
        StringPairArray headers;
        headers.set ("Authorization", "Bearer " + token);
        return fetchAndParseJSON (url, headers, err);
    }

    // Parse into FigmaNode tree
    std::unique_ptr<FigmaNode> parseDocumentTree (const var& fileJson)
    {
        if (auto* obj = fileJson.getDynamicObject())
            return parseNode (obj->getProperty ("document"));

        return {};
    }

    // Fetch raster image (PNG/JPG) at scale
    Image fetchNodeImage (const String& fileKey,
                          const String& nodeId,
                          const String& format = "png",
                          float scale = 1.0f,
                          String* err = nullptr)
    {
        URL url ("https://api.figma.com/v1/images/" + fileKey + "?ids=" + nodeId + "&format=" + format + "&scale=" + String (scale, 1));

        StringPairArray headers;
        headers.set ("Authorization", "Bearer " + token);

        auto json = fetchAndParseJSON (url, headers, err);
        if (json.isVoid())
            return {};

        String imageUrl;
        if (auto* obj = json.getDynamicObject())
        {
            auto images = obj->getProperty ("images");
            if (auto* imgObj = images.getDynamicObject())
                imageUrl = imgObj->getProperty (nodeId).toString();
        }

        if (imageUrl.isEmpty())
        {
            if (err != nullptr)
                *err = "No image URL returned for node " + nodeId;

            return {};
        }

        if (auto in = URL (imageUrl).createInputStream (false))
        {
            MemoryBlock mb;
            in->readIntoMemoryBlock (mb);
            return ImageFileFormat::loadFrom (mb.getData(), (int) mb.getSize());
        }

        if (err != nullptr)
            *err = "Failed to download image data from " + imageUrl;

        return {};
    }

    // Fetch SVG/Drawable for a node
    std::unique_ptr<Drawable> fetchNodeSVG (const String& fileKey, const String& nodeId)
    {
        StringPairArray headers;
        headers.set ("Authorization", "Bearer " + token);

        URL url ("https://api.figma.com/v1/images/" + fileKey + "?ids=" + nodeId + "&format=svg");

        const auto json = fetchAndParseJSON (url, headers, nullptr);

        if (auto* obj = json.getDynamicObject())
        {
            const auto images = obj->getProperty ("images");
            if (auto* imgObj = images.getDynamicObject())
            {
                const auto svgUrl = imgObj->getProperty (nodeId).toString();
                if (auto in = URL (svgUrl).createInputStream (false))
                    return Drawable::createFromImageDataStream (*in);
            }

        }

        return nullptr;
    }

private:
    String token;

    var fetchAndParseJSON (const URL& url, const StringPairArray& headers, String* errorMessage)
    {
        StringPairArray responseHeaders;
        auto stream = url.createInputStream (false, nullptr, &responseHeaders, headers, 20000, nullptr, true);

        if (stream == nullptr)
        {
            if (errorMessage)
                *errorMessage = "Failed to open stream to " + url.toString (true);

            return {};
        }

        const auto body = stream->readEntireStreamAsString();
/*
        if (const auto statusCode = stream->getStatusCode(); statusCode != 200)
        {
            if (errorMessage)
                *errorMessage = "HTTP error " + String (statusCode) + ": " + body;

            return {};
        }
*/
        var json;
        const auto parseResult = JSON::parse (body, json);
        if (parseResult.failed())
        {
            if (errorMessage)
                *errorMessage = "JSON parse failed: " + parseResult.getErrorMessage();

            return {};
        }

        return json;
    }

    // Recursive node parser
    std::unique_ptr<FigmaNode> parseNode (const var& nodeVar)
    {
        std::unique_ptr<FigmaNode> node;

        if (auto* obj = nodeVar.getDynamicObject())
        {
            node->id = obj->getProperty ("id").toString();
            node->name = obj->getProperty ("name").toString();
            node->type = obj->getProperty ("type").toString();

            // Bounds
            auto bbox = obj->getProperty ("absoluteBoundingBox");
            if (auto* boxObj = bbox.getDynamicObject())
            {
                const auto x = static_cast<float> (boxObj->getProperty ("x"));
                const auto y = static_cast<float> (boxObj->getProperty ("y"));
                const auto w = static_cast<float> (boxObj->getProperty ("width"));
                const auto h = static_cast<float> (boxObj->getProperty ("height"));
                node->bounds = {x, y, w, h};
            }

            // Fills
            const auto fills = obj->getProperty ("fills");
            if (fills.isArray())
            {
                for (auto& fvar : *fills.getArray())
                {
                    if (auto* fobj = fvar.getDynamicObject())
                    {
                        const auto typeStr = fobj->getProperty ("type").toString();
                        FigmaPaint p;
                        p.blendMode = fobj->getProperty ("blendMode").toString();

                        if (typeStr == "SOLID")
                        {
                            auto color = fobj->getProperty ("color");
                            if (auto* col = color.getDynamicObject())
                            {
                                const auto r = (float) static_cast<double> (col->getProperty ("r"));
                                const auto g = (float) static_cast<double> (col->getProperty ("g"));
                                const auto b = (float) static_cast<double> (col->getProperty ("b"));
                                const auto a = (float) static_cast<double> (fobj->getProperty ("opacity"));
                                p.type = FigmaPaint::Type::solid;
                                p.colour = Colour::fromFloatRGBA (r, g, b, a);
                                p.opacity = a;
                            }
                        }
                        else if (typeStr == "IMAGE")
                        {
                            p.type = FigmaPaint::Type::image;
                            p.opacity = (float) static_cast<double> (fobj->getProperty ("opacity"));
                            p.imageRef = fobj->getProperty ("imageRef").toString();
                        }
                        else if (typeStr == "GRADIENT_LINEAR" || typeStr == "GRADIENT_RADIAL")
                        {
                            p.type = typeStr == "GRADIENT_LINEAR"
                                        ? FigmaPaint::Type::linearGradient
                                        : FigmaPaint::Type::radialGradient;
                            p.opacity = (float) static_cast<double> (fobj->getProperty ("opacity"));
                            p.gradient = makeGradientFromFigmaFill (fobj, node->bounds);
                        }

                        node->fills.add (p);
                    }
                }
            }

            // Strokes
            if (const auto strokes = obj->getProperty ("strokes"); strokes.isArray())
            {
                for (auto& svar : *strokes.getArray())
                {
                    if (auto* sObj = svar.getDynamicObject())
                    {
                        const auto sType = sObj->getProperty ("type").toString();
                        if (sType == "SOLID")
                        {
                            auto colour = sObj->getProperty ("color");
                            if (auto* col = colour.getDynamicObject())
                            {
                                const auto r = (float) static_cast<double> (col->getProperty ("r"));
                                const auto g = (float) static_cast<double> (col->getProperty ("g"));
                                const auto b = (float) static_cast<double> (col->getProperty ("b"));
                                const auto a = (float) static_cast<double> (sObj->getProperty ("opacity"));
                                FigmaPaint sp;
                                sp.type = FigmaPaint::Type::solid;
                                sp.colour = Colour::fromFloatRGBA (r, g, b, a);
                                sp.opacity = a;

                                // stroke properties
                                sp.strokeJoin = sObj->getProperty ("strokeJoin").toString();
                                sp.strokeCap = sObj->getProperty ("strokeCap").toString();

                                if (auto dashArr = sObj->getProperty ("dashPattern"); dashArr.isArray())
                                    for (auto& d : *dashArr.getArray())
                                        sp.dashPattern.add ((float) static_cast<double> (d));

                                sp.dashOffset = (float) static_cast<double> (sObj->getProperty ("dashOffset"));

                                node->strokes.add (sp);
                            }
                        }
                    }
                }
            }

            node->strokeWeight = (float) static_cast<double> (obj->getProperty ("strokeWeight"));

            // Text
            if (node->type == "TEXT")
            {
                auto style = obj->getProperty ("style");
                if (auto* styleObj = style.getDynamicObject())
                {
                    node->textStyle.fontFamily = styleObj->getProperty ("fontFamily").toString();
                    node->textStyle.fontSize = (float) static_cast<double> (styleObj->getProperty ("fontSize"));
                    if (! node->fills.isEmpty())
                        node->textStyle.fillColour = node->fills[0].colour;
                }

                node->textContent = obj->getProperty ("characters").toString();
            }

            // Children
            if (auto children = obj->getProperty ("children"); children.isArray())
                for (auto& cvar : *children.getArray())
                    node->children.add (parseNode (cvar));
        }

        return std::move (node);
    }

    ColourGradient makeGradientFromFigmaFill (DynamicObject* fillObj, const juce::Rectangle<float>& bounds)
    {
        const ColourGradient fallback (Colours::white, bounds.getX(), bounds.getY(),
                                       Colours::black, bounds.getRight(), bounds.getBottom(),
                                       false);

        auto stopsVar = fillObj->getProperty ("gradientStops");
        if (! stopsVar.isArray())
            return fallback;

        struct Stop
        {
            float pos;
            Colour col;
        };

        std::vector<Stop> stops;

        for (auto& sv : *stopsVar.getArray())
        {
            if (auto* stopObj = sv.getDynamicObject())
            {
                const auto pos = (float) static_cast<double> (stopObj->getProperty ("position"));
                const auto& colorVar = stopObj->getProperty ("color");
                if (auto* cobj = colorVar.getDynamicObject())
                {
                    const auto r = (float) static_cast<double> (cobj->getProperty ("r"));
                    const auto g = (float) static_cast<double> (cobj->getProperty ("g"));
                    const auto b = (float) static_cast<double> (cobj->getProperty ("b"));
                    const auto a = (float) static_cast<double> (cobj->getProperty ("a"));
                    stops.push_back ({ pos, Colour::fromFloatRGBA (r, g, b, a) });
                }
            }
        }

        if (stops.empty())
            return fallback;

        const auto& gt = fillObj->getProperty ("gradientTransform");
        float a = 1, b = 0, tx = 0, c = 0, d = 1, ty = 0;
        if (auto* arr = gt.getArray(); arr != nullptr && arr->size() >= 2)
        {
            auto row0 = arr->operator[] (0);
            if (auto* r0 = row0.getArray())
            {
                if (r0->size() >= 3)
                {
                    a   = (float) static_cast<double> ((*r0)[0]);
                    b   = (float) static_cast<double> ((*r0)[1]);
                    tx  = (float) static_cast<double> ((*r0)[2]);
                }
            }

            auto row1 = arr->operator[] (1);
            if (auto* r1 = row1.getArray())
            {
                if (r1->size() >= 3)
                {
                    c  = (float) static_cast<double> ((*r1)[0]);
                    d  = (float) static_cast<double> ((*r1)[1]);
                    ty = (float) static_cast<double> ((*r1)[2]);
                }
            }
        }

        auto transformUnit = [&] (float u, float v) noexcept -> juce::Point<float>
        {
            const auto x = a * u + b * v + tx;
            const auto y = c * u + d * v + ty;
            return { bounds.getX() + x * bounds.getWidth(), bounds.getY() + y * bounds.getHeight() };
        };

        const auto p1 = transformUnit (0.0f, 0.0f);
        const auto p2 = transformUnit (1.0f, 0.0f);
        const bool isRadial = fillObj->getProperty ("type").toString() == "GRADIENT_RADIAL";

        ColourGradient grad (stops.front().col, p1.x, p1.y, stops.back().col, p2.x, p2.y, isRadial);
        for (auto& s : stops)
            grad.addColour (s.pos, s.col);

        return grad;
    }
};

//==============================================================================
// Figma -> JUCE renderer
class FigmaComponent : public Component
{
public:
    FigmaComponent (const FigmaNode& n, FigmaClient& c, const String& fk) :
        node (n),
        client (c),
        fileKey (fk)
    {
    }

    void paint (Graphics& g) override
    {
        g.fillAll (Colours::transparentBlack);
        paintNode (g, node);
    }

private:
    const FigmaNode& node;
    FigmaClient& client;
    String fileKey;
    float dpiScale = Desktop::getInstance().getGlobalScaleFactor();

    void paintNode (Graphics& g, const FigmaNode& n)
    {
        const auto& localF = n.bounds;

        // Fills
        for (const auto& f : n.fills)
        {
            if (f.type == FigmaPaint::Type::solid)
            {
                g.setColour (f.colour.withAlpha (f.opacity));
            }
            else if (f.type == FigmaPaint::Type::linearGradient || f.type == FigmaPaint::Type::radialGradient)
            {
                g.setGradientFill (f.gradient);
            }
            else if (f.type == FigmaPaint::Type::image)
            {
                if (auto img = client.fetchNodeImage (fileKey, f.imageRef, "png", dpiScale); img.isValid())
                    g.drawImage (img, localF, RectanglePlacement::centred | RectanglePlacement::stretchToFit);

                continue;
            }

            if (n.type == "RECTANGLE")
                g.fillRect (localF);
            else if (n.type == "ELLIPSE")
                g.fillEllipse (localF);
        }

        // Strokes
        if (! n.strokes.isEmpty() && n.strokeWeight > 0.0f)
        {
            Path p;
            if (n.type == "RECTANGLE")
                p.addRectangle (localF);
            else if (n.type == "ELLIPSE")
                p.addEllipse (localF);

            for (const auto& s : n.strokes)
            {
                auto joint = PathStrokeType::mitered;
                auto cap = PathStrokeType::butt;

                if (s.strokeJoin == "ROUND")
                    joint = PathStrokeType::curved;
                else if (s.strokeJoin == "BEVEL")
                    joint = PathStrokeType::beveled;

                if (s.strokeCap == "ROUND")
                    cap = PathStrokeType::rounded;
                else if (s.strokeCap == "SQUARE")
                    cap = PathStrokeType::square;

                g.setColour (s.colour.withAlpha (s.opacity));
                const auto scaledWeight = jmax (1.0f, n.strokeWeight * dpiScale);
                PathStrokeType strokeType (scaledWeight, joint, cap);

                if (! s.dashPattern.isEmpty())
                    strokeType.createDashedStroke (p, p, s.dashPattern.data(), s.dashPattern.size());

                g.strokePath (p, strokeType);
            }
        }

        // Text
        if (n.type == "TEXT")
        {
            const Font font (n.textStyle.fontFamily, n.textStyle.fontSize * dpiScale, Font::plain);
            g.setFont (font);
            g.setColour (n.textStyle.fillColour);
            g.drawText (n.textContent, localF, Justification::centred);
        }

        // Recursively paint children
        for (const auto& c : n.children)
            paintNode (g, *c);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FigmaComponent)
};
