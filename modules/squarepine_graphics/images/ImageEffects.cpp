JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wall", "-Wconversion", "-Wimplicit-const-int-float-conversion", "-Wsign-conversion")

//==============================================================================
template<typename Type>
uint8_t toByte (Type v)
{
    return (uint8_t) std::clamp (v, static_cast<Type> (0), static_cast<Type> (255));
}

uint8_t getIntensity (uint8_t r, uint8_t g, uint8_t b)
{
    return (uint8_t) ((7471 * b + 38470 * g + 19595 * r) >> 16);
}

uint8_t computeAlpha (uint8_t la, uint8_t ra)
{
    return (uint8_t) (((la * (256 - (ra + (ra >> 7)))) >> 8) + ra);
}

//==============================================================================
template<class PixelType>
PixelType blend (const PixelType& c1, const PixelType& c2)
{
    const auto a = c1.getAlpha();
    const auto invA = 255 - a;
    const auto r = ((c2.getRed()   * invA) + (c1.getRed()   * a)) / 256;
    const auto g = ((c2.getGreen() * invA) + (c1.getGreen() * a)) / 256;
    const auto b = ((c2.getBlue()  * invA) + (c1.getBlue()  * a)) / 256;
    const auto a2 = computeAlpha (c2.getAlpha(), c1.getAlpha());

    PixelType res;
    res.setARGB (a2, toByte (r), toByte (g), toByte (b));
    return res;
}

template<class ValueType, class ResultType>
inline ResultType convert (const ValueType& in)
{
    ResultType out;
    out.setARGB (in.getAlpha(), in.getRed(), in.getGreen(), in.getBlue());
    return out;
}

//==============================================================================
template<class Type>
void applyVignette (Image& img, float amountIn, float radiusIn, float fallOff, ThreadPool* threadPool)
{
    const auto w = img.getWidth();
    const auto h = img.getHeight();
    threadPool = (w >= 256 || h >= 256) ? threadPool : nullptr;

    auto outA = w * 0.5 * radiusIn;
    auto outB = h * 0.5 * radiusIn;
    auto inA = outA * fallOff;
    auto inB = outB * fallOff;
    auto cx = w * 0.5;
    auto cy = h * 0.5;
    auto amount = 1.0 - amountIn;

    Image::BitmapData data (img, Image::BitmapData::readWrite);

    Ellipse<double> outE { outA, outB }, inE { inA, inB };

    multiThreadedFor<int> (0, h, 1, threadPool, [&] (int y)
    {
        auto* p = data.getLinePointer (y);

        auto dy = y - cy;

        for (int x = 0; x < w; x++)
        {
            const auto dx = x - cx;

            const bool outside = outE.isPointOutside ({ dx, dy });
            const bool inside  = inE.isPointInside ({ dx, dy });

            auto* s = (Type*) p;

            if (outside)
            {
                const auto r = toByte (0.5 + (s->getRed() * amount));
                const auto g = toByte (0.5 + (s->getGreen() * amount));
                const auto b = toByte (0.5 + (s->getBlue() * amount));
                const auto a = s->getAlpha();

                s->setARGB (a, r, g, b);
            }
            else if (! inside)
            {
                const auto angle = std::atan2 (dy, dx);
                const auto p1 = outE.getPointAtAngle (angle);
                const auto p2 = inE.getPointAtAngle (angle);
                const auto l1 = Line<double> ({ dx, dy }, p2);
                const auto l2 = Line<double> (p1, p2);
                const auto factor = 1.0 - (amountIn * jlimit (0.0, 1.0, l1.getLength() / l2.getLength()));

                const auto r = toByte (0.5 + (s->getRed() * factor));
                const auto g = toByte (0.5 + (s->getGreen() * factor));
                const auto b = toByte (0.5 + (s->getBlue() * factor));
                const auto a = s->getAlpha();

                s->setARGB (a, r, g, b);
            }

            p += data.pixelStride;
        }
    });
}

