#include "BrowserView.h"
#include <thread>

BrowserView::BrowserView (SamplePlayer& playerToUse, FavoritesStore& favoritesToUse, const Theme::Accent& accentRef)
    : accent (accentRef), player (playerToUse), favorites (favoritesToUse),
      waveform (player, accent), listModel (player, favoritesToUse, accent),
      sampleList ("SampleList", &listModel)
{
    favorites.addChangeListener (this);

    loadButton.setColour (juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    loadButton.setWantsKeyboardFocus (false);
    loadButton.onClick = [this] { loadDrumPack(); };
    addAndMakeVisible (loadButton);

    addAndMakeVisible (waveform);

    folderTree.setColour (juce::TreeView::backgroundColourId, Theme::panel());
    folderTree.setIndentSize (14);
    addAndMakeVisible (folderTree);

    listModel.onLoadIntoSampler = [this] (const juce::File& f)
    {
        if (onLoadIntoSampler != nullptr)
            onLoadIntoSampler (f);
    };

    sampleList.setRowHeight (24);
    sampleList.setColour (juce::ListBox::backgroundColourId, Theme::panel());
    addAndMakeVisible (sampleList);

    player.onFileStarted = [this] (const juce::File& file) { waveform.setFile (file); };

    player.onPlaybackStateChanged = [this]
    {
        sampleList.repaint();
        waveform.repaint();

        if (! player.isPlaying() && onStatusMessage != nullptr)
            onStatusMessage (player.isPaused() ? "PAUSED — PRESS SPACE TO RESUME" : "READY");
    };

    refreshAccentColours();
    refreshLibrary();
}

BrowserView::~BrowserView()
{
    favorites.removeChangeListener (this);
    folderTree.setRootItem (nullptr);
}

void BrowserView::changeListenerCallback (juce::ChangeBroadcaster*)
{
    sampleList.repaint();
    if (showingFavorites)
        showFavorites();
}

void BrowserView::refreshAccentColours()
{
    auto colour = Theme::colourFor (accent);
    loadButton.setColour (juce::TextButton::textColourOffId, colour);
    loadButton.setColour (juce::TextButton::buttonColourId, colour.withAlpha (0.08f));

    folderTree.repaint();
    sampleList.repaint();
    waveform.repaint();
}

void BrowserView::resized()
{
    auto bounds = getLocalBounds();

    auto top = bounds.removeFromTop (34);
    loadButton.setBounds (top.removeFromLeft (170));

    bounds.removeFromTop (8);
    auto waveArea = bounds.removeFromTop (100);
    waveform.setBounds (waveArea);

    bounds.removeFromTop (8);
    folderTree.setBounds (bounds.removeFromLeft (260));
    bounds.removeFromLeft (10);
    sampleList.setBounds (bounds);
}

void BrowserView::loadDrumPack()
{
    fileChooser = std::make_unique<juce::FileChooser> ("Select a drum pack folder", juce::File(), "*");

    fileChooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories,
        [this] (const juce::FileChooser& fc)
        {
            auto folder = fc.getResult();
            if (! folder.isDirectory())
                return;

            importPackAsync (folder);
        });
}

void BrowserView::importPackAsync (const juce::File& sourceFolder)
{
    loadButton.setEnabled (false);
    if (onStatusMessage != nullptr)
        onStatusMessage ("IMPORTING " + sourceFolder.getFileName().toUpperCase() + "...");

    juce::Component::SafePointer<BrowserView> safeThis (this);

    std::thread ([safeThis, sourceFolder]
    {
        auto imported = SampleLibrary::importPack (sourceFolder);

        juce::MessageManager::callAsync ([safeThis, imported]
        {
            if (safeThis == nullptr)
                return;

            safeThis->loadButton.setEnabled (true);

            if (imported == juce::File())
            {
                if (safeThis->onStatusMessage != nullptr)
                    safeThis->onStatusMessage ("IMPORT FAILED");
                return;
            }

            safeThis->refreshLibrary();
        });
    }).detach();
}

void BrowserView::refreshLibrary()
{
    folderTree.setRootItem (nullptr);
    libraryRootItem.reset();
    packs.clear();

    for (auto& folder : SampleLibrary::listStoredPackFolders())
        if (auto node = SampleLibrary::scan (folder))
            packs.push_back (std::move (node));

    libraryRootItem = std::make_unique<LibraryRootItem> (packs, accent);
    libraryRootItem->onSelected = [this] (const SampleFolderNode* node) { showFolder (node); };
    libraryRootItem->onFavoritesSelected = [this] { showFavorites(); };
    libraryRootItem->onDeletePack = [this] (SampleFolderNode* node) { confirmDeletePack (node); };
    libraryRootItem->refresh();

    folderTree.setRootItem (libraryRootItem.get());
    folderTree.setRootItemVisible (false);
    libraryRootItem->setOpen (true);

    listModel.setFiles ({});
    sampleList.updateContent();

    if (onStatusMessage != nullptr)
    {
        onStatusMessage (packs.empty()
            ? "LOAD A DRUM PACK FOLDER TO BEGIN"
            : juce::String ((int) packs.size()) + " PACK(S) — SELECT A FOLDER (RIGHT-CLICK A PACK TO DELETE)");
    }
}

void BrowserView::confirmDeletePack (SampleFolderNode* node)
{
    if (node == nullptr)
        return;

    auto folder = node->folder;
    auto name = node->name;
    juce::Component::SafePointer<BrowserView> safeThis (this);

    juce::NativeMessageBox::showAsync (
        juce::MessageBoxOptions()
            .withIconType (juce::MessageBoxIconType::WarningIcon)
            .withTitle ("Delete Pack")
            .withMessage ("Delete \"" + name + "\" and all its samples? This can't be undone.")
            .withButton ("Delete")
            .withButton ("Cancel")
            .withAssociatedComponent (this),
        [safeThis, folder] (int result)
        {
            if (result != 0 || safeThis == nullptr)
                return;

            SampleLibrary::deletePack (folder);
            safeThis->refreshLibrary();
        });
}

void BrowserView::showFolder (const SampleFolderNode* folder)
{
    if (folder == nullptr)
        return;

    showingFavorites = false;
    listModel.setFiles (folder->audioFiles);
    sampleList.updateContent();
    sampleList.repaint();

    if (onStatusMessage != nullptr)
        onStatusMessage (folder->name.toUpperCase() + "  —  " + juce::String ((int) folder->audioFiles.size()) + " SAMPLE(S)");
}

void BrowserView::showFavorites()
{
    showingFavorites = true;
    auto all = favorites.getAll();
    listModel.setFiles (all);
    sampleList.updateContent();
    sampleList.repaint();

    if (onStatusMessage != nullptr)
        onStatusMessage ("FAVORITES  —  " + juce::String ((int) all.size()) + " SAMPLE(S)");
}
