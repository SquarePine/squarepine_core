/** Base Particle Class. */
class Particle
{
public:
    /** */
    Particle (juce::Rectangle<float> bounds,
              float velocityX, float velocityY,
              Colour colour,
              std::optional<double> lifetimeSeconds)
    {
        consts.bounds   = bounds;
        consts.velocity = { velocityX, velocityY };
        consts.colour   = colour;
        changeable      = consts;
        lifetime        = lifetimeSeconds;
    }

    /** */
    Particle (float x, float y, float size,
              float velocityX, float velocityY,
              Colour colour,
              std::optional<double> lifetimeSeconds) :
        Particle ({ x, y, size, size },
                  velocityX, velocityY,
                  colour, lifetimeSeconds)
    {
    }

    /** */
    virtual ~Particle() = default;

    //==============================================================================
    /** @returns the gravity in pixels per update. */
    virtual float getGravity() const { return 1.0f; }

    /** Updates the particle's position and velocity as per the provided gravity (in px/update).

        @see getGravity
    */
    virtual void update (const Time& currentTime)
    {
        if (! started && lifetime.has_value())
            startTime = currentTime;

        started = true;
        changeable.velocity.y += getGravity();
        changeable.bounds += changeable.velocity;
    }

    /** */
    virtual bool shouldEnd (const Time& currentTime, const juce::Rectangle<float>& area) const
    {
        if (area.contains (changeable.bounds))
            return false;

        if (started
            && lifetime.has_value()
            && (startTime + RelativeTime (*lifetime)) < currentTime)
        {
            return false;
        }

        return true;
    }

    /** Resets the particle's bounds, velocity, and colour to
        those that were set in the constructor.
    */
    virtual void reset()
    {
        changeable = consts;
        startTime = Time();
        started = false;
    }

    /** Your derived particle class must fill this to draw as needed. */
    virtual void paint (Graphics&) = 0;

    //==============================================================================
    /** @returns the lifetime of the particle, as set by setLifetimeSeconds(). */
    [[nodiscard]] std::optional<double> getLifetimeSeconds() const noexcept { return lifetime; }

    /** @returns the original bounds as set in the constructor. */
    [[nodiscard]] const juce::Rectangle<float>& getSourceBounds() const noexcept { return consts.bounds; }
    /** @returns the original velocity as set in the constructor. */
    [[nodiscard]] const juce::Point<float>& getSourceVelocity() const noexcept { return consts.velocity; }
    /** @returns the original colour as set in the constructor. */
    [[nodiscard]] Colour getSourceColour() const noexcept { return consts.colour; }

    /** @returns the current bounds, which may differ from the one set in the constructor.

        This would occur if the particle had its bounds overridden in the update() call.
    */
    [[nodiscard]] const juce::Rectangle<float>& getActiveBounds() const noexcept { return changeable.bounds; }

    /** @returns the current velocity (X, Y), which may differ from the one set in the constructor.

        This would occur if the particle had its velocity overridden in the update() call.
    */
    [[nodiscard]] const juce::Point<float>& getActiveVelocity() const noexcept { return changeable.velocity; }

    /** @returns the current colour, which may differ from the one set in the constructor.

        This would occur if the particle had its Colour overridden in the update() call.
    */
    [[nodiscard]] Colour getActiveColour() const noexcept { return changeable.colour; }

    //==============================================================================
    /** @returns a random float between the given values. */
    static float getRandomFloat (float min, float max)
    {
        const auto n = Random::getSystemRandom().nextFloat();
        return lerp (min, max, n);
    }

    /** */
    static juce::Rectangle<float> createRectangleWithRandomWidth (float min, float max, float height)
    {
        return { height, getRandomFloat (min, max) };
    }

    /** */
    static juce::Rectangle<float> createRectangleWithRandomHeight (float min, float max, float width)
    {
        return { getRandomFloat (min, max), width };
    }

    /** */
    static juce::Rectangle<float> createRectangleWithRandomSize (float min, float max)
    {
        const auto s = getRandomFloat (min, max);
        return { s, s };
    }

protected:
    //==============================================================================
   #if JUCE_MSVC
    #pragma pack (push, 1)
   #endif

