#pragma once

//==============================================================================
/** */
class CurveDisplayComponent final : public Component
{
public:
    /** */
    CurveDisplayComponent (std::function<double (double)> generator,
                           float lineThickness = 0.001f,
                           int64 numPoints = 333)
    {
        Point<double> last;
        last.y = generator (0.0);

        const auto ratio = 1.0 / (double) numPoints;

        for (double x = ratio; x <= 1.0; x += ratio)
        {
            const auto y = generator (x);
            const auto c = Point<double> (x, y);

            plot.addLineSegment ({ last.toFloat(), c.toFloat() }, lineThickness);
            last = c;
        }

        // Because JUCE's coords are backwards...
        plot.applyTransform (AffineTransform::verticalFlip (1.0f));

        setColour (textColourId, Colours::darkgrey);
        setColour (backgroundColourId, Colours::white);
        setColour (lineColourId, Colours::darkgrey);
    }

    //==============================================================================
    /** @internal */
    void paint (Graphics& g) override
    {
        g.fillAll (findColour (backgroundColourId));

        if (getName().isNotEmpty())
        {
            g.setColour (findColour (textColourId));
            g.setFont (Font ((float) fontHeight));
            g.drawFittedText (TRANS (getName()), textBounds, Justification::centred, 2, 1.0f);
        }

        g.setColour (findColour (lineColourId));
        g.fillPath (scalablePlot);
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

        auto t = plot.getTransformToScaleToFit (b.toFloat(), true, Justification::centred);
        scalablePlot.applyTransform (t);
    }

    //==============================================================================
    /** */
    enum ColourIds
    {
        textColourId = 0x77112200,  //<
        backgroundColourId,         //<
        lineColourId                //< 
    };

private:
    //==============================================================================
    enum { margin = 8 };

    int fontHeight = 18;
    Rectangle<int> textBounds;
    Path plot, scalablePlot;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CurveDisplayComponent)
};

//==============================================================================
/** A general demonstration of all pre-configured easing functions. */
class EaseListComponent final : public Component,
                                public ListBoxModel
{
public:
    EaseListComponent()
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

        if (isPositiveAndBelow (row, getNumRows()))
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
