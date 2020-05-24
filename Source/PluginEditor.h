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
#include "AutomationManager.h"
#include "BannerComponent.h"
#include "FieldComponent.h"
#include "GrisLookAndFeel.h"
#include "InterfaceBoxComponent.h"
#include "ParametersBoxComponent.h"
#include "PluginProcessor.h"
#include "PositionPresetComponent.h"
#include "SettingsBoxComponent.h"
#include "Source.h"
#include "SourceBoxComponent.h"
#include "TrajectoryBoxComponent.h"

class ControlGrisAudioProcessorEditor final
    : public AudioProcessorEditor
    , private Value::Listener
    , public FieldComponent::Listener
    , public ParametersBoxComponent::Listener
    , public SettingsBoxComponent::Listener
    , public SourceBoxComponent::Listener
    , public TrajectoryBoxComponent::Listener
    , public InterfaceBoxComponent::Listener
    , public PositionPresetComponent::Listener
{
public:
    ControlGrisAudioProcessorEditor(ControlGrisAudioProcessor &,
                                    AudioProcessorValueTreeState & vts,
                                    AutomationManager & automan,
                                    AutomationManager & automanAlt);
    ~ControlGrisAudioProcessorEditor() final;

    void paint(Graphics &) final;
    void resized() final;
    void valueChanged(Value &) final;

    // FieldComponent::Listeners
    void fieldSourcePositionChanged(int sourceId, int whichField) final;
    void fieldTrajectoryHandleClicked(int whichField) final;

    // ParametersBoxComponent::Listeners
    void parametersBoxSelectedSourceClicked() final;
    void parametersBoxParameterChanged(int parameterId, double value) final;

    // SettingsBoxComponent::Listeners
    void settingsBoxOscFormatChanged(SpatMode mode) final;
    void settingsBoxOscPortNumberChanged(int oscPort) final;
    void settingsBoxOscActivated(bool state) final;
    void settingsBoxNumberOfSourcesChanged(int numOfSources) final;
    void settingsBoxFirstSourceIdChanged(int firstSourceId) final;

    // SourceBoxComponent::Listeners
    void sourceBoxSelectionChanged(int sourceNum) final;
    void sourceBoxPlacementChanged(SourcePlacement value) final;
    void sourceBoxPositionChanged(int sourceNum, float angle, float rayLen) final;

    // TrajectoryBoxComponent::Listeners
    void trajectoryBoxSourceLinkChanged(SourceLink value) final;
    void trajectoryBoxSourceLinkAltChanged(SourceLinkAlt value) final;
    void trajectoryBoxTrajectoryTypeChanged(TrajectoryType value) final;
    void trajectoryBoxTrajectoryTypeAltChanged(TrajectoryTypeAlt value) final;
    void trajectoryBoxBackAndForthChanged(bool value) final;
    void trajectoryBoxBackAndForthAltChanged(bool value) final;
    void trajectoryBoxDampeningCyclesChanged(int value) final;
    void trajectoryBoxDampeningCyclesAltChanged(int value) final;
    void trajectoryBoxDeviationPerCycleChanged(float value) final;
    void trajectoryBoxCycleDurationChanged(double duration, int mode) final;
    void trajectoryBoxDurationUnitChanged(double duration, int mode) final;
    void trajectoryBoxActivateChanged(bool value) final;
    void trajectoryBoxActivateAltChanged(bool value) final;

    // PositionPresetComponent::Listeners
    void positionPresetChanged(int presetNumber) final;
    void positionPresetSaved(int presetNumber) final;
    void positionPresetDeleted(int presetNumber) final;

    // InterfaceBoxComponent::Listeners
    void oscOutputPluginIdChanged(int value) final;
    void oscInputConnectionChanged(bool state, int oscPort) final;
    void oscOutputConnectionChanged(bool state, String oscAddress, int oscPort) final;

    void setPluginState();
    void updateSpanLinkButton(bool state);
    void updateSourceLinkCombo(SourceLink value);
    void updateSourceLinkAltCombo(SourceLinkAlt value);
    void updatePositionPreset(int presetNumber);

    void refresh();

private:
    ControlGrisAudioProcessor & processor;

    GrisLookAndFeel grisLookAndFeel;

    AudioProcessorValueTreeState & valueTreeState;

    AutomationManager & automationManager;
    AutomationManager & automationManagerAlt;

    BannerComponent mainBanner;
    BannerComponent elevationBanner;
    BannerComponent trajectoryBanner;
    BannerComponent settingsBanner;
    BannerComponent positionPresetBanner;

    MainFieldComponent mainField;
    ElevationFieldComponent elevationField;

    ParametersBoxComponent parametersBox;

    TrajectoryBoxComponent trajectoryBox;

    TabbedComponent configurationComponent{ TabbedButtonBar::Orientation::TabsAtTop };

    SettingsBoxComponent settingsBox;
    SourceBoxComponent sourceBox;
    InterfaceBoxComponent interfaceBox;

    PositionPresetComponent positionPresetBox;

    bool m_isInsideSetPluginState;
    int m_selectedSource;

    Value lastUIWidth, lastUIHeight;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControlGrisAudioProcessorEditor)
};
