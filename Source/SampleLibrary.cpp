#include "SampleLibrary.h"
#include <algorithm>

bool SampleLibrary::isSupportedAudioFile (const juce::File& f)
{
    static const juce::StringArray exts { ".wav", ".wave", ".aif", ".aiff", ".flac", ".ogg", ".mp3" };
    return exts.contains (f.getFileExtension().toLowerCase());
}

static bool hasAudioUnderneath (const juce::File& folder)
{
    for (juce::DirectoryEntry entry : juce::RangedDirectoryIterator (folder, true, "*", juce::File::findFiles))
        if (SampleLibrary::isSupportedAudioFile (entry.getFile()))
            return true;
    return false;
}

std::unique_ptr<SampleFolderNode> SampleLibrary::scan (const juce::File& root)
{
    if (! root.isDirectory())
        return nullptr;

    auto node = std::make_unique<SampleFolderNode>();
    node->name = root.getFileName();
    node->folder = root;

    for (juce::DirectoryEntry entry : juce::RangedDirectoryIterator (root, false, "*", juce::File::findFilesAndDirectories))
    {
        const auto& f = entry.getFile();

        if (f.isDirectory())
        {
            if (hasAudioUnderneath (f))
                if (auto child = scan (f))
                    node->subfolders.push_back (std::move (child));
        }
        else if (isSupportedAudioFile (f))
        {
            node->audioFiles.push_back (f);
        }
    }

    std::sort (node->audioFiles.begin(), node->audioFiles.end(),
               [] (const juce::File& a, const juce::File& b)
               { return a.getFileName().compareIgnoreCase (b.getFileName()) < 0; });

    std::sort (node->subfolders.begin(), node->subfolders.end(),
               [] (const auto& a, const auto& b) { return a->name.compareIgnoreCase (b->name) < 0; });

    return node;
}
