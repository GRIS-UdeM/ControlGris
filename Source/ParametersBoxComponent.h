# pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "GrisLookAndFeel.h"

class ParametersBoxComponent : public Component,
                               public Button::Listener
{
public:
    ParametersBoxComponent();
    ~ParametersBoxComponent();

    void buttonClicked(Button *button);
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

    //-------------------------------------

    Label           xLabel;
    ToggleButton    xLinkButton;
    Slider          xSlider;

    Label           yLabel;
    ToggleButton    yLinkButton;
    Slider          ySlider;

    Label           zLabel;
    ToggleButton    zLinkButton;
    Slider          zSlider;

    //-------------------------------------

    Label           azimuthSpanLabel;
    ToggleButton    azimuthSpanLinkButton;
    Slider          azimuthSpanSlider;

    Label           elevationSpanLabel;
    ToggleButton    elevationSpanLinkButton;
    Slider          elevationSpanSlider;

    ToggleButton    activatorXYZ;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParametersBoxComponent)
};
