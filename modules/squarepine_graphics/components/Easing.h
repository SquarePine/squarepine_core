//==============================================================================
/** */
class CurveDisplayComponent final : public Component
{
public:
    //==============================================================================
    /** */
    class Generator
    {
    public:
        /** */
        Generator() = default;
        /** */
        virtual ~Generator() = default;

        /** */
        virtual double generate (double value) const = 0;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Generator)
    };

    //==============================================================================
    /** */
    CurveDisplayComponent (Generator& generator,
                           float lineThickness = 0.001f,
                           int64 numPoints = 10000)
    {
        Point<double> last;
        last.y = generator.generate (0.0);

        const auto ratio = 1.0 / (double) numPoints;

        for (double x = ratio; x <= 1.0; x += ratio)
        {
            const auto y = generator.generate (x);
            const auto c = Point<double> (x, y);

            plot.addLineSegment ({ last.toFloat(), c.toFloat() }, lineThickness);
            last = c;
        }

        // Because JUCE is backwards...
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
            g.drawFittedText (getName(), textBounds, Justification::centred, 2, 1.0f);
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
/** @see https://easings.net/ */
class EaseListComponent final : public Component
{
public:
    EaseListComponent()
    {
        addCurveDisplay<ease::audio::linear>            ("Linear");
        addCurveDisplay<ease::audio::smoothstepEase>    ("Smoothstep");
        addCurveDisplay<ease::audio::smootherstepEase>  ("Smootherstep");

        addCurveDisplay<ease::audio::sinEase>       ("sin");
        addCurveDisplay<ease::audio::cosEase>       ("cos");
        addCurveDisplay<ease::audio::sincEase>      ("sinc");

        addCurveDisplay<ease::audio::squareWave>    ("Square");
        addCurveDisplay<ease::audio::sawtoothWave>  ("Saw");
        addCurveDisplay<ease::audio::triangleWave>  ("Triangle");

        addCurveDisplay<ease::cubic::in::back>      ("In - Back");
        addCurveDisplay<ease::cubic::out::back>     ("Out - Back");
        addCurveDisplay<ease::cubic::inOut::back>   ("InOut - Back");

        addCurveDisplay<ease::cubic::in::bounce>    ("In - Bounce");
        addCurveDisplay<ease::cubic::out::bounce>   ("Out - Bounce");
        addCurveDisplay<ease::cubic::inOut::bounce> ("InOut - Bounce");

        addCurveDisplay<ease::cubic::in::circ>      ("In - Circ");
        addCurveDisplay<ease::cubic::out::circ>     ("Out - Circ");
        addCurveDisplay<ease::cubic::inOut::circ>   ("InOut - Circ");

        addCurveDisplay<ease::cubic::in::cubic>     ("In - Cubic");
        addCurveDisplay<ease::cubic::out::cubic>    ("Out - Cubic");
        addCurveDisplay<ease::cubic::inOut::cubic>  ("InOut - Cubic");

        addCurveDisplay<ease::cubic::in::elastic>   ("In - Elastic");
        addCurveDisplay<ease::cubic::out::elastic>  ("Out - Elastic");
        addCurveDisplay<ease::cubic::inOut::elastic>("InOut - Elastic");

        addCurveDisplay<ease::cubic::in::expo>      ("In - Expo");
        addCurveDisplay<ease::cubic::out::expo>     ("Out - Expo");
        addCurveDisplay<ease::cubic::inOut::expo>   ("InOut - Expo");

        addCurveDisplay<ease::cubic::in::quad>      ("In - Quad");
        addCurveDisplay<ease::cubic::out::quad>     ("Out - Quad");
        addCurveDisplay<ease::cubic::inOut::quad>   ("InOut - Quad");

        addCurveDisplay<ease::cubic::in::quart>     ("In - Quart");
        addCurveDisplay<ease::cubic::out::quart>    ("Out - Quart");
        addCurveDisplay<ease::cubic::inOut::quart>  ("InOut - Quart");

        addCurveDisplay<ease::cubic::in::quint>     ("In - Quint");
        addCurveDisplay<ease::cubic::out::quint>    ("Out - Quint");
        addCurveDisplay<ease::cubic::inOut::quint>  ("InOut - Quint");

        addCurveDisplay<ease::cubic::in::sine>      ("In - Sine");
        addCurveDisplay<ease::cubic::out::sine>     ("Out - Sine");
        addCurveDisplay<ease::cubic::inOut::sine>   ("InOut - Sine");

        grid.rowGap         = Grid::Px (margin);
        grid.columnGap      = Grid::Px (margin);
        grid.autoColumns    = Grid::TrackInfo (Grid::Fr (1));
        grid.autoRows       = Grid::TrackInfo (Grid::Fr (1));
        grid.autoFlow       = Grid::AutoFlow::column;

        constexpr auto numRowGroups = 3;
        for (int i = numRowGroups; --i >= 0;)
            grid.templateRows.add (Grid::TrackInfo (1_fr));

        for (int i = grid.items.size() / numRowGroups; --i >= 0;)
            grid.templateColumns.add (Grid::TrackInfo (1_fr));
    }

    //==============================================================================
    void resized()
    {
        grid.performLayout (getLocalBounds());
    }

private:
    //==============================================================================
    enum { margin = 8 };

    //==============================================================================
   #if JUCE_CXX17_IS_AVAILABLE
    template<auto weightGenerationFunction>
   #else
    #error Fill this out...
   #endif
    class AnyFunctionGenerator final : public CurveDisplayComponent::Generator
    {
    public:
        AnyFunctionGenerator() = default;

        double generate (double value) const override
        {
            return std::invoke (weightGenerationFunction, value);
        }
    };

    //==============================================================================
    OwnedArray<CurveDisplayComponent> curves;
    Grid grid;

    //==============================================================================
    template<auto weightGenerationFunction>
    void addCurveDisplay (const String& name)
    {
        AnyFunctionGenerator<weightGenerationFunction> generator;

        auto* c = curves.add (new CurveDisplayComponent (generator, 0.01f));
        c->setName (name);
        addAndMakeVisible (c);

        grid.items.add (GridItem (c));
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EaseListComponent)
};
