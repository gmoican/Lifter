#include "PluginEditor.h"

PluginEditor::PluginEditor (LifterProcessor& p)
    : AudioProcessorEditor (&p),
      processorRef (p)
{
    juce::ignoreUnused (processorRef);
    juce::LookAndFeel::setDefaultLookAndFeel(&myCustomLnF);
    
    // --- LAYOUT ---
    header.setColour (juce::TextButton::buttonColourId, punk_dsp::UIConstants::background.brighter(0.5f)
                                                                                         .withAlpha(0.25f)
                      );
    header.setEnabled(false);
    header.setButtonText ("Punk DSP - Lifter");
    addAndMakeVisible (header);
    
    params.setColour (juce::TextButton::buttonColourId, punk_dsp::UIConstants::background.brighter(0.5f)
                                                                                         .withAlpha(0.25f)
                      );
    params.setEnabled(false);
    addAndMakeVisible (params);
    
    // Ratio knob
    ratioSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    ratioSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    ratioSlider.setRange(Parameters::ratioMin, Parameters::ratioMax, 0.1);
    ratioSlider.setValue(Parameters::ratioDefault);
    ratioSlider.setName("Ratio");
    addAndMakeVisible(ratioSlider);
    
    ratioAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::ratioId, ratioSlider);
    
    // Thres knob
    thresSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    thresSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    thresSlider.setRange(Parameters::thresMin, Parameters::thresMax, 0.1);
    thresSlider.setValue(Parameters::thresDefault);
    thresSlider.setName("Thres");
    addAndMakeVisible(thresSlider);
    
    thresAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::thresId, thresSlider);
    
    // Knee knob
    kneeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    kneeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    kneeSlider.setRange(Parameters::kneeMin, Parameters::kneeMax, 0.1);
    kneeSlider.setValue(Parameters::kneeDefault);
    kneeSlider.setName("Knee");
    addAndMakeVisible(kneeSlider);
    
    kneeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::kneeId, kneeSlider);

    // Attack knob
    attackSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    attackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    attackSlider.setRange(Parameters::attackMin, Parameters::attackMax, 0.1);
    attackSlider.setValue(Parameters::attackDefault);
    attackSlider.setName("Att");
    addAndMakeVisible(attackSlider);
    
    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::attackId, attackSlider);

    // Release knob
    releaseSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    releaseSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    releaseSlider.setRange(Parameters::releaseMin, Parameters::releaseMax, 0.1);
    releaseSlider.setValue(Parameters::releaseDefault);
    releaseSlider.setName("Rel");
    addAndMakeVisible(releaseSlider);
    
    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::releaseId, releaseSlider);

    // Makeup knob
    makeupSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    makeupSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    makeupSlider.setRange(Parameters::makeupMin, Parameters::makeupMax, 0.1);
    makeupSlider.setValue(Parameters::makeupDefault);
    makeupSlider.setName("Makeup");
    addAndMakeVisible(makeupSlider);
    
    makeupAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::makeupId, makeupSlider);
    
    // Mix knob
    mixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    mixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mixSlider.setRange(Parameters::mixMin, Parameters::mixMax, 0.01);
    mixSlider.setValue(Parameters::mixDefault);
    mixSlider.setName("Mix");
    addAndMakeVisible(mixSlider);
    
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::mixId, mixSlider);

    // Topology button
    feedButton.setClickingTogglesState(true);
    feedButton.onClick = [this]() { updateToggleButtonText(); };
    updateToggleButtonText();
    addAndMakeVisible(feedButton);
    
    feedAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processorRef.apvts, Parameters::feedId, feedButton);
    
    // Gain Reduction Display
    gaDisplay.setClickingTogglesState(false);
    addAndMakeVisible(gaDisplay);
    gaDisplay.setButtonText ("GA: 0.0 dB");
    startTimer(50);
    
    // Sizing calculations
    const int numCols = 3;
    const int numRows = 3;
    
    const int totalWidth = (numCols * (punk_dsp::UIConstants::knobSize + 2 * punk_dsp::UIConstants::margin)) + (10 * 2);
    const int totalHeight = punk_dsp::UIConstants::headerHeight + (numRows * (punk_dsp::UIConstants::knobSize + 2 * punk_dsp::UIConstants::margin)) + (10 * 2);
    
    setSize (totalWidth, totalHeight);
}

