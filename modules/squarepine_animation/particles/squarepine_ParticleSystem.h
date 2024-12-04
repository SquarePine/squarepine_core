#if JUCE_MSVC
 #pragma pack (push, 1)
#endif

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
    Particle (float size,
              float velocityX, float velocityY,
              Colour colour,
              std::optional<double> lifetimeSeconds) :
        Particle (0.0f, 0.0f, size,
                  velocityX, velocityY,
                  colour, lifetimeSeconds)
    {
    }

    /** */
    virtual ~Particle() = default;

    //==============================================================================
    /** Updates the particle's position and velocity as per the provided gravity (in px/update).

        @see getGravity
    */
    virtual void update (const Time& currentTime,
                         double deltaTimeMs,
                         float gravityPxPerUpdate)
    {
        ignoreUnused (deltaTimeMs);

        if (! started && lifetime.has_value())
            startTime = currentTime;

        started = true;
        changeable.velocity.y += gravityPxPerUpdate;
        changeable.bounds += changeable.velocity;
    }

    /**

        @returns true
    */
    virtual bool shouldEnd (const Time& currentTime,
                            double deltaTimeMs,
                            const juce::Rectangle<float>& paintingArea) const
    {
        ignoreUnused (deltaTimeMs);

        if (started
            && lifetime.has_value()
            && (startTime + RelativeTime (*lifetime)) < currentTime)
        {
            return false;
        }

        return ! paintingArea.contains (changeable.bounds);
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

    /** */
    void setStartX (float x)
    {
        consts.bounds.setX (x);
    }

    /** */
    void setStartY (float y)
    {
        consts.bounds.setX (y);
    }

    /** */
    void setStartPosition (juce::Point<float> newPosition)
    {
        consts.bounds.setPosition (newPosition);
    }

    /** */
    void setStartPosition (float x, float y)
    {
        consts.bounds.setPosition (x, y);
    }

    //==============================================================================
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
        static Xorshift64 rng;
        return lerp (min, max, (float) rng.generateNormalised());
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
    /** */
    struct Details final
    {
        juce::Rectangle<float> bounds;
        juce::Point<float> velocity;
        Colour colour;
    } JUCE_PACKED;

    Details changeable;

private:
    //==============================================================================
    Details consts;
    std::optional<double> lifetime;
    bool started = false;
    Time startTime;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Particle)
} JUCE_PACKED;

//==============================================================================
class RainParticle final : public Particle
{
public:
    RainParticle (std::optional<double> lifetimeSeconds = {}) :
        Particle (createRectangleWithRandomHeight (5.0f, 10.0f, 1.0f),
                  0.0f, getRandomFloat (5.0f, 15.0f),
                  juce::Colours::blue,
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
} JUCE_PACKED;

//==============================================================================
/**
*/
class SnowParticle final : public Particle
{
public:
    SnowParticle (std::optional<double> lifetimeSeconds = {}) :
        Particle (createRectangleWithRandomSize (3.0f, 5.0f),
                  getRandomFloat (-2.0f, 2.0f), getRandomFloat (1.0f, 3.0f),
                  juce::Colours::white, lifetimeSeconds)
    {
    }

    void update (const Time& currentTime, double deltaTimeMs, float gravityPxPerUpdate) override
    {
        changeable.velocity.x += getRandomFloat (-0.5f, 0.5f);
        Particle::update (currentTime, deltaTimeMs, gravityPxPerUpdate);
    }

    void paint (Graphics& g) override
    {
        g.setColour (getActiveColour());
        g.fillEllipse (getActiveBounds());
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SnowParticle)
} JUCE_PACKED;

#if JUCE_MSVC
 #pragma pack (pop)
#endif

//==============================================================================
/**
*/
class ParticleEmitter final
{
public:
    /** Constructor.

        @param gravityInPxPerUpdate
        @param shouldRemoveParticlesOnEnd
    */
    ParticleEmitter (float gravityInPxPerUpdate = 1.0f,
                     bool shouldRemoveParticlesOnEnd = false) :
        shouldRemoveOnEnd (shouldRemoveParticlesOnEnd),
        gravityPxPerUpdate (gravityInPxPerUpdate)
    {
        jassert (std::isnormal (gravityPxPerUpdate));
    }

    /** */
    ~ParticleEmitter() = default;

    //==============================================================================
    /** @returns */
    [[nodiscard]] bool removesParticlesOnEnd() const noexcept { return shouldRemoveOnEnd; }

    /**
    */
    void setGravity (float newGravityInPxPerUpdate)
    {
        jassert (std::isnormal (newGravityInPxPerUpdate));

        gravityPxPerUpdate = newGravityInPxPerUpdate;
    }

    /** @returns the gravity in pixels per update. */
    [[nodiscard]] float getGravity() const noexcept { return gravityPxPerUpdate; }

