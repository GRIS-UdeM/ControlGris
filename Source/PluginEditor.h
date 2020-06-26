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

//==============================================================================
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
private:
    //==============================================================================
    ControlGrisAudioProcessor & mProcessor;

    GrisLookAndFeel mGrisLookAndFeel;

    AudioProcessorValueTreeState & mAudioProcessorValueTreeState;

    PositionAutomationManager & mPositionAutomationManager;
    ElevationAutomationManager & mElevationAutomationManager;

    BannerComponent mMainBanner;
    BannerComponent mElevationBanner;
    BannerComponent mTrajectoryBanner;
    BannerComponent mSettingsBanner;
    BannerComponent mPositionPresetBanner;

    PositionFieldComponent mPositionField;
    ElevationFieldComponent mElevationField;

    ParametersBoxComponent mParametersBox;

    TrajectoryBoxComponent mTrajectoryBox;

    TabbedComponent mConfigurationComponent{ TabbedButtonBar::Orientation::TabsAtTop };

    SettingsBoxComponent mSettingsBox;
    SourceBoxComponent mSourceBox;
    InterfaceBoxComponent mInterfaceBox;

    PositionPresetComponent mPositionPresetBox;

    bool mIsInsideSetPluginState;
    SourceIndex mSelectedSource;

    Value mLastUIWidth;
    Value mLastUIHeight;

public:
    //==============================================================================
    ControlGrisAudioProcessorEditor(ControlGrisAudioProcessor & controlGrisAudioProcessor,
                                    AudioProcessorValueTreeState & vts,
                                    PositionAutomationManager & positionAutomationManager,
                                    ElevationAutomationManager & elevationAutomationManager);
    ~ControlGrisAudioProcessorEditor() final;
    //==============================================================================
    void paint(Graphics &) final;
    void resized() final;
    void valueChanged(Value &) final;

    // FieldComponent::Listeners
    void fieldSourcePositionChanged(SourceIndex sourceIndex, int whichField) final;
    void fieldTrajectoryHandleClicked(int whichField) final;

    // ParametersBoxComponent::Listeners
    void parametersBoxSelectedSourceClicked() final;
    void parametersBoxParameterChanged(SourceParameter sourceId, double value) final;
    void parametersBoxAzimuthSpanDragStarted() final;
    void parametersBoxAzimuthSpanDragEnded() final;
    void parametersBoxElevationSpanDragStarted() final;
    void parametersBoxElevationSpanDragEnded() final;

    // SettingsBoxComponent::Listeners
    void settingsBoxOscFormatChanged(SpatMode mode) final;
    void settingsBoxOscPortNumberChanged(int oscPort) final;
    void settingsBoxOscActivated(bool state) final;
    void settingsBoxNumberOfSourcesChanged(int numOfSources) final;
    void settingsBoxFirstSourceIdChanged(SourceId firstSourceId) final;

    // SourceBoxComponent::Listeners
    void sourceBoxSelectionChanged(SourceIndex sourceIndex) final;
    void sourceBoxPlacementChanged(SourcePlacement value) final;
    void sourceBoxPositionChanged(SourceIndex sourceIndex, Radians angle, float rayLen) final;

    // TrajectoryBoxComponent::Listeners
    void trajectoryBoxPositionSourceLinkChanged(PositionSourceLink value) final;
    void trajectoryBoxElevationSourceLinkChanged(ElevationSourceLink value) final;
    void trajectoryBoxPositionTrajectoryTypeChanged(PositionTrajectoryType value) final;
    void trajectoryBoxElevationTrajectoryTypeChanged(ElevationTrajectoryType value) final;
    void trajectoryBoxPositionBackAndForthChanged(bool value) final;
    void trajectoryBoxElevationBackAndForthChanged(bool value) final;
    void trajectoryBoxPositionDampeningCyclesChanged(int value) final;
    void trajectoryBoxElevationDampeningCyclesChanged(int value) final;
    void trajectoryBoxDeviationPerCycleChanged(float value) final;
    void trajectoryBoxCycleDurationChanged(double duration, int mode) final;
    void trajectoryBoxDurationUnitChanged(double duration, int mode) final;
    void trajectoryBoxPositionActivateChanged(bool value) final;
    void trajectoryBoxElevationActivateChanged(bool value) final;

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
    void updateSourceLinkCombo(PositionSourceLink value);
    void updateElevationSourceLinkCombo(ElevationSourceLink value);
    void updatePositionPreset(int presetNumber);

    void refresh();

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControlGrisAudioProcessorEditor)
};
