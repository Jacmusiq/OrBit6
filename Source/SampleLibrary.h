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
}
