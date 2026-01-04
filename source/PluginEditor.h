#pragma once

#include "PluginProcessor.h"

namespace UIConstants
{
// 5-color palette
const juce::Colour background    = juce::Colour(0xff1B3C53);
const juce::Colour primary       = juce::Colour(0xffDFD0B8);
const juce::Colour secondary     = juce::Colour(0xff456882);
const juce::Colour text          = juce::Colour(0xffE0D9D9);
const juce::Colour highlight     = juce::Colour(0xffD3DAD9);

// Sizing constants
const int knobSize = 80;
const int margin = 20;
}

//==============================================================================
class PluginEditor : public juce::AudioProcessorEditor
{
public:
    explicit PluginEditor (LifterProcessor&);
    ~PluginEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    LifterProcessor& processorRef;
    
    // Custom Look and Feel
    // punk_dsp::ExamplesLnF myCustomLnF;
    
    // Layout utilities
    juce::TextButton header, params;
    
    // Sliders - Rotary knobs
    juce::Slider ratioSlider, thresSlider, kneeSlider, attackSlider, releaseSlider, makeupSlider, mixSlider;
    
    juce::TextButton feedButton { "FeedForward" };
    juce::TextButton grDisplay { "GA (dB)" };
        
    // Attachments for linking sliders-parameters
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ratioAttachment, thresAttachment, kneeAttachment, attackAttachment, releaseAttachment, makeupAttachment, mixAttachment;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> feedAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
