#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <juce_audio_processors/juce_audio_processors.h>


struct BiquadFilter
{
    double b0 = 1.0, b1 = 0.0, b2 = 0.0;
    double a0 = 1.0, a1 = 0.0, a2 = 0.0;

    // State:
    double z1 = 0.0, z2 = 0.0;

    void setCoefficients(
            double b0_, 
            double b1_, 
            double b2_, 
            double a0_, 
            double a1_, 
            double a2_)
    {
        b0 = b0_; b1 = b1_; b2 = b2_;
        a0 = a0_; a1 = a1_; a2 = a2_;
    }

    void reset()
    {
        z1 = 0.0;
        z2 = 0.0;
    }

    float processSample(float x)
    {
        // Direct Form II implementation
        double w = x - a1/a0*z1 - a2/a0*z2;
        double y = b0/a0*w + b1/a0*z1 + b2/a0*z2;
        z2 = z1;
        z1 = w;
        return (float) y;
    }
};


//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
     apvts(*this, nullptr, "PARAMS", createParameterLayout())
{
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

// Parameter Layout
juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // High Pass
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
                "HPFREQ",   // ID
                "High Pass Frequency", // name
                juce::NormalisableRange<float>(20.0f, 20000.0f, 0.01f, 0.5f),
                                                                 30.0f)); // default value

    // Low Pass
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
                "LPFREQ", 
                "Low Pass Frequency",
                juce::NormalisableRange<float>(20.0f, 20000.0f, 0.01f, 0.5f),
                                                                 18000.0f));

    // Bell 1
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
                "BELL1FREQ", 
                "Bell1 Frequency",
                juce::NormalisableRange<float>(20.0f, 20000.0f, 0.01f, 0.5f),
                                                                 200.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
                "BELL1GAIN", 
                "Bell1 Gain (dB)",
                juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f),
                                                             0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
                "BELL1Q", 
                "Bell1 Q",
                juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f),
                                                            0.707f));

    // Bell 2
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
                "BELL2FREQ", 
                "Bell2 Frequency", 
                juce::NormalisableRange<float>(20.0f, 20000.0f, 0.01f, 0.5f), 1000.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
                "BELL2GAIN", 
                "Bell2 Gain (dB)", 
                juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f), 3.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
                "BELL2Q",    
                "Bell2 Q", 
                juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f), 0.707f));

    // Bell 3
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
                "BELL3FREQ", 
                "Bell3 Frequency", 
                juce::NormalisableRange<float>(20.0f, 20000.0f, 0.01f, 0.5f), 5000.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
                "BELL3GAIN", 
                "Bell3 Gain (dB)", 
                juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f), -2.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
                "BELL3Q",    
                "Bell3 Q", 
                juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f), 0.707f));

    // Boolean parameters for shelf modes
    params.push_back(std::make_unique<juce::AudioParameterBool>(
                "ISLOWSHELFMODE", 
                "Is Low Shelf Mode", 
                false));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
                "ISHIGHSHELFMODE",
                "Is High Shelf Mode", 
                false));

    return { params.begin(), params.end() };
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

