#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <unordered_set>
#include <vector>

/** The set of favorited sample files, persisted to a small text file under
    the app's storage. Broadcasts a change message whenever the set changes,
    so every view showing a favorite star or a favorites list can stay
    in sync.
*/
class FavoritesStore : public juce::ChangeBroadcaster
{
public:
    FavoritesStore();

    bool isFavorite (const juce::File& file) const;
    void setFavorite (const juce::File& file, bool shouldBeFavorite);

    /** All favorited files that still exist on disk, sorted by name. */
    std::vector<juce::File> getAll() const;

private:
    juce::File storeFile() const;
    void load();
    void save() const;

    std::unordered_set<juce::String> favoritePaths;
};
