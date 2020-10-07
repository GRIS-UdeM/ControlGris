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

#include <JuceHeader.h>

#include "cg_BannerComponent.hpp"
#include "cg_FieldComponent.hpp"
#include "cg_InterfaceBoxComponent.hpp"
#include "cg_ParametersBoxComponent.hpp"
#include "cg_PositionPresetComponent.hpp"
#include "cg_SourceBoxComponent.hpp"
#include "cg_TrajectoryBoxComponent.hpp"

namespace gris
{
//==============================================================================
class ControlGrisAudioProcessorEditor final
    : public juce::AudioProcessorEditor
    , private juce::Value::Listener
    , public FieldComponent::Listener
    , public ParametersBoxComponent::Listener
    , public SettingsBoxComponent::Listener
    , public SourceBoxComponent::Listener
    , public TrajectoryBoxComponent::Listener
    , public InterfaceBoxComponent::Listener
    , public PositionPresetComponent::Listener
{
    ControlGrisAudioProcessor & mProcessor;
    GrisLookAndFeel mGrisLookAndFeel;
    juce::AudioProcessorValueTreeState & mAudioProcessorValueTreeState;

    PositionTrajectoryManager & mPositionTrajectoryManager;
    ElevationTrajectoryManager & mElevationTrajectoryManager;

    BannerComponent mMainBanner;
    BannerComponent mElevationBanner;
    BannerComponent mTrajectoryBanner;
    BannerComponent mSettingsBanner;
    BannerComponent mPositionPresetBanner;

    PositionFieldComponent mPositionField;
    ElevationFieldComponent mElevationField;

    ParametersBoxComponent mParametersBox;
    TrajectoryBoxComponent mTrajectoryBox;

    juce::TabbedComponent mConfigurationComponent{ juce::TabbedButtonBar::Orientation::TabsAtTop };

    SettingsBoxComponent mSettingsBox;
    SourceBoxComponent mSourceBox;
    InterfaceBoxComponent mInterfaceBox;

    PositionPresetComponent mPositionPresetBox;

    bool mIsInsideSetPluginState;
    SourceIndex mSelectedSource;

    juce::Value mLastUIWidth;
    juce::Value mLastUIHeight;

public:
    //==============================================================================
    ControlGrisAudioProcessorEditor() = delete;
    ~ControlGrisAudioProcessorEditor() override;

    ControlGrisAudioProcessorEditor(ControlGrisAudioProcessorEditor const &) = delete;
    ControlGrisAudioProcessorEditor(ControlGrisAudioProcessorEditor &&) = delete;

    ControlGrisAudioProcessorEditor & operator=(ControlGrisAudioProcessorEditor const &) = delete;
    ControlGrisAudioProcessorEditor & operator=(ControlGrisAudioProcessorEditor &&) = delete;
    //==============================================================================
    ControlGrisAudioProcessorEditor(ControlGrisAudioProcessor & controlGrisAudioProcessor,
                                    juce::AudioProcessorValueTreeState & vts,
                                    PositionTrajectoryManager & positionAutomationManager,
                                    ElevationTrajectoryManager & elevationAutomationManager);
    //==============================================================================
    void paint(juce::Graphics &) override;
    void resized() override;
    void valueChanged(juce::Value &) override;

    // FieldComponent::Listeners
    void fieldSourcePositionChanged(SourceIndex sourceIndex, int whichField) override;

    // ParametersBoxComponent::Listeners
    void parametersBoxSelectedSourceClicked() override;
    void parametersBoxParameterChanged(SourceParameter sourceParameter, double value) override;
    void parametersBoxAzimuthSpanDragStarted() override;
    void parametersBoxAzimuthSpanDragEnded() override;
    void parametersBoxElevationSpanDragStarted() override;
    void parametersBoxElevationSpanDragEnded() override;

    // SettingsBoxComponent::Listeners
    void settingsBoxOscFormatChanged(SpatMode mode) override;
    void settingsBoxOscPortNumberChanged(int oscPort) override;
    void settingsBoxOscActivated(bool state) override;
    void settingsBoxNumberOfSourcesChanged(int numOfSources) override;
    void settingsBoxFirstSourceIdChanged(SourceId firstSourceId) override;

    // SourceBoxComponent::Listeners
    void sourceBoxSelectionChanged(SourceIndex sourceIndex) override;
    void sourceBoxPlacementChanged(SourcePlacement sourcePlacement) override;
    void sourceBoxPositionChanged(SourceIndex sourceIndex,
                                  std::optional<Radians> azimuth,
                                  std::optional<Radians> elevation,
                                  std::optional<float> x,
                                  std::optional<float> y,
                                  std::optional<float> z) override;

    // TrajectoryBoxComponent::Listeners
    void trajectoryBoxPositionSourceLinkChanged(PositionSourceLink sourceLink) override;
    void trajectoryBoxElevationSourceLinkChanged(ElevationSourceLink sourceLink) override;
    void trajectoryBoxPositionTrajectoryTypeChanged(PositionTrajectoryType value) override;
    void trajectoryBoxElevationTrajectoryTypeChanged(ElevationTrajectoryType value) override;
    void trajectoryBoxPositionBackAndForthChanged(bool value) override;
    void trajectoryBoxElevationBackAndForthChanged(bool value) override;
    void trajectoryBoxPositionDampeningCyclesChanged(int value) override;
    void trajectoryBoxElevationDampeningCyclesChanged(int value) override;
    void trajectoryBoxDeviationPerCycleChanged(float degrees) override;
    void trajectoryBoxCycleDurationChanged(double duration, int mode) override;
    void trajectoryBoxDurationUnitChanged(double duration, int mode) override;
    void trajectoryBoxPositionActivateChanged(bool value) override;
    void trajectoryBoxElevationActivateChanged(bool value) override;

    // PositionPresetComponent::Listeners
    void positionPresetChanged(int presetNumber) override;
    void positionPresetSaved(int presetNumber) override;
    void positionPresetDeleted(int presetNumber) override;

    // InterfaceBoxComponent::Listeners
    void oscOutputPluginIdChanged(int value) override;
    void oscInputConnectionChanged(bool state, int oscPort) override;
    void oscOutputConnectionChanged(bool state, juce::String oscAddress, int oscPort) override;

    void reloadUiState();
    void updateSpanLinkButton(bool state);
    void updateSourceLinkCombo(PositionSourceLink value);
    void updateElevationSourceLinkCombo(ElevationSourceLink value);
    void updatePositionPreset(int presetNumber);

    void refresh();

    void setSpatMode(SpatMode spatMode);

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR(ControlGrisAudioProcessorEditor);
};

} // namespace gris