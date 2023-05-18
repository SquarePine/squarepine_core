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
Meter::Meter (Model* model_) :
    model (model_)
{
    channels.resize (2);
}

void Meter::setModel (Model* model_)
{
    if (model != model_)
    {
        model = model_;
        refresh();
    }
}

#if 0
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
#endif

void Meter::resized()
{
}

void Meter::paint (Graphics&)
{
}

bool Meter::refresh()
{
    levels.clearQuick();
    int64 maxLevelExpiryMs = 3000;
    bool needsMaxLevel = false;
    float decayRate = 0.8f;

    if (model != nullptr)
    {
        levels = model->getChannelLevels();
        maxLevelExpiryMs = model->getExpiryTimeMs();
        needsMaxLevel = model->needsMaxLevel();
    }

    bool areLevelsDifferent = false;
    bool isMaxLevelDelayExpired = false;
    const auto numChans = jmin (levels.size(), channels.size());

    for (int i = 0; i < numChans; ++i)
    {
        auto& channel = channels.getReference (i);
        auto level = lerp (channel.getLevel(), levels[i], 0.9f);
        dsp::util::snapToZero (level);
        channel.setLevel (level);

        if (needsMaxLevel)
        {
            if (channel.getLevel() > channel.getMaxLevel())
            {
                channel.setLastMaxAudioLevelTime (Time::currentTimeMillis());
                channel.setMaxLevel (channel.getLevel());
            }
            else if (Time::currentTimeMillis() - channel.getLastMaxAudioLevelTime() > maxLevelExpiryMs)
            {
                channel.setMaxLevel (channel.getMaxLevel() * decayRate);
                isMaxLevelDelayExpired = true;
            }

            areLevelsDifferent |= ! approximatelyEqual (channel.getMaxLevel(), channel.getLastMaxLevel());
        }

        areLevelsDifferent |= ! approximatelyEqual (channel.getLevel(), channel.getLastLevel());

        channel.setLastLevel (channel.getLevel());
        channel.setLastMaxLevel (channel.getMaxLevel());
    }

    if (areLevelsDifferent)
        updateClippingLevel (isMaxLevelDelayExpired);

    return areLevelsDifferent;
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
