//==============================================================================
double DecibelHelpers::calculateSnapPoint (double snapDb) noexcept
{
    return decibelsToMeterProportion (snapDb, minSliderLevelDb, maxSliderLevelDb);
}

double DecibelHelpers::gainToMeterProportion (double gain, int minimumDecibels, int maximumDecibels) noexcept
{
    const auto decibels = std::clamp (Decibels::gainToDecibels (gain), (double) minimumDecibels, (double) maximumDecibels);
    return decibelsToMeterProportion (decibels, minimumDecibels, maximumDecibels);
}

double DecibelHelpers::meterProportionToGain (double meterProportion, int minimumDecibels, int maximumDecibels) noexcept
{
    const auto decibels = meterProportionToDecibels (meterProportion, minimumDecibels, maximumDecibels);
    return std::clamp (Decibels::decibelsToGain (decibels), (double) minimumDecibels, (double) maximumDecibels);
}

double DecibelHelpers::decibelsToMeterProportion (double decibels, int minimumDecibels, int maximumDecibels) noexcept
{
    decibels = std::clamp (decibels, (double) minimumDecibels, (double) maximumDecibels);
    const auto linearProportion = (decibels - minimumDecibels) / (double) (maximumDecibels - minimumDecibels);
    return linearToCurved (linearProportion);
}

double DecibelHelpers::meterProportionToDecibels (double meterProportion, int minimumDecibels, int maximumDecibels) noexcept
{
    const auto linearProportion = curvedToLinear (meterProportion);
    const auto decibels = minimumDecibels + linearProportion * (maximumDecibels - minimumDecibels);
    return std::clamp (decibels, (double) minimumDecibels, (double) maximumDecibels);
}

double DecibelHelpers::linearToCurved (double value) noexcept
{
    const auto curvedValue = (std::exp (value * std::log (1 + curveTensionDb)) - 1.0) / (double) curveTensionDb;
    return std::clamp (curvedValue, 0.0, 1.0);
}

double DecibelHelpers::curvedToLinear (double curvedValue) noexcept
{
    return std::log (1.0 + (double) curveTensionDb * curvedValue) / std::log (1.0 + (double) curveTensionDb);
}

//==============================================================================
Meter::Meter (MeterModel* model_) :
    model (model_)
{
    channels.resize (2);
}

//==============================================================================
void Meter::setMeterModel (MeterModel* newModel)
{
    if (model != newModel)
    {
        assignModelPtr (newModel);
        if (refresh())
            repaint();
    }
}

void Meter::assignModelPtr (MeterModel* newModel)
{
    model = newModel;

   #if ! JUCE_DISABLE_ASSERTIONS
    weakModelPtr = model != nullptr ? model->sharedState : nullptr;
   #endif
}

#if ! JUCE_DISABLE_ASSERTIONS

void Meter::checkModelPtrIsValid() const
{
    /** If this is hit, the model was destroyed while the Meter was still using it.
        You should ensure that the model remains alive for as long as the Meter holds a pointer to it.
        If this assertion is hit in the destructor of a Meter instance, do one of the following:
        - Adjust the order in which your destructors run, so that the Meter destructor runs
          before the destructor of your MeterModel, or
        - Call Meter::setMeterModel (nullptr) before destroying your ListBoxModel.
    */
    jassert ((model == nullptr) == (weakModelPtr.lock() == nullptr));
}

#endif

void Meter::resized()
{
    gradient = {};
    imageSource = {};
    imageClipped = {};

    if (model == nullptr)
        return;

    auto positions = model->getColourPositions();

    // Must have at least 1 colour!
    jassert (! positions.empty());

    const auto b = getLocalBounds();

    std::sort (std::begin (positions), std::end (positions),
               [] (const auto& lhs, const auto& rhs) { return lhs.decibels < rhs.decibels; });

    const bool isHorizontal = model->isHorizontal();
    if (isHorizontal)
        gradient = ColourGradient::horizontal (positions.front().colour, positions.back().colour, b);
    else
        gradient = ColourGradient::vertical (positions.front().colour, positions.back().colour, b);

    if (const auto numPos = (int64) positions.size(); numPos > 1)
        for (int64 i = 1; i < (numPos - 1); ++i)
            positions[(size_t) i].addToGradient (gradient, isHorizontal);

    {
        imageSource = { Image::RGB, getWidth(), getHeight(), false };
        Graphics ig (imageSource);
        ig.setGradientFill (gradient);
        ig.fillAll();
    }

    imageClipped = imageSource;
    // TODO force refresh async + repaint
}

void Meter::paint (Graphics& g)
{
    //auto b = getLocalBounds();
    //DBG (b.toString());

    if (imageSource.isNull())
        return; // Nothing to draw.

    g.setColour (Colours::red);

    for (const auto& c : channels)
    {
        // imageClipped = imageSource.getClippedImage (c.meterArea);
        // g.drawImage (imageClipped, c.meterArea.toFloat(), RectanglePlacement::doNotResize);

        g.fillRect (c.meterArea);
    }
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

    const auto numChans = std::min (levels.size(), channels.size());
    const auto chanWidthPx = roundToIntAccurate ((double) getWidth() / (double) numChans);
    const auto hPx = getHeight();

    bool areLevelsDifferent = channels.getFirst().meterArea.getWidth() != chanWidthPx;
    bool isMaxLevelDelayExpired = false;

    for (int i = 0; i < numChans; ++i)
    {
        auto& channel = channels.getReference (i);
        const auto lastLevel = channel.level;
        const auto lastMaxLevel = channel.maxLevel;

        channel.level = std::lerp (lastLevel, levels[i], 0.9f);

        if (needsMaxLevel)
        {
            if (channel.level > lastMaxLevel)
            {
                channel.timeOfMaximumMs = Time::currentTimeMillis();
                channel.maxLevel = lastLevel;
            }
            else if (Time::currentTimeMillis() - channel.timeOfMaximumMs > maxLevelExpiryMs)
            {
                channel.maxLevel = lastMaxLevel * decayRate;
                isMaxLevelDelayExpired = true;
            }

            areLevelsDifferent |= ! approximatelyEqual (channel.maxLevel, lastMaxLevel);
        }

        areLevelsDifferent |= ! approximatelyEqual (channel.level, lastLevel);

        const auto h = (double) hPx * (double) DecibelHelpers::gainToMeterProportion ((double) channel.level);

        channel.meterArea = channel.meterArea
                                   .withWidth (chanWidthPx)
                                   .withHeight (roundToIntAccurate (h));
    }

    if (areLevelsDifferent)
        updateClippingLevel (isMaxLevelDelayExpired);

    return areLevelsDifferent;
}

void Meter::updateClippingLevel (bool forceUpdate)
{
    auto maxLevel = 0.0f;
    for (const auto& channel : channels)
        maxLevel = std::max (channel.level, maxLevel);

    maxLevel = Decibels::gainToDecibels (maxLevel);

    auto currClippingLevel = ClippingLevel::none;

    if (maxLevel >= 0.0f)
        currClippingLevel = ClippingLevel::clipping;
    else if (maxLevel >= -12.0f)
        currClippingLevel = ClippingLevel::warning;

    // Only update if higher, or if the time delay has expired.
    if (clippingLevel < currClippingLevel || forceUpdate)
        clippingLevel = currClippingLevel;
}
