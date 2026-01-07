#pragma once

#include "PluginProcessor.h"

//==============================================================================
class PluginEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit PluginEditor (LifterProcessor&);
    ~PluginEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    LifterProcessor& processorRef;
    
    // Custom Look and Feel
    punk_dsp::ExamplesLnF myCustomLnF;
    
    // Layout utilities
    juce::TextButton header, params;
    
    // Sliders - Rotary knobs
    juce::Slider ratioSlider, thresSlider, kneeSlider, attackSlider, releaseSlider, makeupSlider, mixSlider;
    
    juce::TextButton feedButton;
    juce::TextButton gaDisplay;
    
    // Attachments for linking sliders-parameters
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ratioAttachment, thresAttachment, kneeAttachment, attackAttachment, releaseAttachment, makeupAttachment, mixAttachment;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> feedAttachment;
    
    void updateToggleButtonText()
    {
        // Check the current toggle state and update text accordingly
        if (feedButton.getToggleState())
            feedButton.setButtonText("Feed-Forward");
        else
            feedButton.setButtonText("Feed-Back");
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