PluginEditor::~PluginEditor()
{
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
}

void PluginEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (punk_dsp::UIConstants::background);
}

void PluginEditor::timerCallback()
{
    float gaValue = processorRef.sendGainAddition();
    gaDisplay.setButtonText ("GA: " + juce::String (gaValue, 1) + " dB");
}

void PluginEditor::resized()
{
    // layout the positions of your child components here
    auto area = getLocalBounds();
    
    // --- LAYOUT SETUP ---
    auto headerArea = area.removeFromTop( 30 );
    auto paramsArea = area.reduced( 10 );
    
    header.setBounds(headerArea);
    params.setBounds(paramsArea);
    
    // --- PARAMS LAYOUT ---
    juce::FlexBox fb;
    fb.flexDirection = juce::FlexBox::Direction::row;
    fb.flexWrap = juce::FlexBox::Wrap::wrap;
    fb.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
    fb.alignContent = juce::FlexBox::AlignContent::spaceBetween;
    
    // Add sliders to the FlexBox
    fb.items.add(juce::FlexItem(ratioSlider).withMinWidth(punk_dsp::UIConstants::knobSize)
                                            .withMinHeight(punk_dsp::UIConstants::knobSize)
                                            .withMargin(punk_dsp::UIConstants::margin));
    fb.items.add(juce::FlexItem(thresSlider).withMinWidth(punk_dsp::UIConstants::knobSize)
                                            .withMinHeight(punk_dsp::UIConstants::knobSize)
                                            .withMargin(punk_dsp::UIConstants::margin));
    fb.items.add(juce::FlexItem(kneeSlider).withMinWidth(punk_dsp::UIConstants::knobSize)
                                           .withMinHeight(punk_dsp::UIConstants::knobSize)
                                           .withMargin(punk_dsp::UIConstants::margin));
    
    fb.items.add(juce::FlexItem(attackSlider).withMinWidth(punk_dsp::UIConstants::knobSize)
                                             .withMinHeight(punk_dsp::UIConstants::knobSize)
                                             .withMargin(punk_dsp::UIConstants::margin));
    fb.items.add(juce::FlexItem(releaseSlider).withMinWidth(punk_dsp::UIConstants::knobSize)
                                              .withMinHeight(punk_dsp::UIConstants::knobSize)
                                              .withMargin(punk_dsp::UIConstants::margin));
    fb.items.add(juce::FlexItem(makeupSlider).withMinWidth(punk_dsp::UIConstants::knobSize)
                                             .withMinHeight(punk_dsp::UIConstants::knobSize)
                                             .withMargin(punk_dsp::UIConstants::margin));
    
    fb.items.add(juce::FlexItem(feedButton).withMinWidth(punk_dsp::UIConstants::knobSize)
                                           .withMinHeight(punk_dsp::UIConstants::knobSize)
                                           .withMargin(punk_dsp::UIConstants::margin));
    fb.items.add(juce::FlexItem(mixSlider).withMinWidth(punk_dsp::UIConstants::knobSize)
                                          .withMinHeight(punk_dsp::UIConstants::knobSize)
                                          .withMargin(punk_dsp::UIConstants::margin));
    fb.items.add(juce::FlexItem(gaDisplay).withMinWidth(punk_dsp::UIConstants::knobSize)
                                          .withMinHeight(punk_dsp::UIConstants::knobSize)
                                          .withMargin(punk_dsp::UIConstants::margin));
    
    // Perform the layout
    fb.performLayout(paramsArea);
}
