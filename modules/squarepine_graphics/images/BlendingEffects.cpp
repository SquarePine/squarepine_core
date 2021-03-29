template<class PixelType, uint8 (*blendFunc) (uint8, uint8)>
void applyBlend (Image& dest, const Image& source, float alpha, Point<int> position, ThreadPool* threadPool)
{
    const auto rcLower = dest.getBounds();
    const auto rcUpper = source.getBounds().withPosition (position);

    const auto rcOverlap = rcLower.getIntersection (rcUpper);
    if (rcOverlap.isEmpty())
        return;

    const auto w = rcOverlap.getWidth();
    const auto h = rcOverlap.getHeight();

    const auto cropX = position.x < 0 ? -position.x : 0;
    const auto cropY = position.y < 0 ? -position.y : 0;

    threadPool = (w >= 256 || h >= 256) ? threadPool : nullptr;

    Image::BitmapData srcData (source, Image::BitmapData::readOnly);
    Image::BitmapData dstData (dest, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, threadPool, [&] (int y)
    {
        auto* lineSource = srcData.getLinePointer (cropY + y);
        auto* lineDest = dstData.getLinePointer (rcOverlap.getY() + y);

        lineSource += srcData.pixelStride * cropX;
        lineDest += dstData.pixelStride * rcOverlap.getX();

        for (int x = 0; x < w; x++)
        {
            auto* ac = (PixelType*) lineSource;
            auto* bc = (PixelType*) lineDest;

            auto ar = ac->getRed();
            auto ag = ac->getGreen();
            auto ab = ac->getBlue();
            auto aa = ac->getAlpha();
            auto br = bc->getRed();
            auto bg = bc->getGreen();
            auto bb = bc->getBlue();
            auto ba = bc->getAlpha();

            if (ba == 255)
            {
                auto pixelAlpha = alpha * aa / 255.0f;

                br = channelBlendAlpha (blendFunc (ar, br), br, pixelAlpha);
                bg = channelBlendAlpha (blendFunc (ag, bg), bg, pixelAlpha);
                bb = channelBlendAlpha (blendFunc (ab, bb), bb, pixelAlpha);
            }
            else
            {
                const auto srcAlpha = alpha * aa / 255.0f;
                const auto dstAlpha = ba / 255.0f;
                const auto outAlpha = srcAlpha + dstAlpha * (1.0f - srcAlpha);

                if (outAlpha == 0.0)
                {
                    br = 0;
                    bg = 0;
                    bb = 0;
                }
                else
                {
                    const auto r = blendFunc (ar, br);
                    const auto g = blendFunc (ag, bg);
                    const auto b = blendFunc (ab, bb);

                    br = (uint8) ((r * srcAlpha + br * dstAlpha * (1.0f - srcAlpha)) / outAlpha);
                    bg = (uint8) ((g * srcAlpha + bg * dstAlpha * (1.0f - srcAlpha)) / outAlpha);
                    bb = (uint8) ((b * srcAlpha + bb * dstAlpha * (1.0f - srcAlpha)) / outAlpha);
                }
            }

            bc->setARGB (ba, br, bg, bb);

            lineSource += srcData.pixelStride;
            lineDest += dstData.pixelStride;
        }
    });
}

