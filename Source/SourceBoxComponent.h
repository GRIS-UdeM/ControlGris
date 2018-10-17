# pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "GrisLookAndFeel.h"

class SourceBoxComponent : public Component
{
public:
    SourceBoxComponent();
    ~SourceBoxComponent();

    void paint(Graphics&) override;
    void resized() override;

private:
    GrisLookAndFeel mGrisFeel;

    Label           sourcePlacementLabel;
    ComboBox        sourcePlacementCombo;

    Label           sourceNumberLabel;
    ComboBox        sourceNumberCombo;

    Label           rayLengthLabel;
    Slider          rayLengthSlider;

    Label           angleLabel;
    Slider          angleSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SourceBoxComponent)
};
