//==============================================================================
/** */
enum class EasingCurveType
{
    linear,
    smoothstep,
    smootherstep,
    sgn,
    sin,
    cos,
    sinc,
    square,
    sawtooth,
    triangle,

    #undef defineEase
    #define defineEase(name) \
        easeIn##name, \
        easeOut##name, \
        easeInOut##name,

    defineEase (Sine)
    defineEase (Quad)
    defineEase (Cubic)
    defineEase (Quart)
    defineEase (Quint)
    defineEase (Expo)
    defineEase (Circ)
    defineEase (Back)
    defineEase (Elastic)
    defineEase (Bounce)

    #undef defineEase
};

//==============================================================================
/** */
class PropertyTimeline final : public ProjectItem
{
public:
    /** */
    PropertyTimeline() :
        ProjectItem (propertyTimelineId)
    {
    }

    //==============================================================================
    /** */
    PropertyTimeline& addPoint (double time,
                                double value,
                                UndoManager* undoManager = nullptr)
    {
        ValueTree c (pointId);
        c.setProperty (timeId, time, nullptr)
         .setProperty (valueId, value, nullptr);

        state.appendChild (c, undoManager);
        return *this;
    }

    /** */
    PropertyTimeline& addPoint (double time,
                                double value,
                                EasingCurveType type,
                                double tension = 1.0,
                                UndoManager* undoManager = nullptr)
    {
        addPoint (time, value, undoManager);

        auto c = getPoint (getNumPoints() - 1);
        c.setProperty (easingTypeId, (int) type, nullptr)
         .setProperty (tensionId, tension, nullptr);

        return *this;
    }

    /** */
    ValueTree getPoint (int index) const
    {
        return state.getChild (index);
    }

    /** */
    void movePoint (int index, double newTime, UndoManager* undoManager = nullptr)
    {
        auto p = getPoint (index);
        if (p.isValid())
        {
           #if 0 // Check if time isn't already set somewhere
            bool isUnique = true;
            for (const auto& p : points)
            {
                if (approximatelyEqual (p.time, tp.time))
            }

            if (isUnique)
           #endif

            state.setProperty (timeId, newTime, undoManager);
        }
    }

    /** */
    void removePoint (int index, UndoManager* undoManager = nullptr)
    {
        state.removeChild (index, undoManager);
    }

    /** @returns the number of property items in this timeline. */
    int getNumPoints() const noexcept { return getState().getNumChildren(); }

    //==============================================================================
    ValueTree findClosestBefore (double currentTimeNormalised) const
    {
        for (int i = state.getNumChildren(); --i >= 0;)
        {
            const auto c = state.getChild (i);
            if (! c.hasType (pointId))
                continue;

            const auto t = static_cast<double> (c.getProperty (timeId));
            if (t >= currentTimeNormalised)
                continue;

            return c;
        }

        return {};
    }

    ValueTree findClosestAfter (double currentTimeNormalised) const
    {
        for (int i = 0; i < state.getNumChildren(); ++i)
        {
            const auto c = state.getChild (i);
            if (! c.hasType (pointId))
                continue;

            const auto t = static_cast<double> (c.getProperty (timeId));
            if (t <= currentTimeNormalised)
                continue;

            return c;
        }

        return {};
    }

