#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include "SamplePlayer.h"
#include "Theme.h"

/** Shows the waveform of the sample currently playing (or last played), with
    a moving playhead while it plays — the visual counterpart to one-click
    preview.
*/
class WaveformComponent : public juce::Component,
                           private juce::ChangeListener,
                           private juce::Timer
{
public:
    WaveformComponent (SamplePlayer& playerToUse, const Theme::Accent& accentRef);
    ~WaveformComponent() override;

    void setFile (const juce::File& file);

    void paint (juce::Graphics& g) override;

private:
    void changeListenerCallback (juce::ChangeBroadcaster*) override;
    void timerCallback() override;

    SamplePlayer& player;
    const Theme::Accent& accent;

    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbnailCache { 8 };
    juce::AudioThumbnail thumbnail;
    juce::String currentFileLabel;
};