//==============================================================================
// Program Stuff (Old School Presets)

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (samplesPerBlock);

    highPassFilter.reset();
    bell1Filter.reset();
    bell2Filter.reset();
    bell3Filter.reset();
    lowPassFilter.reset();

    double highPassFreq = *apvts.getRawParameterValue("HPFREQ");
    double lowPassFreq = *apvts.getRawParameterValue("LPFREQ");

    double bell1Freq = *apvts.getRawParameterValue("BELL1FREQ");
    double bell1Gain = *apvts.getRawParameterValue("BELL1GAIN");
    double bell1Q = *apvts.getRawParameterValue("BELL1Q");

    double bell2Freq = *apvts.getRawParameterValue("BELL2FREQ");
    double bell2Gain = *apvts.getRawParameterValue("BELL2GAIN");
    double bell2Q = *apvts.getRawParameterValue("BELL2Q");
    
    double bell3Freq = *apvts.getRawParameterValue("BELL3FREQ");
    double bell3Gain = *apvts.getRawParameterValue("BELL3GAIN");
    double bell3Q = *apvts.getRawParameterValue("BELL3Q");

    bool isLowShelfMode = *apvts.getRawParameterValue("ISLOWSHELFMODE") > 0.5f;
    bool isHighShelfMode = *apvts.getRawParameterValue("ISHIGHSHELFMODE") > 0.5f;

    // Using the EQ Cookbook formulas:
    auto highPassCoefs = makeHighPass(sampleRate, highPassFreq, 0.707); // Q=0.707 as an example
    highPassFilter.setCoefficients(highPassCoefs[0], highPassCoefs[1], highPassCoefs[2], 
                                   highPassCoefs[3], highPassCoefs[4], highPassCoefs[5]);


    // Choose between the Low Shelf and the bell 3
    if (isLowShelfMode)
    {
        auto lowShelfCoefs = makeLowShelf(sampleRate, bell1Freq, bell1Q, bell1Gain);
        bell2Filter.setCoefficients(lowShelfCoefs[0], lowShelfCoefs[1], lowShelfCoefs[2],
                                    lowShelfCoefs[3], lowShelfCoefs[4], lowShelfCoefs[5]);
    }
    else
    {
        auto bell1Coefs = makePeaking(sampleRate, bell1Freq, bell1Q, bell1Gain);
        bell1Filter.setCoefficients(bell1Coefs[0], bell1Coefs[1], bell1Coefs[2],
                bell1Coefs[3], bell1Coefs[4], bell1Coefs[5]);
    }

    auto bell2Coefs = makePeaking(sampleRate, bell2Freq, bell2Q, bell2Gain);
    bell2Filter.setCoefficients(bell2Coefs[0], bell2Coefs[1], bell2Coefs[2],
                                bell2Coefs[3], bell2Coefs[4], bell2Coefs[5]);

    // Choose between the High Shelf and the bell 3
    if (isHighShelfMode)
    {
        auto highShelfCoefs = makeHighShelf(sampleRate, bell3Freq, bell3Q, bell3Gain);
        bell2Filter.setCoefficients(highShelfCoefs[0], highShelfCoefs[1], highShelfCoefs[2],
                                    highShelfCoefs[3], highShelfCoefs[4], highShelfCoefs[5]);
    }
    else
    {
        auto bell3Coefs = makePeaking(sampleRate, bell3Freq, bell3Q, bell3Gain);
        bell3Filter.setCoefficients(bell3Coefs[0], bell3Coefs[1], bell3Coefs[2],
                bell3Coefs[3], bell3Coefs[4], bell3Coefs[5]);
    }

    auto lowPassCoefs = makeLowPass(sampleRate, lowPassFreq, 0.707);
    lowPassFilter.setCoefficients(lowPassCoefs[0], lowPassCoefs[1], lowPassCoefs[2],
                                  lowPassCoefs[3], lowPassCoefs[4], lowPassCoefs[5]);


}

std::array<double,6> AudioPluginAudioProcessor::makeLowPass(
        double sampleRate, double freq, double Q)
{
    double w0 = 2.0 * juce::MathConstants<double>::pi * (freq / sampleRate);
    double alpha = std::sin(w0)/(2.0*Q);

    double cosw0 = std::cos(w0);

    double b0 =  (1.0 - cosw0)*0.5;
    double b1 =   1.0 - cosw0;
    double b2 =  (1.0 - cosw0)*0.5;
    double a0 =   1.0 + alpha;
    double a1 =  -2.0*cosw0;
    double a2 =   1.0 - alpha;

    return { b0, b1, b2, a0, a1, a2 };
}

std::array<double,6> AudioPluginAudioProcessor::makeHighPass(
        double sampleRate, double freq, double Q)
{
    double w0 = 2.0 * juce::MathConstants<double>::pi * (freq / sampleRate);
    double alpha = std::sin(w0)/(2.0*Q);
    double cosw0 = std::cos(w0);

    double b0 =  (1.0 + cosw0)*0.5;
    double b1 = -(1.0 + cosw0);
    double b2 =  (1.0 + cosw0)*0.5;
    double a0 =   1.0 + alpha;
    double a1 =  -2.0*cosw0;
    double a2 =   1.0 - alpha;

    return { b0, b1, b2, a0, a1, a2 };
}