//==============================================================================
template<class T>
void applySepia (Image& img, ThreadPool* threadPool)
{
    const auto w = img.getWidth();
    const auto h = img.getHeight();

    threadPool = (w >= 256 || h >= 256) ? threadPool : nullptr;

    Image::BitmapData data (img, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, threadPool, [&] (int y)
    {
        auto* p = data.getLinePointer (y);

        for (int x = 0; x < w; x++)
        {
            auto* s = (PixelARGB*)p;

            auto r = s->getRed();
            auto g = s->getGreen();
            auto b = s->getBlue();
            auto a = s->getAlpha();
            auto ro = toByte ((r * .393) + (g *.769) + (b * .189));
            auto go = toByte ((r * .349) + (g *.686) + (b * .168));
            auto bo = toByte ((r * .272) + (g *.534) + (b * .131));

            s->setARGB (a, ro, go, bo);

            p += data.pixelStride;
        }
    });
}

template<class T>
void applyGreyScale (Image& img, ThreadPool* threadPool)
{
    const auto w = img.getWidth();
    const auto h = img.getHeight();
    threadPool = (w >= 256 || h >= 256) ? threadPool : nullptr;

    Image::BitmapData data (img, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, threadPool, [&] (int y)
    {
        auto* p = data.getLinePointer (y);

        for (int x = 0; x < w; x++)
        {
            auto* s = (T*)p;

            auto r = s->getRed();
            auto g = s->getGreen();
            auto b = s->getBlue();
            auto a = s->getAlpha();
            auto ro = toByte (r * 0.30 + 0.5);
            auto go = toByte (g * 0.59 + 0.5);
            auto bo = toByte (b * 0.11 + 0.5);

            s->setARGB (a,
                        toByte (ro + go + bo),
                        toByte (ro + go + bo),
                        toByte (ro + go + bo));

            p += data.pixelStride;
        }
    });
}

template<class T>
void applySoften (Image& img, ThreadPool* threadPool)
{
    const auto w = img.getWidth();
    const auto h = img.getHeight();
    threadPool = (w >= 256 || h >= 256) ? threadPool : nullptr;

    Image dst (img.getFormat(), w, h, true);

    Image::BitmapData srcData (img, Image::BitmapData::readOnly);
    Image::BitmapData dstData (dst, Image::BitmapData::writeOnly);

    multiThreadedFor<int> (0, h, 1, threadPool, [&] (int y)
    {
        for (int x = 0; x < w; x++)
        {
            int ro = 0, go = 0, bo = 0;
            uint8_t a = 0;

            for (int m = -1; m <= 1; m++)
            {
                for (int n = -1; n <= 1; n++)
                {
                    int cx = jlimit (0, w - 1, x + m);
                    int cy = jlimit (0, h - 1, y + n);

                    T* s = (T*) srcData.getPixelPointer (cx, cy);

                    ro += s->getRed();
                    go += s->getGreen();
                    bo += s->getBlue();
                }
            }

            auto* s = (T*) srcData.getPixelPointer (x, y);
            a = s->getAlpha();

            auto* d = (T*) dstData.getPixelPointer (x, y);

            d->setARGB (a, toByte (ro / 9), toByte (go / 9), toByte (bo / 9));
        }
    });
    img = dst;
}

