#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include "Theme.h"

/** A static waveform display for the Sampler's loaded sample, with the
    trimmed-out head/tail shaded so editing the start/end sliders gives
    immediate visual feedback.
*/
class SamplerWaveformComponent : public juce::Component,
                                  private juce::ChangeListener
{
public:
    explicit SamplerWaveformComponent (const Theme::Accent& accentRef);
    ~SamplerWaveformComponent() override;

    void setFile (const juce::File& file);
    void setTrim (float startFraction, float endFraction);

    void paint (juce::Graphics& g) override;

private:
    void changeListenerCallback (juce::ChangeBroadcaster*) override;

    const Theme::Accent& accent;
    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbnailCache { 4 };
    juce::AudioThumbnail thumbnail;

    float trimStart = 0.0f;
    float trimEnd = 1.0f;
};
