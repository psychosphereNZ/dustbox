#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

//==============================================================================
// DUSTBOX 12 – Vintage-Sampler-Emulation
// Signalweg:
//   Input-Gain -> Drive (heißer Wandler-Eingang) -> Anti-Alias-Filter (abschaltbar)
//   -> Sample-&-Hold (Samplerate-Reduktion, erzeugt Aliasing) -> Bit-Reduktion
//   -> 4-Pol-Ausgangsfilter (Rekonstruktion) -> Rauschen -> Dry/Wet -> Output
//==============================================================================
class DustboxAudioProcessor : public juce::AudioProcessor
{
public:
    DustboxAudioProcessor();
    ~DustboxAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override  { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override;
    int getCurrentProgram() override { return currentProgram; }
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
    juce::AudioProcessorValueTreeState apvts;

private:
    double hostRate = 44100.0;
    int currentProgram = 0;

    // Sample-&-Hold-Zustand (eine gemeinsame Phase -> Stereo bleibt phasengleich)
    double holdPhase = 1.0;
    std::vector<float> heldSample;

    juce::dsp::StateVariableTPTFilter<float> inputFilter;
    juce::dsp::LadderFilter<float> outputFilter;
    juce::dsp::DryWetMixer<float> dryWet;

    juce::SmoothedValue<float> inGainSm, outGainSm, driveSm, noiseSm;
    juce::Random rng;

    std::atomic<float>* pInput  = nullptr;
    std::atomic<float>* pDrive  = nullptr;
    std::atomic<float>* pRate   = nullptr;
    std::atomic<float>* pBits   = nullptr;
    std::atomic<float>* pAlias  = nullptr;
    std::atomic<float>* pCutoff = nullptr;
    std::atomic<float>* pReso   = nullptr;
    std::atomic<float>* pNoise  = nullptr;
    std::atomic<float>* pMix    = nullptr;
    std::atomic<float>* pOutput = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DustboxAudioProcessor)
};
