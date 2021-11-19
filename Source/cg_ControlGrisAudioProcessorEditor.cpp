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

#include "cg_ControlGrisAudioProcessorEditor.hpp"

#include "cg_ControlGrisAudioProcessor.hpp"
#include "cg_constants.hpp"

namespace gris
{
//==============================================================================
ControlGrisAudioProcessorEditor::ControlGrisAudioProcessorEditor(
    ControlGrisAudioProcessor & audioProcessor,
    juce::AudioProcessorValueTreeState & vts,
    PositionTrajectoryManager & positionTrajectoryManager,
    ElevationTrajectoryManager & elevationTrajectoryManager)
    : AudioProcessorEditor(&audioProcessor)
    , mAudioProcessor(audioProcessor)
    , mAudioProcessorValueTreeState(vts)
    , mPositionTrajectoryManager(positionTrajectoryManager)
    , mElevationTrajectoryManager(elevationTrajectoryManager)
    , mPositionField(audioProcessor.getSources(), positionTrajectoryManager)
    , mElevationField(audioProcessor.getSources(), elevationTrajectoryManager)
    , mSpansSection(audioProcessor, *this, mGrisLookAndFeel)
    , mTrajectorySection(audioProcessor, mGrisLookAndFeel)
    , mGeneralSettingsTab(mGrisLookAndFeel)
    , mSourcePositionTab(mGrisLookAndFeel, audioProcessor.getSpatMode())
    , mOscControllerTab(mGrisLookAndFeel)
    , mPositionPresetComponent(audioProcessor.getPresetsManager())
{
    setLookAndFeel(&mGrisLookAndFeel);

    mIsInsideSetPluginState = false;
    mSelectedSource = {};

    // Set up the interface.
    //----------------------
    mMainBanner.setLookAndFeel(&mGrisLookAndFeel);
    mMainBanner.setText("Azimuth - Elevation", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(&mMainBanner);

    mElevationBanner.setLookAndFeel(&mGrisLookAndFeel);
    mElevationBanner.setText("Elevation", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(&mElevationBanner);

    mTrajectoryBanner.setLookAndFeel(&mGrisLookAndFeel);
    mTrajectoryBanner.setText("Trajectories", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(&mTrajectoryBanner);

    mSettingsBanner.setLookAndFeel(&mGrisLookAndFeel);
    mSettingsBanner.setText("Configuration", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(&mSettingsBanner);

    mPresetsBanner.setLookAndFeel(&mGrisLookAndFeel);
    mPresetsBanner.setText("Preset", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(&mPresetsBanner);

    mPositionField.setLookAndFeel(&mGrisLookAndFeel);
    mPositionField.addListener(this);
    addAndMakeVisible(&mPositionField);

    mElevationField.setLookAndFeel(&mGrisLookAndFeel);
    mElevationField.addListener(this);
    addAndMakeVisible(&mElevationField);

    mSpansSection.setLookAndFeel(&mGrisLookAndFeel);
    addAndMakeVisible(&mSpansSection);

    mTrajectorySection.setLookAndFeel(&mGrisLookAndFeel);
    mTrajectorySection.addListener(this);
    addAndMakeVisible(mTrajectorySection);

    mGeneralSettingsTab.setLookAndFeel(&mGrisLookAndFeel);
    mGeneralSettingsTab.addListener(this);

    mSourcePositionTab.setLookAndFeel(&mGrisLookAndFeel);
    mSourcePositionTab.addListener(this);

    mOscControllerTab.setLookAndFeel(&mGrisLookAndFeel);
    mOscControllerTab.addListener(this);

    auto const bg{ mGrisLookAndFeel.findColour(juce::ResizableWindow::backgroundColourId) };

    mConfigurationTabs.setLookAndFeel(&mGrisLookAndFeel);
    mConfigurationTabs.setColour(juce::TabbedComponent::backgroundColourId, bg);
    mConfigurationTabs.addTab("Settings", bg, &mGeneralSettingsTab, false);
    mConfigurationTabs.addTab("Sources", bg, &mSourcePositionTab, false);
    mConfigurationTabs.addTab("Controllers", bg, &mOscControllerTab, false);
    addAndMakeVisible(mConfigurationTabs);

    mPositionPresetComponent.setLookAndFeel(&mGrisLookAndFeel);
    mPositionPresetComponent.addListener(this);
    addAndMakeVisible(&mPositionPresetComponent);

    // Add sources to the fields.
    //---------------------------
    mPositionField.refreshSources();
    mElevationField.refreshSources();

    mSpansSection.setSelectedSource(&mAudioProcessor.getSources()[mSelectedSource]);

    // Manage dynamic window size of the plugin.
    //------------------------------------------
    setResizable(true, true);
    setResizeLimits(MIN_FIELD_WIDTH_PIXELS + 50, MIN_FIELD_WIDTH_PIXELS + 20, 1800, 1300);

    mLastUiWidth.referTo(
        mAudioProcessor.getValueTreeState().state.getChildWithName("uiState").getPropertyAsValue("width", nullptr));
    mLastUiHeight.referTo(
        mAudioProcessor.getValueTreeState().state.getChildWithName("uiState").getPropertyAsValue("height", nullptr));

    // set our component's initial size to be the last one that was stored in the filter's settings
    setSize(mLastUiWidth.getValue(), mLastUiHeight.getValue());

    mLastUiWidth.addListener(this);
    mLastUiHeight.addListener(this);

    // Load the last saved state of the plugin.
    //-----------------------------------------
    reloadUiState();
}

//==============================================================================
ControlGrisAudioProcessorEditor::~ControlGrisAudioProcessorEditor()
{
    mConfigurationTabs.setLookAndFeel(nullptr);
    setLookAndFeel(nullptr);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::reloadUiState()
{
    mIsInsideSetPluginState = true;

    // Set global settings values.
    //----------------------------
    oscFormatChangedCallback(mAudioProcessor.getSpatMode());
    oscPortChangedCallback(mAudioProcessor.getOscPortNumber());
    oscAddressChangedCallback(mAudioProcessor.getOscAddress());
    oscStateChangedCallback(mAudioProcessor.isOscActive());
    firstSourceIdChangedCallback(mAudioProcessor.getFirstSourceId());
    numberOfSourcesChangedCallback(mAudioProcessor.getSources().size());

    mOscControllerTab.setOscOutputPluginId(
        mAudioProcessorValueTreeState.state.getProperty(parameters::statics::OSC_OUTPUT_PLUGIN_ID,
                                                        DEFAULT_OSC_OUTPUT_PLUGIN_ID));
    mOscControllerTab.setOscReceiveToggleState(
        mAudioProcessorValueTreeState.state.getProperty(parameters::statics::OSC_INPUT_CONNECTED, false));
    mOscControllerTab.setOscReceiveInputPort(
        mAudioProcessorValueTreeState.state.getProperty(parameters::statics::OSC_INPUT_PORT, DEFAULT_OSC_INPUT_PORT));

    mOscControllerTab.setOscSendToggleState(
        mAudioProcessorValueTreeState.state.getProperty(parameters::statics::OSC_OUTPUT_CONNECTED, false));
    mOscControllerTab.setOscSendOutputAddress(
        mAudioProcessorValueTreeState.state.getProperty(parameters::statics::OSC_OUTPUT_ADDRESS,
                                                        DEFAULT_OSC_OUTPUT_ADDRESS));
    mOscControllerTab.setOscSendOutputPort(
        mAudioProcessorValueTreeState.state.getProperty(parameters::statics::OSC_OUTPUT_PORT, DEFAULT_OSC_OUTPUT_PORT));

    // Set state for trajectory box persistent values.
    //-----------------------------------------------
    mTrajectorySection.setPositionBackAndForth(
        mAudioProcessorValueTreeState.state.getProperty(parameters::statics::POSITION_BACK_AND_FORTH, false));
    mTrajectorySection.setElevationBackAndForth(
        mAudioProcessorValueTreeState.state.getProperty(parameters::statics::ELEVATION_BACK_AND_FORTH, false));
    mTrajectorySection.setPositionDampeningCycles(
        mAudioProcessorValueTreeState.state.getProperty(parameters::statics::POSITION_DAMPENING_CYCLES, 0));
    mPositionTrajectoryManager.setPositionDampeningCycles(
        mAudioProcessorValueTreeState.state.getProperty(parameters::statics::POSITION_DAMPENING_CYCLES, 0));
    mTrajectorySection.setElevationDampeningCycles(
        mAudioProcessorValueTreeState.state.getProperty(parameters::statics::ELEVATION_DAMPENING_CYCLES, 0));
    mElevationTrajectoryManager.setPositionDampeningCycles(
        mAudioProcessorValueTreeState.state.getProperty(parameters::statics::ELEVATION_DAMPENING_CYCLES, 0));
    mTrajectorySection.setDeviationPerCycle(
        mAudioProcessorValueTreeState.state.getProperty(parameters::statics::DEVIATION_PER_CYCLE, 0));
    mPositionTrajectoryManager.setDeviationPerCycle(
        Degrees{ mAudioProcessorValueTreeState.state.getProperty(parameters::statics::DEVIATION_PER_CYCLE, 0) });
    mTrajectorySection.setCycleDuration(
        mAudioProcessorValueTreeState.state.getProperty(parameters::statics::CYCLE_DURATION, 5.0));
    mTrajectorySection.setDurationUnit(
        mAudioProcessorValueTreeState.state.getProperty(parameters::statics::DURATION_UNIT, 1));

    // Update the position preset box.
    //--------------------------------
    auto const savedPresets{ mAudioProcessor.getPresetsManager().getSavedPresets() };
    int index{ 1 };
    for (auto const saved : savedPresets) {
        mPositionPresetComponent.presetSaved(index++, saved);
    }

    // Update the interface.
    //----------------------
    mSpansSection.setSelectedSource(&mAudioProcessor.getSources()[mSelectedSource]);
    mPositionField.setSelectedSource(mSelectedSource);
    mElevationField.setSelectedSource(mSelectedSource);
    mSourcePositionTab.updateSelectedSource(&mAudioProcessor.getSources()[mSelectedSource],
                                            mSelectedSource,
                                            mAudioProcessor.getSpatMode());

    auto const preset{ static_cast<int>(static_cast<float>(
        mAudioProcessorValueTreeState.getParameterAsValue(parameters::dynamic::POSITION_PRESET).getValue())) };
    mPositionPresetComponent.setPreset(preset, false);

    mIsInsideSetPluginState = false;
}

//==============================================================================
void ControlGrisAudioProcessorEditor::updateSpanLinkButton(bool const state)
{
    mSpansSection.setSpanLinkState(state);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::updatePositionPreset(int const presetNumber)
{
    juce::MessageManagerLock mml{};
    mPositionPresetComponent.setPreset(presetNumber, true);
}

//==============================================================================
// Value::Listener callback. Called when the stored window size changes.
void ControlGrisAudioProcessorEditor::valueChanged(juce::Value &)
{
    setSize(mLastUiWidth.getValue(), mLastUiHeight.getValue());
}

//==============================================================================
// SectionGeneralSettings::Listener callbacks.
void ControlGrisAudioProcessorEditor::oscFormatChangedCallback(SpatMode const mode)
{
    mGeneralSettingsTab.setOscFormat(mode);
    mAudioProcessor.setSpatMode(mode);
    mPositionField.setSpatMode(mode);
    mTrajectorySection.setSpatMode(mode);
    repaint();
    resized();
}

//==============================================================================
void ControlGrisAudioProcessorEditor::oscPortChangedCallback(int const oscPort)
{
    mAudioProcessor.setOscPortNumber(oscPort);
    mGeneralSettingsTab.setOscPortNumber(oscPort);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::oscAddressChangedCallback(juce::String const & address)
{
    mAudioProcessor.setOscAddress(address);
    mGeneralSettingsTab.setOscAddress(address);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::oscStateChangedCallback(bool const state)
{
    mAudioProcessor.setOscActive(state);
    mGeneralSettingsTab.setActivateButtonState(mAudioProcessor.isOscActive());
}

//==============================================================================
void ControlGrisAudioProcessorEditor::numberOfSourcesChangedCallback(int const numOfSources)
{
    if (mAudioProcessor.getSources().size() != numOfSources || mIsInsideSetPluginState) {
        auto const initSourcePlacement{ mAudioProcessor.getSources().size() != numOfSources };
        auto const currentPositionSourceLink{ mPositionTrajectoryManager.getSourceLink() };
        auto const symmetricLinkAllowed{ numOfSources == 2 };
        mTrajectorySection.setSymmetricLinkComboState(symmetricLinkAllowed);
        if (!symmetricLinkAllowed) {
            auto const isCurrentPositionSourceLinkSymmetric{ currentPositionSourceLink == PositionSourceLink::symmetricX
                                                             || currentPositionSourceLink
                                                                    == PositionSourceLink::symmetricY };
            if (isCurrentPositionSourceLinkSymmetric) {
                mAudioProcessor.setPositionSourceLink(PositionSourceLink::independent,
                                                      SourceLinkEnforcer::OriginOfChange::user);
            }
        }

        mSelectedSource = {};
        mAudioProcessor.setNumberOfSources(numOfSources);
        mGeneralSettingsTab.setNumberOfSources(numOfSources);
        mTrajectorySection.setNumberOfSources(numOfSources);
        mSpansSection.setSelectedSource(&mAudioProcessor.getSources()[mSelectedSource]);
        mPositionField.refreshSources();
        mElevationField.refreshSources();
        mSourcePositionTab.setNumberOfSources(numOfSources, mAudioProcessor.getFirstSourceId());
        if (initSourcePlacement) {
            sourcesPlacementChangedCallback(SourcePlacement::leftAlternate);
        }
    }
}

//==============================================================================
void ControlGrisAudioProcessorEditor::firstSourceIdChangedCallback(SourceId const firstSourceId)
{
    mAudioProcessor.setFirstSourceId(firstSourceId);
    mGeneralSettingsTab.setFirstSourceId(firstSourceId);
    mSpansSection.setSelectedSource(&mAudioProcessor.getSources()[mSelectedSource]);
    mSourcePositionTab.setNumberOfSources(mAudioProcessor.getSources().size(), firstSourceId);

    mPositionField.rebuildSourceComponents(mAudioProcessor.getSources().size());
    mElevationField.rebuildSourceComponents(mAudioProcessor.getSources().size());
    if (mAudioProcessor.getSpatMode() == SpatMode::cube)
        mElevationField.repaint();

    mSpansSection.repaint();
}

//==============================================================================
// SectionSourcePosition::Listener callbacks.
void ControlGrisAudioProcessorEditor::sourceSelectionChangedCallback(SourceIndex const sourceIndex)
{
    mSelectedSource = sourceIndex;

    mSpansSection.setSelectedSource(&mAudioProcessor.getSources()[mSelectedSource]);
    mPositionField.setSelectedSource(mSelectedSource);
    mElevationField.setSelectedSource(mSelectedSource);
    mSourcePositionTab.updateSelectedSource(&mAudioProcessor.getSources()[mSelectedSource],
                                            mSelectedSource,
                                            mAudioProcessor.getSpatMode());
}

//==============================================================================
void ControlGrisAudioProcessorEditor::sourcesPlacementChangedCallback(SourcePlacement const sourcePlacement)
{
    auto const numOfSources = mAudioProcessor.getSources().size();
    constexpr Degrees azims2[2] = { Degrees{ -90.0f }, Degrees{ 90.0f } };
    constexpr Degrees azims4[4] = { Degrees{ -45.0f }, Degrees{ 45.0f }, Degrees{ -135.0f }, Degrees{ 135.0f } };
    constexpr Degrees azims6[6] = { Degrees{ -30.0f }, Degrees{ 30.0f },   Degrees{ -90.0f },
                                    Degrees{ 90.0f },  Degrees{ -150.0f }, Degrees{ 150.0f } };
    constexpr Degrees azims8[8] = { Degrees{ -22.5f },  Degrees{ 22.5f },  Degrees{ -67.5f },  Degrees{ 67.5f },
                                    Degrees{ -112.5f }, Degrees{ 112.5f }, Degrees{ -157.5f }, Degrees{ 157.5f } };

    auto const isCubeMode{ mAudioProcessor.getSpatMode() == SpatMode::cube };
    jassert(numOfSources > 0);
    auto const offset{ Degrees{ 360.0f } / narrow<float>(numOfSources) / 2.0f };
    auto const distance{ isCubeMode ? 0.7f : 1.0f };

    auto const getAzimuths = [&]() {
        if (numOfSources <= 2) {
            return azims2;
        }
        if (numOfSources <= 4) {
            return azims4;
        }
        if (numOfSources <= 6) {
            return azims6;
        }
        return azims8;
    };

    auto const * const azimuths{ getAzimuths() };

    auto const getAzimuthValue = [&](int const sourceIndex) {
        switch (sourcePlacement) {
        case SourcePlacement::leftAlternate:
            return azimuths[sourceIndex];
        case SourcePlacement::rightAlternate:
            return -azimuths[sourceIndex];
        case SourcePlacement::leftClockwise:
            return Degrees{ 360.0f } / narrow<float>(numOfSources * sourceIndex) - offset;
        case SourcePlacement::leftCounterClockwise:
            return Degrees{ 360.0f } / narrow<float>(numOfSources * -sourceIndex) - offset;
        case SourcePlacement::rightClockwise:
            return Degrees{ 360.0f } / narrow<float>(numOfSources * sourceIndex) + offset;
        case SourcePlacement::rightCounterClockwise:
            return Degrees{ 360.0f } / narrow<float>(numOfSources * -sourceIndex) + offset;
        case SourcePlacement::topClockwise:
            return Degrees{ 360.0f } / narrow<float>(numOfSources * sourceIndex);
        case SourcePlacement::topCounterClockwise:
            return Degrees{ 360.0f } / narrow<float>(numOfSources * -sourceIndex);
        default:
            jassertfalse;
            break;
        }
        return Degrees{};
    };

    auto const sourceLink{ mPositionTrajectoryManager.getSourceLink() };
    mAudioProcessor.setPositionSourceLink(PositionSourceLink::independent,
                                          SourceLinkEnforcer::OriginOfChange::automation);

    auto const setPosition = [&](int const sourceIndex) {
        auto & source{ mAudioProcessor.getSources()[sourceIndex] };
        auto const elevation{ isCubeMode ? source.getElevation() : MAX_ELEVATION };
        source.setCoordinates(getAzimuthValue(sourceIndex),
                              elevation,
                              distance,
                              Source::OriginOfChange::userAnchorMove);
    };

    for (auto sourceIndex{ numOfSources - 1 }; sourceIndex >= 0; --sourceIndex) {
        setPosition(sourceIndex);
    }

    // mAudioProcessor.updatePrimarySourceParameters(Source::ChangeType::position);

    for (SourceIndex i{}; i < SourceIndex{ numOfSources }; ++i) {
        mAudioProcessor.setSourceParameterValue(i,
                                                SourceParameter::azimuth,
                                                mAudioProcessor.getSources()[i].getNormalizedAzimuth().get());
        mAudioProcessor.setSourceParameterValue(i,
                                                SourceParameter::elevation,
                                                mAudioProcessor.getSources()[i].getNormalizedElevation().get());
        mAudioProcessor.setSourceParameterValue(i,
                                                SourceParameter::distance,
                                                mAudioProcessor.getSources()[i].getDistance());
    }

    mSourcePositionTab.updateSelectedSource(&mAudioProcessor.getSources()[mSelectedSource],
                                            SourceIndex{},
                                            mAudioProcessor.getSpatMode());

    mPositionTrajectoryManager.setTrajectoryType(mPositionTrajectoryManager.getTrajectoryType(),
                                                 mAudioProcessor.getSources().getPrimarySource().getPos());

    mAudioProcessor.setPositionSourceLink(sourceLink, SourceLinkEnforcer::OriginOfChange::automation);

    repaint();
}

//==============================================================================
void ControlGrisAudioProcessorEditor::sourcePositionChangedCallback(SourceIndex const sourceIndex,
                                                                    std::optional<Radians> const azimuth,
                                                                    std::optional<Radians> const elevation,
                                                                    std::optional<float> const x,
                                                                    std::optional<float> const y,
                                                                    std::optional<float> const z)
{
    auto & source{ mAudioProcessor.getSources()[sourceIndex] };

    if (azimuth) {
        source.setAzimuth(*azimuth, Source::OriginOfChange::userMove);
    } else if (elevation) {
        source.setElevation(*elevation, Source::OriginOfChange::userMove);
    } else if (x) {
        source.setX(*x, Source::OriginOfChange::userMove);
    } else if (y) {
        source.setY(*y, Source::OriginOfChange::userMove);
    } else if (z) {
        source.setElevation(MAX_ELEVATION * *z, Source::OriginOfChange::userMove);
    } else {
        jassertfalse;
    }
}

//==============================================================================
void ControlGrisAudioProcessorEditor::selectedSourceClicked()
{
    // increment source index
    mSelectedSource = SourceIndex{ (mSelectedSource.get() + 1) % mAudioProcessor.getSources().size() };

    mSpansSection.setSelectedSource(&mAudioProcessor.getSources()[mSelectedSource]);
    mPositionField.setSelectedSource(mSelectedSource);
    mElevationField.setSelectedSource(mSelectedSource);
    mSourcePositionTab.updateSelectedSource(&mAudioProcessor.getSources()[mSelectedSource],
                                            mSelectedSource,
                                            mAudioProcessor.getSpatMode());
}

//==============================================================================
void ControlGrisAudioProcessorEditor::positionTrajectoryBackAndForthChangedCallback(bool value)
{
    mAudioProcessorValueTreeState.state.setProperty(parameters::statics::POSITION_BACK_AND_FORTH, value, nullptr);
    mPositionTrajectoryManager.setPositionBackAndForth(value);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::elevationTrajectoryBackAndForthChangedCallback(bool value)
{
    mAudioProcessorValueTreeState.state.setProperty(parameters::statics::ELEVATION_BACK_AND_FORTH, value, nullptr);
    mElevationTrajectoryManager.setPositionBackAndForth(value);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::positionTrajectoryDampeningCyclesChangedCallback(int value)
{
    mAudioProcessorValueTreeState.state.setProperty(parameters::statics::POSITION_DAMPENING_CYCLES, value, nullptr);
    mPositionTrajectoryManager.setPositionDampeningCycles(value);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::elevationTrajectoryDampeningCyclesChangedCallback(int value)
{
    mAudioProcessorValueTreeState.state.setProperty(parameters::statics::ELEVATION_DAMPENING_CYCLES, value, nullptr);
    mElevationTrajectoryManager.setPositionDampeningCycles(value);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::trajectoryDeviationPerCycleChangedCallback(float degrees)
{
    mAudioProcessorValueTreeState.state.setProperty(parameters::statics::DEVIATION_PER_CYCLE, degrees, nullptr);
    mPositionTrajectoryManager.setDeviationPerCycle(Degrees{ degrees });
}

//==============================================================================
void ControlGrisAudioProcessorEditor::trajectoryCycleDurationChangedCallback(double duration, int mode)
{
    mAudioProcessorValueTreeState.state.setProperty(parameters::statics::CYCLE_DURATION, duration, nullptr);
    double dur = duration;
    if (mode == 2) {
        dur = duration * 60.0 / mAudioProcessor.getBpm();
    }
    mPositionTrajectoryManager.setPlaybackDuration(dur);
    mElevationTrajectoryManager.setPlaybackDuration(dur);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::trajectoryDurationUnitChangedCallback(double duration, int mode)
{
    mAudioProcessorValueTreeState.state.setProperty(parameters::statics::DURATION_UNIT, mode, nullptr);
    double dur = duration;
    if (mode == 2) {
        dur = duration * 60.0 / mAudioProcessor.getBpm();
    }
    mPositionTrajectoryManager.setPlaybackDuration(dur);
    mElevationTrajectoryManager.setPlaybackDuration(dur);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::positionTrajectoryStateChangedCallback(bool value)
{
    mPositionTrajectoryManager.setPositionActivateState(value);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::elevationTrajectoryStateChangedCallback(bool value)
{
    mElevationTrajectoryManager.setPositionActivateState(value);
}

//==============================================================================
// Update the interface if anything has changed (mostly automations).
void ControlGrisAudioProcessorEditor::refresh()
{
    mSpansSection.setSelectedSource(&mAudioProcessor.getSources()[mSelectedSource]);
    mSourcePositionTab.updateSelectedSource(&mAudioProcessor.getSources()[mSelectedSource],
                                            mSelectedSource,
                                            mAudioProcessor.getSpatMode());

    mPositionField.setIsPlaying(mAudioProcessor.isPlaying());
    mElevationField.setIsPlaying(mAudioProcessor.isPlaying());

    if (mTrajectorySection.getPositionActivateState() != mPositionTrajectoryManager.getPositionActivateState()) {
        mTrajectorySection.setPositionActivateState(mPositionTrajectoryManager.getPositionActivateState());
    }
    if (mTrajectorySection.getElevationActivateState() != mElevationTrajectoryManager.getPositionActivateState()) {
        mTrajectorySection.setElevationActivateState(mElevationTrajectoryManager.getPositionActivateState());
    }
}

//==============================================================================
// FieldComponent::Listener callback.
void ControlGrisAudioProcessorEditor::fieldSourcePositionChangedCallback(SourceIndex const sourceIndex,
                                                                         int const whichField)
{
    mAudioProcessor.sourcePositionChanged(sourceIndex, whichField);
    mSelectedSource = sourceIndex;
    mSpansSection.setSelectedSource(&mAudioProcessor.getSources()[sourceIndex]);
    mPositionField.setSelectedSource(mSelectedSource);
    mElevationField.setSelectedSource(mSelectedSource);
    mSourcePositionTab.updateSelectedSource(&mAudioProcessor.getSources()[mSelectedSource],
                                            mSelectedSource,
                                            mAudioProcessor.getSpatMode());
}

//==============================================================================
// PositionPresetComponent::Listener callback.
void ControlGrisAudioProcessorEditor::positionPresetChangedCallback(int const presetNumber)
{
    mAudioProcessor.getPresetsManager().forceLoad(presetNumber);

    auto * parameter{ mAudioProcessorValueTreeState.getParameter(parameters::dynamic::POSITION_PRESET) };
    auto const newValue{ static_cast<float>(presetNumber) / static_cast<float>(NUMBER_OF_POSITION_PRESETS) };

    auto const gestureLock{ mAudioProcessor.getChangeGestureManager().getScopedLock(
        parameters::dynamic::POSITION_PRESET) };
    parameter->setValueNotifyingHost(newValue);

    mAudioProcessor.updatePrimarySourceParameters(Source::ChangeType::position);
    if (mAudioProcessor.getSpatMode() == SpatMode::cube) {
        mAudioProcessor.updatePrimarySourceParameters(Source::ChangeType::elevation);
    }
}

//==============================================================================
void ControlGrisAudioProcessorEditor::positionPresetSavedCallback(int presetNumber)
{
    mAudioProcessor.getPresetsManager().save(presetNumber);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::positionPresetDeletedCallback(int presetNumber)
{
    [[maybe_unused]] auto const success{ mAudioProcessor.getPresetsManager().deletePreset(presetNumber) };
    jassert(success);
}

//==============================================================================
// SectionOscController::Listener callback.
void ControlGrisAudioProcessorEditor::oscOutputPluginIdChangedCallback(int const value)
{
    mAudioProcessor.setOscOutputPluginId(value);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::oscInputConnectionChangedCallback(bool const state, int const oscPort)
{
    if (state) {
        [[maybe_unused]] auto const success{ mAudioProcessor.createOscInputConnection(oscPort) };
        jassert(success);
    } else {
        [[maybe_unused]] auto const success{ mAudioProcessor.disconnectOscInput(oscPort) };
        jassert(success);
    }
}

//==============================================================================
void ControlGrisAudioProcessorEditor::oscOutputConnectionChangedCallback(bool const state,
                                                                         juce::String const oscAddress,
                                                                         int const oscPort)
{
    if (state) {
        [[maybe_unused]] auto const success{ mAudioProcessor.createOscOutputConnection(oscAddress, oscPort) };
        jassert(success);
    } else {
        [[maybe_unused]] auto const success{ mAudioProcessor.disconnectOscOutput(oscAddress, oscPort) };
        jassert(success);
    }
}

//==============================================================================
void ControlGrisAudioProcessorEditor::repaintPositionField()
{
    mPositionField.repaint();
}

//==============================================================================
void ControlGrisAudioProcessorEditor::repaintElevationField()
{
    mElevationField.repaint();
}

//==============================================================================
void ControlGrisAudioProcessorEditor::paint(juce::Graphics & g)
{
    g.fillAll(mGrisLookAndFeel.findColour(juce::ResizableWindow::backgroundColourId));
}

//==============================================================================
void ControlGrisAudioProcessorEditor::resized()
{
    auto const width{ getWidth() - 50 }; // Remove position preset space.
    auto const height{ getHeight() };

    auto const fieldSize{ std::max(width / 2, MIN_FIELD_WIDTH_PIXELS) };

    mMainBanner.setBounds(0, 0, fieldSize, 20);
    mPositionField.setBounds(0, 20, fieldSize, fieldSize);

    if (mAudioProcessor.getSpatMode() == SpatMode::cube) {
        mMainBanner.setText("Azimuth - Distance", juce::NotificationType::dontSendNotification);
        mElevationBanner.setVisible(true);
        mElevationField.setVisible(true);
        mElevationBanner.setBounds(fieldSize, 0, fieldSize, 20);
        mElevationField.setBounds(fieldSize, 20, fieldSize, fieldSize);
    } else {
        mMainBanner.setText("Azimuth - Elevation", juce::NotificationType::dontSendNotification);
        mElevationBanner.setVisible(false);
        mElevationField.setVisible(false);
    }

    mSpansSection.setBounds(0, fieldSize + 20, width, 50);

    mTrajectoryBanner.setBounds(0, fieldSize + 70, width, 20);
    mTrajectorySection.setBounds(0, fieldSize + 90, width, 160);

    mSettingsBanner.setBounds(0, fieldSize + 250, width, 20);
    mConfigurationTabs.setBounds(0, fieldSize + 270, width, 160);

    mLastUiWidth = getWidth();
    mLastUiHeight = getHeight();

    mPresetsBanner.setBounds(width, 0, 50, 20);
    mPositionPresetComponent.setBounds(width, 20, 50, height - 20);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::setSpatMode(SpatMode spatMode)
{
    mSourcePositionTab.setSpatMode(spatMode);
}

} // namespace gris