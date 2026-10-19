#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "SampleLibrary.h"
#include "FolderTreeItem.h"
#include "SampleListBoxModel.h"
#include "SamplePlayer.h"
#include "Theme.h"

class MainComponent : public juce::Component,
                       public juce::DragAndDropContainer
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    void loadDrumPack();
    void showFolder (const SampleFolderNode* folder);
    void setAccent (Theme::Accent a);
    void rebuildAccentButtons();

    Theme::Accent accent;

    juce::TextButton loadButton { "LOAD DRUM PACK..." };
    juce::Label titleLabel;
    juce::Label statusLabel;
    juce::OwnedArray<juce::TextButton> accentButtons;

    juce::TreeView folderTree;

    SamplePlayer player;
    SampleListBoxModel listModel;
    SampleListBox sampleList;

    std::unique_ptr<SampleFolderNode> libraryRoot;
    std::unique_ptr<FolderTreeItem> rootTreeItem;
    std::unique_ptr<juce::FileChooser> fileChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
