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
#include "AutomationManager.h"

class ControlGrisAudioProcessorEditor : public AudioProcessorEditor,
                                        private Value::Listener,
                                        public FieldComponent::Listener,
                                        public ParametersBoxComponent::Listener,
                                        public SettingsBoxComponent::Listener,
                                        public SourceBoxComponent::Listener,
                                        public TrajectoryBoxComponent::Listener,
                                        public Timer
{
public:
    ControlGrisAudioProcessorEditor (ControlGrisAudioProcessor&,
                                     AudioProcessorValueTreeState& vts,
                                     AutomationManager& automan);
    ~ControlGrisAudioProcessorEditor();

    void paint (Graphics&) override;
    void resized() override;
    void valueChanged (Value&) override;
    void timerCallback() override;

    void sourcePositionChanged(int sourceId) override;
    void selectedSourceClicked() override;
    void parameterLinkChanged(int parameterId, bool value) override;
    void parameterChanged(int parameterId, double value) override;
    void oscFormatChanged(int selectedId) override;
    void oscPortNumberChanged(int oscPort) override;
    void oscActivated(bool state) override;
    void numberOfSourcesChanged(int numOfSources) override;
    void firstSourceIdChanged(int firstSourceId) override;
    // SourceBoxComponent::Listeners
    void sourcePlacementChanged(int value) override;
    void sourceNumberPositionChanged(int sourceNum, float angle, float rayLen) override;
    // TrajectoryBoxComponent::Listeners
    void trajectoryBoxSourceLinkChanged(int value) override;
    void trajectoryBoxTrajectoryTypeChanged(int value) override;
    void trajectoryBoxDurationChanged(double value) override;
    void trajectoryBoxNumOfCycleChanged(int value) override;
    void trajectoryBoxActivateChanged(bool value) override;
    void trajectoryBoxFixSourceButtonClicked() override;
    void trajectoryBoxClearButtonClicked() override;

    void setPluginState();

private:
    ControlGrisAudioProcessor& processor;

    AudioProcessorValueTreeState& valueTreeState;

    AutomationManager& automationManager;

    GrisLookAndFeel grisLookAndFeel;

    BannerComponent mainBanner;
    BannerComponent elevationBanner;
    BannerComponent trajectoryBanner;
    BannerComponent settingsBanner;

    MainFieldComponent  mainField;
    ElevationFieldComponent  elevationField;

    ParametersBoxComponent  parametersBox;

    TrajectoryBoxComponent  trajectoryBox;

    TabbedComponent configurationComponent { TabbedButtonBar::Orientation::TabsAtTop };

    SettingsBoxComponent    settingsBox;
    SourceBoxComponent      sourceBox;
    InterfaceBoxComponent   interfaceBox;

    int m_selectedSource;

    double m_lastTime;

    Value lastUIWidth, lastUIHeight;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControlGrisAudioProcessorEditor)
};
