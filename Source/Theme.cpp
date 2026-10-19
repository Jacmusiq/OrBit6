#include "Theme.h"

static juce::PropertiesFile& getProps()
{
    static juce::PropertiesFile::Options opts = []
    {
        juce::PropertiesFile::Options o;
        o.applicationName = "ApolloCrate";
        o.filenameSuffix = "settings";
        o.osxLibrarySubFolder = "Application Support";
        o.folderName = "ApolloCrate";
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
