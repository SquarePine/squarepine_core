//==============================================================================
/** */
class EasingPathDisplayComponent final : public Component
{
public:
    /** */
    EasingPathDisplayComponent() = default;

    //==============================================================================
    /** */
    void setPath (std::shared_ptr<Path> pathToUse,
                  ValueAnimatorBuilder::EasingFn easingFuncToUse = Easings::createLinear())
    {
        plot = pathToUse;
        easingFunc = easingFuncToUse;
        scalablePlot.clear();
        resized();
    }

    //==============================================================================
    void tickAnimation (float value,
                        bool animateForward,
                        bool shouldShowBall)
    {
        ignoreUnused (animateForward);

        progress.x  = value;
        progress.y  = easingFunc (value);
        showBall    = shouldShowBall;
    }

    //==============================================================================
    /** @internal */
    void paint (Graphics& g) override
    {
        const bool mouseOver    = isMouseOver();
        const auto bgColour     = findColour (ListBox::backgroundColourId, true);
        const auto textColour   = findColour (ListBox::textColourId, true);

        if (! textBounds.isEmpty())
        {
            g.setColour (textColour);

            if (mouseOver)
            {
                g.fillRect (textBounds);
                g.setColour (bgColour);     // The text will have the opposing background colour to stand out.
            }

            g.setFont ((float) fontHeight);
            g.drawFittedText (TRANS (getName()), textBounds, Justification::centred, 2, 1.0f);
        }

        const auto outlineColour = findColour (ListBox::outlineColourId, true);
        g.setColour (outlineColour);
        g.strokePath (scalablePlot, PathStrokeType (1.0f));

        // Draw the ball:
        if (showBall)
        {
            auto p = scalablePlot.getPointAlongPath (progress.x * plotLength);

            // Centred along the line.
            const auto ballB = ball.getBounds();
            p = p.translated (-(ballB.getWidth() / 2.0f),
                              -(ballB.getHeight() / 2.0f));

            auto movedBall = ball;
            movedBall.applyTransform (AffineTransform::translation (p));

            g.strokePath (movedBall, PathStrokeType (ballStrokeThickness));
        }
    }

    /** @internal */
    void resized() override
    {
        auto b = getLocalBounds();

        textBounds = {};

        if (b.isEmpty() || ! b.isFinite())
            return;

        b = b.reduced (margin);

        if (getName().isNotEmpty())
        {
            textBounds = b.removeFromTop (fontHeight);
            b.removeFromTop (margin);
        }

        if (plot != nullptr)
        {
            scalablePlot = *plot;

            // The 'ease' type graph on the right side:
            plotTransform = AffineTransform::scale (1.0f, 0.75f)
                            .followedBy (AffineTransform::translation (0.0f, 0.2f))
                            .followedBy (plot->getTransformToScaleToFit (b.toFloat(), true));
            scalablePlot.applyTransform (plotTransform);
        }
        else
        {
            scalablePlot.clear();
            plotTransform = {};
        }

        plotLength = scalablePlot.getLength();

        ball.clear();
        ball.addEllipse (Rectangle<int> (fontHeight, fontHeight).toFloat());
    }

private:
    //==============================================================================
    enum
    {
        fontHeight = 18,
        margin = 8,
        halfMargin = margin / 2
    };

    static inline constexpr auto plotStrokeThickness = 1.0f;
    static inline constexpr auto ballStrokeThickness = plotStrokeThickness * 2.0f;

    Rectangle<int> textBounds;
    std::shared_ptr<Path> plot;
    float plotLength = 0.0f;
    ValueAnimatorBuilder::EasingFn easingFunc = Easings::createLinear();
    Path scalablePlot, ball;
    AffineTransform plotTransform, ballTransform;
    juce::Point<float> progress; // { x = linear, y = eased }
    bool showBall = true;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EasingPathDisplayComponent)
};