template<class T>
void applySharpen (Image& img, ThreadPool* threadPool)
{
    const auto w = img.getWidth();
    const auto h = img.getHeight();
    threadPool = (w >= 256 || h >= 256) ? threadPool : nullptr;

    Image dst (img.getFormat(), w, h, true);

    Image::BitmapData srcData (img, Image::BitmapData::readOnly);
    Image::BitmapData dstData (dst, Image::BitmapData::writeOnly);

    multiThreadedFor<int> (0, h, 1, threadPool, [&] (int y)
    {
        for (int x = 0; x < w; x++)
        {
            auto getPixelPointer = [&] (int cx, int cy) -> T*
            {
                cx = jlimit (0, w - 1, cx);
                cy = jlimit (0, h - 1, cy);

                return (T*) srcData.getPixelPointer (cx, cy);
            };

            int ro = 0, go = 0, bo = 0;
            uint8 ao = 0;

            auto* s = getPixelPointer (x, y);

            ro = s->getRed()   * 5;
            go = s->getGreen() * 5;
            bo = s->getBlue()  * 5;
            ao = s->getAlpha();

            s = getPixelPointer (x, y - 1);
            ro -= s->getRed();
            go -= s->getGreen();
            bo -= s->getBlue();

            s = getPixelPointer (x - 1, y);
            ro -= s->getRed();
            go -= s->getGreen();
            bo -= s->getBlue();

            s = getPixelPointer (x + 1, y);
            ro -= s->getRed();
            go -= s->getGreen();
            bo -= s->getBlue();

            s = getPixelPointer (x, y + 1);
            ro -= s->getRed();
            go -= s->getGreen();
            bo -= s->getBlue();

            auto* d = (T*) dstData.getPixelPointer (x, y);

            d->setARGB (ao, toByte (ro), toByte (go), toByte (bo));
        }
    });

    img = dst;
}

template<class T>
void applyGamma (Image& img, float gamma, ThreadPool* threadPool)
{
    const auto w = img.getWidth();
    const auto h = img.getHeight();
    threadPool = (w >= 256 || h >= 256) ? threadPool : nullptr;

    Image::BitmapData data (img, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, threadPool, [&] (int y)
    {
        auto* p = data.getLinePointer (y);

        for (int x = 0; x < w; x++)
        {
            auto* s = (T*)p;

            auto r = s->getRed();
            auto g = s->getGreen();
            auto b = s->getBlue();
            auto a = s->getAlpha();
            auto ro = toByte (std::pow (r / 255.0, gamma) * 255.0 + 0.5);
            auto go = toByte (std::pow (g / 255.0, gamma) * 255.0 + 0.5);
            auto bo = toByte (std::pow (b / 255.0, gamma) * 255.0 + 0.5);

            s->setARGB (a, ro, go, bo);

            p += data.pixelStride;
        }
    });
}

template<class T>
void applyInvert (Image& img, ThreadPool* threadPool)
{
    const auto w = img.getWidth();
    const auto h = img.getHeight();
    threadPool = (w >= 256 || h >= 256) ? threadPool : nullptr;

    Image::BitmapData data (img, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, threadPool, [&] (int y)
    {
        auto* p = data.getLinePointer (y);

        for (int x = 0; x < w; x++)
        {
            auto* s = (T*) p;

            const auto r = s->getRed();
            const auto g = s->getGreen();
            const auto b = s->getBlue();
            const auto ro = 255 - r;
            const auto go = 255 - g;
            const auto bo = 255 - b;

            s->setARGB ((uint8) s->getAlpha(), (uint8) ro, (uint8) go, (uint8) bo);

            p += data.pixelStride;
        }
    });
}

//==============================================================================
template<class T>
void applyContrast (Image& img, float contrast, ThreadPool* threadPool)
{
    const auto w = img.getWidth();
    const auto h = img.getHeight();
    threadPool = (w >= 256 || h >= 256) ? threadPool : nullptr;

    contrast = (100.0f + contrast) / 100.0f;
    contrast = square (contrast);

    Image::BitmapData data (img, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, threadPool, [&] (int y)
    {
        auto* p = data.getLinePointer (y);

        for (int x = 0; x < w; x++)
        {
            auto* s = (T*)p;

            auto r = s->getRed();
            auto g = s->getGreen();
            auto b = s->getBlue();
            auto a = s->getAlpha();

            auto ro = (double) r / 255.0;
            ro = ro - 0.5;
            ro = ro * contrast;
            ro = ro + 0.5;
            ro = ro * 255.0;

            auto go = (double) g / 255.0;
            go = go - 0.5;
            go = go * contrast;
            go = go + 0.5;
            go = go * 255.0;

            auto bo = (double) b / 255.0;
            bo = bo - 0.5;
            bo = bo * contrast;
            bo = bo + 0.5;
            bo = bo * 255.0;

            ro = toByte (ro);
            go = toByte (go);
            bo = toByte (bo);

            s->setARGB (a, toByte (ro), toByte (go), toByte (bo));

            p += data.pixelStride;
        }
    });
}

