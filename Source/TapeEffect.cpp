#include "TapeEffect.h"

TapeEffect::Params TapeEffect::paramsFor (TapeStyle s)
{
    switch (s)
    {
        case TapeStyle::reelToReel:
            return { 1.6f, 1.2f, 0.7f, 0.25f, 6.5f, 0.0015f, 0.25f };
        case TapeStyle::cassette:
            return { 2.4f, 2.0f, 0.9f, 0.5f, 8.0f, 0.004f, 0.55f };
        case TapeStyle::lofi:
            return { 3.5f, 3.2f, 1.1f, 0.9f, 10.0f, 0.009f, 0.8f };
    }
    return { 1.6f, 1.2f, 0.7f, 0.25f, 6.5f, 0.0015f, 0.25f };
}

void TapeEffect::prepare (double sampleRateToUse, int numChannelsToUse)
{
    sampleRate = sampleRateToUse;
    numChannels = juce::jlimit (1, 2, numChannelsToUse);

    delaySize = (int) ((maxDelayMs / 1000.0) * sampleRate) + 4;
    for (auto& line : delayLines)
    {
        line.setSize (1, delaySize);
        line.clear();
    }

    reset();
}

void TapeEffect::reset()
{
    delayWritePos = 0;
    wowPhase = 0.0;
    flutterPhase = 0.0;
    toneState = { 0.0f, 0.0f };
    for (auto& line : delayLines)
        line.clear();
}

void TapeEffect::process (juce::AudioBuffer<float>& buffer)
{
    if (! enabled || amount <= 0.0f || delaySize <= 1)
        return;

    auto p = paramsFor (style);
    auto numSamples = buffer.getNumSamples();
    auto channels = juce::jmin (numChannels, buffer.getNumChannels());

    const double baseDelayMs = maxDelayMs * 0.5;
    const double baseDelaySamples = (baseDelayMs / 1000.0) * sampleRate;
    const double wowDepthSamples = (p.wowDepthMs * amount / 1000.0) * sampleRate;
    const double flutterDepthSamples = (p.flutterDepthMs * amount / 1000.0) * sampleRate;

    const double wowInc = juce::MathConstants<double>::twoPi * p.wowRateHz / sampleRate;
    const double flutterInc = juce::MathConstants<double>::twoPi * p.flutterRateHz / sampleRate;

    const float driveAmt = 1.0f + (p.drive - 1.0f) * amount;
    const float normaliser = std::tanh (driveAmt) > 0.0001f ? 1.0f / std::tanh (driveAmt) : 1.0f;
    const float toneCoeff = juce::jlimit (0.02f, 0.9f, p.toneAmount * amount);
    const float hiss = p.hissLevel * amount;

    for (int ch = 0; ch < channels; ++ch)
    {
        auto* data = buffer.getWritePointer (ch);
        auto* delay = delayLines[(size_t) ch].getWritePointer (0);
        int writePos = delayWritePos;
        double wPhase = wowPhase;
        double fPhase = flutterPhase;
        float tone = toneState[(size_t) ch];

        for (int i = 0; i < numSamples; ++i)
        {
            delay[writePos] = data[i];

            auto modOffset = wowDepthSamples * std::sin (wPhase) + flutterDepthSamples * std::sin (fPhase);
            double readPos = (double) writePos - baseDelaySamples - modOffset;

            while (readPos < 0.0)
                readPos += delaySize;

            int readIndexA = (int) readPos;
            int readIndexB = (readIndexA + 1) % delaySize;
            float frac = (float) (readPos - (double) readIndexA);
            readIndexA %= delaySize;

            float wet = delay[readIndexA] + frac * (delay[readIndexB] - delay[readIndexA]);

            // soft saturation
            wet = std::tanh (wet * driveAmt) * normaliser;

            // tape hiss
            wet += hiss * (noiseRng.nextFloat() * 2.0f - 1.0f);

            // gentle HF rolloff
            tone += toneCoeff * (wet - tone);
            wet = tone;

            data[i] = juce::jlimit (-1.5f, 1.5f, data[i] * (1.0f - amount) + wet * amount);

            writePos = (writePos + 1) % delaySize;
            wPhase += wowInc;
            fPhase += flutterInc;
            if (wPhase > juce::MathConstants<double>::twoPi) wPhase -= juce::MathConstants<double>::twoPi;
            if (fPhase > juce::MathConstants<double>::twoPi) fPhase -= juce::MathConstants<double>::twoPi;
        }

        if (ch == channels - 1)
        {
            delayWritePos = writePos;
            wowPhase = wPhase;
            flutterPhase = fPhase;
        }

        toneState[(size_t) ch] = tone;
    }
}
