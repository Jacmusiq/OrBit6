#include "WaveformComponent.h"

WaveformComponent::WaveformComponent (SamplePlayer& playerToUse, const Theme::Accent& accentRef)
    : player (playerToUse), accent (accentRef), thumbnail (512, formatManager, thumbnailCache)
{
    formatManager.registerBasicFormats();
    thumbnail.addChangeListener (this);
    startTimerHz (24);
}

WaveformComponent::~WaveformComponent()
{
    thumbnail.removeChangeListener (this);
}

void WaveformComponent::setFile (const juce::File& file)
{
    currentFileLabel = file.getFileName() + "   " + player.describeFormat (file);
    thumbnail.setSource (new juce::FileInputSource (file));
    repaint();
}

void WaveformComponent::changeListenerCallback (juce::ChangeBroadcaster*)
{
    repaint();
}

void WaveformComponent::timerCallback()
{
    if (player.isPlaying())
        repaint();
}

void WaveformComponent::paint (juce::Graphics& g)
{
    g.fillAll (Theme::panel());

    auto colour = Theme::colourFor (accent);
    auto bounds = getLocalBounds().reduced (8);

    auto labelArea = bounds.removeFromTop (16);
    Theme::drawPixelText (g, currentFileLabel, labelArea, juce::Justification::centredLeft, Theme::dim (colour), 12.0f);

    if (bounds.isEmpty())
        return;

    auto length = thumbnail.getTotalLength();
    bool playing = player.isPlaying();

    if (length <= 0.0)
    {
        Theme::drawPixelText (g, "NO SAMPLE SELECTED", bounds, juce::Justification::centred, Theme::dim (colour), 13.0f);
        return;
    }

    g.setColour (playing ? colour : Theme::dim (colour));
    thumbnail.drawChannels (g, bounds, 0.0, length, playing ? 1.0f : 0.8f);

    if (playing)
    {
        auto position = player.getCurrentPosition();
        auto x = bounds.getX() + (float) (position / length) * (float) bounds.getWidth();

        g.setColour (juce::Colours::white);
        g.drawLine (x, (float) bounds.getY(), x, (float) bounds.getBottom(), 2.0f);
    }
}
