#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
    // Werkspresets – Namen bewusst ohne Hardware-Markennamen
    struct Preset
    {
        const char* name;
        float input, drive, rate, bits, alias, cutoff, reso, noise, mix, output;
    };

    const Preset presets[] =
    {
        //  name              in    drive  rate     bits  alias cutoff   reso  noise mix   out
        { "Classic Sixty",    0.f,  0.20f, 40000.f, 12.f, 0.15f, 14000.f, 0.10f, 0.10f, 1.0f,  0.f },
        { "Twelve Hundred",   0.f,  0.35f, 26040.f, 12.f, 1.00f,  9000.f, 0.25f, 0.15f, 1.0f, -1.f },
        { "Nine-Fifty",       0.f,  0.25f, 32000.f, 12.f, 0.05f,  7000.f, 0.15f, 0.08f, 1.0f,  0.f },
        { "Dusty Loop",       2.f,  0.40f, 22050.f, 10.f, 0.60f,  5000.f, 0.20f, 0.30f, 1.0f, -2.f },
        { "Toy Keys",         0.f,  0.50f,  8000.f,  8.f, 1.00f,  4000.f, 0.30f, 0.25f, 1.0f, -2.f },
        { "Radio Ghost",      0.f,  0.60f,  6000.f,  6.f, 0.30f,  2500.f, 0.50f, 0.50f, 0.8f,  0.f },
        { "Bit Destroyer",    0.f,  0.70f,  4000.f,  4.f, 1.00f, 20000.f, 0.00f, 0.00f, 1.0f, -6.f },
        { "Init (Clean)",     0.f,  0.00f, 48000.f, 16.f, 0.00f, 20000.f, 0.00f, 0.00f, 1.0f,  0.f },
    };
    constexpr int numPresets = (int) (sizeof (presets) / sizeof (presets[0]));

    juce::String hz (float v)
    {
        return v >= 1000.f ? juce::String (v / 1000.f, 1) + " kHz"
                           : juce::String ((int) v) + " Hz";
    }
    juce::String pct (float v) { return juce::String (juce::roundToInt (v * 100.f)) + " %"; }
    juce::String db  (float v) { return (v > 0 ? "+" : "") + juce::String (v, 1) + " dB"; }
}

//==============================================================================
DustboxAudioProcessor::DustboxAudioProcessor()
    : AudioProcessor (BusesProperties()
                        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMS", createLayout())
{
    pInput  = apvts.getRawParameterValue ("input");
    pDrive  = apvts.getRawParameterValue ("drive");
    pRate   = apvts.getRawParameterValue ("rate");
    pBits   = apvts.getRawParameterValue ("bits");
    pAlias  = apvts.getRawParameterValue ("alias");
    pCutoff = apvts.getRawParameterValue ("cutoff");
    pReso   = apvts.getRawParameterValue ("reso");
    pNoise  = apvts.getRawParameterValue ("noise");
    pMix    = apvts.getRawParameterValue ("mix");
    pOutput = apvts.getRawParameterValue ("output");
}

juce::AudioProcessorValueTreeState::ParameterLayout DustboxAudioProcessor::createLayout()
{
    using P = juce::AudioParameterFloat;
    using A = juce::AudioParameterFloatAttributes;
    juce::AudioProcessorValueTreeState::ParameterLayout l;

    auto dbAttr  = A().withStringFromValueFunction ([] (float v, int) { return db (v); });
    auto pctAttr = A().withStringFromValueFunction ([] (float v, int) { return pct (v); });
    auto hzAttr  = A().withStringFromValueFunction ([] (float v, int) { return hz (v); });

    const auto& d = presets[0];

    l.add (std::make_unique<P> (juce::ParameterID { "input", 1 }, "Input",
                                juce::NormalisableRange<float> (-24.f, 24.f, 0.1f), d.input, dbAttr));
    l.add (std::make_unique<P> (juce::ParameterID { "drive", 1 }, "Drive",
                                juce::NormalisableRange<float> (0.f, 1.f, 0.001f), d.drive, pctAttr));
    l.add (std::make_unique<P> (juce::ParameterID { "rate", 1 }, "Sample Rate",
                                juce::NormalisableRange<float> (2000.f, 48000.f, 1.f, 0.45f), d.rate, hzAttr));
    l.add (std::make_unique<P> (juce::ParameterID { "bits", 1 }, "Bits",
                                juce::NormalisableRange<float> (4.f, 16.f, 0.01f), d.bits,
                                A().withStringFromValueFunction ([] (float v, int) { return juce::String (v, 1) + " bit"; })));
    l.add (std::make_unique<P> (juce::ParameterID { "alias", 1 }, "Aliasing",
                                juce::NormalisableRange<float> (0.f, 1.f, 0.001f), d.alias, pctAttr));
    l.add (std::make_unique<P> (juce::ParameterID { "cutoff", 1 }, "Filter",
                                juce::NormalisableRange<float> (200.f, 20000.f, 1.f, 0.3f), d.cutoff, hzAttr));
    l.add (std::make_unique<P> (juce::ParameterID { "reso", 1 }, "Resonance",
                                juce::NormalisableRange<float> (0.f, 1.f, 0.001f), d.reso, pctAttr));
    l.add (std::make_unique<P> (juce::ParameterID { "noise", 1 }, "Noise",
                                juce::NormalisableRange<float> (0.f, 1.f, 0.001f), d.noise, pctAttr));
    l.add (std::make_unique<P> (juce::ParameterID { "mix", 1 }, "Mix",
                                juce::NormalisableRange<float> (0.f, 1.f, 0.001f), d.mix, pctAttr));
    l.add (std::make_unique<P> (juce::ParameterID { "output", 1 }, "Output",
                                juce::NormalisableRange<float> (-24.f, 12.f, 0.1f), d.output, dbAttr));
    return l;
}

//==============================================================================
bool DustboxAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto out = layouts.getMainOutputChannelSet();
    if (out != juce::AudioChannelSet::mono() && out != juce::AudioChannelSet::stereo())
        return false;
    return layouts.getMainInputChannelSet() == out;
}

void DustboxAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    hostRate = sampleRate;
    const auto numCh = (juce::uint32) juce::jmax (1, getTotalNumOutputChannels());
    juce::dsp::ProcessSpec spec { sampleRate, (juce::uint32) samplesPerBlock, numCh };

    inputFilter.prepare (spec);
    inputFilter.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
    inputFilter.setResonance (0.707f);

    outputFilter.prepare (spec);
    outputFilter.setMode (juce::dsp::LadderFilterMode::LPF24);
    outputFilter.setDrive (1.2f);

    dryWet.prepare (spec);
    dryWet.setMixingRule (juce::dsp::DryWetMixingRule::balanced);

    for (auto* s : { &inGainSm, &outGainSm, &driveSm, &noiseSm })
        s->reset (sampleRate, 0.03);

    inGainSm.setCurrentAndTargetValue  (juce::Decibels::decibelsToGain (pInput->load()));
    outGainSm.setCurrentAndTargetValue (juce::Decibels::decibelsToGain (pOutput->load()));
    driveSm.setCurrentAndTargetValue   (pDrive->load());
    noiseSm.setCurrentAndTargetValue   (pNoise->load());

    heldSample.assign (numCh, 0.0f);
    holdPhase = 1.0;
}

void DustboxAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numCh = juce::jmin (buffer.getNumChannels(), (int) heldSample.size());
    const int numSamples = buffer.getNumSamples();
    for (int ch = numCh; ch < buffer.getNumChannels(); ++ch)
        buffer.clear (ch, 0, numSamples);
    if (numCh == 0 || numSamples == 0)
        return;

    auto block = juce::dsp::AudioBlock<float> (buffer).getSubsetChannelBlock (0, (size_t) numCh);

    // Dry-Signal sichern
    dryWet.setWetMixProportion (pMix->load());
    dryWet.pushDrySamples (block);

    // --- Parameter für diesen Block --------------------------------------
    const float nyquistHost = (float) hostRate * 0.45f;
    const float targetRate  = pRate->load();
    const float bits        = pBits->load();
    const float alias       = pAlias->load();
    const float levels      = std::pow (2.0f, bits - 1.0f);
    const bool  doHold      = targetRate < (float) hostRate - 1.0f;
    const double phaseInc   = (double) targetRate / hostRate;

    inGainSm.setTargetValue  (juce::Decibels::decibelsToGain (pInput->load()));
    outGainSm.setTargetValue (juce::Decibels::decibelsToGain (pOutput->load()));
    driveSm.setTargetValue   (pDrive->load());
    noiseSm.setTargetValue   (pNoise->load());

    // --- 1) Input-Gain + Drive (heißer Wandler-Eingang) --------------------
    for (int i = 0; i < numSamples; ++i)
    {
        const float g  = inGainSm.getNextValue();
        const float dr = driveSm.getNextValue();
        const float k  = 1.0f + dr * 8.0f;
        const float norm = 1.0f / std::sqrt (k);
        for (int ch = 0; ch < numCh; ++ch)
        {
            auto* d = buffer.getWritePointer (ch);
            const float x = d[i] * g;
            d[i] = x + dr * (std::tanh (x * k) * norm - x);   // Drive 0 % = linear
        }
    }

    // --- 2) Anti-Alias-Filter: bei 0 % sauber an Nyquist der Zielrate,
    //        bei 100 % praktisch offen -> harte Spiegelfrequenzen wie bei
    //        Samplern ganz ohne Eingangsfilter
    {
        const float base = targetRate * 0.5f;
        const float cut  = juce::jlimit (20.0f, nyquistHost, base * (1.0f + alias * alias * 15.0f));
        inputFilter.setCutoffFrequency (cut);
        if (alias < 0.999f)
        {
            juce::dsp::ProcessContextReplacing<float> ctx (block);
            inputFilter.process (ctx);
        }
    }

    // --- 3) Sample-&-Hold + 4) Bit-Reduktion ------------------------------
    for (int i = 0; i < numSamples; ++i)
    {
        bool sampleNow = true;
        if (doHold)
        {
            holdPhase += phaseInc;
            sampleNow = holdPhase >= 1.0;
            if (sampleNow)
                holdPhase -= std::floor (holdPhase);
        }

        for (int ch = 0; ch < numCh; ++ch)
        {
            auto* d = buffer.getWritePointer (ch);
            if (sampleNow)
            {
                const float x = juce::jlimit (-1.0f, 1.0f, d[i]);    // Wandler clippt
                heldSample[(size_t) ch] = std::round (x * levels) / levels;
            }
            d[i] = heldSample[(size_t) ch];
        }
    }

    // --- 5) 4-Pol-Ausgangsfilter (Rekonstruktion / Klangfarbe) -------------
    outputFilter.setCutoffFrequencyHz (juce::jmin (pCutoff->load(), nyquistHost));
    outputFilter.setResonance (pReso->load() * 0.85f);
    {
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        outputFilter.process (ctx);
    }

    // --- 6) Rauschen (analoges Grundrauschen) + Output-Gain ---------------
    for (int i = 0; i < numSamples; ++i)
    {
        const float n   = noiseSm.getNextValue();
        const float amp = n * n * 0.03f;
        const float og  = outGainSm.getNextValue();
        for (int ch = 0; ch < numCh; ++ch)
        {
            auto* d = buffer.getWritePointer (ch);
            const float hiss = amp > 0.0f ? (rng.nextFloat() * 2.0f - 1.0f) * amp : 0.0f;
            d[i] = (d[i] + hiss) * og;
        }
    }

    // --- 7) Dry/Wet --------------------------------------------------------
    dryWet.mixWetSamples (block);
}

