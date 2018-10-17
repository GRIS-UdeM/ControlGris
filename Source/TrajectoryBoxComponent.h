# pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "GrisLookAndFeel.h"

class TrajectoryBoxComponent : public Component
{
public:
    TrajectoryBoxComponent();
    ~TrajectoryBoxComponent();

    void paint(Graphics&) override;
    void resized() override;

private:
    GrisLookAndFeel mGrisFeel;

    Label           sourceLinkLabel;
    ComboBox        sourceLinkCombo;

    Label           trajectoryTypeLabel;
    ComboBox        trajectoryTypeCombo;

    Label           timeScaleLabel;
    ComboBox        timeScaleCombo;

    Label           cycleSpeedLabel;
    Slider          cycleSpeedSlider;

    TextButton      activateButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrajectoryBoxComponent)
};
