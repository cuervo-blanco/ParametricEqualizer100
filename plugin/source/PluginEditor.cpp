#include "ParametricEqualizer100/PluginProcessor.h"
#include "ParametricEqualizer100/PluginEditor.h"



//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);
    setSize (800, 500);

    // Helper lambda to configure a rotary slider with a text box below.
    auto configureSlider = [&](juce::Slider& slider) {
        slider.setSliderStyle(juce::Slider::Rotary);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        addAndMakeVisible(slider);
    };

    // High Pass Frequency
    configureSlider(hpFreqSlider);
    hpFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.getValueTreeState(), "HPFREQ", hpFreqSlider);

    // Low Pass Frequency
    configureSlider(lopFreqSlider);
    lopFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.getValueTreeState(), "LPFREQ", lopFreqSlider);

    // Bell 1
    configureSlider(bell1FreqSlider);
    bell1FreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.getValueTreeState(), "BELL1FREQ", bell1FreqSlider);

    configureSlider(bell1GainSlider);
    bell1GainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.getValueTreeState(), "BELL1GAIN", bell1GainSlider);

    configureSlider(bell1QSlider);
    bell1QAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.getValueTreeState(), "BELL1Q", bell1QSlider);

    // Bell 2
    configureSlider(bell2FreqSlider);
    bell2FreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.getValueTreeState(), "BELL2FREQ", bell2FreqSlider);

    configureSlider(bell2GainSlider);
    bell2GainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.getValueTreeState(), "BELL2GAIN", bell2GainSlider);

    configureSlider(bell2QSlider);
    bell2QAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.getValueTreeState(), "BELL2Q", bell2QSlider);

    // Bell 3
    configureSlider(bell3FreqSlider);
    bell3FreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.getValueTreeState(), "BELL3FREQ", bell3FreqSlider);

    configureSlider(bell3GainSlider);
    bell3GainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.getValueTreeState(), "BELL3GAIN", bell3GainSlider);

    configureSlider(bell3QSlider);
    bell3QAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.getValueTreeState(), "BELL3Q", bell3QSlider);

    // Shelf Mode Toggles
    addAndMakeVisible(isLowShelfModeButton);
    isLowShelfModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processorRef.getValueTreeState(), "ISLOWSHELFMODE", isLowShelfModeButton);
    isLowShelfModeButton.setButtonText("Low Shelf Mode");

    addAndMakeVisible(isHighShelfModeButton);
    isHighShelfModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processorRef.getValueTreeState(), "ISHIGHSHELFMODE", isHighShelfModeButton);
    isHighShelfModeButton.setButtonText("High Shelf Mode");}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor() {
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::darkgrey);

    g.setColour(juce::Colours::white);
    g.setFont(15.0f);

    // Labels for sections
    g.drawFittedText(
            "High Pass", 
            hpFreqSlider.getX(), 
            hpFreqSlider.getY()-30, 
            hpFreqSlider.getWidth(), 
            20, 
            juce::Justification::centred,
            1 );
    g.drawFittedText(
            "Low Pass", 
            lopFreqSlider.getX(), 
            lopFreqSlider.getY()-30, 
            lopFreqSlider.getWidth(), 
            20, 
            juce::Justification::centred, 
            1 );

    g.drawFittedText(
            "Bell 1", 
            bell1FreqSlider.getX(), 
            bell1FreqSlider.getY()-30, 
            bell1FreqSlider.getWidth()*3,
            20,
            juce::Justification::centred,
            1);
    g.drawFittedText(
            "Bell 2", 
            bell2FreqSlider.getX(), 
            bell2FreqSlider.getY()-30, 
            bell2FreqSlider.getWidth()*3, 
            20, 
            juce::Justification::centred,
            1);
    g.drawFittedText(
            "Bell 3", 
            bell3FreqSlider.getX(), 
            bell3FreqSlider.getY()-30,
            bell3FreqSlider.getWidth()*3,
            20,
            juce::Justification::centred, 
            1);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // We'll divide the layout into rows and columns for clarity.

    // Top row: HP and LP
    // Second row: Bell1 (3 sliders)
    // Third row: Bell2 (3 sliders)
    // Fourth row: Bell3 (3 sliders)
    // Bottom row: Shelf toggles

    auto area = getLocalBounds().reduced(10);
    auto rowHeight = area.getHeight() / 5;

    // First row (HP/LP)
    auto firstRow = area.removeFromTop(rowHeight);
    hpFreqSlider.setBounds(firstRow.removeFromLeft(firstRow.getWidth()/2).withSizeKeepingCentre(100,100));
    lopFreqSlider.setBounds(firstRow.withSizeKeepingCentre(100,100));

    // Helper to layout three sliders in a row evenly
    auto layoutThreeSliders = [&](juce::Rectangle<int> rowArea, juce::Slider& s1, juce::Slider& s2, juce::Slider& s3) {
        int sliderWidth = 80;
        int spacing = (rowArea.getWidth() - 3 * sliderWidth) / 4;
        int yCenter = rowArea.getCentreY();
        
        auto r = rowArea;
        auto s1X = r.getX() + spacing;
        auto s2X = s1X + sliderWidth + spacing;
        auto s3X = s2X + sliderWidth + spacing;

        s1.setBounds(s1X, yCenter - 50, sliderWidth, 100);
        s2.setBounds(s2X, yCenter - 50, sliderWidth, 100);
        s3.setBounds(s3X, yCenter - 50, sliderWidth, 100);
    };

    // Bell 1 row
    auto bell1Row = area.removeFromTop(rowHeight);
    layoutThreeSliders(bell1Row, bell1FreqSlider, bell1GainSlider, bell1QSlider);

    // Bell 2 row
    auto bell2Row = area.removeFromTop(rowHeight);
    layoutThreeSliders(bell2Row, bell2FreqSlider, bell2GainSlider, bell2QSlider);

    // Bell 3 row
    auto bell3Row = area.removeFromTop(rowHeight);
    layoutThreeSliders(bell3Row, bell3FreqSlider, bell3GainSlider, bell3QSlider);

    // Shelf mode row
    auto shelfRow = area.removeFromTop(rowHeight);
    isLowShelfModeButton.setBounds(shelfRow.removeFromLeft(shelfRow.getWidth()/2).withSizeKeepingCentre(120, 24));
    isHighShelfModeButton.setBounds(shelfRow.withSizeKeepingCentre(120, 24));
}