    static double getWeightAlongPath (EasingCurveType type, double weight)
    {
        switch (type)
        {
            case EasingCurveType::linear:           weight = ease::audio::linear (weight); break;
            case EasingCurveType::smoothstep:       weight = ease::audio::smoothstepEase (weight); break;
            case EasingCurveType::smootherstep:     weight = ease::audio::smootherstepEase (weight); break;
            case EasingCurveType::sgn:              weight = ease::audio::sgnEase (weight); break;
            case EasingCurveType::sin:              weight = ease::audio::sinEase (weight); break;
            case EasingCurveType::cos:              weight = ease::audio::cosEase (weight); break;
            case EasingCurveType::sinc:             weight = ease::audio::sincEase (weight); break;
            case EasingCurveType::square:           weight = ease::audio::squareWave (weight); break;
            case EasingCurveType::sawtooth:         weight = ease::audio::sawtoothWave (weight); break;
            case EasingCurveType::triangle:         weight = ease::audio::triangleWave (weight); break;

            case EasingCurveType::easeInBack:       weight = ease::cubic::in::back (weight); break;
            case EasingCurveType::easeInCirc:       weight = ease::cubic::in::circ (weight); break;
            case EasingCurveType::easeInCubic:      weight = ease::cubic::in::cubic (weight); break;
            case EasingCurveType::easeInExpo:       weight = ease::cubic::in::expo (weight); break;
            case EasingCurveType::easeInQuad:       weight = ease::cubic::in::quad (weight); break;
            case EasingCurveType::easeInQuart:      weight = ease::cubic::in::quart (weight); break;
            case EasingCurveType::easeInQuint:      weight = ease::cubic::in::quint (weight); break;
            case EasingCurveType::easeInSine:       weight = ease::cubic::in::sine (weight); break;
            case EasingCurveType::easeInBounce:     weight = ease::cubic::in::bounce (weight); break;
            case EasingCurveType::easeInElastic:    weight = ease::cubic::in::elastic (weight); break;
            case EasingCurveType::easeOutBack:      weight = ease::cubic::out::back (weight); break;
            case EasingCurveType::easeOutCirc:      weight = ease::cubic::out::circ (weight); break;
            case EasingCurveType::easeOutCubic:     weight = ease::cubic::out::cubic (weight); break;
            case EasingCurveType::easeOutExpo:      weight = ease::cubic::out::expo (weight); break;
            case EasingCurveType::easeOutQuad:      weight = ease::cubic::out::quad (weight); break;
            case EasingCurveType::easeOutQuart:     weight = ease::cubic::out::quart (weight); break;
            case EasingCurveType::easeOutQuint:     weight = ease::cubic::out::quint (weight); break;
            case EasingCurveType::easeOutSine:      weight = ease::cubic::out::sine (weight); break;
            case EasingCurveType::easeOutBounce:    weight = ease::cubic::out::bounce (weight); break;
            case EasingCurveType::easeOutElastic:   weight = ease::cubic::out::elastic (weight); break;
            case EasingCurveType::easeInOutBack:    weight = ease::cubic::inOut::back (weight); break;
            case EasingCurveType::easeInOutCirc:    weight = ease::cubic::inOut::circ (weight); break;
            case EasingCurveType::easeInOutCubic:   weight = ease::cubic::inOut::cubic (weight); break;
            case EasingCurveType::easeInOutExpo:    weight = ease::cubic::inOut::expo (weight); break;
            case EasingCurveType::easeInOutQuad:    weight = ease::cubic::inOut::quad (weight); break;
            case EasingCurveType::easeInOutQuart:   weight = ease::cubic::inOut::quart (weight); break;
            case EasingCurveType::easeInOutQuint:   weight = ease::cubic::inOut::quint (weight); break;
            case EasingCurveType::easeInOutSine:    weight = ease::cubic::inOut::sine (weight); break;
            case EasingCurveType::easeInOutBounce:  weight = ease::cubic::inOut::bounce (weight); break;
            case EasingCurveType::easeInOutElastic: weight = ease::cubic::inOut::elastic (weight); break;

            default:
                // @todo or missing...
                jassertfalse;
            break;
        };

        return weight;
    }

    /** @returns the value along the path at the given time, ranged from 0.0 to 1.0.*/
    double getValueAlongPath (double currentTimeNormalised) const
    {
        const auto left = findClosestBefore (currentTimeNormalised);
        const auto leftTime = static_cast<double> (left.getProperty (timeId));
        const auto leftValue = static_cast<double> (left.getProperty (valueId));
        if (approximatelyEqual (leftTime, currentTimeNormalised))
            return leftValue;

        const auto right = findClosestAfter (currentTimeNormalised);
        const auto rightTime = static_cast<double> (right.getProperty (timeId));
        const auto rightValue = static_cast<double> (right.getProperty (valueId));
        if (approximatelyEqual (rightTime, currentTimeNormalised))
            return rightValue;

        auto weight = lerp (leftTime, rightTime, currentTimeNormalised - leftTime);

        if (right.hasProperty (easingTypeId))
            weight = getWeightAlongPath (static_cast<EasingCurveType> (static_cast<int> (right.getProperty (easingTypeId))), weight);

        return lerp (leftValue, rightValue, weight);
    }

    //==============================================================================
    /** */
    struct TimePoint final
    {
        double time = 0.0, value = 0.0, tension = 0.0;
        EasingCurveType type = EasingCurveType::linear;
    };

    /** */
    struct TimePointComparator final
    {
        constexpr int compareElements (const TimePoint& l, const TimePoint& r) const noexcept
        {
            return l.time < r.time
                    ? -1
                    : (r.time < l.time ? 1 : 0);
        }
    };

