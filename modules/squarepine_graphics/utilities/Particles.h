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
