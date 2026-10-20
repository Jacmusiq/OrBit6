#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "SampleLibrary.h"
#include "SamplePlayer.h"
#include "FavoritesStore.h"
#include "Theme.h"

/** The marker prefix on a drag description that identifies it as one of our
    sample rows, so MainComponent::shouldDropFilesWhenDraggedExternally knows
    to turn it into a real file drag. Shared with MainComponent.cpp.
*/
static constexpr const char* sampleDragMarker = "APOLLO_SAMPLE:";

/** Drives a list of sample rows (used for both a pack folder's contents and
    the flat Favorites list): every click (re)triggers the sample from the
    start — click as many times as you like — right click offers Add/Remove
    Favorite and Load into Sampler, and dragging a row out starts a native OS
    file drag so it can be dropped onto the Desktop, Finder, Logic Pro X, or
    any other DAW.
*/
class SampleListBoxModel : public juce::ListBoxModel
{
public:
    SampleListBoxModel (SamplePlayer& playerToUse, FavoritesStore& favoritesToUse, const Theme::Accent& accentRef);

    void setFiles (const std::vector<juce::File>& newFiles);
    juce::File getFile (int row) const;

    int getNumRows() override;
    void paintListBoxItem (int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void listBoxItemClicked (int row, const juce::MouseEvent&) override;

    juce::var getDragSourceDescription (const juce::SparseSet<int>& rowsToDescribe) override;

    /** Fired when the user picks "Load into Sampler" from a row's right-click menu. */
    std::function<void (const juce::File&)> onLoadIntoSampler;

private:
    void showContextMenu (int row);

    SamplePlayer& player;
    FavoritesStore& favorites;
    const Theme::Accent& accent;
    std::vector<juce::File> files;
};

using SampleListBox = juce::ListBox;
