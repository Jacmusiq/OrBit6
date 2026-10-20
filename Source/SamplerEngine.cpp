#include "SamplerEngine.h"

SamplerEngine::SamplerEngine (juce::AudioDeviceManager& deviceManagerToUse)
    : deviceManager (deviceManagerToUse)
{
    formatManager.registerBasicFormats();
    sourcePlayer.setSource (this);
    deviceManager.addAudioCallback (&sourcePlayer);
    recordingThread.startThread();

    adsrParams.attack = 0.01f;
    adsrParams.decay = 0.15f;
    adsrParams.sustain = 0.8f;
    adsrParams.release = 0.25f;
}

SamplerEngine::~SamplerEngine()
{
    stopRecording();
    deviceManager.removeAudioCallback (&sourcePlayer);
    sourcePlayer.setSource (nullptr);
    recordingThread.stopThread (2000);
}

bool SamplerEngine::loadSample (const juce::File& file)
{
    std::unique_ptr<juce::AudioFormatReader> reader (formatManager.createReaderFor (file));
    if (reader == nullptr)
        return false;

    int numCh = (int) juce::jlimit (1u, 2u, reader->numChannels);
    juce::AudioBuffer<float> newBuffer (numCh, (int) reader->lengthInSamples);
    reader->read (&newBuffer, 0, (int) reader->lengthInSamples, 0, true, true);

    juce::ScopedLock sl (voiceLock);
    for (auto& v : voices)
        v.active = false;

    sampleBuffer = std::move (newBuffer);
    sampleSourceRate = reader->sampleRate;
    loadedFile = file;
    trimStart.store (0.0f);
    trimEnd.store (1.0f);

    return true;
}

void SamplerEngine::setTrim (float startFraction, float endFraction)
{
    startFraction = juce::jlimit (0.0f, 1.0f, startFraction);
    endFraction = juce::jlimit (0.0f, 1.0f, endFraction);
    if (endFraction <= startFraction)
        endFraction = juce::jmin (1.0f, startFraction + 0.01f);

    trimStart.store (startFraction);
    trimEnd.store (endFraction);
}

void SamplerEngine::setADSR (float attackSeconds, float decaySeconds, float sustainLevel, float releaseSeconds)
{
    juce::ScopedLock pl (paramLock);
    adsrParams.attack = juce::jmax (0.0f, attackSeconds);
    adsrParams.decay = juce::jmax (0.0f, decaySeconds);
    adsrParams.sustain = juce::jlimit (0.0f, 1.0f, sustainLevel);
    adsrParams.release = juce::jmax (0.0f, releaseSeconds);
}

SamplerEngine::Voice* SamplerEngine::findVoiceForNote (int note)
{
    for (auto& v : voices)
        if (v.active && v.note == note)
            return &v;
    return nullptr;
}

SamplerEngine::Voice* SamplerEngine::findFreeVoice()
{
    for (auto& v : voices)
        if (! v.active)
            return &v;
    return &voices[0];
}

void SamplerEngine::noteOn (int note, float velocity)
{
    juce::ignoreUnused (velocity);
    if (! hasSample())
        return;

    juce::ScopedLock sl (voiceLock);

    auto* v = findVoiceForNote (note);
    if (v == nullptr)
        v = findFreeVoice();

    double semitoneTotal = note + semitoneOffset.load();
    v->ratio = std::pow (2.0, semitoneTotal / 12.0) * (sampleSourceRate / outputSampleRate);
    v->note = note;
    v->position = 0.0;
    v->active = true;

    juce::ADSR::Parameters paramsCopy;
    {
        juce::ScopedLock pl (paramLock);
        paramsCopy = adsrParams;
    }

    v->envelope.setSampleRate (outputSampleRate);
    v->envelope.setParameters (paramsCopy);
    v->envelope.noteOn();
}

void SamplerEngine::noteOff (int note)
{
    juce::ScopedLock sl (voiceLock);
    if (auto* v = findVoiceForNote (note))
        v->envelope.noteOff();
}

void SamplerEngine::allNotesOff()
{
    juce::ScopedLock sl (voiceLock);
    for (auto& v : voices)
        if (v.active)
            v.envelope.noteOff();
}