//==============================================================================
int DustboxAudioProcessor::getNumPrograms() { return numPresets; }

const juce::String DustboxAudioProcessor::getProgramName (int index)
{
    return juce::isPositiveAndBelow (index, numPresets) ? juce::String (presets[index].name) : juce::String();
}

void DustboxAudioProcessor::setCurrentProgram (int index)
{
    if (! juce::isPositiveAndBelow (index, numPresets))
        return;

    currentProgram = index;
    const auto& p = presets[index];

    auto set = [this] (const char* id, float v)
    {
        if (auto* param = apvts.getParameter (id))
            param->setValueNotifyingHost (param->convertTo0to1 (v));
    };
    set ("input", p.input);   set ("drive", p.drive);   set ("rate", p.rate);
    set ("bits", p.bits);     set ("alias", p.alias);   set ("cutoff", p.cutoff);
    set ("reso", p.reso);     set ("noise", p.noise);   set ("mix", p.mix);
    set ("output", p.output);
}

//==============================================================================
void DustboxAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    state.setProperty ("program", currentProgram, nullptr);
    if (auto xml = state.createXml())
        copyXmlToBinary (*xml, destData);
}

void DustboxAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (apvts.state.getType()))
        {
            auto tree = juce::ValueTree::fromXml (*xml);
            currentProgram = juce::jlimit (0, numPresets - 1, (int) tree.getProperty ("program", 0));
            apvts.replaceState (tree);
        }
}

juce::AudioProcessorEditor* DustboxAudioProcessor::createEditor()
{
    return new DustboxAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DustboxAudioProcessor();
}
