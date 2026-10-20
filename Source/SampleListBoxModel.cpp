#include "SampleListBoxModel.h"

SampleListBoxModel::SampleListBoxModel (SamplePlayer& playerToUse, FavoritesStore& favoritesToUse, const Theme::Accent& accentRef)
    : player (playerToUse), favorites (favoritesToUse), accent (accentRef)
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
    bool fav = favorites.isFavorite (file);

    if (playing)
        g.fillAll (colour.withAlpha (0.28f));
    else if (rowIsSelected)
        g.fillAll (colour.withAlpha (0.12f));

    auto rowColour = playing || rowIsSelected ? colour : Theme::dim (colour);

    auto playMark = playing ? juce::String (juce::CharPointer_UTF8 ("\xe2\x96\xb6 ")) : juce::String ("  ");
    Theme::drawPixelText (g, playMark + file.getFileNameWithoutExtension(), { 8, 0, width - 180, height },
                         juce::Justification::centredLeft, rowColour, 14.0f);

    if (fav)
        Theme::drawPixelText (g, juce::String (juce::CharPointer_UTF8 ("\xe2\x98\x85")), { width - 178, 0, 18, height },
                             juce::Justification::centred, colour, 13.0f);

    Theme::drawPixelText (g, player.describeFormat (file), { width - 150, 0, 140, height },
                         juce::Justification::centredRight, rowColour, 12.0f);
}

void SampleListBoxModel::listBoxItemClicked (int row, const juce::MouseEvent& e)
{
    auto file = getFile (row);
    if (file == juce::File())
        return;

    if (e.mods.isPopupMenu())
    {
        showContextMenu (row);
        return;
    }

    // Every click retriggers from the start, so you can click as many times
    // as you like — no toggling to stop on a repeat click of the same row.
    player.play (file);
}

void SampleListBoxModel::showContextMenu (int row)
{
    auto file = getFile (row);
    if (file == juce::File())
        return;

    bool fav = favorites.isFavorite (file);

    juce::PopupMenu menu;
    menu.addItem (1, fav ? "Remove from Favorites" : "Add to Favorites");
    menu.addItem (2, "Load into Sampler");

    menu.showMenuAsync (juce::PopupMenu::Options(), [this, file, fav] (int result)
    {
        if (result == 1)
            favorites.setFavorite (file, ! fav);
        else if (result == 2 && onLoadIntoSampler != nullptr)
            onLoadIntoSampler (file);
    });
}

juce::var SampleListBoxModel::getDragSourceDescription (const juce::SparseSet<int>& rowsToDescribe)
{
    if (rowsToDescribe.isEmpty())
        return {};

    auto file = getFile (rowsToDescribe[0]);
    if (file == juce::File())
        return {};

    return juce::String (sampleDragMarker) + file.getFullPathName();
}
