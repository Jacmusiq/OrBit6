#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include "TapeEffect.h"
#include <array>

/** The Sampler tab's audio engine: loads one sample into memory, plays it
    polyphonically pitched across the computer-keyboard "piano", applies
    trim/gain/tune/ADSR/reverse editing, an optional vintage tape coloration
    on the output, and can record whatever comes out of it to a WAV file.
*/
class SamplerEngine : public juce::AudioSource
{
public:
    explicit SamplerEngine (juce::AudioDeviceManager& deviceManagerToUse);
    ~SamplerEngine() override;

    // --- Loading -----------------------------------------------------
    bool loadSample (const juce::File& file);
    juce::File getLoadedFile() const noexcept { return loadedFile; }
    bool hasSample() const noexcept { return sampleBuffer.getNumSamples() > 0; }
    const juce::AudioBuffer<float>& getBuffer() const noexcept { return sampleBuffer; }
    double getSampleSourceRate() const noexcept { return sampleSourceRate; }

    // --- Editing -------------------------------------------------------
    void setTrim (float startFraction, float endFraction);
    float getTrimStart() const noexcept { return trimStart.load(); }
    float getTrimEnd() const noexcept { return trimEnd.load(); }

    void setGainDb (float db) noexcept { gainDb.store (db); }
    void setSemitoneOffset (int semitones) noexcept { semitoneOffset.store (semitones); }
    void setReversed (bool shouldReverse) noexcept { reversed.store (shouldReverse); }
    bool isReversed() const noexcept { return reversed.load(); }

    void setADSR (float attackSeconds, float decaySeconds, float sustainLevel, float releaseSeconds);

    // --- Playback (computer keyboard / on-screen piano) ----------------
    /** note is a semitone offset from the sample's natural pitch (0 = unshifted). */
    void noteOn (int note, float velocity = 1.0f);
    void noteOff (int note);
    void allNotesOff();

    // --- Vintage tape coloration ----------------------------------------
    void setTapeEnabled (bool shouldBeEnabled) { tapeEffect.setEnabled (shouldBeEnabled); }
    bool isTapeEnabled() const { return tapeEffect.isEnabled(); }
    void setTapeStyle (TapeStyle style) { tapeEffect.setStyle (style); }
    void setTapeAmount (float amount01) { tapeEffect.setAmount (amount01); }

    // --- Recording -------------------------------------------------------
    bool startRecording (const juce::File& destinationWavFile);
    void stopRecording();
    bool isRecording() const noexcept { return recording.load(); }

    // --- juce::AudioSource ---------------------------------------------
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;

private:
    struct Voice
    {
        bool active = false;
        int note = 0;
        double position = 0.0;
        double ratio = 1.0;
        juce::ADSR envelope;
    };

    Voice* findFreeVoice();
    Voice* findVoiceForNote (int note);

    juce::AudioDeviceManager& deviceManager;
    juce::AudioSourcePlayer sourcePlayer;
    juce::AudioFormatManager formatManager;

    juce::AudioBuffer<float> sampleBuffer;
    double sampleSourceRate = 44100.0;
    juce::File loadedFile;

    double outputSampleRate = 44100.0;

    std::atomic<float> trimStart { 0.0f };
    std::atomic<float> trimEnd { 1.0f };
    std::atomic<float> gainDb { 0.0f };
    std::atomic<int> semitoneOffset { 0 };
    std::atomic<bool> reversed { false };
    juce::ADSR::Parameters adsrParams;
    juce::CriticalSection paramLock;

    static constexpr int numVoices = 16;
    std::array<Voice, numVoices> voices;
    juce::CriticalSection voiceLock;

    TapeEffect tapeEffect;

    juce::TimeSliceThread recordingThread { "OrBit6 Recording" };
    juce::CriticalSection writerLock;
    std::unique_ptr<juce::AudioFormatWriter::ThreadedWriter> activeWriter;
    std::unique_ptr<juce::FileOutputStream> recordingStream;
    std::atomic<bool> recording { false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SamplerEngine)
};