template<class Type>
void applyBlend (Image& dest, const Image& source, BlendMode mode, float alpha, Point<int> position, ThreadPool* threadPool)
{
    switch (mode)
    {
        case BlendMode::normal:         applyBlend<Type, channelBlendNormal> (dest, source, alpha, position, threadPool); break;
        case BlendMode::lighten:        applyBlend<Type, channelBlendLighten> (dest, source, alpha, position, threadPool); break;
        case BlendMode::darken:         applyBlend<Type, channelBlendDarken> (dest, source, alpha, position, threadPool); break;
        case BlendMode::multiply:       applyBlend<Type, channelBlendMultiply> (dest, source, alpha, position, threadPool); break;
        case BlendMode::average:        applyBlend<Type, channelBlendAverage> (dest, source, alpha, position, threadPool); break;
        case BlendMode::add:            applyBlend<Type, channelBlendAdd> (dest, source, alpha, position, threadPool); break;
        case BlendMode::subtract:       applyBlend<Type, channelBlendSubtract> (dest, source, alpha, position, threadPool); break;
        case BlendMode::difference:     applyBlend<Type, channelBlendDifference> (dest, source, alpha, position, threadPool); break;
        case BlendMode::negation:       applyBlend<Type, channelBlendNegation> (dest, source, alpha, position, threadPool); break;
        case BlendMode::screen:         applyBlend<Type, channelBlendScreen> (dest, source, alpha, position, threadPool); break;
        case BlendMode::exclusion:      applyBlend<Type, channelBlendExclusion> (dest, source, alpha, position, threadPool); break;
        case BlendMode::overlay:        applyBlend<Type, channelBlendOverlay> (dest, source, alpha, position, threadPool); break;
        case BlendMode::softLight:      applyBlend<Type, channelBlendSoftLight> (dest, source, alpha, position, threadPool); break;
        case BlendMode::hardLight:      applyBlend<Type, channelBlendHardLight> (dest, source, alpha, position, threadPool); break;
        case BlendMode::colorDodge:     applyBlend<Type, channelBlendColorDodge> (dest, source, alpha, position, threadPool); break;
        case BlendMode::colorBurn:      applyBlend<Type, channelBlendColorBurn> (dest, source, alpha, position, threadPool); break;
        case BlendMode::linearDodge:    applyBlend<Type, channelBlendLinearDodge> (dest, source, alpha, position, threadPool); break;
        case BlendMode::linearBurn:     applyBlend<Type, channelBlendLinearBurn> (dest, source, alpha, position, threadPool); break;
        case BlendMode::linearLight:    applyBlend<Type, channelBlendLinearLight> (dest, source, alpha, position, threadPool); break;
        case BlendMode::vividLight:     applyBlend<Type, channelBlendVividLight> (dest, source, alpha, position, threadPool); break;
        case BlendMode::pinLight:       applyBlend<Type, channelBlendPinLight> (dest, source, alpha, position, threadPool); break;
        case BlendMode::hardMix:        applyBlend<Type, channelBlendHardMix> (dest, source, alpha, position, threadPool); break;
        case BlendMode::reflect:        applyBlend<Type, channelBlendReflect> (dest, source, alpha, position, threadPool); break;
        case BlendMode::glow:           applyBlend<Type, channelBlendGlow> (dest, source, alpha, position, threadPool); break;
        case BlendMode::phoenix:        applyBlend<Type, channelBlendPhoenix> (dest, source, alpha, position, threadPool); break;

        default:
            jassertfalse;
        break;
    }
}

template<class PixelType, uint8 (*blendFunc) (uint8, uint8)>
void applyBlend (Image& dest, Colour c, ThreadPool* threadPool)
{
    const auto w = dest.getWidth();
    const auto h = dest.getHeight();

    threadPool = (w >= 256 || h >= 256) ? threadPool : nullptr;

    Image::BitmapData dstData (dest, Image::BitmapData::readWrite);

    const auto ar = c.getRed();
    const auto ag = c.getGreen();
    const auto ab = c.getBlue();
    const auto aa = c.getAlpha();

    multiThreadedFor<int> (0, h, 1, threadPool, [&] (int y)
    {
        auto* lineDest = dstData.getLinePointer (y);

        for (int x = 0; x < w; x++)
        {
            auto* bc = (PixelType*) lineDest;

            auto br = bc->getRed();
            auto bg = bc->getGreen();
            auto bb = bc->getBlue();
            auto ba = bc->getAlpha();

            if (ba == 255)
            {
                const auto pixelAlpha = aa / 255.0f;

                br = channelBlendAlpha (blendFunc (ar, br), br, pixelAlpha);
                bg = channelBlendAlpha (blendFunc (ag, bg), bg, pixelAlpha);
                bb = channelBlendAlpha (blendFunc (ab, bb), bb, pixelAlpha);
            }
            else
            {
                const auto srcAlpha = aa / 255.0f;
                const auto dstAlpha = ba / 255.0f;
                const auto outAlpha = srcAlpha + dstAlpha * (1.0f - srcAlpha);

                if (outAlpha == 0.0)
                {
                    br = 0;
                    bg = 0;
                    bb = 0;
                }
                else
                {
                    auto r = blendFunc (ar, br);
                    auto g = blendFunc (ag, bg);
                    auto b = blendFunc (ab, bb);

                    br = uint8 ((r * srcAlpha + br * dstAlpha * (1.0f - srcAlpha)) / outAlpha);
                    bg = uint8 ((g * srcAlpha + bg * dstAlpha * (1.0f - srcAlpha)) / outAlpha);
                    bb = uint8 ((b * srcAlpha + bb * dstAlpha * (1.0f - srcAlpha)) / outAlpha);
                }
            }

            bc->setARGB (ba, br, bg, bb);

            lineDest += dstData.pixelStride;
        }
    });
}

