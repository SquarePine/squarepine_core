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

    if (model == nullptr)
        return;

    auto positions = model->getColourPositions();
    if (positions.size() < 2)
    {
        jassertfalse;
        return;
    }

    std::sort (std::begin (positions), std::end (positions),
               [] (const auto& lhs, const auto& rhs) { return lhs.decibels < rhs.decibels; });

    const auto b = getLocalBounds();
    const bool isHorizontal = model->isHorizontal();
    if (isHorizontal)
        gradient = ColourGradient::horizontal (positions.front().colour, positions.back().colour, b);
    else
        gradient = ColourGradient::vertical (positions.front().colour, positions.back().colour, b);

    for (size_t i = 1; i < (positions.size() - 1); ++i)
        positions[i].addToGradient (gradient, isHorizontal);
}

void Meter::paint (Graphics& g)
{
    if (gradient.getNumColours() <= 0)
        return; // Nothing to draw.

    g.setGradientFill (gradient);
    g.fillAll();
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
    const auto numChans = std::min (levels.size(), channels.size());

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
        maxLevel = std::max (channel.getLevel(), maxLevel);

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
