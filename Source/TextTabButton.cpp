#include "TextTabButton.h"

TextTabButton::TextTabButton (juce::String textToShow, const Theme::Accent& accentRef)
    : text (std::move (textToShow)), accent (accentRef)
{
    setMouseCursor (juce::MouseCursor::PointingHandCursor);
    setWantsKeyboardFocus (false);
}

void TextTabButton::setActive (bool shouldBeActive)
{
    if (active != shouldBeActive)
    {
        active = shouldBeActive;
        repaint();
    }
}

void TextTabButton::paint (juce::Graphics& g)
{
    auto colour = Theme::colourFor (accent);
    auto drawColour = active ? colour : Theme::dim (colour);
    auto bounds = getLocalBounds();

    Theme::drawPixelText (g, text, bounds, juce::Justification::centredLeft, drawColour, 22.0f);

    if (active)
    {
        g.setColour (colour);
        g.fillRect (bounds.getX(), bounds.getBottom() - 3, bounds.getWidth() - 10, 3);
    }
}

void TextTabButton::mouseUp (const juce::MouseEvent& e)
{
    if (getLocalBounds().contains (e.getPosition()) && onClick != nullptr)
        onClick();
}