    /** */
    Path creatPath (const TimePoint& left, const TimePoint& right,
                    double totalTime, float lineThickness) const
    {
        const auto leftTimeNormalised = jmap (left.time, 0.0, totalTime, 0.0, 1.0);
        const auto righttTimeNormalised = jmap (right.time, 0.0, totalTime, 0.0, 1.0);
        const auto halfY = lineThickness * 0.5f;

        if (approximatelyEqual (left.value, right.value))
        {
            Path subpath;

            subpath.addRectangle ((float) leftTimeNormalised, (float) left.value - halfY,
                                  (float) (righttTimeNormalised - leftTimeNormalised), halfY);
            subpath.closeSubPath();

            return subpath;
        }

        Array<Point<double>> subPoints;
        for (double w = 0.0; w <= 1.0; w += 0.001)
        {
            const auto weight = getWeightAlongPath (right.type, w);

            subPoints.add (Point<double>
            (
                leftTimeNormalised + lerp (left.time, right.time, w),
                left.value + (left.value * weight)
            ));
        }

        Path subpath;

        for (int i = 0; i < subPoints.size(); ++i)
        {
            const auto p = subPoints.getReference (i).toFloat().translated (0.0f, halfY);

            if (i == 0)
                subpath.startNewSubPath (p);
            else
                subpath.lineTo (p);
        }

        for (int i = subPoints.size(); --i >= 0;)
            subpath.lineTo (subPoints.getReference (i).toFloat().translated (0.0f, -halfY));

        subpath.closeSubPath();

        //const auto angleRads = std::atan2 (left.value - right.value, right.time - left.time);
        //subpath.applyTransform (AffineTransform::rotation ((float) angleRads));

        return subpath;
    }

    /** */
    Path getNormalisedPath (float lineThickness = 0.01f) const
    {
        Array<TimePoint> points;
        points.ensureStorageAllocated (getNumPoints());

        for (auto c : state)
        {
            if (! c.hasType (pointId))
                continue;

            TimePoint tp;
            tp.time     = static_cast<double> (c.getProperty (timeId));
            tp.value    = static_cast<double> (c.getProperty (valueId));
            tp.tension  = static_cast<double> (c.getProperty (tensionId));
            tp.type     = (EasingCurveType) static_cast<int> (c.getProperty (easingTypeId));

            TimePointComparator tpc;
            points.addSorted (tpc, tp);
        }

        if (points.size() <= 1)
            return {};

        Path fullPath;

        const auto totalTime = points.getLast().time;
        auto last = points.getFirst();

        for (int i = 1; i < points.size(); ++i)
        {
            const auto& current = points.getReference (i);

            auto subpath = creatPath (last, current, totalTime, lineThickness);

            fullPath.addPath (subpath);
            last = current;
        }

        // Because JUCE is backwards...
        fullPath.applyTransform (AffineTransform::verticalFlip (1.0f));

        return fullPath;
    }

    //==============================================================================
    /** @internal */
    var toJSON() const override
    {
        return {};
    }

    //==============================================================================
    /** */
    CREATE_INLINE_CLASS_IDENTIFIER (propertyTimeline)

    //==============================================================================
    /** */
    CREATE_INLINE_CLASS_IDENTIFIER (point)
    /** */
    CREATE_INLINE_CLASS_IDENTIFIER (time)
    /** */
    CREATE_INLINE_CLASS_IDENTIFIER (value)
    /** */
    CREATE_INLINE_CLASS_IDENTIFIER (tension)
    /** */
    CREATE_INLINE_CLASS_IDENTIFIER (easingType)

private:
    //==============================================================================
    Path normalisedPath;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PropertyTimeline)
};

//==============================================================================
class AutomationPointComponent final : public Component
{
public:
    AutomationPointComponent (double time, double pointValue) :
        timeBeats (time),
        value (pointValue)
    {
        timer.callback = [&]()
        {
            animationPosition += 0.02;

            if (animationPosition > 1.0)
                animationPosition = 0.0;

            repaint();
        };
    }

    //==============================================================================
    void mouseEnter (const MouseEvent&) override
    {
        isMouseDown = false;
        isMouseOver = true;
        timer.startTimerHz (60);
    }

    void mouseDown (const MouseEvent& e) override
    {
        timer.stopTimer();
        animationPosition = 1.0;

        isMouseDown = e.mods.isLeftButtonDown();

        if (isMouseDown)
            setMouseCursor (MouseCursor::StandardCursorType::DraggingHandCursor);
    }

    void mouseMove (const MouseEvent&) override
    {
        setMouseCursor (MouseCursor::StandardCursorType::PointingHandCursor);

        if (! timer.isTimerRunning())
            timer.startTimerHz (60);
    }

    void mouseDrag (const MouseEvent&) override
    {
        timer.stopTimer();
        animationPosition = 1.0;
    }

    void mouseExit (const MouseEvent&) override
    {
        setMouseCursor (MouseCursor::StandardCursorType::NormalCursor);
        isMouseDown = isMouseOver = false;
        timer.stopTimer();
        repaint();
    }

    void paint (Graphics& g)
    {
        constexpr auto thickness = 2.5;

        auto animatedThickness = thickness;
        auto animatedAlpha = 1.0;

        if (timer.isTimerRunning() && ! isMouseDown)
        {
            animatedAlpha = std::abs (animationPosition - 1.0);

            if (animationPosition >= 0.5)
                animatedAlpha = 1.0 - animatedAlpha;

            animatedThickness = thickness * ease::cubic::inOut::back (animatedAlpha);
        }

        g.setColour (Colours::blue.brighter().withAlpha ((float) animatedAlpha));
        g.drawEllipse (getLocalBounds().toFloat().reduced ((float) animatedThickness), (float) animatedThickness);
    }

