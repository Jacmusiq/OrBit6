#include "MainComponent.h"
#include "SampleListBoxModel.h"

namespace
{
    constexpr int bezelMargin = 14;
}

MainComponent::MainComponent()
    : accent (Theme::loadAccent()),
      player (deviceManager),
      samplerEngine (deviceManager),
      browserView (player, favorites, accent),
      samplerView (samplerEngine, accent)
{
    deviceManager.initialiseWithDefaultDevices (0, 2);

    setWantsKeyboardFocus (true);

    browserTab.onClick = [this] { showTab (Tab::browser); };
    addAndMakeVisible (browserTab);

    samplerTab.onClick = [this] { showTab (Tab::sampler); };
    addAndMakeVisible (samplerTab);

    rebuildAccentButtons();

    addChildComponent (browserView);
    addChildComponent (samplerView);

    browserView.onLoadIntoSampler = [this] (const juce::File& f) { loadIntoSampler (f); };
    browserView.onStatusMessage = [this] (const juce::String& s) { setStatus (s); };

    samplerView.onStatusMessage = [this] (const juce::String& s) { setStatus (s); };

    statusLabel.setFont (Theme::terminalFont (13.0f));
    statusLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (statusLabel);

    brandLabel.setText ("OrBit 6", juce::dontSendNotification);
    brandLabel.setFont (Theme::terminalFont (13.0f));
    brandLabel.setJustificationType (juce::Justification::centredRight);
    addAndMakeVisible (brandLabel);

    setAccent (accent);
    showTab (Tab::browser);

    setSize (1080, 840);
}

MainComponent::~MainComponent()
{
}

bool MainComponent::keyPressed (const juce::KeyPress& key)
{
    if (key == juce::KeyPress::spaceKey)
    {
        if (player.isPlaying())
            player.stop();
        else
            player.resume();

        return true;
    }

    return false;
}

bool MainComponent::shouldDropFilesWhenDraggedExternally (const juce::DragAndDropTarget::SourceDetails& sourceDetails,
                                                           juce::StringArray& files, bool& canMoveFiles)
{
    auto description = sourceDetails.description.toString();
    static const juce::String marker (sampleDragMarker);

    if (! description.startsWith (marker))
        return false;

    files.add (description.substring (marker.length()));
    canMoveFiles = false;
    return true;
}

