#pragma once

#include <juce_core/juce_core.h>
#include <memory>
#include <vector>

/** One folder in a drum pack: its direct audio-file children, plus its
    subfolders (recursively scanned). Mirrors how drum packs ship on disk —
    e.g. "808 Pack/Kicks/", "808 Pack/Snares/" — so the UI can let the user
    drill down through nested kits.
*/
struct SampleFolderNode
{
    juce::String name;
    juce::File folder;
    std::vector<juce::File> audioFiles;
    std::vector<std::unique_ptr<SampleFolderNode>> subfolders;
};

namespace SampleLibrary
{
    /** Audio file extensions this app knows how to preview/export. JUCE's
        format readers cover 8/16/24/32-bit PCM and 32-bit float within these.
    */
    bool isSupportedAudioFile (const juce::File& f);

    /** Recursively scans `root`, returning its tree of subfolders and the
        audio files directly inside each. Folders with no audio anywhere
        underneath them are skipped.
    */
    std::unique_ptr<SampleFolderNode> scan (const juce::File& root);

    /** Where imported packs live for the current macOS user account:
        ~/Library/Application Support/OrBit6/Packs. Created on first use.
    */
    juce::File getPacksRoot();

    /** Copies `sourceFolder` (recursively) into the packs root under a unique
        name, so the pack survives independently of wherever the user
        originally picked it from. Returns the new on-disk location, or an
        invalid File if the copy failed.
    */
    juce::File importPack (const juce::File& sourceFolder);

    /** Permanently deletes a previously imported pack folder (and everything
        in it) from the packs root.
    */
    void deletePack (const juce::File& packFolder);

    /** Lists the top-level folders directly under the packs root — one per
        previously imported pack.
    */
    std::vector<juce::File> listStoredPackFolders();
}
