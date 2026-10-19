#include "SampleListBoxModel.h"

SampleListBoxModel::SampleListBoxModel (SamplePlayer& playerToUse, const Theme::Accent& accentRef)
    : player (playerToUse), accent (accentRef)
{
}

void SampleListBoxModel::setFiles (const std::vector<juce::File>& newFiles)
{
    files = newFiles;
}

juce::File SampleListBoxModel::getFile (int row) const
{
    if (juce::isPositiveAndBelow (row, (int) files.size()))
        return files[(size_t) row];
    return {};
}

int SampleListBoxModel::getNumRows()
{
    return (int) files.size();
}

void SampleListBoxModel::paintListBoxItem (int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
    auto colour = Theme::colourFor (accent);
    auto file = getFile (rowNumber);
    bool playing = player.isPlaying (file);

    if (playing)
        g.fillAll (colour.withAlpha (0.28f));
    else if (rowIsSelected)
        g.fillAll (colour.withAlpha (0.12f));

    g.setColour (playing || rowIsSelected ? colour : Theme::dim (colour));
    g.setFont (Theme::terminalFont (14.0f));

    auto playMark = playing ? juce::String (juce::CharPointer_UTF8 ("\xe2\x96\xb6 ")) : juce::String ("  ");
    g.drawText (playMark + file.getFileNameWithoutExtension(), 8, 0, width - 160, height, juce::Justification::centredLeft);

    g.setFont (Theme::terminalFont (12.0f));
    g.drawText (player.describeFormat (file), width - 150, 0, 140, height, juce::Justification::centredRight);
}

void SampleListBoxModel::listBoxItemClicked (int row, const juce::MouseEvent&)
{
    auto file = getFile (row);
    if (file == juce::File())
        return;

    if (player.isPlaying (file))
        player.stop();
    else
        player.play (file);
}

SampleListBox::SampleListBox (SampleListBoxModel& modelToUse)
    : juce::ListBox ("SampleList", &modelToUse), model (modelToUse)
{
}

void SampleListBox::mouseDown (const juce::MouseEvent& e)
{
    juce::ListBox::mouseDown (e);
    dragRow = getRowContainingPosition (e.x, e.y);
    dragStarted = false;
}

void SampleListBox::mouseDrag (const juce::MouseEvent& e)
{
    if (dragStarted || dragRow < 0)
        return;

    if (e.getDistanceFromDragStart() < 12)
        return;

    dragStarted = true;
    auto file = model.getFile (dragRow);
    if (file == juce::File())
        return;

    if (auto* container = juce::DragAndDropContainer::findParentDragContainerFor (this))
        container->performExternalDragDropOfFiles ({ file.getFullPathName() }, false, this);
}

void SampleListBox::mouseUp (const juce::MouseEvent& e)
{
    juce::ListBox::mouseUp (e);
    dragRow = -1;
    dragStarted = false;
}
