#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

/** One-click preview playback. Holds the audio device + a single transport
    source; playing a new file stops whatever was playing before, so clicking
    through a kit always gives instant, un-overlapped feedback.
*/
class SamplePlayer : private juce::Timer
{
public:
    explicit SamplePlayer (juce::AudioDeviceManager& deviceManagerToUse);
    ~SamplePlayer() override;

    void play (const juce::File& file);

    /** Pauses in place — the position is retained so resume() continues from
        where it left off.
    */
    void stop();

    /** Continues playback of whatever was paused, from where it paused. Does
        nothing if nothing has been loaded yet.
    */
    void resume();

    bool isPlaying (const juce::File& file) const noexcept { return isPlaying() && currentFile == file; }
    bool isPlaying() const noexcept { return transport.isPlaying(); }
    bool hasActiveSample() const noexcept { return readerSource != nullptr; }

    /** True if playback was deliberately paused mid-sample (as opposed to
        never having played anything, or having finished naturally).
    */
    bool isPaused() const noexcept { return ! isPlaying() && readerSource != nullptr && currentFile != juce::File(); }

    double getCurrentPosition() const { return transport.getCurrentPosition(); }
    double getLengthSeconds() const { return transport.getLengthInSeconds(); }

    /** Human-readable bit depth / sample rate tag, e.g. "24-bit / 44.1k", or
        an empty string if the file can't be read.
    */
    juce::String describeFormat (const juce::File& file) const;

    std::function<void()> onPlaybackStateChanged;

    /** Fired right as a new file starts playing, so UI (e.g. the waveform
        view) can switch to showing it.
    */
    std::function<void (const juce::File&)> onFileStarted;

private:
    void timerCallback() override;

    juce::AudioDeviceManager& deviceManager;
    mutable juce::AudioFormatManager formatManager;
    juce::AudioTransportSource transport;
    juce::AudioSourcePlayer sourcePlayer;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::File currentFile;
};
