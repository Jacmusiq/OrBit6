#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "SampleLibrary.h"
#include "Theme.h"

/** A folder in the drum-pack tree on the left. Selecting one tells the
    owner (via onSelected) to show that folder's samples in the list on
    the right — this is how the user "navigates each drumkit through each
    subfolder".
*/
class FolderTreeItem : public juce::TreeViewItem
{
public:
    FolderTreeItem (const SampleFolderNode& nodeToShow, const Theme::Accent& accentRef);

    bool mightContainSubItems() override { return ! node.subfolders.empty(); }
    void itemOpennessChanged (bool isNowOpen) override;
    void paintItem (juce::Graphics& g, int width, int height) override;
    void itemSelectionChanged (bool isNowSelected) override;

    std::function<void (const SampleFolderNode*)> onSelected;

private:
    const SampleFolderNode& node;
    const Theme::Accent& accent;
};
