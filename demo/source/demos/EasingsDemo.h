/** */
class CurveDisplayComponent final : public Component
{
public:
    /** */
    CurveDisplayComponent() = default;

    //==============================================================================
    /** */
    void setGenerator (std::function<double (double)> generatorToUse)
    {
        animTimer.stopTimer();

        generator = generatorToUse;
        jassert (generator != nullptr);

        genMin = genMax = 0.0;

        Point<double> last;
        last.y = generator (0.0);

        // Using 'clear' to reduce allocations:
        plot.clear();
        scalablePlot.clear();

        for (double x = ratio; x <= 1.0; x += ratio)
        {
            const Point<double> c (x, generator (x));
            plot.addLineSegment ({ last.toFloat(), c.toFloat() }, lineThickness);
            last = c;

            genMin = jmin (genMin, c.y);
            genMax = jmax (genMax, c.y);
        }

        // Because JUCE's coords are backwards...
        plot.applyTransform (AffineTransform::verticalFlip (1.0f));

        tickAnimation();

        if (! getLocalBounds().isEmpty())
            resized();

        animTimer.callback = [this]() { tickAnimation(); };
        animTimer.startTimerHz (60);
    }

    //==============================================================================
    /** */
    void setUsingStroke (bool shouldStroke)
    {
        if (stroke != shouldStroke)
        {
            stroke = shouldStroke;
            resized();
        }
    }

    //==============================================================================
    /** @internal */
    void paint (Graphics& g) override
    {
        const bool mouseOver    = isMouseOver();
        const auto bgColour     = findColour (ListBox::backgroundColourId, true);
        const auto textColour   = findColour (ListBox::textColourId, true);

        g.fillAll (bgColour);

        if (getName().isNotEmpty())
        {
            if (mouseOver)
            {
                g.setColour (textColour);
                g.fillRect (textBounds);
            }

            g.setColour (mouseOver ? bgColour : textColour);
            g.setFont ((float) fontHeight);
            g.drawFittedText (TRANS (getName()), textBounds, Justification::centred, 2, 1.0f);
        }

        g.setColour (findColour (ListBox::outlineColourId, true));
        g.fillPath (scalablePlot);

        constexpr auto pSize = 4.5f;

        if (mouseOver)
        {
            g.setColour (bgColour.contrasting());

            const auto mx = getMouseXYRelative().toDouble().x / (double) getWidth();
            const auto mpos = createMovingXPoint (mx, scaledTransform).toFloat();
            constexpr auto size = pSize * MathConstants<float>::pi;
            g.drawEllipse (Rectangle<float> (size, size).withCentre (mpos), MathConstants<float>::pi);
        }

        if (animTimer.isTimerRunning())
        {
            g.setColour (textColour);
            g.fillEllipse (Rectangle<float> (pSize, pSize).withCentre (lineFollowPos));

            g.fillEllipse (Rectangle<float> (pSize, pSize).withCentre (verticalBallBouncePos));
        }
    }

    /** @internal */
    void resized() override
    {
        scalablePlot.clear();
        scalablePlot = Path (plot);

        auto b = getLocalBounds().reduced (margin);

        if (getName().isNotEmpty())
        {
            textBounds = b.removeFromTop (fontHeight);
            b.removeFromTop (margin);
        }
        else
        {
            textBounds = {};
        }

        {
            auto plotB = b.removeFromLeft (getWidth() / 2).reduced (halfMargin);

            scaledTransform = plot.getTransformToScaleToFit (plotB.toFloat(), true);
            scalablePlot.applyTransform (scaledTransform);

            constexpr float dashPattern[] = { 30.0f, 30.0f };

            if (stroke)
                PathStrokeType (1.0f)
                    .createDashedStroke (scalablePlot, scalablePlot, dashPattern, numElementsInArray (dashPattern));
        }

        ballArea = b.reduced (halfMargin).toFloat();
    }

private:
    //==============================================================================
    enum
    {
        margin = 8,
        halfMargin = margin / 2
    };

    static inline constexpr auto ratio          = 1.0 / 400.0;
    static inline constexpr auto lineThickness  = 0.01f;

    bool stroke = false;

    std::function<double (double)> generator;
    double genMin = 0.0, genMax = 1.0;

    bool canAnimateReverse = false;
    double normalisedTime { 0.0 };
    OffloadedTimer animTimer;

    int fontHeight = 18;
    Rectangle<int> textBounds;
    Path plot, scalablePlot;
    AffineTransform scaledTransform;

    Point<float> lineFollowPos, verticalBallBouncePos;
    Rectangle<float> ballArea;

    static Point<double> createPoint (double x, double y, const AffineTransform& at = {})
    {
        return Point<double> (x, y)
                .transformedBy (AffineTransform::verticalFlip (1.0))
                .transformedBy (at);
    }

