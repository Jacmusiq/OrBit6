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

juce::File SampleLibrary::getPacksRoot()
{
    auto root = juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
                    .getChildFile ("OrBit6")
                    .getChildFile ("Packs");
    root.createDirectory();
    return root;
}

juce::File SampleLibrary::importPack (const juce::File& sourceFolder)
{
    if (! sourceFolder.isDirectory())
        return {};

    auto root = getPacksRoot();
    auto baseName = sourceFolder.getFileName();
    auto target = root.getChildFile (baseName);

    for (int suffix = 2; target.exists(); ++suffix)
        target = root.getChildFile (baseName + " " + juce::String (suffix));

    if (! sourceFolder.copyDirectoryTo (target))
        return {};

    return target;
}

void SampleLibrary::deletePack (const juce::File& packFolder)
{
    packFolder.deleteRecursively();
}

std::vector<juce::File> SampleLibrary::listStoredPackFolders()
{
    std::vector<juce::File> result;

    for (juce::DirectoryEntry entry : juce::RangedDirectoryIterator (getPacksRoot(), false, "*", juce::File::findDirectories))
        result.push_back (entry.getFile());

    std::sort (result.begin(), result.end(),
               [] (const juce::File& a, const juce::File& b)
               { return a.getFileName().compareIgnoreCase (b.getFileName()) < 0; });

    return result;
}
