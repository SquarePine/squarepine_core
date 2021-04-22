LFO::Operation::Operation() noexcept                    { }
LFO::Operation::~Operation() noexcept                   { }
LFO::AddOperation::AddOperation() noexcept              { }
LFO::SubtractOperation::SubtractOperation() noexcept    { }
LFO::MultiplyOperation::MultiplyOperation() noexcept    { }
LFO::DivideOperation::DivideOperation() noexcept        { }
LFO::EqualsOperation::EqualsOperation() noexcept        { }

void LFO::AddOperation::perform (double& sample, const double value)        { sample += value; }
void LFO::SubtractOperation::perform (double& sample, const double value)   { sample -= value; }
void LFO::MultiplyOperation::perform (double& sample, const double value)   { sample *= value; }
void LFO::DivideOperation::perform (double& sample, const double value)     { sample /= value; }
void LFO::EqualsOperation::perform (double& sample, const double value)     { sample = value; }

//==============================================================================
LFO::Configuration::Configuration() noexcept
{
}

LFO::Configuration::Configuration (const double sr, const double f, const double p) noexcept :
    sampleRate (sr),
    frequency (f),
    currentPhase (p)
{
    jassert (sampleRate > 0.0);
    jassert (frequency > 0.0);
}

LFO::Configuration::Configuration (const LFO::Configuration& other) noexcept :
    sampleRate (other.sampleRate),
    frequency (other.frequency),
    currentPhase (other.currentPhase)
{
    jassert (sampleRate > 0.0);
    jassert (frequency > 0.0);
}

LFO::Configuration& LFO::Configuration::operator= (const LFO::Configuration& other) noexcept
{
    jassert (other.sampleRate > 0.0);
    jassert (other.frequency > 0.0);

    if (this != &other)
    {
        sampleRate = other.sampleRate;
        frequency = other.frequency;
        currentPhase = other.currentPhase;
    }

    return *this;
}

void LFO::Configuration::prepare (const double newSampleRate, const double newFrequency)
{
    jassert (newSampleRate > 0.0);
    jassert (newFrequency > 0.0);

    sampleRate = newSampleRate;
    frequency = newFrequency;
    currentPhase = 0.0;
}

//==============================================================================
LFO::LFO() :
    operation (new EqualsOperation())
{
}

LFO::~LFO()
{
}

void LFO::setOperation (Operation* newOperation)
{
    if (operation.get() != newOperation)
        operation.reset (newOperation);
}

void LFO::processInternal (float& sample, const float phase)
{
    auto result = static_cast<double> (sample);
    processInternal (result, phase);

    sample = static_cast<float> (result);
}

void LFO::processInternal (double& sample, const double phase)
{
    if (operation != nullptr)
        operation->perform (sample, phase);
}

double LFO::process (juce::AudioBuffer<float>& buffer,
                     const Configuration& configuration)
{
    //double phasePerSample = configuration.currentPhase;

    //if (phasePerSample == 0.0)
    auto phasePerSample = configuration.getPhasePerSample();

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    auto phase = phasePerSample;

    for (int i = 0; i < numSamples; ++i)
    {
        phase = calculateNextPhase (i * phasePerSample);

        for (int f = numChannels; --f >= 0;)
        {
            if (float* const channel = buffer.getWritePointer (i))
            {
                auto sample = static_cast<double> (channel[f]);
                processInternal (sample, phase);
                channel[f] = static_cast<float> (sample);
            }
        }
    }

    return phase;
}

//==============================================================================
SineLFO::SineLFO()
{
}

double SineLFO::calculateNextPhase (const double currentPhase) const
{
    return std::sin (currentPhase);
}

//==============================================================================
CosineLFO::CosineLFO()
{
}

double CosineLFO::calculateNextPhase (const double currentPhase) const
{
    return std::cos (currentPhase);
}

//==============================================================================
TangentLFO::TangentLFO()
{
}

double TangentLFO::calculateNextPhase (const double currentPhase) const
{
    return std::tan (currentPhase);
}

//==============================================================================
TriangleLFO::TriangleLFO()
{
}

double TriangleLFO::calculateNextPhase (const double currentPhase) const
{
    return 1.0 - (4.0 * std::abs (currentPhase - 0.5));
}

//==============================================================================
RampLFO::RampLFO()
{
}

double RampLFO::calculateNextPhase (const double currentPhase) const
{
    return (currentPhase + std::abs (currentPhase)) * 0.5;
}

//==============================================================================
SawLFO::SawLFO()
{
}

double SawLFO::calculateNextPhase (const double currentPhase) const
{
    return (currentPhase * 2.0) - 1.0;
}

//==============================================================================
SquareLFO::SquareLFO()
{
}

double SquareLFO::calculateNextPhase (const double currentPhase) const
{
    return currentPhase > 0.0 ? 1.0 : -1.0;
}

//==============================================================================
WhiteNoiseLFO::WhiteNoiseLFO()
{
}

/*
    http://www.dsprelated.com/showcode/233.php
    http://www.dsprelated.com/showcode/213.php
    http://www.dsprelated.com/showcode/176.php

    http://www.musicdsp.org/archive.php?classid=0#129
*/
double WhiteNoiseLFO::calculateNextPhase (const double currentPhase) const
{
    return (currentPhase * 2.0) - 1.0;
}
