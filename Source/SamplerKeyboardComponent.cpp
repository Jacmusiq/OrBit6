#include "SamplerKeyboardComponent.h"

namespace
{
    constexpr int numWhiteKeys = 10;
    constexpr int numBlackKeys = 7;
    const int whiteSemitones[numWhiteKeys] = { 0, 2, 4, 5, 7, 9, 11, 12, 14, 16 };
    const char whiteChars[numWhiteKeys] = { 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';' };
    const int blackSemitones[numBlackKeys] = { 1, 3, 6, 8, 10, 13, 15 };
    const char blackChars[numBlackKeys] = { 'W', 'E', 'T', 'Y', 'U', 'O', 'P' };
    const int blackAfterWhiteIndex[numBlackKeys] = { 0, 1, 3, 4, 5, 7, 8 };
    constexpr int noNote = -1000;
    constexpr int hintStripHeight = 16;
}

const std::vector<SamplerKeyboardComponent::KeyDef>& SamplerKeyboardComponent::keyDefs()
{
    static const std::vector<KeyDef> defs = []
    {
        std::vector<KeyDef> v;
        for (int i = 0; i < numWhiteKeys; ++i)
            v.push_back ({ whiteChars[i], whiteSemitones[i], false });
        for (int i = 0; i < numBlackKeys; ++i)
            v.push_back ({ blackChars[i], blackSemitones[i], true });
        return v;
    }();
    return defs;
}

SamplerKeyboardComponent::SamplerKeyboardComponent (SamplerEngine& engineToUse, const Theme::Accent& accentRef)
    : engine (engineToUse), accent (accentRef)
{
    setWantsKeyboardFocus (true);
    startTimerHz (100);
}

SamplerKeyboardComponent::~SamplerKeyboardComponent()
{
    stopTimer();
    engine.allNotesOff();
}

void SamplerKeyboardComponent::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop (hintStripHeight);

    float whiteW = (float) bounds.getWidth() / (float) numWhiteKeys;

    for (int i = 0; i < numWhiteKeys; ++i)
        whiteKeyBounds[i] = juce::Rectangle<int> (bounds.getX() + (int) (i * whiteW), bounds.getY(),
                                                   (int) whiteW + 1, bounds.getHeight());

    int blackH = (int) (bounds.getHeight() * 0.62f);
    int blackW = (int) (whiteW * 0.6f);

    for (int i = 0; i < numBlackKeys; ++i)
    {
        int afterIdx = blackAfterWhiteIndex[i];
        int centerX = bounds.getX() + (int) ((afterIdx + 1) * whiteW);
        blackKeyBounds[i] = juce::Rectangle<int> (centerX - blackW / 2, bounds.getY(), blackW, blackH);
    }
}

void SamplerKeyboardComponent::paint (juce::Graphics& g)
{
    auto colour = Theme::colourFor (accent);
    g.fillAll (Theme::panel());

    Theme::drawPixelText (g, "Z / X = OCTAVE DOWN / UP   (OCTAVE " + juce::String (octaveOffset) + ")",
                         getLocalBounds().removeFromTop (hintStripHeight), juce::Justification::centredLeft,
                         Theme::dim (colour), 11.0f);

    for (int i = 0; i < numWhiteKeys; ++i)
    {
        auto r = whiteKeyBounds[i];
        int note = whiteSemitones[i] + octaveOffset * 12;
        bool held = keyWasDown[(size_t) (juce::uint8) whiteChars[i]] || mouseHeldNote == note;

        g.setColour (held ? colour.withAlpha (0.35f) : juce::Colour (0xff1a1510));
        g.fillRect (r);
        g.setColour (Theme::dim (colour));
        g.drawRect (r, 1);

        auto labelArea = r.removeFromBottom (18);
        Theme::drawPixelText (g, juce::String::charToString ((juce::juce_wchar) whiteChars[i]), labelArea,
                             juce::Justification::centred, held ? colour : Theme::dim (colour), 12.0f);
    }

    for (int i = 0; i < numBlackKeys; ++i)
    {
        auto r = blackKeyBounds[i];
        int note = blackSemitones[i] + octaveOffset * 12;
        bool held = keyWasDown[(size_t) (juce::uint8) blackChars[i]] || mouseHeldNote == note;

        g.setColour (held ? colour : juce::Colour (0xff050302));
        g.fillRect (r);
        g.setColour (Theme::dim (colour));
        g.drawRect (r, 1);

        auto labelArea = r.removeFromBottom (16);
        Theme::drawPixelText (g, juce::String::charToString ((juce::juce_wchar) blackChars[i]), labelArea,
                             juce::Justification::centred, held ? Theme::panel() : colour.withAlpha (0.8f), 11.0f);
    }
}

int SamplerKeyboardComponent::noteForMouse (juce::Point<int> p) const
{
    for (int i = 0; i < numBlackKeys; ++i)
        if (blackKeyBounds[i].contains (p))
            return blackSemitones[i] + octaveOffset * 12;

    for (int i = 0; i < numWhiteKeys; ++i)
        if (whiteKeyBounds[i].contains (p))
            return whiteSemitones[i] + octaveOffset * 12;

    return noNote;
}

void SamplerKeyboardComponent::mouseDown (const juce::MouseEvent& e)
{
    auto note = noteForMouse (e.getPosition());
    if (note == noNote)
        return;

    mouseHeldNote = note;
    startNote (note);
    repaint();
}

void SamplerKeyboardComponent::mouseDrag (const juce::MouseEvent& e)
{
    if (mouseHeldNote == noNote)
        return;

    auto note = noteForMouse (e.getPosition());
    if (note != noNote && note != mouseHeldNote)
    {
        endNote (mouseHeldNote);
        mouseHeldNote = note;
        startNote (note);
        repaint();
    }
}

void SamplerKeyboardComponent::mouseUp (const juce::MouseEvent&)
{
    if (mouseHeldNote != noNote)
    {
        endNote (mouseHeldNote);
        mouseHeldNote = noNote;
        repaint();
    }
}

bool SamplerKeyboardComponent::keyPressed (const juce::KeyPress& key)
{
    auto c = key.getTextCharacter();

    if (c == 'z' || c == 'Z')
    {
        octaveOffset = juce::jmax (-3, octaveOffset - 1);
        repaint();
        return true;
    }

    if (c == 'x' || c == 'X')
    {
        octaveOffset = juce::jmin (3, octaveOffset + 1);
        repaint();
        return true;
    }

    return false;
}

void SamplerKeyboardComponent::startNote (int note)
{
    engine.noteOn (note);
}

void SamplerKeyboardComponent::endNote (int note)
{
    engine.noteOff (note);
}

void SamplerKeyboardComponent::timerCallback()
{
    bool changed = false;

    for (auto& kd : keyDefs())
    {
        bool down = juce::KeyPress::isKeyCurrentlyDown ((int) (juce::uint8) kd.character);
        auto idx = (size_t) (juce::uint8) kd.character;

        if (down != keyWasDown[idx])
        {
            changed = true;
            int note = kd.semitone + octaveOffset * 12;

            if (down)
                startNote (note);
            else
                endNote (note);

            keyWasDown[idx] = down;
        }
    }

    if (changed)
        repaint();
}
