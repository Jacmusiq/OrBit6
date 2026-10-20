#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "SamplerEngine.h"
#include "Theme.h"
#include <array>
#include <vector>

/** A typing-keyboard "piano": the computer keyboard (A S D F G H J K L ; for
    white keys, W E T Y U O P for the black keys in between, Z/X to shift
    octave) plays the Sampler's loaded sample pitched per key, polled each
    frame so key-up reliably ends the note. Also drawn as an on-screen piano
    that can be played with the mouse.
*/
class SamplerKeyboardComponent : public juce::Component,
                                  private juce::Timer
{
public:
    explicit SamplerKeyboardComponent (SamplerEngine& engineToUse, const Theme::Accent& accentRef);
    ~SamplerKeyboardComponent() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseUp (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    bool keyPressed (const juce::KeyPress& key) override;

private:
    struct KeyDef
    {
        char character;
        int semitone;
        bool isBlack;
    };

    void timerCallback() override;
    int noteForMouse (juce::Point<int> p) const;
    void startNote (int note);
    void endNote (int note);

    SamplerEngine& engine;
    const Theme::Accent& accent;

    static const std::vector<KeyDef>& keyDefs();

    std::array<bool, 128> keyWasDown {};
    int octaveOffset = 0;
    int mouseHeldNote = -1000;

    juce::Rectangle<int> whiteKeyBounds[11];
    juce::Rectangle<int> blackKeyBounds[11];
};