    Point<double> createMovingXPoint (double normPos, const AffineTransform& at) const
    {
        return createPoint (normPos, generator (normPos), at);
    }

    Point<double> createMovingYPoint (double normPos, const AffineTransform& at) const
    {
        return createPoint (generator (normPos), normPos, at);
    }

    double mapTo (double v, double x, double y) const
    {
        return jmap (v, genMin, genMax, x, y);
    }

    void tickAnimation()
    {
        if (scalablePlot.isEmpty()
            || ballArea.isEmpty()
            || approximatelyEqual (genMax, 0.0))
            return;

        const auto v = generator (normalisedTime);
        const auto rv = canAnimateReverse ? generator (1.0 - normalisedTime) : v;

        auto t = scaledTransform;
        if (canAnimateReverse)
            t = scaledTransform.followedBy (AffineTransform::verticalFlip (-1.0));

        lineFollowPos = createPoint (normalisedTime, rv, t).toFloat();

        const auto pb = scalablePlot.getBounds();

        {
            auto x = mapTo (rv, 0.0, (double) pb.getWidth());
            auto y = mapTo (rv, 0.0, (double) pb.getHeight());

            lineFollowPos = { (float) x, (float) y };
            lineFollowPos = lineFollowPos.translated (pb.getX(), pb.getY());
            lineFollowPos = lineFollowPos.transformedBy (AffineTransform::verticalFlip (lineFollowPos.y));
        }

        lineFollowPos.x = (float) mapTo (rv, 0.0, (double) pb.getWidth());
        lineFollowPos.y = (float) mapTo (rv, 0.0, (double) pb.getHeight());
        lineFollowPos.x += pb.getX();
        lineFollowPos.y += pb.getY();
        lineFollowPos = lineFollowPos.transformedBy (AffineTransform::verticalFlip (1.0));

        verticalBallBouncePos.x = ballArea.getCentreX();
        verticalBallBouncePos.y = (float) mapTo (rv, 0.0, (double) ballArea.getHeight());
        verticalBallBouncePos.y += ballArea.getY();

        normalisedTime += ratio;
        if (normalisedTime > 1.0)
        {
            canAnimateReverse = ! canAnimateReverse;
            normalisedTime = 0.0;
        }

        repaint();
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CurveDisplayComponent)
};

