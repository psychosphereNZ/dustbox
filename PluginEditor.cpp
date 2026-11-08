#include "PluginEditor.h"

namespace Col
{
    const juce::Colour body      { 0xff1d1c1a };
    const juce::Colour plate     { 0xffd8cdb4 };
    const juce::Colour plateDark { 0xffb9ad92 };
    const juce::Colour ink       { 0xff2a2724 };
    const juce::Colour orange    { 0xffe8742a };
    const juce::Colour knob      { 0xff2e2c29 };
    const juce::Colour lcdBg     { 0xff1f2c16 };
    const juce::Colour lcdText   { 0xffa6dc5c };
}

//==============================================================================
DustLookAndFeel::DustLookAndFeel()
{
    setColour (juce::Slider::textBoxTextColourId,       Col::ink);
    setColour (juce::Slider::textBoxOutlineColourId,    juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    setColour (juce::Label::textColourId,               Col::ink);
    setColour (juce::ComboBox::textColourId,            Col::lcdText);
    setColour (juce::ComboBox::arrowColourId,           Col::lcdText);
    setColour (juce::PopupMenu::backgroundColourId,     Col::lcdBg);
    setColour (juce::PopupMenu::textColourId,           Col::lcdText);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, Col::lcdText.withAlpha (0.25f));
    setColour (juce::PopupMenu::highlightedTextColourId, Col::lcdText.brighter (0.3f));
}

void DustLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h, float pos,
                                        float startAngle, float endAngle, juce::Slider&)
{
    const auto bounds = juce::Rectangle<int> (x, y, w, h).toFloat().reduced (4.0f);
    const float radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
    const auto c = bounds.getCentre();
    const float angle = startAngle + pos * (endAngle - startAngle);

    // Skalenstriche
    g.setColour (Col::ink.withAlpha (0.55f));
    for (int i = 0; i <= 10; ++i)
    {
        const float a = startAngle + (float) i / 10.0f * (endAngle - startAngle);
        const float r1 = radius * 0.92f, r2 = radius * (i % 5 == 0 ? 1.0f : 0.97f);
        g.drawLine (c.x + r1 * std::sin (a), c.y - r1 * std::cos (a),
                    c.x + r2 * std::sin (a), c.y - r2 * std::cos (a), i % 5 == 0 ? 1.6f : 1.0f);
    }

    // Wert-Bogen
    const float arcR = radius * 0.82f;
    juce::Path track;
    track.addCentredArc (c.x, c.y, arcR, arcR, 0.0f, startAngle, endAngle, true);
    g.setColour (Col::plateDark);
    g.strokePath (track, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    juce::Path val;
    val.addCentredArc (c.x, c.y, arcR, arcR, 0.0f, startAngle, angle, true);
    g.setColour (Col::orange);
    g.strokePath (val, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Knopfkappe mit Schatten
    const float kr = radius * 0.66f;
    g.setColour (juce::Colours::black.withAlpha (0.35f));
    g.fillEllipse (c.x - kr + 1.5f, c.y - kr + 3.0f, kr * 2.0f, kr * 2.0f);

    juce::ColourGradient grad (Col::knob.brighter (0.35f), c.x, c.y - kr,
                               Col::knob.darker (0.4f),     c.x, c.y + kr, false);
    g.setGradientFill (grad);
    g.fillEllipse (c.x - kr, c.y - kr, kr * 2.0f, kr * 2.0f);
    g.setColour (juce::Colours::black.withAlpha (0.6f));
    g.drawEllipse (c.x - kr, c.y - kr, kr * 2.0f, kr * 2.0f, 1.0f);

    // Zeiger
    juce::Path ptr;
    ptr.addRoundedRectangle (-1.6f, -kr * 0.92f, 3.2f, kr * 0.5f, 1.5f);
    g.setColour (Col::orange);
    g.fillPath (ptr, juce::AffineTransform::rotation (angle).translated (c.x, c.y));
}

void DustLookAndFeel::drawComboBox (juce::Graphics& g, int w, int h, bool,
                                    int, int, int, int, juce::ComboBox&)
{
    auto r = juce::Rectangle<float> (0, 0, (float) w, (float) h);
    g.setColour (Col::lcdBg);
    g.fillRoundedRectangle (r, 4.0f);
    g.setColour (juce::Colours::black);
    g.drawRoundedRectangle (r.reduced (0.5f), 4.0f, 1.5f);

    // LCD-Pixelraster
    g.setColour (Col::lcdText.withAlpha (0.05f));
    for (int yy = 2; yy < h - 2; yy += 3)
        g.drawHorizontalLine (yy, 3.0f, (float) w - 3.0f);

    // Pfeil
    juce::Path arrow;
    const float ax = (float) w - 18.0f, ay = (float) h * 0.5f;
    arrow.addTriangle (ax, ay - 3.0f, ax + 8.0f, ay - 3.0f, ax + 4.0f, ay + 3.0f);
    g.setColour (Col::lcdText);
    g.fillPath (arrow);
}

juce::Font DustLookAndFeel::getComboBoxFont (juce::ComboBox&)
{
    return juce::Font (juce::FontOptions (juce::Font::getDefaultMonospacedFontName(), 17.0f, juce::Font::bold));
}

void DustLookAndFeel::positionComboBoxText (juce::ComboBox& box, juce::Label& label)
{
    label.setBounds (10, 1, box.getWidth() - 34, box.getHeight() - 2);
    label.setFont (getComboBoxFont (box));
}

//==============================================================================
DustboxAudioProcessorEditor::DustboxAudioProcessorEditor (DustboxAudioProcessor& p)
    : AudioProcessorEditor (&p), proc (p)
{
    setLookAndFeel (&lnf);

    addKnob (input,  "input",  "INPUT");
    addKnob (drive,  "drive",  "DRIVE");
    addKnob (rate,   "rate",   "RATE");
    addKnob (bits,   "bits",   "BITS");
    addKnob (alias,  "alias",  "ALIAS");
    addKnob (cutoff, "cutoff", "FILTER");
    addKnob (reso,   "reso",   "RESO");
    addKnob (noise,  "noise",  "NOISE");
    addKnob (mix,    "mix",    "MIX");
    addKnob (output, "output", "OUTPUT");

    for (int i = 0; i < proc.getNumPrograms(); ++i)
        presetBox.addItem (juce::String (i + 1).paddedLeft ('0', 2) + "  " + proc.getProgramName (i).toUpperCase(), i + 1);
    presetBox.setSelectedId (proc.getCurrentProgram() + 1, juce::dontSendNotification);
    presetBox.onChange = [this]
    {
        const int idx = presetBox.getSelectedId() - 1;
        if (idx >= 0)
            proc.setCurrentProgram (idx);
    };
    addAndMakeVisible (presetBox);

    setSize (820, 450);
}

DustboxAudioProcessorEditor::~DustboxAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void DustboxAudioProcessorEditor::addKnob (Knob& k, const juce::String& paramId, const juce::String& name)
{
    k.slider.setColour (juce::Slider::textBoxTextColourId,       Col::ink);
    k.slider.setColour (juce::Slider::textBoxOutlineColourId,    juce::Colours::transparentBlack);
    k.slider.setColour (juce::Slider::textBoxBackgroundColourId, Col::plateDark.withAlpha (0.35f));
    k.slider.setColour (juce::Slider::textBoxHighlightColourId,  Col::orange.withAlpha (0.4f));
    k.slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    k.slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 90, 20);
    k.slider.setRotaryParameters (juce::degreesToRadians (225.0f), juce::degreesToRadians (495.0f), true);
    addAndMakeVisible (k.slider);

    k.label.setText (name, juce::dontSendNotification);
    k.label.setJustificationType (juce::Justification::centred);
    k.label.setFont (juce::Font (juce::FontOptions (13.0f, juce::Font::bold)));
    addAndMakeVisible (k.label);

    k.attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (proc.apvts, paramId, k.slider);
}

//==============================================================================
void DustboxAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto full = getLocalBounds().toFloat();

    // Gehäuse
    g.fillAll (Col::body);

    // Frontplatte
    auto plate = full.reduced (12.0f);
    juce::ColourGradient grad (Col::plate.brighter (0.06f), 0, plate.getY(),
                               Col::plate.darker (0.06f),   0, plate.getBottom(), false);
    g.setGradientFill (grad);
    g.fillRoundedRectangle (plate, 6.0f);

    // Kopfleiste
    auto header = plate.removeFromTop (78.0f);
    g.setColour (Col::ink);
    g.fillRoundedRectangle (header.withTrimmedBottom (0).reduced (0, 0), 6.0f);
    g.fillRect (header.withTop (header.getBottom() - 8.0f));
    g.setColour (Col::orange);
    g.fillRect (header.getX(), header.getBottom() - 4.0f, header.getWidth(), 4.0f);

    g.setColour (Col::plate);
    g.setFont (juce::Font (juce::FontOptions (34.0f, juce::Font::bold)));
    g.drawText ("DUSTBOX", (int) header.getX() + 26, (int) header.getY() + 10, 200, 40, juce::Justification::left);
    g.setColour (Col::orange);
    g.drawText ("12", (int) header.getX() + 182, (int) header.getY() + 10, 60, 40, juce::Justification::left);
    g.setColour (Col::plate.withAlpha (0.7f));
    g.setFont (juce::Font (juce::FontOptions (11.5f)));
    g.drawText ("VINTAGE SAMPLER EMULATOR  /  12-BIT CHARACTER UNIT",
                (int) header.getX() + 28, (int) header.getY() + 48, 400, 16, juce::Justification::left);

    // Sektionsrahmen
    auto drawSection = [&g] (juce::Rectangle<int> r, const juce::String& title)
    {
        auto rf = r.toFloat();
        auto t = juce::Rectangle<float> (rf.getX() + 16.0f, rf.getY() - 9.0f,
                                         (float) title.length() * 9.0f + 16.0f, 18.0f);
        {
            juce::Graphics::ScopedSaveState s (g);
            g.excludeClipRegion (t.toNearestInt());
            g.setColour (Col::ink.withAlpha (0.55f));
            g.drawRoundedRectangle (rf, 5.0f, 1.2f);
        }
        g.setColour (Col::ink);
        g.setFont (juce::Font (juce::FontOptions (12.5f, juce::Font::bold)));
        g.drawText (title, t, juce::Justification::centred);
    };
    drawSection (adcArea, "CONVERTER");
    drawSection (outArea, "OUTPUT STAGE");

    // Schrauben
    auto screw = [&g] (float cx, float cy)
    {
        g.setColour (Col::plateDark.darker (0.3f));
        g.fillEllipse (cx - 6, cy - 6, 12, 12);
        g.setColour (Col::ink.withAlpha (0.7f));
        g.drawLine (cx - 4, cy - 4, cx + 4, cy + 4, 1.4f);
    };
    const auto pb = getLocalBounds().toFloat().reduced (12.0f);
    screw (pb.getX() + 14, pb.getBottom() - 14);
    screw (pb.getRight() - 14, pb.getBottom() - 14);

    g.setColour (Col::ink.withAlpha (0.5f));
    g.setFont (juce::Font (juce::FontOptions (10.5f)));
    g.drawText ("G AUDIO", getLocalBounds().withTrimmedBottom (18).removeFromBottom (14),
                juce::Justification::centred);
}

void DustboxAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (12);
    auto header = area.removeFromTop (78);
    presetBox.setBounds (header.removeFromRight (300).reduced (18, 20));

    area.removeFromTop (22);
    area.removeFromBottom (28);
    area.reduce (18, 0);

    const int sectionH = (area.getHeight() - 22) / 2;
    adcArea = area.removeFromTop (sectionH);
    area.removeFromTop (22);
    outArea = area.removeFromTop (sectionH);

    auto layoutRow = [] (juce::Rectangle<int> r, std::initializer_list<Knob*> knobs)
    {
        r = r.reduced (10, 12);
        const int w = r.getWidth() / (int) knobs.size();
        for (auto* k : knobs)
        {
            auto cell = r.removeFromLeft (w);
            k->label.setBounds (cell.removeFromTop (16));
            k->slider.setBounds (cell.reduced (8, 0));
        }
    };
    layoutRow (adcArea, { &input, &drive, &rate, &bits, &alias });
    layoutRow (outArea, { &cutoff, &reso, &noise, &mix, &output });
}
