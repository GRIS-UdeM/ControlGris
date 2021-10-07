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
    , mSectionSourceSpan(audioProcessor, *this, mGrisLookAndFeel)
    , mSectionTrajectory(mGrisLookAndFeel)
    , mSectionGeneralSettings(mGrisLookAndFeel)
    , mSectionSourcePosition(mGrisLookAndFeel, audioProcessor.getSpatMode())
    , mSectionOscController(mGrisLookAndFeel)
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

    mPositionPresetBanner.setLookAndFeel(&mGrisLookAndFeel);
    mPositionPresetBanner.setText("Preset", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(&mPositionPresetBanner);

    mPositionField.setLookAndFeel(&mGrisLookAndFeel);
    mPositionField.addListener(this);
    addAndMakeVisible(&mPositionField);

    mElevationField.setLookAndFeel(&mGrisLookAndFeel);
    mElevationField.addListener(this);
    addAndMakeVisible(&mElevationField);

    mSectionSourceSpan.setLookAndFeel(&mGrisLookAndFeel);
    addAndMakeVisible(&mSectionSourceSpan);

    mSectionTrajectory.setLookAndFeel(&mGrisLookAndFeel);
    mSectionTrajectory.addListener(this);
    addAndMakeVisible(mSectionTrajectory);
    mSectionTrajectory.setPositionSourceLink(mPositionTrajectoryManager.getSourceLink());
    mSectionTrajectory.setElevationSourceLink(
        static_cast<ElevationSourceLink>(mElevationTrajectoryManager.getSourceLink()));

    mSectionGeneralSettings.setLookAndFeel(&mGrisLookAndFeel);
    mSectionGeneralSettings.addListener(this);

    mSectionSourcePosition.setLookAndFeel(&mGrisLookAndFeel);
    mSectionSourcePosition.addListener(this);

    mSectionOscController.setLookAndFeel(&mGrisLookAndFeel);
    mSectionOscController.addListener(this);

    auto const bg{ mGrisLookAndFeel.findColour(juce::ResizableWindow::backgroundColourId) };

    mConfigurationComponent.setLookAndFeel(&mGrisLookAndFeel);
    mConfigurationComponent.setColour(juce::TabbedComponent::backgroundColourId, bg);
    mConfigurationComponent.addTab("Settings", bg, &mSectionGeneralSettings, false);
    mConfigurationComponent.addTab("Sources", bg, &mSectionSourcePosition, false);
    mConfigurationComponent.addTab("Controllers", bg, &mSectionOscController, false);
    addAndMakeVisible(mConfigurationComponent);

    mPositionPresetComponent.setLookAndFeel(&mGrisLookAndFeel);
    mPositionPresetComponent.addListener(this);
    addAndMakeVisible(&mPositionPresetComponent);

    // Add sources to the fields.
    //---------------------------
    mPositionField.refreshSources();
    mElevationField.refreshSources();

    mSectionSourceSpan.setSelectedSource(&mAudioProcessor.getSources()[mSelectedSource]);

    // Manage dynamic window size of the plugin.
    //------------------------------------------
    setResizable(true, true);
    setResizeLimits(MIN_FIELD_WIDTH + 50, MIN_FIELD_WIDTH + 20, 1800, 1300);

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
    mConfigurationComponent.setLookAndFeel(nullptr);
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

    mSectionOscController.setOscOutputPluginId(mAudioProcessorValueTreeState.state.getProperty("oscOutputPluginId", 1));
    mSectionOscController.setOscReceiveToggleState(
        mAudioProcessorValueTreeState.state.getProperty("oscInputConnected", false));
    mSectionOscController.setOscReceiveInputPort(
        mAudioProcessorValueTreeState.state.getProperty("oscInputPortNumber", 9000));

    mSectionOscController.setOscSendToggleState(
        mAudioProcessorValueTreeState.state.getProperty("oscOutputConnected", false));
    mSectionOscController.setOscSendOutputAddress(
        mAudioProcessorValueTreeState.state.getProperty("oscOutputAddress", "192.168.1.100"));
    mSectionOscController.setOscSendOutputPort(
        mAudioProcessorValueTreeState.state.getProperty("oscOutputPortNumber", 8000));

    // Set state for trajectory box persistent values.
    //------------------------------------------------
    mSectionTrajectory.setTrajectoryType(mAudioProcessorValueTreeState.state.getProperty("trajectoryType", 1));
    mSectionTrajectory.setElevationTrajectoryType(
        mAudioProcessorValueTreeState.state.getProperty("trajectoryTypeAlt", 1));
    mSectionTrajectory.setPositionBackAndForth(mAudioProcessorValueTreeState.state.getProperty("backAndForth", false));
    mSectionTrajectory.setElevationBackAndForth(
        mAudioProcessorValueTreeState.state.getProperty("backAndForthAlt", false));
    mSectionTrajectory.setPositionDampeningCycles(
        mAudioProcessorValueTreeState.state.getProperty("dampeningCycles", 0));
    mPositionTrajectoryManager.setPositionDampeningCycles(
        mAudioProcessorValueTreeState.state.getProperty("dampeningCycles", 0));
    mSectionTrajectory.setElevationDampeningCycles(
        mAudioProcessorValueTreeState.state.getProperty("dampeningCyclesAlt", 0));
    mElevationTrajectoryManager.setPositionDampeningCycles(
        mAudioProcessorValueTreeState.state.getProperty("dampeningCyclesAlt", 0));
    mSectionTrajectory.setDeviationPerCycle(mAudioProcessorValueTreeState.state.getProperty("deviationPerCycle", 0));
    mPositionTrajectoryManager.setDeviationPerCycle(
        Degrees{ mAudioProcessorValueTreeState.state.getProperty("deviationPerCycle", 0) });
    mSectionTrajectory.setCycleDuration(mAudioProcessorValueTreeState.state.getProperty("cycleDuration", 5.0));
    mSectionTrajectory.setDurationUnit(mAudioProcessorValueTreeState.state.getProperty("durationUnit", 1));

    // Update the position preset box.
    //--------------------------------
    auto const savedPresets{ mAudioProcessor.getPresetsManager().getSavedPresets() };
    int index{ 1 };
    for (auto const saved : savedPresets) {
        mPositionPresetComponent.presetSaved(index++, saved);
    }

    // Update the interface.
    //----------------------
    mSectionSourceSpan.setSelectedSource(&mAudioProcessor.getSources()[mSelectedSource]);
    mPositionField.setSelectedSource(mSelectedSource);
    mElevationField.setSelectedSource(mSelectedSource);
    mSectionSourcePosition.updateSelectedSource(&mAudioProcessor.getSources()[mSelectedSource],
                                                mSelectedSource,
                                                mAudioProcessor.getSpatMode());

    auto const preset{ static_cast<int>(static_cast<float>(
        mAudioProcessorValueTreeState.getParameterAsValue(Automation::Ids::POSITION_PRESET).getValue())) };
    mPositionPresetComponent.setPreset(preset, false);

    mIsInsideSetPluginState = false;
}

