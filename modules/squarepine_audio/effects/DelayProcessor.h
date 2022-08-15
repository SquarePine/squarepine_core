//
//  Fractional delay class
//
//  Created by Eric Tarr on 2/6/20.
//  Copyright © 2020 Eric Tarr. All rights reserved.
//

class FractionalDelay {
    
public:
    
    FractionalDelay();
    
    // Destructor
    ~FractionalDelay();
    
    float processSample(float x,int channel);

    void setFs(float _Fs);
    
    void setDelaySamples(float _delay);

    
private:
    
    float Fs = 48000.f;
    
    float delay = 5.f;
    float smoothDelay[2] = {5.f};
    
    static const int MAX_BUFFER_SIZE = 192000;
    float delayBuffer[MAX_BUFFER_SIZE][2] = {{0.0f}};
    int index[2] = {0};

};



//This is a wrapper around Eric Tarr's Fractional Delay class that can be integrated with Juce/Squarepine processors

class DelayProcessor final : public BandProcessor
{
public:
    //Constructor with ID
    DelayProcessor (int idNum = 1);
    ~DelayProcessor()override;

    //============================================================================== Audio processing
    void prepareToPlay (double Fs, int bufferSize) override;
    void processAudioBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&) override;
    //============================================================================== House keeping
    const String getName() const override;
    /** @internal */
    Identifier getIdentifier() const override;
    /** @internal */
    bool supportsDoublePrecisionProcessing() const override;
    //============================================================================== Parameter callbacks
    void parameterValueChanged (int paramNum, float value) override;
    void parameterGestureChanged (int, bool) override{}

private:
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    NotifiableAudioParameterFloat* wetDryParam = nullptr;
    NotifiableAudioParameterFloat* delayTimeParam = nullptr;
    NotifiableAudioParameterFloat* beatParam = nullptr;
    NotifiableAudioParameterFloat* xPadParam = nullptr;
    NotifiableAudioParameterFloat* fxOnParam = nullptr;

    SmoothedValue<float, ValueSmoothingTypes::Linear> wetDry { 0.0f };
    SmoothedValue<float, ValueSmoothingTypes::Linear> delayTime{ 0.0f };

    int idNumber = 1;

    FractionalDelay delayUnit;
};
