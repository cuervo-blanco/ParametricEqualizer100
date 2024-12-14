#pragma once

// #include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
class AudioPluginAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    AudioPluginAudioProcessor& processorRef;

    // High Pass
    juce::Slider hpFreqSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> hpFreqAttachment;

    // Low Pass
    juce::Slider lopFreqSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lopFreqAttachment;

    // Bell 1
    juce::Slider bell1FreqSlider, bell1GainSlider, bell1QSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bell1FreqAttachment, bell1GainAttachment, bell1QAttachment;

    // Bell 2
    juce::Slider bell2FreqSlider, bell2GainSlider, bell2QSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bell2FreqAttachment, bell2GainAttachment, bell2QAttachment;

    // Bell 3
    juce::Slider bell3FreqSlider, bell3GainSlider, bell3QSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bell3FreqAttachment, bell3GainAttachment, bell3QAttachment;

    // Shelf Mode Toggles
    juce::ToggleButton isLowShelfModeButton, isHighShelfModeButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> isLowShelfModeAttachment, isHighShelfModeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};