    /** */
    struct Details final
    {
        juce::Rectangle<float> bounds;
        juce::Point<float> velocity;
        Colour colour;
    } JUCE_PACKED;

   #if JUCE_MSVC
    #pragma pack (pop)
   #endif

    Details changeable;

private:
    //==============================================================================
    Details consts;
    std::optional<double> lifetime;
    bool started = false;
    Time startTime;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Particle)
};

//==============================================================================
class RainParticle final : public Particle
{
public:
    RainParticle (float x, float y, std::optional<double> lifetimeSeconds = {}) :
        Particle (createRectangleWithRandomHeight (5.0f, 10.0f, 1.0f).withPosition (x, y),
                  0.0f, getRandomFloat (5.0f, 15.0f),
                  juce::Colours::blue.withAlpha (0.5f),
                  lifetimeSeconds)
    {
    }

    void paint (Graphics& g) override
    {
        g.setColour (getActiveColour());
        g.fillRect (getActiveBounds().withWidth (1.0f));
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RainParticle)
};

//==============================================================================
class SnowParticle final : public Particle
{
public:
    SnowParticle (float x, float y, std::optional<double> lifetimeSeconds = {}) :
        Particle (createRectangleWithRandomSize (3.0f, 5.0f).withPosition (x, y),
                  getRandomFloat (-2.0f, 2.0f), getRandomFloat (1.0f, 3.0f),
                  juce::Colours::white.withAlpha (0.5f), lifetimeSeconds)
    {
    }

    float getGravity() const override { return 0.03f; }

    void update (const Time& currentTime) override
    {
        changeable.velocity.x += getRandomFloat (-0.5f, 0.5f);
        Particle::update (currentTime);
    }

    void paint (Graphics& g) override
    {
        g.setColour (getActiveColour());
        g.fillEllipse (getActiveBounds());
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SnowParticle)
};

//==============================================================================
/**
*/
class ParticleEmitter final
{
public:
    /** Constructor.

        @param shouldRemoveParticlesOnEnd
    */
    ParticleEmitter (bool shouldRemoveParticlesOnEnd = false) :
        shouldRemoveOnEnd (shouldRemoveParticlesOnEnd)
    {
    }

    //==============================================================================
    /** @returns */
    [[nodiscard]] bool removesParticlesOnEnd() const noexcept { return shouldRemoveOnEnd; }

    //==============================================================================
    /** Appends a new particle to the list of particles.

        Note that this particle will be deleted by the particle emitter when it is removed,
        so be careful not to delete it somewhere else.

        Also be careful not to add the same particle to the emitter more than once,
        as this will obviously cause deletion of dangling pointers.

        @param newObject    The new object to add to the array.
        @returns            The new object that was added.
    */
    Particle* add (Particle* particle) { return particles.add (particle); }

    /** Appends a new particle to the list of particles.

        Note that this particle will be deleted by the particle emitter when it is removed,
        so be careful not to delete it somewhere else.

        Also be careful not to add the same particle to the emitter more than once,
        as this will obviously cause deletion of dangling pointers.

        @param newObject    The new object to add to the array.
        @returns            The new object that was added.
    */
    Particle* add (std::unique_ptr<Particle> newParticle) { return add (newParticle.release()); }

    /** Removes a specified particle.

        If the particle isn't found, no action is taken.

        @param particle     The particle to try to remove.
        @param deleteObject Whether to delete the particle (if it's found).
    */
    void remove (Particle* particle, bool deleteObject = true)
    {
        particles.removeObject (particle, deleteObject);
    }

    /** @returns the number of particles currently stored in this particle emitter. */
    [[nodiscard]] int getNumParticles() const noexcept { return particles.size(); }

    /** Reduces the amount of storage being used by the array of particles.

        Arrays typically allocate slightly more storage than they need, and after
        removing elements, they may have quite a lot of unused space allocated.
        This method will reduce the amount of allocated storage to a minimum.
    */
    void minimiseStorageOverheads() noexcept { particles.minimiseStorageOverheads(); }

