#pragma once

#include "PluginProcessor.h"

//==============================================================================
// Eigener Skin: cremefarbene Frontplatte wie bei alten Rack-Samplern,
// dunkle Knöpfe mit orangefarbenem Zeiger, grünes LCD für die Presets.
class DustLookAndFeel : public juce::LookAndFeel_V4
{
public:
    DustLookAndFeel();
    void drawRotarySlider (juce::Graphics&, int x, int y, int w, int h, float pos,
                           float startAngle, float endAngle, juce::Slider&) override;
    void drawComboBox (juce::Graphics&, int w, int h, bool isDown,
                       int bx, int by, int bw, int bh, juce::ComboBox&) override;
    juce::Font getComboBoxFont (juce::ComboBox&) override;
    void positionComboBoxText (juce::ComboBox&, juce::Label&) override;
};

//==============================================================================
class DustboxAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit DustboxAudioProcessorEditor (DustboxAudioProcessor&);
    ~DustboxAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    struct Knob
    {
        juce::Slider slider;
        juce::Label label;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };

    void addKnob (Knob&, const juce::String& paramId, const juce::String& name);

    DustboxAudioProcessor& proc;
    DustLookAndFeel lnf;

    Knob input, drive, rate, bits, alias;      // Sektion "CONVERTER"
    Knob cutoff, reso, noise, mix, output;     // Sektion "OUTPUT"
    juce::ComboBox presetBox;

    juce::Rectangle<int> adcArea, outArea, lcdArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DustboxAudioProcessorEditor)
};
