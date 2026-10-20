#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <array>

enum class TapeStyle
{
    reelToReel,
    cassette,
    lofi
};

/** A small, self-contained "vintage tape machine" coloration effect: soft
    saturation, wow & flutter (slow + fast pitch wobble via a modulated delay
    line), a touch of hiss, and a tape-like high-frequency rolloff. Applied
    in place to the Sampler's final mixed output when enabled.
*/
class TapeEffect
{
public:
    void prepare (double sampleRateToUse, int numChannelsToUse);
    void reset();

    void setEnabled (bool shouldBeEnabled) noexcept { enabled = shouldBeEnabled; }
    bool isEnabled() const noexcept { return enabled; }

    void setStyle (TapeStyle newStyle) noexcept { style = newStyle; }
    void setAmount (float amount01) noexcept { amount = juce::jlimit (0.0f, 1.0f, amount01); }

    void process (juce::AudioBuffer<float>& buffer);

private:
    struct Params
    {
        float drive;
        float wowDepthMs, wowRateHz;
        float flutterDepthMs, flutterRateHz;
        float hissLevel;
        float toneAmount; // 0 = no rolloff, 1 = heavy rolloff
    };

    static Params paramsFor (TapeStyle s);

    bool enabled = false;
    TapeStyle style = TapeStyle::reelToReel;
    float amount = 0.5f;

    double sampleRate = 44100.0;
    int numChannels = 2;

    static constexpr int maxDelayMs = 25;
    std::array<juce::AudioBuffer<float>, 2> delayLines;
    int delayWritePos = 0;
    int delaySize = 1;

    double wowPhase = 0.0;
    double flutterPhase = 0.0;

    juce::Random noiseRng;
    std::array<float, 2> toneState { 0.0f, 0.0f };
};
