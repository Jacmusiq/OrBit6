#include "SamplerView.h"
#include "SampleLibrary.h"

namespace
{
    void placeLabeledSlider (SamplerView::LabeledSlider&, juce::Rectangle<int>);
}

SamplerView::SamplerView (SamplerEngine& engineToUse, const Theme::Accent& accentRef)
    : engine (engineToUse), accent (accentRef), waveform (accentRef), keyboard (engineToUse, accentRef)
{
    sampleNameLabel.setText ("NO SAMPLE LOADED", juce::dontSendNotification);
    sampleNameLabel.setFont (Theme::terminalFont (16.0f));
    sampleNameLabel.setColour (juce::Label::textColourId, Theme::colourFor (accent));
    addAndMakeVisible (sampleNameLabel);

    addAndMakeVisible (waveform);

    setUpSlider (startTrim, "START %", 0.0, 99.0, 0.0, 1.0);
    setUpSlider (endTrim, "END %", 1.0, 100.0, 100.0, 1.0);
    setUpSlider (gain, "GAIN dB", -24.0, 12.0, 0.0, 0.5);
    setUpSlider (tune, "TUNE st", -24.0, 24.0, 0.0, 1.0);
    setUpSlider (attack, "ATTACK s", 0.0, 2.0, 0.01, 0.01);
    setUpSlider (decay, "DECAY s", 0.0, 2.0, 0.15, 0.01);
    setUpSlider (sustain, "SUSTAIN", 0.0, 1.0, 0.8, 0.01);
    setUpSlider (release, "RELEASE s", 0.0, 3.0, 0.25, 0.01);
    setUpSlider (tapeAmount, "AMOUNT", 0.0, 100.0, 50.0, 1.0);

    startTrim.slider.onValueChange = [this] { updateTrim(); };
    endTrim.slider.onValueChange = [this] { updateTrim(); };
    gain.slider.onValueChange = [this] { engine.setGainDb ((float) gain.slider.getValue()); };
    tune.slider.onValueChange = [this] { engine.setSemitoneOffset ((int) tune.slider.getValue()); };
    attack.slider.onValueChange = [this] { updateADSR(); };
    decay.slider.onValueChange = [this] { updateADSR(); };
    sustain.slider.onValueChange = [this] { updateADSR(); };
    release.slider.onValueChange = [this] { updateADSR(); };
    updateADSR();

    reverseButton.onClick = [this] { engine.setReversed (reverseButton.getToggleState()); };
    addAndMakeVisible (reverseButton);

    tapeButton.onClick = [this] { engine.setTapeEnabled (tapeButton.getToggleState()); };
    addAndMakeVisible (tapeButton);

    tapeStyleBox.addItem ("Reel-to-Reel", 1);
    tapeStyleBox.addItem ("Cassette", 2);
    tapeStyleBox.addItem ("Lo-Fi Boombox", 3);
    tapeStyleBox.setSelectedId (1, juce::dontSendNotification);
    tapeStyleBox.onChange = [this]
    {
        auto id = tapeStyleBox.getSelectedId();
        engine.setTapeStyle (id == 2 ? TapeStyle::cassette : id == 3 ? TapeStyle::lofi : TapeStyle::reelToReel);
    };
    addAndMakeVisible (tapeStyleBox);

    tapeAmount.slider.onValueChange = [this] { engine.setTapeAmount ((float) (tapeAmount.slider.getValue() / 100.0)); };
    engine.setTapeAmount (0.5f);

    recordButton.onClick = [this] { toggleRecording(); };
    addAndMakeVisible (recordButton);

    recordStatusLabel.setFont (Theme::terminalFont (12.0f));
    recordStatusLabel.setColour (juce::Label::textColourId, Theme::dim (Theme::colourFor (accent)));
    addAndMakeVisible (recordStatusLabel);

    addAndMakeVisible (keyboard);
}

void SamplerView::setUpSlider (LabeledSlider& ls, const juce::String& text, double minV, double maxV, double defaultV, double step)
{
    auto colour = Theme::colourFor (accent);

    ls.label.setText (text, juce::dontSendNotification);
    ls.label.setFont (Theme::terminalFont (11.0f));
    ls.label.setColour (juce::Label::textColourId, Theme::dim (colour));
    addAndMakeVisible (ls.label);

    ls.slider.setSliderStyle (juce::Slider::LinearHorizontal);
    ls.slider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 52, 18);
    ls.slider.setRange (minV, maxV, step);
    ls.slider.setValue (defaultV, juce::dontSendNotification);
    ls.slider.setColour (juce::Slider::thumbColourId, colour);
    ls.slider.setColour (juce::Slider::trackColourId, colour.withAlpha (0.5f));
    ls.slider.setColour (juce::Slider::backgroundColourId, Theme::panel());
    ls.slider.setColour (juce::Slider::textBoxTextColourId, colour);
    ls.slider.setColour (juce::Slider::textBoxOutlineColourId, Theme::dim (colour));
    addAndMakeVisible (ls.slider);
}

void SamplerView::sampleWasLoaded()
{
    auto file = engine.getLoadedFile();
    sampleNameLabel.setText (file.getFileName(), juce::dontSendNotification);
    waveform.setFile (file);

    startTrim.slider.setValue (0.0, juce::dontSendNotification);
    endTrim.slider.setValue (100.0, juce::dontSendNotification);
    updateTrim();

    reverseButton.setToggleState (false, juce::dontSendNotification);
    engine.setReversed (false);

    tune.slider.setValue (0.0, juce::dontSendNotification);
    engine.setSemitoneOffset (0);

    gain.slider.setValue (0.0, juce::dontSendNotification);
    engine.setGainDb (0.0f);

    if (onStatusMessage != nullptr)
        onStatusMessage ("LOADED INTO SAMPLER: " + file.getFileName().toUpperCase());
}