std::array<double,6> AudioPluginAudioProcessor::makePeaking(
        double sampleRate, double freq, double Q, double dBgain)
{
    double A = std::pow(10.0, dBgain / 40.0);
    double w0 = 2.0 * juce::MathConstants<double>::pi * (freq / sampleRate);
    double alpha = std::sin(w0)/(2.0*Q);

    double cosw0 = std::cos(w0);

    double b0 = 1.0 + alpha*A;
    double b1 = -2.0*cosw0;
    double b2 = 1.0 - alpha*A;
    double a0 = 1.0 + alpha/A;
    double a1 = -2.0*cosw0;
    double a2 = 1.0 - alpha/A;

    return { b0, b1, b2, a0, a1, a2 };
}


std::array<double,6> AudioPluginAudioProcessor::makeLowShelf(
        double sampleRate, double freq, double Q, double dBgain)
{
    double A = std::pow(10.0, dBgain / 40.0);
    double w0 = 2.0 * juce::MathConstants<double>::pi * (freq / sampleRate);
    double alpha = std::sin(w0) / 2.0 * std::sqrt( (A + 1.0/A)*(1.0/Q - 1.0) + 2.0 );
    double cosw0 = std::cos(w0);

    double b0 = A * ( (A+1.0) - (A-1.0)*cosw0 + 2.0*std::sqrt(A)*alpha );
    double b1 = 2.0*A * ( (A-1.0) - (A+1.0)*cosw0 );
    double b2 = A * ( (A+1.0) - (A-1.0)*cosw0 - 2.0*std::sqrt(A)*alpha );
    double a0 = (A+1.0) + (A-1.0)*cosw0 + 2.0*std::sqrt(A)*alpha;
    double a1 = -2.0 * ( (A-1.0) + (A+1.0)*cosw0 );
    double a2 = (A+1.0) + (A-1.0)*cosw0 - 2.0*std::sqrt(A)*alpha;

    return {b0, b1, b2, a0, a1, a2};
}

std::array<double,6> AudioPluginAudioProcessor::makeHighShelf(
        double sampleRate, double freq, double Q, double dBgain)
{
    double A = std::pow(10.0, dBgain / 40.0);
    double w0 = 2.0 * juce::MathConstants<double>::pi * (freq / sampleRate);
    double alpha = std::sin(w0) / 2.0 * std::sqrt( (A + 1.0/A)*(1.0/Q - 1.0) + 2.0 );
    double cosw0 = std::cos(w0);

    double b0 = A*( (A+1.0) + (A-1.0)*cosw0 + 2.0*std::sqrt(A)*alpha );
    double b1 = -2.0*A * ( (A-1.0) + (A+1.0)*cosw0 );
    double b2 = A*( (A+1.0) + (A-1.0)*cosw0 - 2.0*std::sqrt(A)*alpha );
    double a0 = (A+1.0) - (A-1.0)*cosw0 + 2.0*std::sqrt(A)*alpha;
    double a1 = 2.0*( (A-1.0) - (A+1.0)*cosw0 );
    double a2 = (A+1.0) - (A-1.0)*cosw0 - 2.0*std::sqrt(A)*alpha;

    return {b0, b1, b2, a0, a1, a2};
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock (
        juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear extra channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Assuming stereo processing. If mono, just one channel loop.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer (channel);

        for (int n = 0; n < buffer.getNumSamples(); ++n)
        {
            float sample = channelData[n];

            // Pass through filters:
            sample = highPassFilter.processSample(sample);
            sample = bell1Filter.processSample(sample);
            sample = bell2Filter.processSample(sample);
            sample = bell3Filter.processSample(sample);
            sample = lowPassFilter.processSample(sample);

            channelData[n] = sample;
        }
    }
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
