#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "SampleLibrary.h"
#include "Theme.h"

/** A folder in the drum-pack tree on the left. Selecting one tells the
    owner (via onSelected) to show that folder's samples in the list on
    the right — this is how the user "navigates each drumkit through each
    subfolder". A top-level pack (isPackRoot) also offers a right-click
    "Delete Pack..." menu.
*/
class FolderTreeItem : public juce::TreeViewItem
{
public:
    FolderTreeItem (const SampleFolderNode& nodeToShow, const Theme::Accent& accentRef);

    bool mightContainSubItems() override { return ! node.subfolders.empty(); }
    void itemOpennessChanged (bool isNowOpen) override;
    void paintItem (juce::Graphics& g, int width, int height) override;
    void itemSelectionChanged (bool isNowSelected) override;
    void itemClicked (const juce::MouseEvent& e) override;

    std::function<void (const SampleFolderNode*)> onSelected;
    std::function<void()> onDeleteRequested;
    bool isPackRoot = false;

private:
    const SampleFolderNode& node;
    const Theme::Accent& accent;
};

/** The "★ FAVORITES" entry, always first in the tree — selecting it shows
    every favorited sample across every pack in the sample list.
*/
class FavoritesTreeItem : public juce::TreeViewItem
{
public:
    explicit FavoritesTreeItem (const Theme::Accent& accentRef) : accent (accentRef) {}

    bool mightContainSubItems() override { return false; }
    void itemSelectionChanged (bool isNowSelected) override
    {
        if (isNowSelected && onSelected != nullptr)
            onSelected();
    }

    void paintItem (juce::Graphics& g, int width, int height) override
    {
        auto colour = Theme::colourFor (accent);
        g.fillAll (isSelected() ? colour.withAlpha (0.18f) : juce::Colours::transparentBlack);
        Theme::drawPixelText (g, juce::String (juce::CharPointer_UTF8 ("\xe2\x98\x85 FAVORITES")),
                             { 4, 0, width - 8, height }, juce::Justification::centredLeft, colour, 14.0f);
    }

    std::function<void()> onSelected;

private:
    const Theme::Accent& accent;
};

/** An invisible root whose children are the Favorites entry plus the
    imported packs, so the TreeView (which only ever has one root item) can
    show them all side by side as top-level entries. Call refresh() whenever
    the pack list changes.
*/
class LibraryRootItem : public juce::TreeViewItem
{
public:
    LibraryRootItem (std::vector<std::unique_ptr<SampleFolderNode>>& packsToShow, const Theme::Accent& accentRef);

    bool mightContainSubItems() override { return true; }

    /** Rebuilds the child items from the current pack list. */
    void refresh();

    std::function<void (const SampleFolderNode*)> onSelected;
    std::function<void()> onFavoritesSelected;
    std::function<void (SampleFolderNode*)> onDeletePack;

private:
    std::vector<std::unique_ptr<SampleFolderNode>>& packs;
    const Theme::Accent& accent;
};
