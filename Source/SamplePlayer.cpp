#include "SamplePlayer.h"

SamplePlayer::SamplePlayer()
{
    formatManager.registerBasicFormats();
    deviceManager.initialiseWithDefaultDevices (0, 2);
    sourcePlayer.setSource (&transport);
    deviceManager.addAudioCallback (&sourcePlayer);
    startTimerHz (20);
}

SamplePlayer::~SamplePlayer()
{
    stopTimer();
    transport.setSource (nullptr);
    deviceManager.removeAudioCallback (&sourcePlayer);
    sourcePlayer.setSource (nullptr);
}

void SamplePlayer::play (const juce::File& file)
{
    std::unique_ptr<juce::AudioFormatReader> reader (formatManager.createReaderFor (file));
    if (reader == nullptr)
        return;

    transport.stop();
    transport.setSource (nullptr);
    readerSource.reset();

    readerSource = std::make_unique<juce::AudioFormatReaderSource> (reader.release(), true);
    transport.setSource (readerSource.get(), 0, nullptr, readerSource->getAudioFormatReader()->sampleRate);
    currentFile = file;
    transport.setPosition (0.0);
    transport.start();

    if (onPlaybackStateChanged != nullptr)
        onPlaybackStateChanged();
}

void SamplePlayer::stop()
{
    transport.stop();
    if (onPlaybackStateChanged != nullptr)
        onPlaybackStateChanged();
}

juce::String SamplePlayer::describeFormat (const juce::File& file) const
{
    std::unique_ptr<juce::AudioFormatReader> reader (formatManager.createReaderFor (file));
    if (reader == nullptr)
        return {};

    juce::String bits = reader->usesFloatingPointData ? "32-bit float"
                                                        : juce::String (reader->bitsPerSample) + "-bit";
    return bits + " / " + juce::String (reader->sampleRate / 1000.0, 1) + "k";
}

void SamplePlayer::timerCallback()
{
    // Detect natural end-of-playback so the UI can clear its "now playing" row.
    if (! transport.isPlaying() && currentFile != juce::File() && readerSource != nullptr)
    {
        if (transport.hasStreamFinished())
        {
            currentFile = juce::File();
            if (onPlaybackStateChanged != nullptr)
                onPlaybackStateChanged();
        }
    }
}