void MainComponent::paint (juce::Graphics& g)
{
    auto full = getLocalBounds();

    g.setColour (Theme::chassis());
    g.fillRect (full);

    auto screen = full.reduced (bezelMargin);

    g.setColour (Theme::chassisShadow().withAlpha (0.6f));
    g.drawRect (screen.expanded (2), 1);
    g.setColour (Theme::chassisHighlight().withAlpha (0.5f));
    g.drawRect (screen.expanded (1), 1);

    auto drawScrew = [&] (juce::Point<int> p)
    {
        g.setColour (juce::Colours::black.withAlpha (0.5f));
        g.fillEllipse ((float) p.x - 3.5f, (float) p.y - 3.5f, 7.0f, 7.0f);
        g.setColour (Theme::chassisHighlight().withAlpha (0.6f));
        g.fillEllipse ((float) p.x - 2.0f, (float) p.y - 3.0f, 2.5f, 2.5f);
    };
    drawScrew ({ bezelMargin / 2, bezelMargin / 2 });
    drawScrew ({ full.getWidth() - bezelMargin / 2, bezelMargin / 2 });
    drawScrew ({ bezelMargin / 2, full.getHeight() - bezelMargin / 2 });
    drawScrew ({ full.getWidth() - bezelMargin / 2, full.getHeight() - bezelMargin / 2 });

    g.setColour (Theme::background());
    g.fillRect (screen);

    g.setTiledImageFill (Theme::noiseTexture(), screen.getX(), screen.getY(), 1.0f);
    g.fillRect (screen);

    juce::ColourGradient vignette (juce::Colours::transparentBlack,
                                   (float) screen.getCentreX(), (float) screen.getCentreY(),
                                   juce::Colours::black.withAlpha (0.5f),
                                   (float) screen.getX(), (float) screen.getY(), true);
    g.setGradientFill (vignette);
    g.fillRect (screen);

    auto colour = Theme::colourFor (accent);
    g.setColour (Theme::dim (colour));
    g.drawLine ((float) screen.getX(), (float) screen.getY() + 56.0f, (float) screen.getRight(), (float) screen.getY() + 56.0f, 1.0f);
    g.drawLine ((float) screen.getX(), (float) screen.getBottom() - 28.0f, (float) screen.getRight(), (float) screen.getBottom() - 28.0f, 1.0f);
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds().reduced (bezelMargin);

    auto top = bounds.removeFromTop (56).reduced (12, 10);

    top.removeFromRight (4);
    const int swatchSize = 26;
    const int swatchGap = 4;
    auto swatchCluster = top.removeFromRight (accentButtons.size() * (swatchSize + swatchGap));
    for (auto* b : accentButtons)
    {
        b->setBounds (swatchCluster.removeFromLeft (swatchSize).reduced (2));
        swatchCluster.removeFromLeft (swatchGap);
    }

    top.removeFromRight (20);
    browserTab.setBounds (top.removeFromLeft (110));
    top.removeFromLeft (28);
    samplerTab.setBounds (top.removeFromLeft (220));

    auto bottom = bounds.removeFromBottom (28).reduced (12, 4);
    brandLabel.setBounds (bottom.removeFromRight (100));
    bottom.removeFromRight (8);
    statusLabel.setBounds (bottom);

    bounds.reduce (10, 10);
    browserView.setBounds (bounds);
    samplerView.setBounds (bounds);
}

void MainComponent::showTab (Tab t)
{
    currentTab = t;

    browserView.setVisible (t == Tab::browser);
    samplerView.setVisible (t == Tab::sampler);

    browserTab.setActive (t == Tab::browser);
    samplerTab.setActive (t == Tab::sampler);

    if (t == Tab::sampler)
        samplerView.grabKeyboardFocusForPlaying();
    else
        grabKeyboardFocus();
}

void MainComponent::setAccent (Theme::Accent a)
{
    accent = a;
    Theme::saveAccent (a);

    auto colour = Theme::colourFor (a);
    statusLabel.setColour (juce::Label::textColourId, Theme::dim (colour));
    brandLabel.setColour (juce::Label::textColourId, Theme::dim (colour));

    browserView.refreshAccentColours();
    samplerView.refreshAccentColours();

    browserTab.repaint();
    samplerTab.repaint();
    repaint();
}

void MainComponent::loadIntoSampler (const juce::File& file)
{
    if (samplerEngine.loadSample (file))
    {
        samplerView.sampleWasLoaded();
        showTab (Tab::sampler);
    }
    else
    {
        setStatus ("COULD NOT LOAD " + file.getFileName().toUpperCase() + " INTO SAMPLER");
    }
}

void MainComponent::setStatus (const juce::String& text)
{
    statusLabel.setText (text, juce::dontSendNotification);
}

void MainComponent::rebuildAccentButtons()
{
    accentButtons.clear();

    const Theme::Accent all[] { Theme::Accent::green, Theme::Accent::pink, Theme::Accent::white,
                                 Theme::Accent::blue, Theme::Accent::purple };

    for (auto a : all)
    {
        auto* b = accentButtons.add (new juce::TextButton());
        b->setColour (juce::TextButton::buttonColourId, Theme::colourFor (a));
        b->setColour (juce::TextButton::buttonOnColourId, Theme::colourFor (a));
        b->setWantsKeyboardFocus (false);
        b->onClick = [this, a] { setAccent (a); };
        b->setTooltip (Theme::nameFor (a));
        addAndMakeVisible (*b);
    }
}