//==============================================================================
/** A general demonstration of all pre-configured easing functions. */
class EaseListComponent final : public DemoBase,
                                public ListBoxModel
{
public:
    EaseListComponent (SharedObjects& sharedObjs) :
        DemoBase (sharedObjs, NEEDS_TRANS ("Easings"))
    {
        generators.ensureStorageAllocated (64);

        // These are first because they're different wrt implementation:
        addGenerator (ease::cubic::in::bounce,                  NEEDS_TRANS ("In - Bounce"));
        addGenerator (ease::cubic::out::bounce,                 NEEDS_TRANS ("Out - Bounce"));
        addGenerator (ease::cubic::inOut::bounce,               NEEDS_TRANS ("InOut - Bounce"));
        addGenerator (ease::cubic::in::elastic,                 NEEDS_TRANS ("In - Elastic"));
        addGenerator (ease::cubic::out::elastic,                NEEDS_TRANS ("Out - Elastic"));
        addGenerator (ease::cubic::inOut::elastic,              NEEDS_TRANS ("InOut - Elastic"));

        addGenerator (ease::bezier::createEaseInSine(),         NEEDS_TRANS ("In - Sine"));
        addGenerator (ease::bezier::createEaseOutSine(),        NEEDS_TRANS ("Out - Sine"));
        addGenerator (ease::bezier::createEaseInOutSine(),      NEEDS_TRANS ("InOut - Sine"));
        addGenerator (ease::bezier::createEaseInQuad(),         NEEDS_TRANS ("In - Quad"));
        addGenerator (ease::bezier::createEaseOutQuad(),        NEEDS_TRANS ("Out - Quad"));
        addGenerator (ease::bezier::createEaseInOutQuad(),      NEEDS_TRANS ("InOut - Quad"));
        addGenerator (ease::bezier::createEaseInCubic(),        NEEDS_TRANS ("In - Cubic"));
        addGenerator (ease::bezier::createEaseOutCubic(),       NEEDS_TRANS ("Out - Cubic"));
        addGenerator (ease::bezier::createEaseInOutCubic(),     NEEDS_TRANS ("InOut - Cubic"));
        addGenerator (ease::bezier::createEaseInQuart(),        NEEDS_TRANS ("In - Quart"));
        addGenerator (ease::bezier::createEaseOutQuart(),       NEEDS_TRANS ("Out - Quart"));
        addGenerator (ease::bezier::createEaseInOutQuart(),     NEEDS_TRANS ("InOut - Quart"));
        addGenerator (ease::bezier::createEaseInQuint(),        NEEDS_TRANS ("In - Quint"));
        addGenerator (ease::bezier::createEaseOutQuint(),       NEEDS_TRANS ("Out - Quint"));
        addGenerator (ease::bezier::createEaseInOutQuint(),     NEEDS_TRANS ("InOut - Quint"));
        addGenerator (ease::bezier::createEaseInExpo(),         NEEDS_TRANS ("In - Expo"));
        addGenerator (ease::bezier::createEaseOutExpo(),        NEEDS_TRANS ("Out - Expo"));
        addGenerator (ease::bezier::createEaseInOutExpo(),      NEEDS_TRANS ("InOut - Expo"));
        addGenerator (ease::bezier::createEaseInCirc(),         NEEDS_TRANS ("In - Circ"));
        addGenerator (ease::bezier::createEaseOutCirc(),        NEEDS_TRANS ("Out - Circ"));
        addGenerator (ease::bezier::createEaseInOutCirc(),      NEEDS_TRANS ("InOut - Circ"));
        addGenerator (ease::bezier::createEaseInBack(),         NEEDS_TRANS ("In - Back"));
        addGenerator (ease::bezier::createEaseOutBack(),        NEEDS_TRANS ("Out - Back"));
        addGenerator (ease::bezier::createEaseInOutBack(),      NEEDS_TRANS ("InOut - Back"));

        // And here are some silly examples:
        addGenerator (ease::audio::linear,                      NEEDS_TRANS ("Linear"));
        addGenerator (ease::audio::smoothstepEase,              NEEDS_TRANS ("Smoothstep"));
        addGenerator (ease::audio::smootherstepEase,            NEEDS_TRANS ("Smootherstep"));
        addGenerator (ease::audio::sinEase,                     NEEDS_TRANS ("sin"));
        addGenerator (ease::audio::cosEase,                     NEEDS_TRANS ("cos"));
        addGenerator (ease::audio::sincEase,                    NEEDS_TRANS ("sinc"));
        addGenerator (ease::audio::squareWave,                  NEEDS_TRANS ("Square"));
        addGenerator (ease::audio::sawtoothWave,                NEEDS_TRANS ("Saw"));
        addGenerator (ease::audio::triangleWave,                NEEDS_TRANS ("Triangle"));

        generators.minimiseStorageOverheads();

        listbox.setRowHeight (192);
        listbox.setModel (this);
        addAndMakeVisible (listbox);

        showAnimatingBall.addListener (&dashSwitchAttachment);

        showAnimatingBall.onClick = [this]()
        {
            listbox.updateContent();
        };

        showAnimatingBall.addShortcut (KeyPress ('s'));
        showAnimatingBall.setClickingTogglesState (true);
        showAnimatingBall.setToggleState (true, dontSendNotification);
        addAndMakeVisible (showAnimatingBall);

        ballAnimator.start();
        updater.addAnimator (ballAnimator);

        updateWithNewTranslations();
    }

    ~EaseListComponent() override
    {
        updater.removeAnimator (ballAnimator);
        showAnimatingBall.removeListener (&dashSwitchAttachment);
    }

    //==============================================================================
    void updateWithNewTranslations() override
    {
        SQUAREPINE_CRASH_TRACER

        showAnimatingBall.setName (TRANS ("Show Animating Ball"));
        showAnimatingBall.setButtonText (showAnimatingBall.getName());
        listbox.updateContent();
        repaint();
    }

    int getNumRows() override                                       { return generators.size(); }
    void paintListBoxItem (int, Graphics&, int, int, bool) override { }

    void resized() override
    {
        auto b = getLocalBounds().reduced (margin);

        {
            auto top = b.removeFromTop (32);

            showAnimatingBall.setBounds (top.removeFromLeft (top.getWidth() / 2));
        }

        b.removeFromTop (margin);
        listbox.setBounds (b);
    }

    Component* refreshComponentForRow (int row, bool, Component* comp) override
    {
        auto* gen = generators[row];
        if (gen == nullptr)
        {
            deleteAndZero (comp);
            return nullptr;
        }

        std::unique_ptr<EasingPathDisplayComponent> epdc (static_cast<EasingPathDisplayComponent*> (comp));
        comp = nullptr;

        if (epdc == nullptr)
            epdc.reset (new EasingPathDisplayComponent());

        epdc->setName (TRANS (gen->untranslatedName));
        epdc->setPath (gen->getPath(), gen->makeEasingFn());

        return epdc.release();
    }

private:
    //==============================================================================
    enum { margin = 4 };

    struct Generator final
    {
        Generator (const String& n, CubicBezier::EasingHQFn genFunc, bool generatePath) :
            untranslatedName (n),
            generator (genFunc)
        {
            jassert (generator != nullptr);

            if (generatePath)
            {
                constexpr auto stepSize = 0.001f;
                sourcePath = std::make_shared<Path>();

                auto x          = 0.0f;
                auto localEaser = makeEasingFn();
                auto nextY      = [&]() { return localEaser (x); };
                auto y          = nextY();

                sourcePath->startNewSubPath (x, y);

                while (x < 1.0f)
                {
                    x += stepSize;
                    y = nextY();

                    sourcePath->lineTo (x, y); // TODO would be nice to use Path::quadraticTo() or Path::cubicTo()...
                }
            }
        }

        Generator (const String& n, const CubicBezier& cb) :
            Generator (n, cb.createEasingHQFn(), false)
        {
            cubicBezier = cb;
            sourcePath = std::make_shared<Path> (cb.createLinePath());
            jassert (! sourcePath->isEmpty());
        }

        const std::shared_ptr<Path>& getPath() const noexcept { return sourcePath; }

        ValueAnimatorBuilder::EasingFn makeEasingFn()
        {
            if (generator != nullptr)
            {
                return [g = generator] (float v)
                {
                    return (float) g ((double) v);
                };
            }

            return Easings::createLinear();
        }

        String untranslatedName;

    private:
        static inline constexpr auto lineThickness = 0.01f;
        CubicBezier::EasingHQFn generator = nullptr;
        std::optional<CubicBezier> cubicBezier;
        mutable std::shared_ptr<Path> sourcePath;
    };

    TextButton showAnimatingBall;
    GoogleAnalyticsAttachment dashSwitchAttachment { trackingId, "button", "showBall" };
    ListBox listbox;
    OwnedArray<Generator> generators;
    VBlankAnimatorUpdater updater { this };

    Animator ballAnimator = makeBuilder().build();

    //==============================================================================
    void addGenerator (const CubicBezier& cb, const String& name)
    {
        generators.add (new Generator (name, cb));
    }

    void addGenerator (CubicBezier::EasingHQFn func, const String& name)
    {
        generators.add (new Generator (name, func, true));
    }

    //==============================================================================
    ValueAnimatorBuilder makeBuilder()
    {
        return ValueAnimatorBuilder()
            .runningInfinitely()
            .withEasing (Easings::createLinear())
            .withDurationMs (3141.0)
            .withValueChangedCallback ([this] (float value) { tickAnimation (value); });
    }

    void tickAnimation (float value)
    {
        jassert (value >= 0.0f);

        value                       = std::abs (value);
        const auto fValue           = std::floor (value);
        const bool animateForward   = isEven ((int) fValue);
        value                       = value - fValue;
        value                       = animateForward ? value : (1.0f - value);

        const bool showBall = showAnimatingBall.getToggleState();

        for (int i = 0; i < getNumRows(); ++i)
            if (auto* c = dynamic_cast<EasingPathDisplayComponent*> (listbox.getComponentForRowNumber (i)))
                c->tickAnimation (value, animateForward, showBall);

        repaint();
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EaseListComponent)
};