    double timeBeats = 0.0;
    double value = 0.0;

private:
    //==============================================================================
    OffloadedTimer timer;
    bool isMouseOver = false, isMouseDown = false;
    double animationPosition = 0.0; 

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutomationPointComponent)
};

//==============================================================================
class AutomationPointComponentContainer final : public Component,
                                                public AudioProcessorListener
{
public:
    AutomationPointComponentContainer (AudioProcessor& p, int paramIndex) :
        processor (p),
        parameterIndex (paramIndex)
    {
        parameter = processor.getParameters()[paramIndex];
        jassert (parameter != nullptr);

        timeline.addPoint (0.0, 0.0);
        timeline.addPoint (8.0, 0.5, EasingCurveType::sawtooth);
        timeline.addPoint (16.0, 0.5, EasingCurveType::square);
        timeline.addPoint (24.0, 0.5, EasingCurveType::easeInBack);

/*
        addAndMakeVisible (points.add (new AutomationPointComponent (0.0, 0.25)));
        addAndMakeVisible (points.add (new AutomationPointComponent (16.0, 0.5)));
        addAndMakeVisible (points.add (new AutomationPointComponent (50.0, 0.5)));
        addAndMakeVisible (points.add (new AutomationPointComponent (80.0, 0.75)));
*/
    }

    ~AutomationPointComponentContainer() override
    {
    }

    //==============================================================================
    void resized() override
    {
/*
        lines.clear();

        constexpr auto lineThickness = 2.5f;
        constexpr auto size = 16;
        constexpr auto halfSize = size / 2;
        const auto h = getHeight();

        Point<float> lastPoint;

        for (auto* p : points)
        {
            const auto timeBeats = p->timeBeats;

            p->setBounds (roundToInt (timeBeats * 8.0) - halfSize,
                          h - roundToInt (p->value * (double) h) + halfSize,
                          size, size);

            const auto b = p->getBounds();
            if (approximatelyEqual (timeBeats, 0.0))
            {
                if (lines.isEmpty())
                {
                    lastPoint = b.getPosition().toFloat();
                    lines.startNewSubPath (lastPoint);
                }
            }
            else
            {
                if (lines.isEmpty())
                    lines.startNewSubPath (0.0f, (float) h);

                const auto cp = b.getCentre().toFloat();
                lines.addLineSegment ({ lastPoint, cp }, lineThickness);
                lastPoint = cp;
            }
        }

        lines.closeSubPath();
*/

        lines = timeline.getNormalisedPath();
        const auto t = lines.getTransformToScaleToFit (getLocalBounds().toFloat(), true, Justification::centred);
        lines.applyTransform (t);
    }

    void paint (Graphics& g) override
    {
        g.setColour (Colours::black);
        g.fillPath (lines);
    }

    void audioProcessorParameterChanged (AudioProcessor*,
                                         int,
                                         float) override
    {
    }

    void audioProcessorChanged (AudioProcessor*, const ChangeDetails&) override
    {
    }

private:
    //==============================================================================
    AudioProcessor& processor;
    const int parameterIndex;
    AudioProcessorParameter* parameter = nullptr;
    PropertyTimeline timeline;

    Path lines;
    OwnedArray<AutomationPointComponent> points;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutomationPointComponentContainer)
};

//==============================================================================
class AutomationLaneComponent final : public Component
{
public:
    AutomationLaneComponent (AudioProcessor& p, int paramIndex) :
        processor (p),
        parameterIndex (paramIndex),
        container (p, paramIndex)
    {
        parameter = processor.getParameters()[paramIndex];
        jassert (parameter != nullptr);

        addAndMakeVisible (container);
    }

    ~AutomationLaneComponent() override
    {
    }

    //==============================================================================
    void resized() override
    {
        auto b = getLocalBounds();
        label = b.removeFromTop (16).toFloat();
        container.setBounds (b);
    }

    void paint (Graphics& g) override
    {
        g.fillAll (Colours::red.brighter());

        g.setColour (Colours::grey);
        g.fillRect (label);

        g.setColour (Colours::black);
        g.setFont (Font (label.getHeight() - 2.0f));
        g.drawText (processor.getName() + " - " + parameter->getName (64), label, Justification::centredLeft);
    }

private:
    //==============================================================================
    AudioProcessor& processor;
    const int parameterIndex;
    AudioProcessorParameter* parameter = nullptr;

    AutomationPointComponentContainer container;
    Rectangle<float> label;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutomationLaneComponent)
};
