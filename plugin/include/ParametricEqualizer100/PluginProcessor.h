#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <array>

struct BiquadFilter {
    double b0 = 1.0, b1 = 0.0, b2 = 0.0;
    double a0 = 1.0, a1 = 0.0, a2 = 0.0;

    // State:
    double z1 = 0.0, z2 = 0.0;

    void setCoefficients(
            double b0_, double b1_, double b2_, 
            double a0_, double a1_, double a2_
            ) {
        b0 = b0_; b1 = b1_; b2 = b2_;
        a0 = a0_; a1 = a1_; a2 = a2_;
    }

    void reset() {
        z1 = 0.0;
        z2 = 0.0;
    }

    float processSample(float x) {
        // Direct Form II implementation
        double w = x - a1/a0*z1 - a2/a0*z2;
        double y = b0/a0*w + b1/a0*z1 + b2/a0*z2;
        z2 = z1;
        z1 = w;
        return (float) y;
    }
};

//=============================================================================
class AudioPluginAudioProcessor final : public juce::AudioProcessor {
public:
    //=========================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    // Audio Processor Overrides
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //Program Handling
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    // Processing
    using AudioProcessor::processBlock;
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    // State
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Create Editor
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    juce::AudioProcessorValueTreeState& getValueTreeState() { return apvts; }

private:

    // Parameter Layout
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Filter calculation helper methods
    static std::array<double,6> makeLowPass(double sampleRate, double freq, double Q);
    static std::array<double,6> makeHighPass(double sampleRate, double freq, double Q);
    static std::array<double,6> makePeaking(double sampleRate, double freq, double Q, double dBgain);
    static std::array<double,6> makeLowShelf(double sampleRate, double freq, double Q, double dBgain);
    static std::array<double,6> makeHighShelf(double sampleRate, double freq, double Q, double dBgain);

    // Our Filters
    BiquadFilter highPassFilter;
    BiquadFilter bell1Filter;
    BiquadFilter bell2Filter;
    BiquadFilter bell3Filter;
    BiquadFilter lowPassFilter;

    // Parameter State
    juce::AudioProcessorValueTreeState apvts;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};

