#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

/** One-click preview playback. Holds the audio device + a single transport
    source; playing a new file stops whatever was playing before, so clicking
    through a kit always gives instant, un-overlapped feedback.
*/
class SamplePlayer : private juce::Timer
{
public:
    SamplePlayer();
    ~SamplePlayer() override;

    void play (const juce::File& file);
    void stop();

    bool isPlaying (const juce::File& file) const noexcept { return isPlaying() && currentFile == file; }
    bool isPlaying() const noexcept { return transport.isPlaying(); }

    /** Human-readable bit depth / sample rate tag, e.g. "24-bit / 44.1k", or
        an empty string if the file can't be read.
    */
    juce::String describeFormat (const juce::File& file) const;

    std::function<void()> onPlaybackStateChanged;

private:
    void timerCallback() override;

    juce::AudioDeviceManager deviceManager;
    mutable juce::AudioFormatManager formatManager;
    juce::AudioTransportSource transport;
    juce::AudioSourcePlayer sourcePlayer;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::File currentFile;
};