bool SamplerEngine::startRecording (const juce::File& destinationWavFile)
{
    stopRecording();

    destinationWavFile.getParentDirectory().createDirectory();
    destinationWavFile.deleteFile();

    std::unique_ptr<juce::OutputStream> stream = destinationWavFile.createOutputStream();
    if (stream == nullptr)
        return false;

    juce::WavAudioFormat wavFormat;
    auto options = juce::AudioFormatWriterOptions()
                       .withSampleRate (outputSampleRate)
                       .withNumChannels (2)
                       .withBitsPerSample (24);

    auto writer = wavFormat.createWriterFor (stream, options);
    if (writer == nullptr)
        return false;

    juce::ScopedLock wl (writerLock);
    activeWriter = std::make_unique<juce::AudioFormatWriter::ThreadedWriter> (writer.release(), recordingThread, 32768);
    recording.store (true);
    return true;
}

void SamplerEngine::stopRecording()
{
    recording.store (false);
    juce::ScopedLock wl (writerLock);
    activeWriter.reset();
}

void SamplerEngine::prepareToPlay (int, double sampleRate)
{
    outputSampleRate = sampleRate;
    tapeEffect.prepare (sampleRate, 2);
}

void SamplerEngine::releaseResources()
{
}

void SamplerEngine::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    if (! hasSample() || bufferToFill.buffer == nullptr)
        return;

    auto numSamples = bufferToFill.numSamples;
    juce::AudioBuffer<float> scratch (2, numSamples);
    scratch.clear();

    {
        juce::ScopedLock sl (voiceLock);

        auto totalSamples = sampleBuffer.getNumSamples();
        auto numCh = sampleBuffer.getNumChannels();
        int startSample = (int) (trimStart.load() * (float) totalSamples);
        int endSample = (int) (trimEnd.load() * (float) totalSamples);
        int regionLength = juce::jmax (1, endSample - startSample);
        bool rev = reversed.load();
        float gain = juce::Decibels::decibelsToGain (gainDb.load());

        auto* outL = scratch.getWritePointer (0);
        auto* outR = scratch.getWritePointer (1);

        for (auto& v : voices)
        {
            if (! v.active)
                continue;

            for (int i = 0; i < numSamples; ++i)
            {
                if (! v.active)
                    break;

                if (v.position >= (double) regionLength)
                {
                    v.active = false;
                    break;
                }

                int idx = (int) v.position;
                float frac = (float) (v.position - (double) idx);
                int idxNext = juce::jmin (idx + 1, regionLength - 1);

                int srcIdxA = startSample + (rev ? (regionLength - 1 - idx) : idx);
                int srcIdxB = startSample + (rev ? (regionLength - 1 - idxNext) : idxNext);
                srcIdxA = juce::jlimit (0, totalSamples - 1, srcIdxA);
                srcIdxB = juce::jlimit (0, totalSamples - 1, srcIdxB);

                float envVal = v.envelope.getNextSample();

                float s0L = sampleBuffer.getSample (0, srcIdxA);
                float s1L = sampleBuffer.getSample (0, srcIdxB);
                float sL = s0L + frac * (s1L - s0L);

                float sR = sL;
                if (numCh > 1)
                {
                    float s0R = sampleBuffer.getSample (1, srcIdxA);
                    float s1R = sampleBuffer.getSample (1, srcIdxB);
                    sR = s0R + frac * (s1R - s0R);
                }

                outL[i] += sL * envVal * gain;
                outR[i] += sR * envVal * gain;

                v.position += v.ratio;

                if (! v.envelope.isActive())
                {
                    v.active = false;
                    break;
                }
            }
        }
    }

    tapeEffect.process (scratch);

    for (int ch = 0; ch < bufferToFill.buffer->getNumChannels(); ++ch)
        bufferToFill.buffer->addFrom (ch, bufferToFill.startSample, scratch, juce::jmin (ch, 1), 0, numSamples);

    if (recording.load())
    {
        juce::ScopedLock wl (writerLock);
        if (activeWriter != nullptr)
        {
            const float* chans[2] = { scratch.getReadPointer (0), scratch.getReadPointer (1) };
            activeWriter->write (chans, numSamples);
        }
    }
}
