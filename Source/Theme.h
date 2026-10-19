#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

/** The 80s-terminal colour scheme: always-black background, one switchable
    phosphor accent colour used for all text, borders and highlights.
*/
class Theme
{
public:
    enum class Accent
    {
        green,
        pink,
        white,
        blue,
        purple
    };

    static juce::Colour background() noexcept { return juce::Colour (0xff050505); }
    static juce::Colour panel() noexcept      { return juce::Colour (0xff0a0a0a); }
    static juce::Colour dim (juce::Colour c) noexcept { return c.withAlpha (0.45f); }

    static juce::Colour colourFor (Accent a) noexcept
    {
        switch (a)
        {
            case Accent::green:  return juce::Colour (0xff39ff14);
            case Accent::pink:   return juce::Colour (0xffff2fb2);
            case Accent::white:  return juce::Colour (0xffe8e8e8);
            case Accent::blue:   return juce::Colour (0xff00d9ff);
            case Accent::purple: return juce::Colour (0xffb026ff);
        }
        return juce::Colours::white;
    }

    static const char* nameFor (Accent a) noexcept
    {
        switch (a)
        {
            case Accent::green:  return "GREEN";
            case Accent::pink:   return "PINK";
            case Accent::white:  return "WHITE";
            case Accent::blue:   return "BLUE";
            case Accent::purple: return "PURPLE";
        }
        return "";
    }

    static juce::Font terminalFont (float height) noexcept
    {
        auto opts = juce::Font::getDefaultMonospacedFontName();
        return juce::Font (juce::FontOptions (opts, height, juce::Font::plain));
    }

    /** Persisted accent colour choice, backed by a small properties file. */
    static Accent loadAccent();
    static void saveAccent (Accent a);
};