//==============================================================================
template<class T>
void applyBrightnessContrast (Image& img, float brightness, float contrast, ThreadPool* threadPool)
{
    const auto w = img.getWidth();
    const auto h = img.getHeight();

    threadPool = (w >= 256 || h >= 256) ? threadPool : nullptr;

    Image::BitmapData data (img, Image::BitmapData::readWrite);

    auto multiply = 1.0f;
    auto divide = 1.0f;

    if (contrast < 0.0f)
    {
        multiply = contrast + 100;
        divide = 100.0f;
    }
    else if (contrast > 0.0f)
    {
        multiply = 100.0f;
        divide = 100.0f - contrast;
    }
    else
    {
        multiply = 1.0f;
        divide = 1.0f;
    }

    auto* rgbTable = new uint8_t[65536];

    if (divide == 0.0f)
    {
        for (int intensity = 0; intensity < 256; intensity++)
        {
            if ((float) intensity + brightness < 128.0f)
                rgbTable[intensity] = 0;
            else
                rgbTable[intensity] = 255;
        }
    }
    else if (divide == 100.0f)
    {
        for (int intensity = 0; intensity < 256; intensity++)
        {
            auto shift = int ((intensity - 127.0f) * multiply / divide + 127.0f - (float) intensity + brightness);

            for (int col = 0; col < 256; col++)
            {
                auto index = (intensity * 256) + col;
                rgbTable[index] = toByte (col + shift);
            }
        }
    }
    else
    {
        for (int intensity = 0; intensity < 256; intensity++)
        {
            auto shift = int ((intensity - 127.0f + brightness) * multiply / divide + 127.0f - intensity);

            for (int col = 0; col < 256; col++)
            {
                auto index = (intensity * 256) + col;
                rgbTable[index] = toByte (col + shift);
            }
        }
    }

    multiThreadedFor<int> (0, h, 1, threadPool, [&] (int y)
    {
        auto* p = data.getLinePointer (y);

        for (int x = 0; x < w; x++)
        {
            auto* s = (T*)p;

            auto r = s->getRed();
            auto g = s->getGreen();
            auto b = s->getBlue();
            auto a = s->getAlpha();

            if (divide == 0)
            {
                auto i = getIntensity (toByte (r), toByte (g), toByte (b));
                auto c = rgbTable[i];

                s->setARGB (a, c, c, c);
            }
            else
            {
                auto i = getIntensity (toByte (r), toByte (g), toByte (b));
                auto shiftIndex = i * 256;

                auto ro = rgbTable[shiftIndex + r];
                auto go = rgbTable[shiftIndex + g];
                auto bo = rgbTable[shiftIndex + b];

                ro = toByte (ro);
                go = toByte (go);
                bo = toByte (bo);

                s->setARGB (a, ro, go, bo);
            }

            p += data.pixelStride;
        }
    });

    delete[] rgbTable;
}

