#include "MainComponent.h"

MainComponent::MainComponent()
    : accent (Theme::loadAccent()),
      listModel (player, accent),
      sampleList (listModel)
{
    titleLabel.setText ("APOLLO CRATE", juce::dontSendNotification);
    titleLabel.setFont (Theme::terminalFont (24.0f));
    titleLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (titleLabel);

    loadButton.setColour (juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    loadButton.onClick = [this] { loadDrumPack(); };
    addAndMakeVisible (loadButton);

    rebuildAccentButtons();

    folderTree.setColour (juce::TreeView::backgroundColourId, Theme::panel());
    folderTree.setIndentSize (14);
    addAndMakeVisible (folderTree);

    sampleList.setRowHeight (24);
    sampleList.setColour (juce::ListBox::backgroundColourId, Theme::panel());
    addAndMakeVisible (sampleList);

    statusLabel.setText ("LOAD A DRUM PACK FOLDER TO BEGIN", juce::dontSendNotification);
    statusLabel.setFont (Theme::terminalFont (13.0f));
    statusLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (statusLabel);

    player.onPlaybackStateChanged = [this]
    {
        sampleList.repaint();
        if (! player.isPlaying())
            statusLabel.setText ("READY", juce::dontSendNotification);
        statusLabel.setColour (juce::Label::textColourId, Theme::colourFor (accent));
    };

    setAccent (accent);
    setSize (1000, 640);
}

MainComponent::~MainComponent()
{
    folderTree.setRootItem (nullptr);
}

void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (Theme::background());

    auto colour = Theme::colourFor (accent);
    g.setColour (Theme::dim (colour));
    g.drawLine (0.0f, 56.0f, (float) getWidth(), 56.0f, 1.0f);
    g.drawLine (0.0f, (float) getHeight() - 28.0f, (float) getWidth(), (float) getHeight() - 28.0f, 1.0f);
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();

    auto top = bounds.removeFromTop (56).reduced (12, 10);
    titleLabel.setBounds (top.removeFromLeft (240));

    top.removeFromRight (4);
    loadButton.setBounds (top.removeFromRight (160));

    top.removeFromRight (10);
    const int swatchSize = 26;
    const int swatchGap = 4;
    auto cluster = top.removeFromRight (accentButtons.size() * (swatchSize + swatchGap));
    for (auto* b : accentButtons)
    {
        b->setBounds (cluster.removeFromLeft (swatchSize).reduced (2));
        cluster.removeFromLeft (swatchGap);
    }

    auto bottom = bounds.removeFromBottom (28).reduced (12, 4);
    statusLabel.setBounds (bottom);

    bounds.reduce (10, 10);
    folderTree.setBounds (bounds.removeFromLeft (260));
    bounds.removeFromLeft (10);
    sampleList.setBounds (bounds);
}

void MainComponent::loadDrumPack()
{
    fileChooser = std::make_unique<juce::FileChooser> ("Select a drum pack folder", juce::File(), "*");

    fileChooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories,
        [this] (const juce::FileChooser& fc)
        {
            auto folder = fc.getResult();
            if (! folder.isDirectory())
                return;

            folderTree.setRootItem (nullptr);
            rootTreeItem.reset();

            libraryRoot = SampleLibrary::scan (folder);
            if (libraryRoot == nullptr)
                return;

            rootTreeItem = std::make_unique<FolderTreeItem> (*libraryRoot, accent);
            rootTreeItem->onSelected = [this] (const SampleFolderNode* node) { showFolder (node); };
            folderTree.setRootItem (rootTreeItem.get());
            rootTreeItem->setOpen (true);

            showFolder (libraryRoot.get());
        });
}

void MainComponent::showFolder (const SampleFolderNode* folder)
{
    if (folder == nullptr)
        return;

    listModel.setFiles (folder->audioFiles);
    sampleList.updateContent();
    sampleList.repaint();

    statusLabel.setText (folder->name.toUpperCase() + "  —  " + juce::String ((int) folder->audioFiles.size()) + " SAMPLE(S)",
                          juce::dontSendNotification);
}

void MainComponent::setAccent (Theme::Accent a)
{
    accent = a;
    Theme::saveAccent (a);

    auto colour = Theme::colourFor (a);
    titleLabel.setColour (juce::Label::textColourId, colour);
    loadButton.setColour (juce::TextButton::textColourOffId, colour);
    loadButton.setColour (juce::TextButton::buttonColourId, colour.withAlpha (0.08f));
    statusLabel.setColour (juce::Label::textColourId, Theme::dim (colour));

    folderTree.repaint();
    sampleList.repaint();
    repaint();
}

void MainComponent::rebuildAccentButtons()
{
    accentButtons.clear();

    const Theme::Accent all[] { Theme::Accent::green, Theme::Accent::pink, Theme::Accent::white,
                                 Theme::Accent::blue, Theme::Accent::purple };

    for (auto a : all)
    {
        auto* b = accentButtons.add (new juce::TextButton());
        b->setColour (juce::TextButton::buttonColourId, Theme::colourFor (a));
        b->setColour (juce::TextButton::buttonOnColourId, Theme::colourFor (a));
        b->onClick = [this, a] { setAccent (a); };
        b->setTooltip (Theme::nameFor (a));
        addAndMakeVisible (*b);
    }
}
