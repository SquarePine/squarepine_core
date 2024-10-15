// Constants
const int NUM_RAIN_PARTICLES = 1000;
const int NUM_SNOW_PARTICLES = 500;
const float GRAVITY_RAIN = 9.8f;
const float GRAVITY_SNOW = 3.0f;
const int GROUND_LEVEL = 500;

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
    Particle (juce::Rectangle<float> bounds_,
              float velocityX, float velocityY,
              juce::Colour c) :
        bounds (bounds_),
        velocity (velocityX, velocityY),
        colour (c)
    {
    }

    /** */
    Particle (float x, float y, float size,
              float velocityX, float velocityY,
              juce::Colour c) :
        Particle ({ x, y, size, size }, velocityX, velocityY, c)
    {
    }

    /** */
    virtual ~Particle() = default;

    /** Virtual method for updating the particle's position. */
    virtual void update (float gravity)
    {
        velocity.y += gravity;
        bounds += velocity;
    }

    /** */
    virtual bool isOutOfBounds (juce::Rectangle<float> area)
    {
        return ! area.contains (bounds);
    }

    /** Method to draw the particle. */
    virtual void paint (Graphics& g) = 0;

protected:
    juce::Rectangle<float> bounds;
    juce::Point<float> velocity;
    juce::Colour colour;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Particle)
};

// RainParticle Class
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
        g.setColour (colour);
        g.fillRect (bounds);
    }
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

    void update (float gravity) override
    {
        velocity.x += getRandomFloat (-0.5f, 0.5f);
        Particle::update (gravity);
    }

    void paint (Graphics& g) override
    {
        g.setColour (colour);
        g.fillEllipse (bounds);
    }
};

class ParticleSystemComponent : public Component,
                                private Timer
{
public:
    ParticleSystemComponent()
    {
        for (int i = 0; i < NUM_RAIN_PARTICLES; ++i)
            add (new RainParticle (getRandomFloat (0, 800), getRandomFloat (0, 600)));

        for (int i = 0; i < NUM_SNOW_PARTICLES; ++i)
            add (new SnowParticle (getRandomFloat (0, 800), getRandomFloat (0, 600)));

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

        for (auto& particle : particles)
        {
            if (auto* snowParticle = dynamic_cast<SnowParticle*> (particle))
                particle->update (GRAVITY_SNOW);

            if (auto* rainParticle = dynamic_cast<RainParticle*> (particle))
                particle->update (GRAVITY_RAIN);

            if (particle->isOutOfBounds (b))
            {
            }
        }

        repaint();
    }

/*
    void resetParticle (Particle* particle, bool isRain)
    {
        float x = getRandomFloat(0, 800);
        float y = 0;

        if (isRain)
            particle.reset (new RainParticle (x, y));
        else
            particle.reset (new SnowParticle (x, y));
    }
*/

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
