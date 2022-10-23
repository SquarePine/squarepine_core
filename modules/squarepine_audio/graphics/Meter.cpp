double DecibelHelpers::calculateSnapPoint (double snapDb) noexcept
{
    return decibelsToMeterProportion (snapDb, minSliderLevelDb, maxSliderLevelDb);
}

double DecibelHelpers::gainToMeterProportion (double gain, int minimumDecibels, int maximumDecibels) noexcept
{
    const auto decibels = jlimit ((double) minimumDecibels, (double) maximumDecibels, Decibels::gainToDecibels (gain));
    return decibelsToMeterProportion (decibels, minimumDecibels, maximumDecibels);
}

double DecibelHelpers::meterProportionToGain (double meterProportion, int minimumDecibels, int maximumDecibels) noexcept
{
    const auto decibels = meterProportionToDecibels (meterProportion, minimumDecibels, maximumDecibels);
    return jlimit ((double) minimumDecibels, (double) maximumDecibels, Decibels::decibelsToGain(decibels));
}

double DecibelHelpers::decibelsToMeterProportion (double decibels, int minimumDecibels, int maximumDecibels) noexcept
{
    decibels = jlimit ((double) minimumDecibels, (double) maximumDecibels, decibels);
    const auto linearProportion = (decibels - minimumDecibels) / (double) (maximumDecibels - minimumDecibels);
    return linearToCurved (linearProportion);
}

double DecibelHelpers::meterProportionToDecibels (double meterProportion, int minimumDecibels, int maximumDecibels) noexcept
{
    const auto linearProportion = curvedToLinear (meterProportion);
    const auto decibels = minimumDecibels + linearProportion * (maximumDecibels - minimumDecibels);
    return jlimit ((double) minimumDecibels, (double) maximumDecibels, decibels);
}

double DecibelHelpers::linearToCurved (double value) noexcept
{
    const auto curvedValue = (std::exp (value * std::log (1 + curveTensionDb)) - 1.0) / (double) curveTensionDb;
    return jlimit (0.0, 1.0, curvedValue);
}

double DecibelHelpers::curvedToLinear (double curvedValue) noexcept
{
    return std::log (1.0 + (double) curveTensionDb * curvedValue) / std::log (1.0 + (double) curveTensionDb);
}

//==============================================================================
Meter::Meter (bool willNeedMaxLevel) :
    needMaxLevel (willNeedMaxLevel)
{
    channels.resize (2);

    setGradientColours (Colours::darkblue, Colours::darkorange, Colours::indianred);
}

bool Meter::refreshLevels()
{
    levels.clearQuick();
    getChannelLevels (levels);

    bool areLevelsDifferent = false;
    bool isMaxLevelDelayExpired = false;
    const auto numChans = jmin (levels.size(), channels.size());

    for (int i = 0; i < numChans; ++i)
    {
        auto& context = channels.getReference (i);
        auto level = lerp (context.getLevel(), levels[i], 0.9f);
        dsp::util::snapToZero (level);
        context.setLevel (level);

        if (needMaxLevel)
        {
            if (context.getLevel() > context.getMaxLevel())
            {
                context.setLastMaxAudioLevelTime (Time::currentTimeMillis());
                context.setMaxLevel (context.getLevel());
            }
            else if (Time::currentTimeMillis() - context.getLastMaxAudioLevelTime() > maxLevelExpiryMs)
            {
                context.setMaxLevel (context.getMaxLevel() * 0.8f); //Decay rate
                isMaxLevelDelayExpired = true;
            }

            areLevelsDifferent |= context.getMaxLevel() != context.getLastMaxLevel();
        }

        areLevelsDifferent |= context.getLevel() != context.getLastLevel();

        context.setLastLevel (context.getLevel());
        context.setLastMaxLevel (context.getMaxLevel());
    }

    if (areLevelsDifferent)
        updateClippingLevel (isMaxLevelDelayExpired);

    return areLevelsDifferent;
}

void Meter::initVolumeGradient (int width, int height, bool isVertical)
{
    if (width <= 0 || height <= 0)
        return;

    ColourGradient gradient;
    const auto low = (float) DecibelHelpers::decibelsToMeterProportion (-18.0);
    const auto mid = (float) DecibelHelpers::decibelsToMeterProportion (-9.0);
    const auto high = (float) DecibelHelpers::decibelsToMeterProportion (0.0);

    if (isVertical)
    {
        gradient = ColourGradient::vertical (colourHighIntensity, 0.0f, colourLowIntensity, (float) height);
        gradient.addColour (1.0f - high, colourHighIntensity);
        gradient.addColour (1.0f - mid, colourMediumIntensity);
        gradient.addColour (1.0f - low, colourLowIntensity);
    }
    else
    {
        gradient = ColourGradient::horizontal (colourLowIntensity, 0.0f, colourHighIntensity, (float) width);
        gradient.addColour (high, colourHighIntensity);
        gradient.addColour (mid, colourMediumIntensity);
        gradient.addColour (low, colourLowIntensity);
    }

    gradientImage = { Image::RGB, width, height, false };
    Graphics g (gradientImage);
    g.setGradientFill (gradient);
    g.fillAll();
}

void Meter::setGradientColours (Colour firstColour, Colour secondColour)
{
    colourLowIntensity = firstColour;
    colourMediumIntensity = firstColour.interpolatedWith (secondColour, 0.5f);
    colourHighIntensity = secondColour;
}

void Meter::setGradientColours (Colour firstColour, Colour secondColour, Colour thirdColour)
{
    colourLowIntensity = firstColour;
    colourMediumIntensity = secondColour;
    colourHighIntensity = thirdColour;
}

void Meter::updateClippingLevel (bool timeToUpdate)
{
    auto maxLevel = 0.0f;
    for (const auto& channel : channels)
        maxLevel = jmax (channel.getLevel(), maxLevel);

    maxLevel = Decibels::gainToDecibels (maxLevel);

    auto currClippingLevel = ClippingLevel::none;

    if (maxLevel >= 0.0f)
        currClippingLevel = ClippingLevel::clipping;
    else if (maxLevel >= -12.0f)
        currClippingLevel = ClippingLevel::warning;

    // Only update if higher, or if the time delay has expired.
    if (clippingLevel < currClippingLevel || timeToUpdate)
        clippingLevel = currClippingLevel;
}