//==============================================================================
template<class T>
void applyHueSaturationLightness (Image& img, float hueIn, float saturation, float lightness, ThreadPool* threadPool)
{
    const auto w = img.getWidth();
    const auto h = img.getHeight();
    threadPool = (w >= 256 || h >= 256) ? threadPool : nullptr;

    if (saturation > 100)
        saturation = ((saturation - 100) * 3) + 100;

    saturation = (saturation * 1024) / 100;

    hueIn /= 360.0f;

    Image::BitmapData data (img, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, threadPool, [&] (int y)
    {
        auto* p = data.getLinePointer (y);

        for (int x = 0; x < w; x++)
        {
            auto* s = (T*)p;

            auto r = s->getRed();
            auto g = s->getGreen();
            auto b = s->getBlue();
            auto a = s->getAlpha();

            auto intensity = getIntensity (toByte (r), toByte (g), toByte (b));
            auto ro = toByte (int (intensity * 1024 + (r - intensity) * saturation) >> 10);
            auto go = toByte (int (intensity * 1024 + (g - intensity) * saturation) >> 10);
            auto bo = toByte (int (intensity * 1024 + (b - intensity) * saturation) >> 10);

            Colour c (toByte (ro), toByte (go), toByte (bo));
            auto hue = c.getHue();
            hue += hueIn;

            while (hue < 0.0f)  hue += 1.0f;
            while (hue >= 1.0f) hue -= 1.0f;

            c = Colour::fromHSV (hue, c.getSaturation(), c.getBrightness(), float (a));
            ro = c.getRed();
            go = c.getGreen();
            bo = c.getBlue();

            ro = toByte (ro);
            go = toByte (go);
            bo = toByte (bo);

            s->setARGB (a, toByte (ro), toByte (go), toByte (bo));

            if (lightness > 0)
            {
                auto blended = blend (PixelARGB (toByte ((lightness * 255) / 100 * (a / 255.0)), 255, 255, 255), convert<T, PixelARGB> (*s));
                *s = convert<PixelARGB, T> (blended);
            }
            else if (lightness < 0)
            {
                auto blended = blend (PixelARGB (toByte ((-lightness * 255) / 100 * (a / 255.0)), 0, 0, 0), convert<T, PixelARGB> (*s));
                *s = convert<PixelARGB, T> (blended);
            }

            p += data.pixelStride;
        }
    });
}

//==============================================================================
template<class T>
void applyGradientMap (Image& img, const ColourGradient& gradient, ThreadPool* threadPool)
{
    const auto w = img.getWidth();
    const auto h = img.getHeight();
    threadPool = (w >= 256 || h >= 256) ? threadPool : nullptr;

    Image::BitmapData data (img, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, threadPool, [&] (int y)
    {
        auto* p = data.getLinePointer (y);

        for (int x = 0; x < w; x++)
        {
            auto* s = (T*)p;

            auto r = s->getRed();
            auto g = s->getGreen();
            auto b = s->getBlue();
            auto a = s->getAlpha();
            auto ro = toByte (r * 0.30 + 0.5);
            auto go = toByte (g * 0.59 + 0.5);
            auto bo = toByte (b * 0.11 + 0.5);

            auto proportion = float (ro + go + bo) / 256.0f;
            auto c = gradient.getColourAtPosition (proportion);

            s->setARGB (a,
                        c.getRed(),
                        c.getGreen(),
                        c.getBlue());

            p += data.pixelStride;
        }
    });
}

//==============================================================================
template<class T>
void applyColour (Image& img, Colour c, ThreadPool* threadPool)
{
    const auto w = img.getWidth();
    const auto h = img.getHeight();
    threadPool = (w >= 256 || h >= 256) ? threadPool : nullptr;

    auto r = c.getRed();
    auto g = c.getGreen();
    auto b = c.getBlue();
    auto a = c.getAlpha();

    Image::BitmapData data (img, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, threadPool, [&] (int y)
    {
        auto* p = data.getLinePointer (y);

        for (int x = 0; x < w; x++)
        {
            auto* s = (T*)p;
            s->setARGB (a, r, g, b);
            p += data.pixelStride;
        }
    });
}

//==============================================================================
void applyVignette (Image& img, float amountIn, float radiusIn, float fallOff, ThreadPool* threadPool)
{
    if (img.getFormat() == Image::ARGB)
        applyVignette<PixelARGB> (img, amountIn, radiusIn, fallOff, threadPool);
    else if (img.getFormat() == Image::RGB)
        applyVignette<PixelRGB>  (img, amountIn, radiusIn, fallOff, threadPool);
    else
        jassertfalse;
}

void applySepia (Image& img, ThreadPool* threadPool)
{
    if (img.getFormat() == Image::ARGB)
        applySepia<PixelARGB> (img, threadPool);
    else if (img.getFormat() == Image::RGB)
        applySepia<PixelRGB>  (img, threadPool);
    else
        jassertfalse;
}

