/** */
class SquarePineDemoSplashScreen final : public Component,
                                         public DarkModeSettingListener,
                                         private Timer
                                
{
public:
    SquarePineDemoSplashScreen (bool shouldDeleteAfterCompletion = true) :
        deleteAfterCompletion (shouldDeleteAfterCompletion)
    {
        logo = SharedObjects::getMainLogoSVG();
        Desktop::getInstance().addDarkModeSettingListener (this);
        setInterceptsMouseClicks (false, false);

        const bool shouldBeFullscreen =
       #if SQUAREPINE_IS_MOBILE
        true;
       #else
        false;
       #endif

        makeVisible (512, 256, false, shouldBeFullscreen);
    }

    ~SquarePineDemoSplashScreen() override
    {
        Desktop::getInstance().removeDarkModeSettingListener (this);
    }

    void run (RelativeTime timeout = RelativeTime::seconds (MathConstants<double>::twoPi))
    {
        if (Desktop::getInstance().isHeadless())
        {
            endSplashScreen();
            return;
        }

        if (isTimerRunning())
            return;

        minimumVisibleTime = timeout;
        startTime = Time::getCurrentTime();
        endTime = startTime + minimumVisibleTime;

        startTimerHz (60);
    }

    void paint (Graphics& g) override
    {
        g.fillAll (getLookAndFeel().findColour (TextEditor::backgroundColourId));

        if (logo != nullptr)
            logo->drawWithin (g, logoArea, RectanglePlacement::centred | RectanglePlacement::onlyReduceInSize, 1.0f);

        g.setColour (getLookAndFeel().findColour (TextEditor::textColourId));

        String appName = "SquarePine Demo";
        if (auto* app = JUCEApplicationBase::getInstance())
             appName = makeMainWindowTitle (app->getApplicationName(), app->getApplicationVersion());

        g.drawFittedText (appName, textArea, Justification::centred, 1, 1.0f);
    }

    void resized() override
    {
        auto b = getLocalBounds();
        b = b.reduced (dims::marginPx);

        textArea = b.removeFromBottom (dims::splashTitleHeightPx);
        logoArea = b.toFloat();
    }

    void darkModeSettingChanged() override
    {
        if (! aboutToBeDeleted)
            repaint();
    }

    std::function<void()> onComplete;

private:
    const bool deleteAfterCompletion = true;
    bool aboutToBeDeleted = false;
    Time startTime, endTime;

    std::unique_ptr<Drawable> logo;
    Rectangle<float> logoArea;
    Rectangle<int> textArea;
    RelativeTime minimumVisibleTime;

    void makeVisible (int w, int h, bool useDropShadow, bool fullscreen)
    {
        const auto screenSize = []()
        {
            if (auto* primaryDisplay = Desktop::getInstance().getDisplays().getPrimaryDisplay())
                return primaryDisplay->userArea;

            // Headless fallback...
            return Rectangle<int> (800, 600);
        }();

        const auto width        = fullscreen ? screenSize.getWidth()   : w;
        const auto height       = fullscreen ? screenSize.getHeight()  : h;

        setAlwaysOnTop (true);
        setVisible (true);
        centreWithSize (width, height);
        addToDesktop (useDropShadow ? ComponentPeer::windowHasDropShadow : 0);

        if (fullscreen)
            if (auto* peer = getPeer())
                peer->setFullScreen (true);

        toFront (true);
    }

    void endSplashScreen()
    {
        aboutToBeDeleted = true;
        stopTimer();

        if (onComplete != nullptr)
            onComplete();

        if (deleteAfterCompletion)
            delete this;
    }
 
    void timerCallback() override
    {
        constexpr auto fadeOutPos = 0.8;

        const auto currentTime = Time::getCurrentTime();
        const auto normalisedPos = jmap ((double) currentTime.toMilliseconds(),
                                         (double) startTime.toMilliseconds(),
                                         (double) endTime.toMilliseconds(),
                                         0.0, 1.0);

        if (isValueBetween (normalisedPos, fadeOutPos, 1.0))
        {
            const auto pos = jmap (normalisedPos,
                                   fadeOutPos, 1.0,
                                   0.0, 1.0);

            const auto alpha = 1.0 - ease::cubic::out::expo (pos);

            setAlpha ((float) std::clamp (alpha, 0.0, 1.0));
            repaint();
        }
        else if (normalisedPos >= 1.0)
        {
            endSplashScreen();
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SquarePineDemoSplashScreen)
};
