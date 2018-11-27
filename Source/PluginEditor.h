/**************************************************************************
 * Copyright 2018 UdeM - GRIS - Olivier Belanger                          *
 *                                                                        *
 * This file is part of ControlGris, a multi-source spatialization plugin *
 *                                                                        *
 * ControlGris is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU Lesser General Public License as         *
 * published by the Free Software Foundation, either version 3 of the     *
 * License, or (at your option) any later version.                        *
 *                                                                        *
 * ControlGris is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 * GNU Lesser General Public License for more details.                    *
 *                                                                        *
 * You should have received a copy of the GNU Lesser General Public       *
 * License along with ControlGris.  If not, see                           *
 * <http://www.gnu.org/licenses/>.                                        *
 *************************************************************************/
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

const int MaxNumOfSources = 8;

class ControlGrisAudioProcessorEditor : public AudioProcessorEditor,
                                        public AudioProcessorValueTreeState::Listener,
                                        private Value::Listener,
                                        public FieldComponent::Listener,
                                        public ParametersBoxComponent::Listener,
                                        public SettingsBoxComponent::Listener
{
public:
    ControlGrisAudioProcessorEditor (ControlGrisAudioProcessor&, AudioProcessorValueTreeState& vts);
    ~ControlGrisAudioProcessorEditor();

    void paint (Graphics&) override;
    void resized() override;
    void valueChanged (Value&) override;

    void sourcePositionChanged(int sourceId) override;
    void parameterLinkChanged(int parameterId, bool value) override;
    void parameterChanged(int parameterId, double value) override;              // ParametersBoxComponent
    void parameterChanged(const String &parameterID, float newValue) override;  // AudioProcessorValueTreeState
    void oscFormatChanged(int selectedId) override;

    void sendOscMessage();
    void setSourceParameterValue(int sourceId, int parameterId, double value);
    void setLinkedParameterValue(int sourceId, int parameterId);

    bool getParameterLinkState(int parameterId);
    void setParameterLinkState(int parameterId, bool state);

    void setPluginState();

    Source * getSources();
    int getSelectedSource();

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ControlGrisAudioProcessor& processor;

    // Reference to the parameter tree state.
    AudioProcessorValueTreeState& valueTreeState;

    GrisLookAndFeel mGrisFeel;

    BannerComponent mainBanner;
    BannerComponent elevationBanner;
    BannerComponent parametersBanner;
    BannerComponent trajectoryBanner;
    BannerComponent settingsBanner;

    MainFieldComponent  mainField;
    ElevationFieldComponent  elevationField;

    TabbedComponent configurationComponent { TabbedButtonBar::Orientation::TabsAtTop };

    ParametersBoxComponent  parametersBox;
    TrajectoryBoxComponent  trajectoryBox;

    SettingsBoxComponent    settingsBox;
    SourceBoxComponent      sourceBox;
    InterfaceBoxComponent   interfaceBox;

    Source sources[MaxNumOfSources];
    int m_numOfSources;
    int m_selectedSource;
    int m_selectedOscFormat;

    // These are used to persist the UI's size - the values are stored along with the
    // filter's other parameters, and the UI component will update them when it gets resized.
    Value lastUIWidth, lastUIHeight;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControlGrisAudioProcessorEditor)
};