template<class Type>
void applyBlend (Image& dest, BlendMode mode, Colour c, ThreadPool* threadPool)
{
    switch (mode)
    {
        case BlendMode::normal:         applyBlend<Type, channelBlendNormal<uint8>> (dest, c, threadPool); break;
        case BlendMode::lighten:        applyBlend<Type, channelBlendLighten> (dest, c, threadPool); break;
        case BlendMode::darken:         applyBlend<Type, channelBlendDarken> (dest, c, threadPool); break;
        case BlendMode::multiply:       applyBlend<Type, channelBlendMultiply> (dest, c, threadPool); break;
        case BlendMode::average:        applyBlend<Type, channelBlendAverage> (dest, c, threadPool); break;
        case BlendMode::add:            applyBlend<Type, channelBlendAdd> (dest, c, threadPool); break;
        case BlendMode::subtract:       applyBlend<Type, channelBlendSubtract> (dest, c, threadPool); break;
        case BlendMode::difference:     applyBlend<Type, channelBlendDifference> (dest, c, threadPool); break;
        case BlendMode::negation:       applyBlend<Type, channelBlendNegation> (dest, c, threadPool); break;
        case BlendMode::screen:         applyBlend<Type, channelBlendScreen> (dest, c, threadPool); break;
        case BlendMode::exclusion:      applyBlend<Type, channelBlendExclusion> (dest, c, threadPool); break;
        case BlendMode::overlay:        applyBlend<Type, channelBlendOverlay> (dest, c, threadPool); break;
        case BlendMode::softLight:      applyBlend<Type, channelBlendSoftLight> (dest, c, threadPool); break;
        case BlendMode::hardLight:      applyBlend<Type, channelBlendHardLight> (dest, c, threadPool); break;
        case BlendMode::colorDodge:     applyBlend<Type, channelBlendColorDodge> (dest, c, threadPool); break;
        case BlendMode::colorBurn:      applyBlend<Type, channelBlendColorBurn> (dest, c, threadPool); break;
        case BlendMode::linearDodge:    applyBlend<Type, channelBlendLinearDodge> (dest, c, threadPool); break;
        case BlendMode::linearBurn:     applyBlend<Type, channelBlendLinearBurn> (dest, c, threadPool); break;
        case BlendMode::linearLight:    applyBlend<Type, channelBlendLinearLight> (dest, c, threadPool); break;
        case BlendMode::vividLight:     applyBlend<Type, channelBlendVividLight> (dest, c, threadPool); break;
        case BlendMode::pinLight:       applyBlend<Type, channelBlendPinLight> (dest, c, threadPool); break;
        case BlendMode::hardMix:        applyBlend<Type, channelBlendHardMix> (dest, c, threadPool); break;
        case BlendMode::reflect:        applyBlend<Type, channelBlendReflect> (dest, c, threadPool); break;
        case BlendMode::glow:           applyBlend<Type, channelBlendGlow> (dest, c, threadPool); break;
        case BlendMode::phoenix:        applyBlend<Type, channelBlendPhoenix> (dest, c, threadPool); break;

        default:
            jassertfalse;
        break;
    }
}

void applyBlend (Image& dest, const Image& source, BlendMode mode, float alpha, Point<int> position, ThreadPool* threadPool)
{
    if (source.getFormat() != dest.getFormat())
    {
        auto copy = source.createCopy().convertedToFormat (dest.getFormat());

        if (source.getFormat() == Image::ARGB)
        {
            applyBlend<PixelARGB> (dest, copy, mode, alpha, position, threadPool);
        }
        else if (source.getFormat() == Image::RGB)
        {
            applyBlend<PixelRGB> (dest, copy, mode, alpha, position, threadPool);
        }
        else
        {
            jassertfalse;
        }
    }
    else
    {
        if (source.getFormat() == Image::ARGB)
        {
            applyBlend<PixelARGB> (dest, source, mode, alpha, position, threadPool);
        }
        else if (source.getFormat() == Image::RGB)
        {
            applyBlend<PixelRGB> (dest, source, mode, alpha, position, threadPool);
        }
        else
        {
            jassertfalse;
        }
    }
}

void applyBlend (Image& dest, BlendMode mode, Colour c, ThreadPool* threadPool)
{
    if (dest.getFormat() == Image::ARGB)
    {
        applyBlend<PixelARGB> (dest, mode, c, threadPool);
    }
    else if (dest.getFormat() == Image::RGB)
    {
        applyBlend<PixelRGB> (dest, mode, c, threadPool);
    }
    else
    {
        jassertfalse;
    }
}
