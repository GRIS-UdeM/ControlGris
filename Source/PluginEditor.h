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
#include "TrajectoryBoxComponent.h"
#include "SettingsBoxComponent.h"
#include "SourceBoxComponent.h"
#include "InterfaceBoxComponent.h"
#include "Source.h"

const int MaxNumOfSources = 36;

//==============================================================================
/**
*/
class SpatGris2AudioProcessorEditor  : public AudioProcessorEditor,
                                       public FieldComponent::Listener
{
public:
    SpatGris2AudioProcessorEditor (SpatGris2AudioProcessor&);
    ~SpatGris2AudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void sourcePositionChanged(int sourceId) override;

    Source * getSources();

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

    ParametersBoxComponent  parametersBox;
    TrajectoryBoxComponent  trajectoryBox;
    SettingsBoxComponent    settingsBox;
    SourceBoxComponent      sourceBox;
    InterfaceBoxComponent   interfaceBox;

    Source sources[MaxNumOfSources];
    int m_numOfSources;

    OSCSender oscSender;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpatGris2AudioProcessorEditor)
};
