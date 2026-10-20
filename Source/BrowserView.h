#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "SampleLibrary.h"
#include "FolderTreeItem.h"
#include "SampleListBoxModel.h"
#include "SamplePlayer.h"
#include "FavoritesStore.h"
#include "WaveformComponent.h"
#include "Theme.h"

/** The Browser tab: load drum-pack folders into the app's own storage,
    navigate their subfolders, preview samples, favorite them, and double
    click (or right-click > Load into Sampler) to send one to the Sampler.
*/
class BrowserView : public juce::Component,
                     private juce::ChangeListener
{
public:
    BrowserView (SamplePlayer& playerToUse, FavoritesStore& favoritesToUse, const Theme::Accent& accentRef);
    ~BrowserView() override;

    void resized() override;

    /** Call when the shared accent colour changes. */
    void refreshAccentColours();

    std::function<void (const juce::File&)> onLoadIntoSampler;
    std::function<void (const juce::String&)> onStatusMessage;

private:
    void loadDrumPack();
    void importPackAsync (const juce::File& sourceFolder);
    void refreshLibrary();
    void confirmDeletePack (SampleFolderNode* node);
    void showFolder (const SampleFolderNode* folder);
    void showFavorites();
    void changeListenerCallback (juce::ChangeBroadcaster*) override;

    const Theme::Accent& accent;
    SamplePlayer& player;
    FavoritesStore& favorites;
    bool showingFavorites = false;

    juce::TextButton loadButton { "LOAD DRUM PACK..." };

    juce::TreeView folderTree;
    WaveformComponent waveform;
    SampleListBoxModel listModel;
    SampleListBox sampleList;

    std::vector<std::unique_ptr<SampleFolderNode>> packs;
    std::unique_ptr<LibraryRootItem> libraryRootItem;
    std::unique_ptr<juce::FileChooser> fileChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BrowserView)
};
