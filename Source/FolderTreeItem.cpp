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

    auto prefix = mightContainSubItems() ? (isOpen() ? "- " : "+ ") : "  ";
    Theme::drawPixelText (g, prefix + node.name, { 4, 0, width - 8, height },
                         juce::Justification::centredLeft, isSelected() ? colour : Theme::dim (colour), 14.0f);
}

void FolderTreeItem::itemClicked (const juce::MouseEvent& e)
{
    if (! isPackRoot || ! e.mods.isPopupMenu())
        return;

    juce::PopupMenu menu;
    menu.addItem (1, "Delete Pack \"" + node.name + "\"...");

    menu.showMenuAsync (juce::PopupMenu::Options(), [this] (int result)
    {
        if (result == 1 && onDeleteRequested != nullptr)
            onDeleteRequested();
    });
}

LibraryRootItem::LibraryRootItem (std::vector<std::unique_ptr<SampleFolderNode>>& packsToShow, const Theme::Accent& accentRef)
    : packs (packsToShow), accent (accentRef)
{
}

void LibraryRootItem::refresh()
{
    clearSubItems();

    auto* favItem = new FavoritesTreeItem (accent);
    favItem->onSelected = onFavoritesSelected;
    addSubItem (favItem);

    for (auto& pack : packs)
    {
        auto* item = new FolderTreeItem (*pack, accent);
        item->isPackRoot = true;
        item->onSelected = onSelected;

        auto* rawNode = pack.get();
        item->onDeleteRequested = [this, rawNode] { if (onDeletePack != nullptr) onDeletePack (rawNode); };

        addSubItem (item);
    }
}