//==============================================================================
/** A general demonstration of all pre-configured easing functions. */
class EaseListComponent final : public DemoBase,
                                public ListBoxModel
{
public:
    EaseListComponent (SharedObjects& sharedObjs) :
        DemoBase (sharedObjs, NEEDS_TRANS ("Ease List Demo"))
    {
        generators.ensureStorageAllocated (64);

        addGenerator (ease::audio::linear,           NEEDS_TRANS ("Linear"));
        addGenerator (ease::audio::smoothstepEase,   NEEDS_TRANS ("Smoothstep"));
        addGenerator (ease::audio::smootherstepEase, NEEDS_TRANS ("Smootherstep"));
        addGenerator (ease::audio::sinEase,          NEEDS_TRANS ("sin"));
        addGenerator (ease::audio::cosEase,          NEEDS_TRANS ("cos"));
        addGenerator (ease::audio::sincEase,         NEEDS_TRANS ("sinc"));
        addGenerator (ease::audio::squareWave,       NEEDS_TRANS ("Square"));
        addGenerator (ease::audio::sawtoothWave,     NEEDS_TRANS ("Saw"));
        addGenerator (ease::audio::triangleWave,     NEEDS_TRANS ("Triangle"));
        addGenerator (ease::cubic::in::back,         NEEDS_TRANS ("In - Back"));
        addGenerator (ease::cubic::out::back,        NEEDS_TRANS ("Out - Back"));
        addGenerator (ease::cubic::inOut::back,      NEEDS_TRANS ("InOut - Back"));
        addGenerator (ease::cubic::in::bounce,       NEEDS_TRANS ("In - Bounce"));
        addGenerator (ease::cubic::out::bounce,      NEEDS_TRANS ("Out - Bounce"));
        addGenerator (ease::cubic::inOut::bounce,    NEEDS_TRANS ("InOut - Bounce"));
        addGenerator (ease::cubic::in::circ,         NEEDS_TRANS ("In - Circ"));
        addGenerator (ease::cubic::out::circ,        NEEDS_TRANS ("Out - Circ"));
        addGenerator (ease::cubic::inOut::circ,      NEEDS_TRANS ("InOut - Circ"));
        addGenerator (ease::cubic::in::cubic,        NEEDS_TRANS ("In - Cubic"));
        addGenerator (ease::cubic::out::cubic,       NEEDS_TRANS ("Out - Cubic"));
        addGenerator (ease::cubic::inOut::cubic,     NEEDS_TRANS ("InOut - Cubic"));
        addGenerator (ease::cubic::in::elastic,      NEEDS_TRANS ("In - Elastic"));
        addGenerator (ease::cubic::out::elastic,     NEEDS_TRANS ("Out - Elastic"));
        addGenerator (ease::cubic::inOut::elastic,   NEEDS_TRANS ("InOut - Elastic"));
        addGenerator (ease::cubic::in::expo,         NEEDS_TRANS ("In - Expo"));
        addGenerator (ease::cubic::out::expo,        NEEDS_TRANS ("Out - Expo"));
        addGenerator (ease::cubic::inOut::expo,      NEEDS_TRANS ("InOut - Expo"));
        addGenerator (ease::cubic::in::quad,         NEEDS_TRANS ("In - Quad"));
        addGenerator (ease::cubic::out::quad,        NEEDS_TRANS ("Out - Quad"));
        addGenerator (ease::cubic::inOut::quad,      NEEDS_TRANS ("InOut - Quad"));
        addGenerator (ease::cubic::in::quart,        NEEDS_TRANS ("In - Quart"));
        addGenerator (ease::cubic::out::quart,       NEEDS_TRANS ("Out - Quart"));
        addGenerator (ease::cubic::inOut::quart,     NEEDS_TRANS ("InOut - Quart"));
        addGenerator (ease::cubic::in::quint,        NEEDS_TRANS ("In - Quint"));
        addGenerator (ease::cubic::out::quint,       NEEDS_TRANS ("Out - Quint"));
        addGenerator (ease::cubic::inOut::quint,     NEEDS_TRANS ("InOut - Quint"));
        addGenerator (ease::cubic::in::sine,         NEEDS_TRANS ("In - Sine"));
        addGenerator (ease::cubic::out::sine,        NEEDS_TRANS ("Out - Sine"));
        addGenerator (ease::cubic::inOut::sine,      NEEDS_TRANS ("InOut - Sine"));

        generators.minimiseStorageOverheads();

        listbox.setRowHeight (192);
        listbox.setModel (this);
        addAndMakeVisible (listbox);

        showDashedLines.addListener (&dashSwitchAttachment);

        showDashedLines.onClick = [this]()
        {
            listbox.updateContent();
        };

        showDashedLines.addShortcut (KeyPress ('d'));
        showDashedLines.setClickingTogglesState (true);
        addAndMakeVisible (showDashedLines);

        updateWithNewTranslations();
    }

    ~EaseListComponent() override
    {
        showDashedLines.removeListener (&dashSwitchAttachment);
    }

    //==============================================================================
    void updateWithNewTranslations() override
    {
        SQUAREPINE_CRASH_TRACER

        DemoBase::updateWithNewTranslations();

        showDashedLines.setName (TRANS ("Show Dashed Lines"));
        showDashedLines.setButtonText (showDashedLines.getName());
    }

    int getNumRows() override                                       { return generators.size(); }
    void paintListBoxItem (int, Graphics&, int, int, bool) override { }

    void resized() override
    {
        auto b = getLocalBounds().reduced (margin);

        {
            auto top = b.removeFromTop (32);
            showDashedLines.setBounds (top.removeFromLeft (192));
        }

        b.removeFromTop (margin);
        listbox.setBounds (b);
    }

    Component* refreshComponentForRow (int row, bool, Component* comp) override
    {
       #if JUCE_DEBUG && ! JUCE_DISABLE_ASSERTIONS
        if (comp != nullptr)
            jassert (dynamic_cast<CurveDisplayComponent*> (comp) != nullptr);
       #endif

        std::unique_ptr<CurveDisplayComponent> cdc (static_cast<CurveDisplayComponent*> (comp));
        comp = nullptr;

        if (! isPositiveAndBelow (row, getNumRows()))
            return nullptr;

        auto* gen = generators[row];

        if (cdc == nullptr)
            cdc.reset (new CurveDisplayComponent ());

        cdc->setName (gen->name);
        cdc->setGenerator (gen->generator);
        cdc->setUsingStroke (showDashedLines.getToggleState());
        return cdc.release();
    }

private:
    //==============================================================================
    enum { margin = 4 };

    struct Generator final
    {
        std::function<double (double)> generator;
        String name;
    };

    TextButton showDashedLines;
    GoogleAnalyticsAttachment dashSwitchAttachment { trackingId, "button", "dash" };

    ListBox listbox;
    OwnedArray<Generator> generators;

    //==============================================================================
    void addGenerator (std::function<double (double)> generator, const String& name)
    {
        auto* gen = generators.add (new Generator());
        gen->generator = generator;
        gen->name = name;
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EaseListComponent)
};