    //==============================================================================
    /** Appends a new particle to the list of particles.

        Note that this particle will be deleted by the particle emitter when it is removed,
        so be careful not to delete it somewhere else.

        Also be careful not to add the same particle to the emitter more than once,
        as this will obviously cause deletion of dangling pointers.

        @param newObject    The new object to add to the array.
        @returns            The new object that was added.
    */
    Particle* add (Particle* particle)
    {
        jassert (particle != nullptr);
        return particles.add (particle);
    }

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

    /** @returns a pointer to the particle at this index in the array.

        If the index is out-of-range, this will return a null pointer.
    */
    Particle* operator[] (int index) const noexcept { return particles[index]; }

    /** @returns a pointer to the first particle in the array.
        This method is provided for compatibility with standard C++ iteration mechanisms.
    */
    Particle** begin() noexcept { return particles.begin(); }

    /** @returns a pointer to the first element in the array.
        This method is provided for compatibility with standard C++ iteration mechanisms.
    */
    Particle* const* begin() const noexcept { return particles.begin(); }

    /** @returns a pointer to the element which follows the last element in the array.
        This method is provided for compatibility with standard C++ iteration mechanisms.
    */
    Particle** end() noexcept { return particles.end(); }

    /** @returns a pointer to the element which follows the last element in the array.
        This method is provided for compatibility with standard C++ iteration mechanisms.
    */
    Particle* const* end() const noexcept { return particles.end(); }

    /** Reduces the amount of storage being used by the array of particles.

        Arrays typically allocate slightly more storage than they need, and after
        removing elements, they may have quite a lot of unused space allocated.
        This method will reduce the amount of allocated storage to a minimum.
    */
    void minimiseStorageOverheads() noexcept { particles.minimiseStorageOverheads(); }

    //==============================================================================
    /** */
    void update (const Time& currentTime,
                 double deltaTimeMs,
                 const juce::Rectangle<float>& paintingArea)
    {
        Array<Particle*> toResetOrRemove;

        for (auto& particle : particles)
        {
            particle->update (currentTime, deltaTimeMs, gravityPxPerUpdate);

            if (particle->shouldEnd (currentTime, deltaTimeMs, paintingArea))
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
    float gravityPxPerUpdate = 1.0f;
    OwnedArray<Particle> particles;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParticleEmitter)
};

//==============================================================================
/**
*/
class ParticleEmitterAggregator : private Timer
{
public:
    /**
    */
    ParticleEmitterAggregator()
    {
        startTimerHz (frameRate);
    }

    /**
    */
    void setTargetBounds (juce::Rectangle<float> target)
    {
        jassert (target.isFinite() && ! target.isEmpty());

        bounds = target;
    }

    /**
    */
    void paint (Graphics& g)
    {
        for (auto& emitter : emitters)
            emitter->paint (g);
    }

private:
    static inline constexpr int frameRate = 60;
    static inline constexpr double deltaTimeMs = 1000.0 / (double) frameRate;

    OwnedArray<ParticleEmitter> emitters;
    juce::Rectangle<float> bounds;

    void timerCallback() override
    {
        const auto b = bounds.expanded (1).toFloat();
        const auto ct = Time::getCurrentTime();

        for (auto& emitter : emitters)
            emitter->update (ct, deltaTimeMs, b);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParticleEmitterAggregator)
};

//==============================================================================
/**
*/
class ParticleSystemComponent : public Component,
                                private Timer
{
public:
    /**
    */
    ParticleSystemComponent()
    {
        snowEmitter.setGravity (0.03f);

        for (int i = 0; i < 1000; ++i)
            rainEmitter.add (new RainParticle());

        for (int i = 0; i < 500; ++i)
            snowEmitter.add (new SnowParticle());

        rainEmitter.minimiseStorageOverheads();
        snowEmitter.minimiseStorageOverheads();

        startTimerHz (frameRate);
    }

    /**
    */
    void resized() override
    {
        const auto w = (float) getWidth();
        const auto h = (float) getHeight();

        for (auto& particle : rainEmitter)
            particle->setStartPosition (Particle::getRandomFloat (0.0f, w),
                                        Particle::getRandomFloat (0.0f, h));

        for (auto& particle : snowEmitter)
            particle->setStartPosition (Particle::getRandomFloat (0.0f, w),
                                        Particle::getRandomFloat (0.0f, h));

        aggregator.setTargetBounds (getLocalBounds().toFloat());
    }

    /**
    */
    void paint (Graphics& g) override
    {
        g.fillAll (Colours::black);

        rainEmitter.paint (g);
        snowEmitter.paint (g);

        aggregator.paint (g);
    }

private:
    static inline constexpr int frameRate = 60;
    static inline constexpr double deltaTimeMs = 1000.0 / (double) frameRate;

    ParticleEmitterAggregator aggregator;

    ParticleEmitter rainEmitter, snowEmitter;

    void timerCallback() override
    {
        const auto b = getLocalBounds().expanded (1).toFloat();
        const auto ct = Time::getCurrentTime();

        rainEmitter.update (ct, deltaTimeMs, b);
        snowEmitter.update (ct, deltaTimeMs, b);

        repaint();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParticleSystemComponent)
};