void SamplerView::grabKeyboardFocusForPlaying()
{
    keyboard.grabKeyboardFocus();
}

void SamplerView::refreshAccentColours()
{
    auto colour = Theme::colourFor (accent);

    sampleNameLabel.setColour (juce::Label::textColourId, colour);
    recordStatusLabel.setColour (juce::Label::textColourId, Theme::dim (colour));

    for (auto* ls : { &startTrim, &endTrim, &gain, &tune, &attack, &decay, &sustain, &release, &tapeAmount })
    {
        ls->label.setColour (juce::Label::textColourId, Theme::dim (colour));
        ls->slider.setColour (juce::Slider::thumbColourId, colour);
        ls->slider.setColour (juce::Slider::trackColourId, colour.withAlpha (0.5f));
        ls->slider.setColour (juce::Slider::textBoxTextColourId, colour);
        ls->slider.setColour (juce::Slider::textBoxOutlineColourId, Theme::dim (colour));
    }

    waveform.repaint();
    keyboard.repaint();
}

void SamplerView::updateTrim()
{
    auto s = (float) (startTrim.slider.getValue() / 100.0);
    auto e = (float) (endTrim.slider.getValue() / 100.0);

    if (e <= s)
    {
        e = juce::jmin (1.0f, s + 0.01f);
        endTrim.slider.setValue (e * 100.0, juce::dontSendNotification);
    }

    engine.setTrim (s, e);
    waveform.setTrim (s, e);
}

void SamplerView::updateADSR()
{
    engine.setADSR ((float) attack.slider.getValue(), (float) decay.slider.getValue(),
                    (float) sustain.slider.getValue(), (float) release.slider.getValue());
}

void SamplerView::toggleRecording()
{
    if (engine.isRecording())
    {
        engine.stopRecording();
        recordButton.setButtonText (juce::String (juce::CharPointer_UTF8 ("\xe2\x97\x8f")) + " REC");
        recordStatusLabel.setText ("SAVED: " + lastRecordingFile.getFileName(), juce::dontSendNotification);

        if (onStatusMessage != nullptr)
            onStatusMessage ("RECORDING SAVED: " + lastRecordingFile.getFullPathName());
        return;
    }

    auto dir = SampleLibrary::getPacksRoot().getParentDirectory().getChildFile ("Recordings");
    dir.createDirectory();
    auto name = "Take-" + juce::Time::getCurrentTime().formatted ("%Y%m%d-%H%M%S") + ".wav";
    lastRecordingFile = dir.getChildFile (name);

    if (engine.startRecording (lastRecordingFile))
    {
        recordButton.setButtonText (juce::String (juce::CharPointer_UTF8 ("\xe2\x96\xa0")) + " STOP");
        recordStatusLabel.setText ("RECORDING... PLAY THE KEYBOARD", juce::dontSendNotification);
    }
    else
    {
        recordStatusLabel.setText ("RECORD FAILED", juce::dontSendNotification);
    }
}

void SamplerView::resized()
{
    auto bounds = getLocalBounds();

    sampleNameLabel.setBounds (bounds.removeFromTop (22));
    bounds.removeFromTop (4);

    waveform.setBounds (bounds.removeFromTop (90));
    bounds.removeFromTop (8);

    auto editRow1 = bounds.removeFromTop (44);
    int slotW = editRow1.getWidth() / 5;
    placeLabeledSlider (startTrim, editRow1.removeFromLeft (slotW).reduced (4, 0));
    placeLabeledSlider (endTrim, editRow1.removeFromLeft (slotW).reduced (4, 0));
    placeLabeledSlider (gain, editRow1.removeFromLeft (slotW).reduced (4, 0));
    placeLabeledSlider (tune, editRow1.removeFromLeft (slotW).reduced (4, 0));
    reverseButton.setBounds (editRow1.reduced (4, 10));

    bounds.removeFromTop (6);
    auto editRow2 = bounds.removeFromTop (44);
    int slotW2 = editRow2.getWidth() / 4;
    placeLabeledSlider (attack, editRow2.removeFromLeft (slotW2).reduced (4, 0));
    placeLabeledSlider (decay, editRow2.removeFromLeft (slotW2).reduced (4, 0));
    placeLabeledSlider (sustain, editRow2.removeFromLeft (slotW2).reduced (4, 0));
    placeLabeledSlider (release, editRow2.removeFromLeft (slotW2).reduced (4, 0));

    bounds.removeFromTop (10);
    auto tapeRow = bounds.removeFromTop (28);
    tapeButton.setBounds (tapeRow.removeFromLeft (140));
    tapeRow.removeFromLeft (8);
    tapeStyleBox.setBounds (tapeRow.removeFromLeft (150));
    tapeRow.removeFromLeft (8);
    tapeAmount.label.setBounds (tapeRow.removeFromLeft (64));
    tapeAmount.slider.setBounds (tapeRow);

    bounds.removeFromTop (8);
    auto recordRow = bounds.removeFromTop (28);
    recordButton.setBounds (recordRow.removeFromLeft (100));
    recordRow.removeFromLeft (8);
    recordStatusLabel.setBounds (recordRow);

    bounds.removeFromTop (10);
    keyboard.setBounds (bounds);
}

namespace
{
    void placeLabeledSlider (SamplerView::LabeledSlider& ls, juce::Rectangle<int> slot)
    {
        ls.label.setBounds (slot.removeFromTop (14));
        ls.slider.setBounds (slot);
    }
}