//==============================================================================
void ControlGrisAudioProcessorEditor::updateSpanLinkButton(bool state)
{
    mSectionSourceSpan.setSpanLinkState(state);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::updateSourceLinkCombo(PositionSourceLink value)
{
    auto action = [=]() {
        mSectionTrajectory.getPositionSourceLinkCombo().setSelectedId(static_cast<int>(value),
                                                                      juce::NotificationType::dontSendNotification);
    };
    auto const isMessageThread{ juce::MessageManager::getInstance()->isThisTheMessageThread() };
    if (isMessageThread) {
        action();
    } else {
        juce::MessageManager::callAsync(action);
    }
}

//==============================================================================
void ControlGrisAudioProcessorEditor::updateElevationSourceLinkCombo(ElevationSourceLink value)
{
    juce::MessageManager::callAsync([=] {
        mSectionTrajectory.getElevationSourceLinkCombo().setSelectedId(static_cast<int>(value),
                                                                       juce::NotificationType::dontSendNotification);
    });
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
    mSectionGeneralSettings.setOscFormat(mode);
    mAudioProcessor.setSpatMode(mode);
    mPositionField.setSpatMode(mode);
    mSectionTrajectory.setSpatMode(mode);
    repaint();
    resized();
}

//==============================================================================
void ControlGrisAudioProcessorEditor::oscPortChangedCallback(int const oscPort)
{
    mAudioProcessor.setOscPortNumber(oscPort);
    mSectionGeneralSettings.setOscPortNumber(oscPort);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::oscAddressChangedCallback(juce::String const & address)
{
    mAudioProcessor.setOscAddress(address);
    mSectionGeneralSettings.setOscAddress(address);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::oscStateChangedCallback(bool const state)
{
    mAudioProcessor.setOscActive(state);
    mSectionGeneralSettings.setActivateButtonState(mAudioProcessor.isOscActive());
}

//==============================================================================
void ControlGrisAudioProcessorEditor::numberOfSourcesChangedCallback(int const numOfSources)
{
    if (mAudioProcessor.getSources().size() != numOfSources || mIsInsideSetPluginState) {
        auto const initSourcePlacement{ mAudioProcessor.getSources().size() != numOfSources };
        auto const currentPositionSourceLink{ mPositionTrajectoryManager.getSourceLink() };
        auto const symmetricLinkAllowed{ numOfSources == 2 };
        mSectionTrajectory.setSymmetricLinkComboState(symmetricLinkAllowed);
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
        mSectionGeneralSettings.setNumberOfSources(numOfSources);
        mSectionTrajectory.setNumberOfSources(numOfSources);
        mSectionSourceSpan.setSelectedSource(&mAudioProcessor.getSources()[mSelectedSource]);
        mPositionField.refreshSources();
        mElevationField.refreshSources();
        mSectionSourcePosition.setNumberOfSources(numOfSources, mAudioProcessor.getFirstSourceId());
        if (initSourcePlacement) {
            sourcesPlacementChangedCallback(SourcePlacement::leftAlternate);
        }
    }
}

//==============================================================================
void ControlGrisAudioProcessorEditor::firstSourceIdChangedCallback(SourceId const firstSourceId)
{
    mAudioProcessor.setFirstSourceId(firstSourceId);
    mSectionGeneralSettings.setFirstSourceId(firstSourceId);
    mSectionSourceSpan.setSelectedSource(&mAudioProcessor.getSources()[mSelectedSource]);
    mSectionSourcePosition.setNumberOfSources(mAudioProcessor.getSources().size(), firstSourceId);

    mPositionField.rebuildSourceComponents(mAudioProcessor.getSources().size());
    mElevationField.rebuildSourceComponents(mAudioProcessor.getSources().size());
    if (mAudioProcessor.getSpatMode() == SpatMode::cube)
        mElevationField.repaint();

    mSectionSourceSpan.repaint();
}

//==============================================================================
// SectionSourcePosition::Listener callbacks.
void ControlGrisAudioProcessorEditor::sourceSelectionChangedCallback(SourceIndex const sourceIndex)
{
    mSelectedSource = sourceIndex;

    mSectionSourceSpan.setSelectedSource(&mAudioProcessor.getSources()[mSelectedSource]);
    mPositionField.setSelectedSource(mSelectedSource);
    mElevationField.setSelectedSource(mSelectedSource);
    mSectionSourcePosition.updateSelectedSource(&mAudioProcessor.getSources()[mSelectedSource],
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

    auto const offset{ Degrees{ 360.0f } / numOfSources / 2.0f };
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
            return Degrees{ 360.0f } / numOfSources * sourceIndex - offset;
        case SourcePlacement::leftCounterClockwise:
            return Degrees{ 360.0f } / numOfSources * -sourceIndex - offset;
        case SourcePlacement::rightClockwise:
            return Degrees{ 360.0f } / numOfSources * sourceIndex + offset;
        case SourcePlacement::rightCounterClockwise:
            return Degrees{ 360.0f } / numOfSources * -sourceIndex + offset;
        case SourcePlacement::topClockwise:
            return Degrees{ 360.0f } / numOfSources * sourceIndex;
        case SourcePlacement::topCounterClockwise:
            return Degrees{ 360.0f } / numOfSources * -sourceIndex;
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

    mSectionSourcePosition.updateSelectedSource(&mAudioProcessor.getSources()[mSelectedSource],
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

    mSectionSourceSpan.setSelectedSource(&mAudioProcessor.getSources()[mSelectedSource]);
    mPositionField.setSelectedSource(mSelectedSource);
    mElevationField.setSelectedSource(mSelectedSource);
    mSectionSourcePosition.updateSelectedSource(&mAudioProcessor.getSources()[mSelectedSource],
                                                mSelectedSource,
                                                mAudioProcessor.getSpatMode());
}

//==============================================================================
// SectionTrajectory::Listener callbacks.
void ControlGrisAudioProcessorEditor::positionSourceLinkChangedCallback(PositionSourceLink const sourceLink)
{
    mAudioProcessor.setPositionSourceLink(sourceLink, SourceLinkEnforcer::OriginOfChange::user);

    auto const howMany{ static_cast<float>(POSITION_SOURCE_LINK_TYPES.size() - 1) };
    auto const value{ (static_cast<float>(sourceLink) - 1.0f) / howMany };
    auto * parameter{ mAudioProcessorValueTreeState.getParameter(Automation::Ids::POSITION_SOURCE_LINK) };
    auto const gestureLock{ mAudioProcessor.getChangeGestureManager().getScopedLock(
        Automation::Ids::POSITION_SOURCE_LINK) };
    parameter->setValueNotifyingHost(value);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::elevationSourceLinkChangedCallback(ElevationSourceLink const sourceLink)
{
    mAudioProcessor.setElevationSourceLink(sourceLink, SourceLinkEnforcer::OriginOfChange::user);

    auto const howMany{ static_cast<float>(ELEVATION_SOURCE_LINK_TYPES.size() - 1) };
    auto const value{ (static_cast<float>(sourceLink) - 1.0f) / howMany };
    auto * parameter{ mAudioProcessorValueTreeState.getParameter(Automation::Ids::ELEVATION_SOURCE_LINK) };
    auto const gestureLock{ mAudioProcessor.getChangeGestureManager().getScopedLock(
        Automation::Ids::ELEVATION_SOURCE_LINK) };
    parameter->setValueNotifyingHost(value);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::positionTrajectoryTypeChangedCallback(PositionTrajectoryType value)
{
    mAudioProcessorValueTreeState.state.setProperty("trajectoryType", static_cast<int>(value), nullptr);
    mPositionTrajectoryManager.setTrajectoryType(value, mAudioProcessor.getSources()[0].getPos());
    mPositionField.repaint();
}

//==============================================================================
void ControlGrisAudioProcessorEditor::elevationTrajectoryTypeChangedCallback(ElevationTrajectoryType value)
{
    mAudioProcessorValueTreeState.state.setProperty("trajectoryTypeAlt", static_cast<int>(value), nullptr);
    mElevationTrajectoryManager.setTrajectoryType(value);
    mElevationField.repaint();
}

//==============================================================================
void ControlGrisAudioProcessorEditor::positionTrajectoryBackAndForthChangedCallback(bool value)
{
    mAudioProcessorValueTreeState.state.setProperty("backAndForth", value, nullptr);
    mPositionTrajectoryManager.setPositionBackAndForth(value);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::elevationTrajectoryBackAndForthChangedCallback(bool value)
{
    mAudioProcessorValueTreeState.state.setProperty("backAndForthAlt", value, nullptr);
    mElevationTrajectoryManager.setPositionBackAndForth(value);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::positionTrajectoryDampeningCyclesChangedCallback(int value)
{
    mAudioProcessorValueTreeState.state.setProperty("dampeningCycles", value, nullptr);
    mPositionTrajectoryManager.setPositionDampeningCycles(value);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::elevationTrajectoryDampeningCyclesChangedCallback(int value)
{
    mAudioProcessorValueTreeState.state.setProperty("dampeningCyclesAlt", value, nullptr);
    mElevationTrajectoryManager.setPositionDampeningCycles(value);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::trajectoryDeviationPerCycleChangedCallback(float degrees)
{
    mAudioProcessorValueTreeState.state.setProperty("deviationPerCycle", degrees, nullptr);
    mPositionTrajectoryManager.setDeviationPerCycle(Degrees{ degrees });
}

//==============================================================================
void ControlGrisAudioProcessorEditor::trajectoryCycleDurationChangedCallback(double duration, int mode)
{
    mAudioProcessorValueTreeState.state.setProperty("cycleDuration", duration, nullptr);
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
    mAudioProcessorValueTreeState.state.setProperty("durationUnit", mode, nullptr);
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
    mSectionSourceSpan.setSelectedSource(&mAudioProcessor.getSources()[mSelectedSource]);
    mSectionSourcePosition.updateSelectedSource(&mAudioProcessor.getSources()[mSelectedSource],
                                                mSelectedSource,
                                                mAudioProcessor.getSpatMode());

    mPositionField.setIsPlaying(mAudioProcessor.isPlaying());
    mElevationField.setIsPlaying(mAudioProcessor.isPlaying());

    if (mSectionTrajectory.getPositionActivateState() != mPositionTrajectoryManager.getPositionActivateState()) {
        mSectionTrajectory.setPositionActivateState(mPositionTrajectoryManager.getPositionActivateState());
    }
    if (mSectionTrajectory.getElevationActivateState() != mElevationTrajectoryManager.getPositionActivateState()) {
        mSectionTrajectory.setElevationActivateState(mElevationTrajectoryManager.getPositionActivateState());
    }
}

//==============================================================================
// FieldComponent::Listener callback.
void ControlGrisAudioProcessorEditor::fieldSourcePositionChangedCallback(SourceIndex const sourceIndex, int whichField)
{
    mAudioProcessor.sourcePositionChanged(sourceIndex, whichField);
    mSelectedSource = sourceIndex;
    mSectionSourceSpan.setSelectedSource(&mAudioProcessor.getSources()[sourceIndex]);
    mPositionField.setSelectedSource(mSelectedSource);
    mElevationField.setSelectedSource(mSelectedSource);
    mSectionSourcePosition.updateSelectedSource(&mAudioProcessor.getSources()[mSelectedSource],
                                                mSelectedSource,
                                                mAudioProcessor.getSpatMode());
}

//==============================================================================
// PositionPresetComponent::Listener callback.
void ControlGrisAudioProcessorEditor::positionPresetChangedCallback(int const presetNumber)
{
    mAudioProcessor.getPresetsManager().forceLoad(presetNumber);

    auto * parameter{ mAudioProcessorValueTreeState.getParameter(Automation::Ids::POSITION_PRESET) };
    auto const newValue{ static_cast<float>(presetNumber) / static_cast<float>(NUMBER_OF_POSITION_PRESETS) };

    auto const gestureLock{ mAudioProcessor.getChangeGestureManager().getScopedLock(Automation::Ids::POSITION_PRESET) };
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
        mAudioProcessor.createOscInputConnection(oscPort);
    } else {
        mAudioProcessor.disconnectOscInput(oscPort);
    }
}

//==============================================================================
void ControlGrisAudioProcessorEditor::oscOutputConnectionChangedCallback(bool const state,
                                                                         juce::String const oscAddress,
                                                                         int const oscPort)
{
    if (state) {
        mAudioProcessor.createOscOutputConnection(oscAddress, oscPort);
    } else {
        mAudioProcessor.disconnectOscOutput(oscAddress, oscPort);
    }
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

    auto const fieldSize{ std::max(width / 2, MIN_FIELD_WIDTH) };

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

    mSectionSourceSpan.setBounds(0, fieldSize + 20, width, 50);

    mTrajectoryBanner.setBounds(0, fieldSize + 70, width, 20);
    mSectionTrajectory.setBounds(0, fieldSize + 90, width, 160);

    mSettingsBanner.setBounds(0, fieldSize + 250, width, 20);
    mConfigurationComponent.setBounds(0, fieldSize + 270, width, 160);

    mLastUiWidth = getWidth();
    mLastUiHeight = getHeight();

    mPositionPresetBanner.setBounds(width, 0, 50, 20);
    mPositionPresetComponent.setBounds(width, 20, 50, height - 20);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::setSpatMode(SpatMode spatMode)
{
    mSectionSourcePosition.setSpatMode(spatMode);
}

} // namespace gris