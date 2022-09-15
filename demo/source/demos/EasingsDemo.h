#pragma once

//==============================================================================
/** */
class CurveDisplayComponent final : public Component
{
public:
    /** */
    CurveDisplayComponent (std::function<double (double)> generatorToUse,
                           int64 numPoints = 400) :
        generator (generatorToUse),
        ratio (1.0 / (double) numPoints)
    {
        Point<double> last;
        last.y = generator (0.0);

        for (double x = ratio; x <= 1.0; x += ratio)
        {
            const Point<double> c (x, generator (x));

            plot.addLineSegment ({ last.toFloat(), c.toFloat() }, lineThickness);
            last = c;
        }

        // Because JUCE's coords are backwards...
        plot.applyTransform (AffineTransform::verticalFlip (1.0f));

        tickAnimation();
        animTimer.callback = [this]() { tickAnimation(); };
        animTimer.startTimerHz (60);
    }

    //==============================================================================
    /** @internal */
    void paint (Graphics& g) override
    {
        const auto bgColour = findColour (ListBox::backgroundColourId, true);
        const auto textColour = findColour (ListBox::textColourId, true);

        g.fillAll (bgColour);

        if (getName().isNotEmpty())
        {
            g.setColour (textColour);
            g.setFont ((float) fontHeight);
            g.drawFittedText (TRANS (getName()), textBounds, Justification::centred, 2, 1.0f);
        }

        g.setColour (findColour (ListBox::outlineColourId, true));
        g.fillPath (scalablePlot);

        constexpr auto pSize = 4.5f;

        if (isMouseOver())
        {
            g.setColour (bgColour.contrasting());

            const auto mx = getMouseXYRelative().toDouble().x / (double) getWidth();
            const auto mpos = createPoint (mx);
            constexpr auto size = pSize * MathConstants<float>::pi;
            g.drawEllipse (Rectangle<float> (size, size).withCentre (mpos), MathConstants<float>::pi);
        }

        if (animTimer.isTimerRunning())
        {
            g.setColour (textColour);
            g.fillEllipse (Rectangle<float> (pSize, pSize).withCentre (pos));
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
            textBounds = Rectangle<int>(); 
        }

        scaledTransform = plot.getTransformToScaleToFit (b.toFloat(), true);
        scalablePlot.applyTransform (scaledTransform);
    }

private:
    //==============================================================================
    enum { margin = 8 };

    std::function<double (double)> generator;
    const double ratio;
    const float lineThickness = 0.01f;

    int fontHeight = 18;
    Rectangle<int> textBounds;
    Path plot, scalablePlot;
    AffineTransform scaledTransform;

    double normalisedTime { 0.0 };
    Point<float> pos;
    OffloadedTimer animTimer;

    Point<float> createPoint (double normPos) const
    {
        return Point<double> (normPos, generator (normPos))
                .transformedBy (AffineTransform::verticalFlip (1.0))
                .toFloat()
                .transformedBy (scaledTransform);
    }

    void tickAnimation()
    {
        if (scalablePlot.isEmpty())
            return;

        pos = createPoint (normalisedTime);
        normalisedTime += ratio;

        if (normalisedTime > 1.0)
            normalisedTime = 0.0;

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

        listbox.setRowHeight (192);
        listbox.setModel (this);
        addAndMakeVisible (listbox);
    }

    //==============================================================================
    int getNumRows() override { return generators.size(); }

    void paintListBoxItem (int, Graphics&, int, int, bool) override {}

    void resized() override
    {
        listbox.setBounds (getLocalBounds().reduced (margin));
    }

    Component* refreshComponentForRow (int row, bool, Component* comp) override
    {
        std::unique_ptr<CurveDisplayComponent> cdc (static_cast<CurveDisplayComponent*> (comp));
        comp = nullptr;

        if (! isPositiveAndBelow (row, getNumRows()))
            return nullptr;

        auto* gen = generators[row];

        cdc.reset (new CurveDisplayComponent (gen->generator));
        cdc->setName (gen->name);
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
