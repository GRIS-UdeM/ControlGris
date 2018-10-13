/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "GrisLookAndFeel.h"
#include "FieldComponent.h"
#include "BannerComponent.h"
#include "ParametersBoxComponent.h"

//==============================================================================
/**
*/
class SpatGris2AudioProcessorEditor  : public AudioProcessorEditor
{
public:
    SpatGris2AudioProcessorEditor (SpatGris2AudioProcessor&);
    ~SpatGris2AudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SpatGris2AudioProcessor& processor;

    GrisLookAndFeel mGrisFeel;

    BannerComponent azimuthElevationBanner;
    BannerComponent radiusBanner;
    BannerComponent parametersBanner;
    BannerComponent trajectoryBanner;
    BannerComponent settingsBanner;
    BannerComponent sourceBanner;
    BannerComponent interfaceBanner;

    FieldComponent  azimuthElevationField;
    FieldComponent  radiusField;

    ParametersBoxComponent parametersBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpatGris2AudioProcessorEditor)
};
