# pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "GrisLookAndFeel.h"

class ParametersBoxComponent : public Component
{
public:
    ParametersBoxComponent();
    ~ParametersBoxComponent();

    void paint(Graphics&) override;
    void resized() override;

private:
    GrisLookAndFeel mGrisFeel;

    Label           azimuthLabel;
    ToggleButton    azimuthLinkButton;
    Slider          azimuthSlider;

    Label           elevationLabel;
    ToggleButton    elevationLinkButton;
    Slider          elevationSlider;

    Label           radiusLabel;
    ToggleButton    radiusLinkButton;
    Slider          radiusSlider;

    Label           azimuthSpanLabel;
    ToggleButton    azimuthSpanLinkButton;
    Slider          azimuthSpanSlider;

    Label           elevationSpanLabel;
    ToggleButton    elevationSpanLinkButton;
    Slider          elevationSpanSlider;

    ToggleButton    activatorXYZ;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParametersBoxComponent)
};
