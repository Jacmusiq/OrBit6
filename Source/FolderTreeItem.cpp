#include "FolderTreeItem.h"

FolderTreeItem::FolderTreeItem (const SampleFolderNode& nodeToShow, const Theme::Accent& accentRef)
    : node (nodeToShow), accent (accentRef)
{
}

void FolderTreeItem::itemOpennessChanged (bool isNowOpen)
{
    if (isNowOpen && getNumSubItems() == 0)
    {
        for (auto& sub : node.subfolders)
        {
            auto* item = new FolderTreeItem (*sub, accent);
            item->onSelected = onSelected;
            addSubItem (item);
        }
    }
}

void FolderTreeItem::itemSelectionChanged (bool isNowSelected)
{
    if (isNowSelected && onSelected != nullptr)
        onSelected (&node);
}

void FolderTreeItem::paintItem (juce::Graphics& g, int width, int height)
{
    auto colour = Theme::colourFor (accent);
    g.fillAll (isSelected() ? colour.withAlpha (0.18f) : juce::Colours::transparentBlack);

    g.setColour (isSelected() ? colour : Theme::dim (colour));
    g.setFont (Theme::terminalFont (14.0f));

    auto prefix = mightContainSubItems() ? (isOpen() ? "- " : "+ ") : "  ";
    g.drawText (prefix + node.name, 4, 0, width - 8, height, juce::Justification::centredLeft);
}
