// Random utility function
inline float getRandomFloat (float min, float max)
{
    const auto n = Random::getSystemRandom().nextFloat();
    return lerp (min, max, n);
}

/** Base Particle Class. */
class Particle
{
public:
    /** */
    Particle (juce::Rectangle<float> bounds,
              float velocityX, float velocityY,
              Colour colour)
    {
        consts.bounds = bounds;
        consts.velocity = { velocityX, velocityY };
        consts.colour = colour;
        changeable = consts;
    }

    /** */
    Particle (float x, float y, float size,
              float velocityX, float velocityY,
              Colour c) :
        Particle ({ x, y, size, size }, velocityX, velocityY, c)
    {
    }

    /** */
    virtual ~Particle() = default;

    /** @returns the gravity in m/s². */
    virtual float getGravity() const { return 9.80665f; }

    /** Virtual method for updating the particle's position. */
    virtual void update()
    {
        changeable.velocity.y += getGravity();
        changeable.bounds += changeable.velocity;
    }

    /** */
    enum class EndingMethod
    {
        reset,
        remove
    };

    /** */
    virtual std::optional<EndingMethod> shouldEnd (juce::Rectangle<float> area) const
    {
        if (area.contains (changeable.bounds))
            return {};

        return { EndingMethod::reset };
    }

    /** Method to draw the particle. */
    virtual void paint (Graphics&) = 0;

    /** */
    virtual void reset() { changeable = consts; }

    /** */
    juce::Rectangle<float> getSourceBounds() const noexcept { return consts.bounds; }
    /** */
    juce::Point<float> getSourceVelocity() const noexcept { return consts.velocity; }
    /** */
    Colour getSourceColour() const noexcept { return consts.colour; }

    /** */
    juce::Rectangle<float> getActiveBounds() const noexcept { return changeable.bounds; }
    /** */
    juce::Point<float> getActiveVelocity() const noexcept { return changeable.velocity; }
    /** */
    Colour getActiveColour() const noexcept { return changeable.colour; }

protected:
    struct Details final
    {
        juce::Rectangle<float> bounds;
        juce::Point<float> velocity;
        Colour colour;
    };

    Details changeable;

private:
    Details consts;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Particle)
};

class RainParticle final : public Particle
{
public:
    RainParticle (float x, float y) :
        Particle (x, y, getRandomFloat (5.0f, 10.0f),
                  0.0f, getRandomFloat (5.0f, 15.0f),
                  juce::Colours::blue.withAlpha (0.5f))
    {
    }

    void paint (Graphics& g) override
    {
        g.setColour (getActiveColour());
        g.fillRect (getActiveBounds());
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RainParticle)
};

// SnowParticle Class
class SnowParticle final : public Particle
{
public:
    SnowParticle (float x, float y) :
        Particle (x, y, getRandomFloat (3.0f, 5.0f),
                  getRandomFloat (-2.0f, 2.0f), getRandomFloat (1.0f, 3.0f),
                  juce::Colours::white)
    {
    }

    float getGravity() const override { return MathConstants<float>::pi; }

    void update() override
    {
        changeable.velocity.x += getRandomFloat (-0.5f, 0.5f);
        Particle::update();
    }

    void paint (Graphics& g) override
    {
        g.setColour (getActiveColour());
        g.fillEllipse (getActiveBounds());
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SnowParticle)
};

class ParticleSystemComponent : public Component,
                                private Timer
{
public:
    ParticleSystemComponent()
    {
        for (int i = 0; i < 1000; ++i)
            add (new RainParticle (getRandomFloat (0.0f, 800.0f), getRandomFloat (0.0f, 600.0f)));

        for (int i = 0; i < 500; ++i)
            add (new SnowParticle (getRandomFloat (0.0f, 800.0f), getRandomFloat (0.0f, 600.0f)));

        startTimerHz (60);
    }

    void paint (Graphics& g) override
    {
        for (auto& particle : particles)
            particle->paint (g);
    }

    void resized() override
    {
    }

    void add (Particle* particle)
    {
        particles.add (particle);
    }

    void remove (Particle* particle)
    {
        particles.removeObject (particle);
    }

private:
    OwnedArray<Particle> particles;

    void timerCallback() override
    {
        const auto b = getLocalBounds().toFloat();

        Array<Particle*> toRemove, toReset;

        for (auto& particle : particles)
        {
            particle->update();

            if (const auto result = particle->shouldEnd (b); result.has_value())
            {
                switch (*result)
                {
                    case Particle::EndingMethod::reset:     toReset.add (particle); break;
                    case Particle::EndingMethod::remove:    toRemove.add (particle); break;
                    default: jassertfalse; break;
                };
            }
        }

        for (auto& particle : toRemove)
            particles.removeObject (particle);

        for (auto& particle : toReset)
            particle->reset();

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