    //==============================================================================
    /** */
    void update (const Time& currentTime, juce::Rectangle<float> paintingArea)
    {
        Array<Particle*> toResetOrRemove;

        for (auto& particle : particles)
        {
            particle->update (currentTime);

            if (particle->shouldEnd (currentTime, paintingArea))
                toResetOrRemove.add (particle);
        }

        if (shouldRemoveOnEnd)
            for (auto& particle : toResetOrRemove)
                remove (particle);
        else
            for (auto& particle : toResetOrRemove)
                particle->reset();
    }

    /** */
    void paint (Graphics& g)
    {
        for (auto& particle : particles)
            particle->paint (g);
    }

private:
    //==============================================================================
    const bool shouldRemoveOnEnd = false;
    OwnedArray<Particle> particles;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParticleEmitter)
};

//==============================================================================
class ParticleSystemComponent : public Component,
                                private Timer
{
public:
    ParticleSystemComponent()
    {
        for (int i = 0; i < 1000; ++i)
            snowEmitter.add (new RainParticle (Particle::getRandomFloat (0.0f, 800.0f),
                                               Particle::getRandomFloat (0.0f, 600.0f)));

        for (int i = 0; i < 500; ++i)
            rainEmitter.add (new SnowParticle (Particle::getRandomFloat (0.0f, 800.0f),
                                               Particle::getRandomFloat (0.0f, 600.0f)));

        startTimerHz (60);
    }

    void paint (Graphics& g) override
    {
        snowEmitter.paint (g);
        rainEmitter.paint (g);
    }

private:
    ParticleEmitter snowEmitter, rainEmitter;

    void timerCallback() override
    {
        const auto b = getLocalBounds().toFloat();
        const auto ct = Time::getCurrentTime();

        snowEmitter.update (ct, b);
        rainEmitter.update (ct, b);

        repaint();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParticleSystemComponent)
};


#if 0
struct Particle
{
    Particle() = default;
    virtual ~Particle() = default;

    virtual void update() = 0;
    virtual void paint (Graphics& g) = 0;

    juce::Rectangle<float> bounds;
    float opacity = 1.0f;
    float rotationDegrees = 0.0f;
    Point<float> velocity, scale { 1.0f, 1.0f };

    RelativeTime currenTime;
    double durationSeconds = 0.0;
};

struct DrawableParticle final : public Particle
{
    DrawableParticle()
    {
        durationSeconds = 3.0 * Random::getSystemRandom().nextDouble();
    }

    void update() override
    {
        transform = AffineTransform().followedBy (AffineTransform::scale (scale.x, scale.y))
                                     .followedBy (AffineTransform::rotation (degreesToRadians (rotationDegrees)))
                                     .followedBy (AffineTransform::translation (bounds.getPosition()));

        if (drawable != nullptr)
            drawable->setTransform (transform);
    }

    void paint (Graphics& g) override
    {
        if (drawable != nullptr)
        {
            drawable->draw (g, opacity);
        }
        else
        {
            g.setColour (Colours::red.withAlpha (1.0f - opacity));
            g.fillRect (bounds);
        }
    }

    AffineTransform transform;
    std::unique_ptr<Drawable> drawable;
};

struct Emitter
{
    Emitter()
    {
        particles.ensureStorageAllocated (maxNumParticles);
    }

    virtual ~Emitter()
    {
    }

    void update()
    {
        for (int i = particles.size(); --i >= 0;)
        {
        }
    }

    void paint (Graphics& g)
    {
        for (auto& p : particles)
            p->paint (g);
    }

    bool active = true;
    juce::Rectangle<float> bounds;
    int maxNumParticles = 500;
    OwnedArray<Particle> particles;
};

class ParticleEngine : public Component
{
public:
    ParticleEngine()
    {
        animationTimer.callback = [&]() { repaint(); };
    }

    void update()
    {
        for (auto* e : emitters)
            if (e->active)
                e->update();
    }

    void paint (Graphics& g) override
    {
        for (auto* e : emitters)
            if (e->active)
                e->paint (g);
    }

private:
    OwnedArray<Emitter> emitters;
    OffloadedTimer animationTimer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParticleEngine)
};

#endif
