#include "Theme.h"

static juce::PropertiesFile& getProps()
{
    static juce::PropertiesFile::Options opts = []
    {
        juce::PropertiesFile::Options o;
        o.applicationName = "OrBit6";
        o.filenameSuffix = "settings";
        o.osxLibrarySubFolder = "Application Support";
        o.folderName = "OrBit6";
        return o;
    }();

    static juce::PropertiesFile props (opts);
    return props;
}

Theme::Accent Theme::loadAccent()
{
    auto value = getProps().getIntValue ("accentColour", (int) Accent::green);
    return (Accent) juce::jlimit (0, (int) Accent::purple, value);
}

void Theme::saveAccent (Accent a)
{
    getProps().setValue ("accentColour", (int) a);
    getProps().saveIfNeeded();
}

void Theme::drawPixelText (juce::Graphics& g, const juce::String& text, juce::Rectangle<int> area,
                           juce::Justification justification, juce::Colour colour, float pixelHeight)
{
    if (text.isEmpty() || area.isEmpty())
        return;

    constexpr int renderHeight = 11;
    auto smallFont = juce::Font (juce::FontOptions (juce::Font::getDefaultMonospacedFontName(), (float) renderHeight, juce::Font::bold));

    juce::GlyphArrangement glyphs;
    glyphs.addLineOfText (smallFont, text, 0.0f, (float) renderHeight);
    auto box = glyphs.getBoundingBox (0, glyphs.getNumGlyphs(), true);
    if (box.getWidth() <= 0.0f)
        return;

    int imgW = juce::jmax (1, (int) std::ceil (box.getWidth()) + 2);
    int imgH = renderHeight + 4;

    juce::Image img (juce::Image::ARGB, imgW, imgH, true);
    {
        juce::Graphics tg (img);
        tg.setColour (colour);
        glyphs.draw (tg);
    }

    float scale = pixelHeight / (float) renderHeight;
    int destW = juce::jmax (1, juce::roundToInt ((float) imgW * scale));
    int destH = juce::jmax (1, juce::roundToInt ((float) imgH * scale));

    auto dest = justification.appliedToRectangle (juce::Rectangle<int> (destW, destH), area);

    g.setImageResamplingQuality (juce::Graphics::lowResamplingQuality);
    g.drawImage (img, dest.toFloat(), juce::RectanglePlacement::stretchToFit);
}

const juce::Image& Theme::noiseTexture()
{
    static juce::Image texture = []
    {
        const int size = 96;
        juce::Image img (juce::Image::ARGB, size, size, true);
        juce::Random rng (0x4a17b2);

        for (int y = 0; y < size; ++y)
        {
            for (int x = 0; x < size; ++x)
            {
                auto v = rng.nextFloat();
                auto alpha = (juce::uint8) juce::jlimit (0, 255, (int) (v * v * 40.0f));
                img.setPixelAt (x, y, juce::Colour (255, 255, 255).withAlpha (alpha));
            }
        }

        return img;
    }();

    return texture;
}
