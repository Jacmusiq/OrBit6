#include "FavoritesStore.h"
#include "SampleLibrary.h"
#include <algorithm>

FavoritesStore::FavoritesStore()
{
    load();
}

juce::File FavoritesStore::storeFile() const
{
    return SampleLibrary::getPacksRoot().getParentDirectory().getChildFile ("favorites.txt");
}

void FavoritesStore::load()
{
    favoritePaths.clear();

    auto f = storeFile();
    if (! f.existsAsFile())
        return;

    for (auto& line : juce::StringArray::fromLines (f.loadFileAsString()))
        if (line.isNotEmpty())
            favoritePaths.insert (line);
}

void FavoritesStore::save() const
{
    juce::StringArray lines;
    for (auto& p : favoritePaths)
        lines.add (p);

    storeFile().replaceWithText (lines.joinIntoString ("\n"));
}

bool FavoritesStore::isFavorite (const juce::File& file) const
{
    return favoritePaths.count (file.getFullPathName()) > 0;
}

void FavoritesStore::setFavorite (const juce::File& file, bool shouldBeFavorite)
{
    auto path = file.getFullPathName();

    if (shouldBeFavorite)
        favoritePaths.insert (path);
    else
        favoritePaths.erase (path);

    save();
    sendChangeMessage();
}

std::vector<juce::File> FavoritesStore::getAll() const
{
    std::vector<juce::File> result;

    for (auto& p : favoritePaths)
    {
        juce::File f (p);
        if (f.existsAsFile())
            result.push_back (f);
    }

    std::sort (result.begin(), result.end(),
               [] (const juce::File& a, const juce::File& b)
               { return a.getFileName().compareIgnoreCase (b.getFileName()) < 0; });

    return result;
}
