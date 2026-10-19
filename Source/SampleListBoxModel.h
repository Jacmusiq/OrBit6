#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "SampleLibrary.h"
#include "SamplePlayer.h"
#include "Theme.h"

/** Drives the right-hand sample list: one click previews a sample instantly
    (stopping whatever was playing before), and dragging a row out of the
    window starts a native OS file drag so it can be dropped straight onto a
    track or sampler in Logic Pro X.
*/
class SampleListBoxModel : public juce::ListBoxModel
{
public:
    SampleListBoxModel (SamplePlayer& playerToUse, const Theme::Accent& accentRef);

    void setFiles (const std::vector<juce::File>& newFiles);
    juce::File getFile (int row) const;

    int getNumRows() override;
    void paintListBoxItem (int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void listBoxItemClicked (int row, const juce::MouseEvent&) override;

private:
    SamplePlayer& player;
    const Theme::Accent& accent;
    std::vector<juce::File> files;
};

/** juce::ListBox doesn't support starting a native external file drag out of
    the box (its built-in drag support is for reordering rows within the app),
    so this subclass adds it: holding and moving the mouse past a small
    threshold on a row kicks off performExternalDragDropOfFiles for that row's
    sample.
*/
class SampleListBox : public juce::ListBox
{
public:
    explicit SampleListBox (SampleListBoxModel& modelToUse);

    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseUp (const juce::MouseEvent& e) override;

private:
    SampleListBoxModel& model;
    int dragRow = -1;
    bool dragStarted = false;
};
