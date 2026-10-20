#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "SamplePlayer.h"
#include "FavoritesStore.h"
#include "SamplerEngine.h"
#include "BrowserView.h"
#include "SamplerView.h"
#include "TextTabButton.h"
#include "Theme.h"

class MainComponent : public juce::Component,
                       public juce::DragAndDropContainer
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    bool keyPressed (const juce::KeyPress& key) override;

protected:
    bool shouldDropFilesWhenDraggedExternally (const juce::DragAndDropTarget::SourceDetails& sourceDetails,
                                               juce::StringArray& files, bool& canMoveFiles) override;

private:
    enum class Tab { browser, sampler };

    void showTab (Tab t);
    void setAccent (Theme::Accent a);
    void rebuildAccentButtons();
    void loadIntoSampler (const juce::File& file);
    void setStatus (const juce::String& text);

    Theme::Accent accent;

    TextTabButton browserTab { "BROWSER", accent };
    TextTabButton samplerTab { "SAMPLER (BETA)", accent };
    juce::Label statusLabel;
    juce::Label brandLabel;
    juce::OwnedArray<juce::TextButton> accentButtons;

    juce::AudioDeviceManager deviceManager;
    SamplePlayer player;
    FavoritesStore favorites;
    SamplerEngine samplerEngine;

    BrowserView browserView;
    SamplerView samplerView;

    Tab currentTab = Tab::browser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
