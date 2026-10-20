#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "Theme.h"

/** A tab switcher styled as plain bold title text — no pill/box chrome — so
    "SAMPLER" reads as the same kind of big brand-style header as the app
    title, rather than a small separate button sitting next to it.
*/
class TextTabButton : public juce::Component
{
public:
    TextTabButton (juce::String textToShow, const Theme::Accent& accentRef);

    void setActive (bool shouldBeActive);
    bool isActive() const noexcept { return active; }

    void paint (juce::Graphics& g) override;
    void mouseUp (const juce::MouseEvent& e) override;

    std::function<void()> onClick;

private:
    juce::String text;
    const Theme::Accent& accent;
    bool active = false;
};
