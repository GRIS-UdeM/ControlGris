/**************************************************************************
 * Copyright 2021 UdeM - GRIS - Samuel Béland & Olivier Belanger          *
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
#include "cg_OscControllerTab.hpp"
#include "cg_SectionPositionPresets.hpp"
#include "cg_SectionSourceSpan.hpp"
#include "cg_SectionTrajectory.hpp"
#include "cg_SourcePositionTab.hpp"

namespace gris
{
//==============================================================================
/** The main UI component used to interact with the plugin. */
class ControlGrisAudioProcessorEditor final
    : public juce::AudioProcessorEditor
    , private juce::Value::Listener
    , public FieldComponent::Listener
    , public GeneralSettingsTab::Listener
    , public SourcePositionTab::Listener
    , public SectionTrajectory::Listener
    , public OscControllerTab::Listener
    , public PositionPresetComponent::Listener
{
    ControlGrisAudioProcessor & mAudioProcessor;
    juce::AudioProcessorValueTreeState & mAudioProcessorValueTreeState;
    PositionTrajectoryManager & mPositionTrajectoryManager;
    ElevationTrajectoryManager & mElevationTrajectoryManager;

    GrisLookAndFeel mGrisLookAndFeel;

    BannerComponent mMainBanner;
    BannerComponent mElevationBanner;
    BannerComponent mTrajectoryBanner;
    BannerComponent mSettingsBanner;
    BannerComponent mPresetsBanner;

    PositionFieldComponent mPositionField;
    ElevationFieldComponent mElevationField;

    SectionSourceSpan mSpansSection;
    SectionTrajectory mTrajectorySection;

    juce::TabbedComponent mConfigurationTabs{ juce::TabbedButtonBar::Orientation::TabsAtTop };

    GeneralSettingsTab mGeneralSettingsTab;
    SourcePositionTab mSourcePositionTab;
    OscControllerTab mOscControllerTab;

    PositionPresetComponent mPositionPresetComponent;

    bool mIsInsideSetPluginState;
    SourceIndex mSelectedSource{};

    juce::Value mLastUiWidth;
    juce::Value mLastUiHeight;

public:
    //==============================================================================
    ControlGrisAudioProcessorEditor(ControlGrisAudioProcessor & audioProcessor,
                                    juce::AudioProcessorValueTreeState & vts,
                                    PositionTrajectoryManager & positionTrajectoryManager,
                                    ElevationTrajectoryManager & elevationTrajectoryManager);
    ControlGrisAudioProcessorEditor() = delete;
    ~ControlGrisAudioProcessorEditor() override;
    //==============================================================================
    ControlGrisAudioProcessorEditor(ControlGrisAudioProcessorEditor const &) = delete;
    ControlGrisAudioProcessorEditor(ControlGrisAudioProcessorEditor &&) = delete;
    ControlGrisAudioProcessorEditor & operator=(ControlGrisAudioProcessorEditor const &) = delete;
    ControlGrisAudioProcessorEditor & operator=(ControlGrisAudioProcessorEditor &&) = delete;
    //==============================================================================
    void repaintPositionField();
    void repaintElevationField();
    void reloadUiState();
    void updateSpanLinkButton(bool state);
    void updatePositionPreset(int presetNumber);
    void refresh();
    void setSpatMode(SpatMode spatMode);
    //==============================================================================
    void paint(juce::Graphics &) override;
    void resized() override;

    // Value::Listener
    void valueChanged(juce::Value &) override;

    // FieldComponent::Listener
    void fieldSourcePositionChangedCallback(SourceIndex sourceIndex, int whichField) override;

    // SectionSourceSpan::Listeners
    void selectedSourceClicked();

    // GeneralSettingsTab::Listeners
    void oscFormatChangedCallback(SpatMode mode) override;
    void oscPortChangedCallback(int oscPort) override;
    void oscAddressChangedCallback(juce::String const & address) override;
    void oscStateChangedCallback(bool state) override;
    void numberOfSourcesChangedCallback(int numOfSources) override;
    void firstSourceIdChangedCallback(SourceId firstSourceId) override;

    // SourcePositionTab::Listeners
    void sourceSelectionChangedCallback(SourceIndex sourceIndex) override;
    void sourcesPlacementChangedCallback(SourcePlacement sourcePlacement) override;
    void sourcePositionChangedCallback(SourceIndex sourceIndex,
                                       std::optional<Radians> azimuth,
                                       std::optional<Radians> elevation,
                                       std::optional<float> x,
                                       std::optional<float> y,
                                       std::optional<float> z) override;

    // SectionTrajectory::Listeners
    void positionTrajectoryBackAndForthChangedCallback(bool value) override;
    void elevationTrajectoryBackAndForthChangedCallback(bool value) override;
    void positionTrajectoryDampeningCyclesChangedCallback(int value) override;
    void elevationTrajectoryDampeningCyclesChangedCallback(int value) override;
    void trajectoryDeviationPerCycleChangedCallback(float degrees) override;
    void trajectoryCycleDurationChangedCallback(double duration, int mode) override;
    void trajectoryDurationUnitChangedCallback(double duration, int mode) override;
    void positionTrajectoryStateChangedCallback(bool value) override;
    void elevationTrajectoryStateChangedCallback(bool value) override;

    // PositionPresetComponent::Listeners
    void positionPresetChangedCallback(int presetNumber) override;
    void positionPresetSavedCallback(int presetNumber) override;
    void positionPresetDeletedCallback(int presetNumber) override;

    // OscControllerTab::Listeners
    void oscOutputPluginIdChangedCallback(int value) override;
    void oscInputConnectionChangedCallback(bool state, int oscPort) override;
    void oscOutputConnectionChangedCallback(bool state, juce::String oscAddress, int oscPort) override;

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR(ControlGrisAudioProcessorEditor)
};

} // namespace gris