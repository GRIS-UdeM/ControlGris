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
                                       public FieldComponent::Listener,
                                       public ParametersBoxComponent::Listener
{
public:
    SpatGris2AudioProcessorEditor (SpatGris2AudioProcessor&);
    ~SpatGris2AudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void sourcePositionChanged(int sourceId) override;
    void parameterChanged(int parameterId, double value) override;

    Source * getSources();
    int getSelectedSource();

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

    FieldComponent  azimuthElevationField;
    FieldComponent  radiusField;

    TabbedComponent configurationComponent { TabbedButtonBar::Orientation::TabsAtTop };

    ParametersBoxComponent  parametersBox;
    TrajectoryBoxComponent  trajectoryBox;

    SettingsBoxComponent    settingsBox;
    SourceBoxComponent      sourceBox;
    InterfaceBoxComponent   interfaceBox;

    Source sources[MaxNumOfSources];
    int m_numOfSources;
    int m_selectedSource;

    OSCSender oscSender;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpatGris2AudioProcessorEditor)
};
