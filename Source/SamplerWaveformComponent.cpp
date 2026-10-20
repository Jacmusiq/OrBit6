#include "SamplerWaveformComponent.h"

SamplerWaveformComponent::SamplerWaveformComponent (const Theme::Accent& accentRef)
    : accent (accentRef), thumbnail (512, formatManager, thumbnailCache)
{
    formatManager.registerBasicFormats();
    thumbnail.addChangeListener (this);
}

SamplerWaveformComponent::~SamplerWaveformComponent()
{
    thumbnail.removeChangeListener (this);
}

void SamplerWaveformComponent::setFile (const juce::File& file)
{
    thumbnail.setSource (new juce::FileInputSource (file));
    repaint();
}

void SamplerWaveformComponent::setTrim (float startFraction, float endFraction)
{
    trimStart = startFraction;
    trimEnd = endFraction;
    repaint();
}

void SamplerWaveformComponent::changeListenerCallback (juce::ChangeBroadcaster*)
{
    repaint();
}

void SamplerWaveformComponent::paint (juce::Graphics& g)
{
    auto colour = Theme::colourFor (accent);
    g.fillAll (Theme::panel());

    auto bounds = getLocalBounds().reduced (4);
    auto length = thumbnail.getTotalLength();

    if (length <= 0.0)
    {
        Theme::drawPixelText (g, "DOUBLE-CLICK A SAMPLE IN THE BROWSER TO LOAD IT HERE", bounds,
                             juce::Justification::centred, Theme::dim (colour), 13.0f);
        return;
    }

    g.setColour (Theme::dim (colour));
    thumbnail.drawChannels (g, bounds, 0.0, length, 0.85f);

    auto shadeLeft = bounds.getX() + (int) (trimStart * (float) bounds.getWidth());
    auto shadeRight = bounds.getX() + (int) (trimEnd * (float) bounds.getWidth());

    g.setColour (juce::Colours::black.withAlpha (0.55f));
    if (shadeLeft > bounds.getX())
        g.fillRect (bounds.getX(), bounds.getY(), shadeLeft - bounds.getX(), bounds.getHeight());
    if (shadeRight < bounds.getRight())
        g.fillRect (shadeRight, bounds.getY(), bounds.getRight() - shadeRight, bounds.getHeight());

    g.setColour (colour);
    g.drawVerticalLine (shadeLeft, (float) bounds.getY(), (float) bounds.getBottom());
    g.drawVerticalLine (shadeRight, (float) bounds.getY(), (float) bounds.getBottom());
}
