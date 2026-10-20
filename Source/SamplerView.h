#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "SamplerEngine.h"
#include "SamplerKeyboardComponent.h"
#include "SamplerWaveformComponent.h"
#include "Theme.h"

/** The Sampler tab: a loaded sample, trim/gain/tune/ADSR/reverse editing, an
    optional vintage tape machine on the output, a recorder that captures
    whatever's played on the keyboard, and the computer-keyboard piano itself.
*/
class SamplerView : public juce::Component
{
public:
    SamplerView (SamplerEngine& engineToUse, const Theme::Accent& accentRef);

    /** Call after SamplerEngine::loadSample succeeds, to refresh the display. */
    void sampleWasLoaded();

    void grabKeyboardFocusForPlaying();
    void refreshAccentColours();

    void resized() override;

    std::function<void (const juce::String&)> onStatusMessage;

    struct LabeledSlider
    {
        juce::Label label;
        juce::Slider slider;
    };

private:
    void setUpSlider (LabeledSlider& ls, const juce::String& text, double minV, double maxV, double defaultV, double step);
    void updateADSR();
    void updateTrim();
    void toggleRecording();

    SamplerEngine& engine;
    const Theme::Accent& accent;

    juce::Label sampleNameLabel;
    SamplerWaveformComponent waveform;

    LabeledSlider startTrim, endTrim, gain, tune, attack, decay, sustain, release;
    juce::ToggleButton reverseButton { "REVERSE" };

    juce::ToggleButton tapeButton { "TAPE MACHINE" };
    juce::ComboBox tapeStyleBox;
    LabeledSlider tapeAmount;

    juce::TextButton recordButton { "\xe2\x97\x8f REC" };
    juce::Label recordStatusLabel;
    juce::File lastRecordingFile;

    SamplerKeyboardComponent keyboard;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SamplerView)
};