void applyGreyScale (Image& img, ThreadPool* threadPool)
{
    if (img.getFormat() == Image::ARGB)
        applyGreyScale<PixelARGB> (img, threadPool);
    else if (img.getFormat() == Image::RGB)
        applyGreyScale<PixelRGB>  (img, threadPool);
    else jassertfalse;
}

void applySoften (Image& img, ThreadPool* threadPool)
{
    if (img.getFormat() == Image::ARGB)
        applySoften<PixelARGB> (img, threadPool);
    else if (img.getFormat() == Image::RGB)
        applySoften<PixelRGB>  (img, threadPool);
    else
        jassertfalse;
}

void applySharpen (Image& img, ThreadPool* threadPool)
{
    if (img.getFormat() == Image::ARGB)
        applySharpen<PixelARGB> (img, threadPool);
    else if (img.getFormat() == Image::RGB)
        applySharpen<PixelRGB>  (img, threadPool);
    else
        jassertfalse;
}

void applyGamma (Image& img, float gamma, ThreadPool* threadPool)
{
    if (img.getFormat() == Image::ARGB)
        applyGamma<PixelARGB> (img, gamma, threadPool);
    else if (img.getFormat() == Image::RGB)
        applyGamma<PixelRGB>  (img, gamma, threadPool);
    else
        jassertfalse;
}

void applyInvert (Image& img, ThreadPool* threadPool)
{
    if (img.getFormat() == Image::ARGB)
        applyInvert<PixelARGB> (img, threadPool);
    else if (img.getFormat() == Image::RGB)
        applyInvert<PixelRGB>  (img, threadPool);
    else
        jassertfalse;
}

void applyContrast (Image& img, float contrast, ThreadPool* threadPool)
{
    if (img.getFormat() == Image::ARGB)
        applyContrast<PixelARGB> (img, contrast, threadPool);
    else if (img.getFormat() == Image::RGB)
        applyContrast<PixelRGB>  (img, contrast, threadPool);
    else
        jassertfalse;
}

void applyBrightnessContrast (Image& img, float brightness, float contrast, ThreadPool* threadPool)
{
    if (img.getFormat() == Image::ARGB)
        applyBrightnessContrast<PixelARGB> (img, brightness, contrast, threadPool);
    else if (img.getFormat() == Image::RGB)
        applyBrightnessContrast<PixelRGB>  (img, brightness, contrast, threadPool);
    else
        jassertfalse;
}

void applyHueSaturationLightness (Image& img, float hue, float saturation, float lightness, ThreadPool* threadPool)
{
    if (img.getFormat() == Image::ARGB)
        applyHueSaturationLightness<PixelARGB> (img, hue, saturation, lightness, threadPool);
    else if (img.getFormat() == Image::RGB)
        applyHueSaturationLightness<PixelRGB>  (img, hue, saturation, lightness, threadPool);
    else
        jassertfalse;
}

void applyGradientMap (Image& img, const ColourGradient& gradient, ThreadPool* threadPool)
{
    if (img.getFormat() == Image::ARGB)
        applyGradientMap<PixelARGB> (img, gradient, threadPool);
    else if (img.getFormat() == Image::RGB)
        applyGradientMap<PixelRGB>  (img, gradient, threadPool);
    else
        jassertfalse;
}

void applyColour (Image& img, Colour c, ThreadPool* threadPool)
{
    if (img.getFormat() == Image::ARGB)
        applyColour<PixelARGB> (img, c, threadPool);
    else if (img.getFormat() == Image::RGB)
        applyColour<PixelRGB>  (img, c, threadPool);
    else
        jassertfalse;
}

//==============================================================================
void applyGradientMap (Image& img, const Colour c1, const Colour c2, ThreadPool* threadPool)
{
    ColourGradient g;
    g.addColour (0.0, c1);
    g.addColour (1.0, c2);

    applyGradientMap (img, g, threadPool);
}

JUCE_END_IGNORE_WARNINGS_GCC_LIKE
