std::unique_ptr<Drawable> createDrawableFromSVG (const File& file)
{
    return svg::Parse::parse (file);
}

std::unique_ptr<Drawable> createDrawableFromSVG (const char* const data)
{
    if (data != nullptr)
        if (auto d = XmlDocument::parse (String (data)))
            return svg::Parse::parse (*d);

    jassertfalse;
    return {};
}

Image createImageFromDrawable (Drawable* drawable, int width, int height)
{
    jassert (width > 0 && height > 0);

    Image image (Image::ARGB, width, height, true);
    Graphics g (image);

    drawable->drawWithin (g, Rectangle<int> (width, height).toFloat(), RectanglePlacement::centred, 1.0f);
    return image;
}

void applyToDrawableColours (Component& component, std::function<void (Colour&)> func)
{
    auto apply = [&] (Drawable& child)
    {
        auto applyToFill = [&] (FillType& fill)
        {
            if (fill.isColour())
            {
                func (fill.colour);
            }
            else if (fill.isGradient())
            {
                auto& grad = *fill.gradient;
                for (int i = 0; i < grad.getNumColours(); ++i)
                {
                    auto col = grad.getColour (i);
                    func (col);
                    grad.setColour (i, col);
                }
            }
        };

        if (auto* ds = dynamic_cast<DrawableShape*> (&child))
        {
            auto fill = ds->getFill();
            auto strokeFill = ds->getStrokeFill();

            applyToFill (fill);
            applyToFill (strokeFill);

            ds->setFill (fill);
            ds->setStrokeFill (strokeFill);
        }

        if (auto* dt = dynamic_cast<DrawableText*> (&child))
        {
            auto col = dt->getColour();
            func (col);
            dt->setColour (col);
        }
    };

    if (auto* d = dynamic_cast<Drawable*> (&component))
        apply (*d);

    for (auto* c : component.getChildren ())
        applyToDrawableColours (*c, func);
}

void replaceAllDrawableColours (Component& component, Colour colour)
{
    auto replace = [colour] (Drawable& child)
    {
        auto replaceFill = [colour] (FillType& fill)
        {
            if (fill.isColour())
            {
                fill.colour = colour;
            }
            else if (fill.isGradient())
            {
                auto& grad = *fill.gradient;
                for (int i = 0; i < grad.getNumColours(); ++i)
                    grad.setColour (i, colour);
            }
        };

        if (auto* ds = dynamic_cast<DrawableShape*> (&child))
        {
            auto fill = ds->getFill();
            auto strokeFill = ds->getStrokeFill();

            replaceFill (fill);
            replaceFill (strokeFill);

            ds->setFill (fill);
            ds->setStrokeFill (strokeFill);
        }

        if (auto* dt = dynamic_cast<DrawableText*> (&child))
            dt->setColour (colour);
    };

    if (auto* d = dynamic_cast<Drawable*> (&component))
        replace (*d);

    for (auto* c : component.getChildren ())
        replaceAllDrawableColours (*c, colour);
}

void replaceAllDrawableColours (Component& component,
                                Colour source,
                                Colour dest)
{
    auto replace = [source, dest] (Drawable& child)
    {
        auto replaceFill = [source, dest] (FillType& fill)
        {
            if (fill.isColour())
            {
                if (fill.colour == source)
                    fill.colour = dest;
            }
            else if (fill.isGradient())
            {
                auto& grad = *fill.gradient;
                for (int i = 0; i < grad.getNumColours(); ++i)
                    if (grad.getColour (i) == source)
                        grad.setColour (i, dest);
            }
        };

        if (auto* ds = dynamic_cast<DrawableShape*> (&child))
        {
            auto fill = ds->getFill();
            auto strokeFill = ds->getStrokeFill();

            replaceFill (fill);
            replaceFill (strokeFill);

            ds->setFill (fill);
            ds->setStrokeFill (strokeFill);
        }

        if (auto* dt = dynamic_cast<DrawableText*> (&child))
            if (dt->getColour() == source)
                dt->setColour (dest);
    };

    if (auto* d = dynamic_cast<Drawable*> (&component))
        replace (*d);

    for (auto* c : component.getChildren ())
        replaceAllDrawableColours (*c, source, dest);
}
