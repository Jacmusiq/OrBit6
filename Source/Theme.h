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

    static juce::Colour background() noexcept { return juce::Colour (0xff0a0806); }
    static juce::Colour panel() noexcept      { return juce::Colour (0xff100d0a); }
    static juce::Colour chassis() noexcept    { return juce::Colour (0xff221d17); }
    static juce::Colour chassisHighlight() noexcept { return juce::Colour (0xff3a332a); }
    static juce::Colour chassisShadow() noexcept    { return juce::Colour (0xff000000); }
    static juce::Colour dim (juce::Colour c) noexcept { return c.withAlpha (0.45f); }

    /** A small tiled grain texture for a worn CRT/hardware look. Built once
        and cached.
    */
    static const juce::Image& noiseTexture();

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

    /** The monospace family every label/slider in the app uses — bold, so
        even smoothly-rendered text (sliders, combo boxes) reads chunkier
        and closer to the pixel-art look used everywhere else.
    */
    static juce::Font terminalFont (float height) noexcept
    {
        return juce::Font (juce::FontOptions (juce::Font::getDefaultMonospacedFontName(), height, juce::Font::bold));
    }

    /** Renders text as a small, un-antialiased bitmap then scales it up with
        nearest-neighbour sampling — a cheap way to get a genuine blocky,
        "raw pixel" look without bundling a bitmap font. Used for all the
        custom-painted text throughout the app (titles, tree/list rows, the
        keyboard, the waveform labels).
    */
    static void drawPixelText (juce::Graphics& g, const juce::String& text, juce::Rectangle<int> area,
                               juce::Justification justification, juce::Colour colour, float pixelHeight);

    /** Persisted accent colour choice, backed by a small properties file. */
    static Accent loadAccent();
    static void saveAccent (Accent a);
};
