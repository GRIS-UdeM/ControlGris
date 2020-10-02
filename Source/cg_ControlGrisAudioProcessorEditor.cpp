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

//==============================================================================
ControlGrisAudioProcessorEditor::ControlGrisAudioProcessorEditor(
    ControlGrisAudioProcessor & controlGrisAudioProcessor,
    AudioProcessorValueTreeState & vts,
    PositionTrajectoryManager & positionAutomationManager,
    ElevationTrajectoryManager & elevationAutomationManager)
    : AudioProcessorEditor(&controlGrisAudioProcessor)
    , mProcessor(controlGrisAudioProcessor)
    , mAudioProcessorValueTreeState(vts)
    , mPositionTrajectoryManager(positionAutomationManager)
    , mElevationTrajectoryManager(elevationAutomationManager)
    , mPositionField(controlGrisAudioProcessor.getSources(), positionAutomationManager)
    , mElevationField(controlGrisAudioProcessor.getSources(), elevationAutomationManager)
    , mParametersBox(mGrisLookAndFeel)
    , mTrajectoryBox(mGrisLookAndFeel)
    , mSettingsBox(mGrisLookAndFeel)
    , mSourceBox(mGrisLookAndFeel)
    , mInterfaceBox(mGrisLookAndFeel)
    , mPositionPresetBox(controlGrisAudioProcessor.getPresetsManager())
{
    setLookAndFeel(&mGrisLookAndFeel);

    mIsInsideSetPluginState = false;
    mSelectedSource = {};

    // Set up the interface.
    //----------------------
    mMainBanner.setLookAndFeel(&mGrisLookAndFeel);
    mMainBanner.setText("Azimuth - Elevation", NotificationType::dontSendNotification);
    addAndMakeVisible(&mMainBanner);

    mElevationBanner.setLookAndFeel(&mGrisLookAndFeel);
    mElevationBanner.setText("Elevation", NotificationType::dontSendNotification);
    addAndMakeVisible(&mElevationBanner);

    mTrajectoryBanner.setLookAndFeel(&mGrisLookAndFeel);
    mTrajectoryBanner.setText("Trajectories", NotificationType::dontSendNotification);
    addAndMakeVisible(&mTrajectoryBanner);

    mSettingsBanner.setLookAndFeel(&mGrisLookAndFeel);
    mSettingsBanner.setText("Configuration", NotificationType::dontSendNotification);
    addAndMakeVisible(&mSettingsBanner);

    mPositionPresetBanner.setLookAndFeel(&mGrisLookAndFeel);
    mPositionPresetBanner.setText("Preset", NotificationType::dontSendNotification);
    addAndMakeVisible(&mPositionPresetBanner);

    mPositionField.setLookAndFeel(&mGrisLookAndFeel);
    mPositionField.addListener(this);
    addAndMakeVisible(&mPositionField);

    mElevationField.setLookAndFeel(&mGrisLookAndFeel);
    mElevationField.addListener(this);
    addAndMakeVisible(&mElevationField);

    mParametersBox.setLookAndFeel(&mGrisLookAndFeel);
    mParametersBox.addListener(this);
    addAndMakeVisible(&mParametersBox);

    mTrajectoryBox.setLookAndFeel(&mGrisLookAndFeel);
    mTrajectoryBox.addListener(this);
    addAndMakeVisible(mTrajectoryBox);
    mTrajectoryBox.setPositionSourceLink(mPositionTrajectoryManager.getSourceLink());
    mTrajectoryBox.setElevationSourceLink(
        static_cast<ElevationSourceLink>(mElevationTrajectoryManager.getSourceLink()));

    mSettingsBox.setLookAndFeel(&mGrisLookAndFeel);
    mSettingsBox.addListener(this);

    mSourceBox.setLookAndFeel(&mGrisLookAndFeel);
    mSourceBox.addListener(this);

    mInterfaceBox.setLookAndFeel(&mGrisLookAndFeel);
    mInterfaceBox.addListener(this);

    auto const bg{ mGrisLookAndFeel.findColour(ResizableWindow::backgroundColourId) };

    mConfigurationComponent.setLookAndFeel(&mGrisLookAndFeel);
    mConfigurationComponent.setColour(TabbedComponent::backgroundColourId, bg);
    mConfigurationComponent.addTab("Settings", bg, &mSettingsBox, false);
    mConfigurationComponent.addTab("Source", bg, &mSourceBox, false);
    mConfigurationComponent.addTab("Controllers", bg, &mInterfaceBox, false);
    addAndMakeVisible(mConfigurationComponent);

    mPositionPresetBox.setLookAndFeel(&mGrisLookAndFeel);
    mPositionPresetBox.addListener(this);
    addAndMakeVisible(&mPositionPresetBox);

    // Add sources to the fields.
    //---------------------------
    mPositionField.refreshSources();
    mElevationField.refreshSources();

    mParametersBox.setSelectedSource(&mProcessor.getSources()[mSelectedSource]);

    // Manage dynamic window size of the plugin.
    //------------------------------------------
    setResizeLimits(MIN_FIELD_WIDTH + 50, MIN_FIELD_WIDTH + 20, 1800, 1300);

    mLastUIWidth.referTo(
        mProcessor.getValueTreeState().state.getChildWithName("uiState").getPropertyAsValue("width", nullptr));
    mLastUIHeight.referTo(
        mProcessor.getValueTreeState().state.getChildWithName("uiState").getPropertyAsValue("height", nullptr));

    // set our component's initial size to be the last one that was stored in the filter's settings
    setSize(mLastUIWidth.getValue(), mLastUIHeight.getValue());

    mLastUIWidth.addListener(this);
    mLastUIHeight.addListener(this);

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
    settingsBoxOscFormatChanged(mProcessor.getSpatMode());
    settingsBoxOscPortNumberChanged(mProcessor.getOscPortNumber());
    settingsBoxOscActivated(mProcessor.isOscConnected());
    settingsBoxFirstSourceIdChanged(mProcessor.getFirstSourceId());
    settingsBoxNumberOfSourcesChanged(mProcessor.getSources().size());

    mInterfaceBox.setOscOutputPluginId(mAudioProcessorValueTreeState.state.getProperty("oscOutputPluginId", 1));
    mInterfaceBox.setOscReceiveToggleState(mAudioProcessorValueTreeState.state.getProperty("oscInputConnected", false));
    mInterfaceBox.setOscReceiveInputPort(mAudioProcessorValueTreeState.state.getProperty("oscInputPortNumber", 9000));

    mInterfaceBox.setOscSendToggleState(mAudioProcessorValueTreeState.state.getProperty("oscOutputConnected", false));
    mInterfaceBox.setOscSendOutputAddress(
        mAudioProcessorValueTreeState.state.getProperty("oscOutputAddress", "192.168.1.100"));
    mInterfaceBox.setOscSendOutputPort(mAudioProcessorValueTreeState.state.getProperty("oscOutputPortNumber", 8000));

    // Set state for trajectory box persistent values.
    //------------------------------------------------
    mTrajectoryBox.setTrajectoryType(mAudioProcessorValueTreeState.state.getProperty("trajectoryType", 1));
    mTrajectoryBox.setElevationTrajectoryType(mAudioProcessorValueTreeState.state.getProperty("trajectoryTypeAlt", 1));
    mTrajectoryBox.setPositionBackAndForth(mAudioProcessorValueTreeState.state.getProperty("backAndForth", false));
    mTrajectoryBox.setElevationBackAndForth(mAudioProcessorValueTreeState.state.getProperty("backAndForthAlt", false));
    mTrajectoryBox.setPositionDampeningCycles(mAudioProcessorValueTreeState.state.getProperty("dampeningCycles", 0));
    mPositionTrajectoryManager.setPositionDampeningCycles(
        mAudioProcessorValueTreeState.state.getProperty("dampeningCycles", 0));
    mTrajectoryBox.setElevationDampeningCycles(
        mAudioProcessorValueTreeState.state.getProperty("dampeningCyclesAlt", 0));
    mElevationTrajectoryManager.setPositionDampeningCycles(
        mAudioProcessorValueTreeState.state.getProperty("dampeningCyclesAlt", 0));
    mTrajectoryBox.setDeviationPerCycle(mAudioProcessorValueTreeState.state.getProperty("deviationPerCycle", 0));
    mPositionTrajectoryManager.setDeviationPerCycle(
        Degrees{ mAudioProcessorValueTreeState.state.getProperty("deviationPerCycle", 0) });
    mTrajectoryBox.setCycleDuration(mAudioProcessorValueTreeState.state.getProperty("cycleDuration", 5.0));
    mTrajectoryBox.setDurationUnit(mAudioProcessorValueTreeState.state.getProperty("durationUnit", 1));

    // Update the position preset box.
    //--------------------------------
    auto const savedPresets{ mProcessor.getPresetsManager().getSavedPresets() };
    int index{ 1 };
    for (auto const saved : savedPresets) {
        mPositionPresetBox.presetSaved(index++, saved);
    }

    // Update the interface.
    //----------------------
    mParametersBox.setSelectedSource(&mProcessor.getSources()[mSelectedSource]);
    mPositionField.setSelectedSource(mSelectedSource);
    mElevationField.setSelectedSource(mSelectedSource);
    mSourceBox.updateSelectedSource(&mProcessor.getSources()[mSelectedSource],
                                    mSelectedSource,
                                    mProcessor.getSpatMode());

    auto const preset{ static_cast<int>(static_cast<float>(
        mAudioProcessorValueTreeState.getParameterAsValue(Automation::Ids::POSITION_PRESET).getValue())) };
    mPositionPresetBox.setPreset(preset, false);

    mIsInsideSetPluginState = false;
}

//==============================================================================
void ControlGrisAudioProcessorEditor::updateSpanLinkButton(bool state)
{
    mParametersBox.setSpanLinkState(state);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::updateSourceLinkCombo(PositionSourceLink value)
{
    auto action = [=]() {
        mTrajectoryBox.getPositionSourceLinkCombo().setSelectedId(static_cast<int>(value),
                                                                  NotificationType::dontSendNotification);
    };
    auto const isMessageThread{ MessageManager::getInstance()->isThisTheMessageThread() };
    if (isMessageThread) {
        action();
    } else {
        MessageManager::callAsync(action);
    }
}

//==============================================================================
void ControlGrisAudioProcessorEditor::updateElevationSourceLinkCombo(ElevationSourceLink value)
{
    MessageManager::callAsync([=] {
        mTrajectoryBox.getElevationSourceLinkCombo().setSelectedId(static_cast<int>(value),
                                                                   NotificationType::dontSendNotification);
    });
}

//==============================================================================
void ControlGrisAudioProcessorEditor::updatePositionPreset(int presetNumber)
{
    mPositionPresetBox.setPreset(presetNumber, true);
}

//==============================================================================
// Value::Listener callback. Called when the stored window size changes.
void ControlGrisAudioProcessorEditor::valueChanged(Value &)
{
    setSize(mLastUIWidth.getValue(), mLastUIHeight.getValue());
}

//==============================================================================
// SettingsBoxComponent::Listener callbacks.
void ControlGrisAudioProcessorEditor::settingsBoxOscFormatChanged(SpatMode mode)
{
    mSettingsBox.setOscFormat(mode);
    mProcessor.setSpatMode(mode);
    auto const selectionIsLBAP{ mode == SpatMode::cube };
    mParametersBox.setDistanceEnabled(selectionIsLBAP);
    mPositionField.setSpatMode(mode);
    mTrajectoryBox.setSpatMode(mode);
    repaint();
    resized();
}

//==============================================================================
void ControlGrisAudioProcessorEditor::settingsBoxOscPortNumberChanged(int oscPort)
{
    mProcessor.setOscPortNumber(oscPort);
    mSettingsBox.setOscPortNumber(oscPort);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::settingsBoxOscActivated(bool state)
{
    mProcessor.handleOscConnection(state);
    mSettingsBox.setActivateButtonState(mProcessor.isOscConnected());
}

//==============================================================================
void ControlGrisAudioProcessorEditor::settingsBoxNumberOfSourcesChanged(int const numOfSources)
{
    if (mProcessor.getSources().size() != numOfSources || mIsInsideSetPluginState) {
        auto const initSourcePlacement{ mProcessor.getSources().size() != numOfSources };
        auto const currentPositionSourceLink{ mPositionTrajectoryManager.getSourceLink() };
        auto const symmetricLinkAllowed{ numOfSources != 2 };
        mTrajectoryBox.setSymmetricLinkComboState(symmetricLinkAllowed);
        if (symmetricLinkAllowed) {
            auto const isCurrentPositionSourceLinkSymmetric{
                currentPositionSourceLink == PositionSourceLink::linkSymmetricX
                || currentPositionSourceLink == PositionSourceLink::linkSymmetricY
            };
            if (isCurrentPositionSourceLinkSymmetric) {
                mProcessor.setPositionSourceLink(PositionSourceLink::independent);
                /*mPositionTrajectoryManager.setSourceLink(PositionSourceLink::independent);
                updateSourceLinkCombo(PositionSourceLink::independent);*/
            }
        }

        mSelectedSource = {};
        mProcessor.setNumberOfSources(numOfSources);
        mSettingsBox.setNumberOfSources(numOfSources);
        mTrajectoryBox.setNumberOfSources(numOfSources);
        mParametersBox.setSelectedSource(&mProcessor.getSources()[mSelectedSource]);
        mPositionField.refreshSources();
        mElevationField.refreshSources();
        mSourceBox.setNumberOfSources(numOfSources, mProcessor.getFirstSourceId());
        if (initSourcePlacement) {
            sourceBoxPlacementChanged(SourcePlacement::leftAlternate);
        }
    }
}

//==============================================================================
void ControlGrisAudioProcessorEditor::settingsBoxFirstSourceIdChanged(SourceId const firstSourceId)
{
    mProcessor.setFirstSourceId(firstSourceId);
    mSettingsBox.setFirstSourceId(firstSourceId);
    mParametersBox.setSelectedSource(&mProcessor.getSources()[mSelectedSource]);
    mSourceBox.setNumberOfSources(mProcessor.getSources().size(), firstSourceId);

    mPositionField.rebuildSourceComponents(mProcessor.getSources().size());
    mElevationField.rebuildSourceComponents(mProcessor.getSources().size());
    if (mProcessor.getSpatMode() == SpatMode::cube)
        mElevationField.repaint();

    mParametersBox.repaint();
}

//==============================================================================
// SourceBoxComponent::Listener callbacks.
void ControlGrisAudioProcessorEditor::sourceBoxSelectionChanged(SourceIndex const sourceIndex)
{
    mSelectedSource = sourceIndex;

    mParametersBox.setSelectedSource(&mProcessor.getSources()[mSelectedSource]);
    mPositionField.setSelectedSource(mSelectedSource);
    mElevationField.setSelectedSource(mSelectedSource);
    mSourceBox.updateSelectedSource(&mProcessor.getSources()[mSelectedSource],
                                    mSelectedSource,
                                    mProcessor.getSpatMode());
}

//==============================================================================
void ControlGrisAudioProcessorEditor::sourceBoxPlacementChanged(SourcePlacement const sourcePlacement)
{
    auto const numOfSources = mProcessor.getSources().size();
    constexpr Degrees azims2[2] = { Degrees{ -90.0f }, Degrees{ 90.0f } };
    constexpr Degrees azims4[4] = { Degrees{ -45.0f }, Degrees{ 45.0f }, Degrees{ -135.0f }, Degrees{ 135.0f } };
    constexpr Degrees azims6[6] = { Degrees{ -30.0f }, Degrees{ 30.0f },   Degrees{ -90.0f },
                                    Degrees{ 90.0f },  Degrees{ -150.0f }, Degrees{ 150.0f } };
    constexpr Degrees azims8[8] = { Degrees{ -22.5f },  Degrees{ 22.5f },  Degrees{ -67.5f },  Degrees{ 67.5f },
                                    Degrees{ -112.5f }, Degrees{ 112.5f }, Degrees{ -157.5f }, Degrees{ 157.5f } };

    auto const isCubeMode{ mProcessor.getSpatMode() == SpatMode::cube };

    auto const offset{ Degrees{ 360.0f } / numOfSources / 2.0f };
    auto const distance{ isCubeMode ? 0.7f : 1.0f };

    switch (sourcePlacement) {
    case SourcePlacement::leftAlternate: {
        for (int i{}; i < numOfSources; ++i) {
            auto & source{ mProcessor.getSources()[i] };
            auto const elevation{ isCubeMode ? source.getElevation() : MAX_ELEVATION };
            if (numOfSources <= 2) {
                source.setCoordinates(azims2[i], elevation, distance, Source::OriginOfChange::userAnchorMove);
            } else if (numOfSources <= 4) {
                source.setCoordinates(azims4[i], elevation, distance, Source::OriginOfChange::userAnchorMove);
            } else if (numOfSources <= 6) {
                source.setCoordinates(azims6[i], elevation, distance, Source::OriginOfChange::userAnchorMove);
            } else {
                source.setCoordinates(azims8[i], elevation, distance, Source::OriginOfChange::userAnchorMove);
            }
        }
        break;
    }
    case SourcePlacement::rightAlternate: {
        for (int i{}; i < numOfSources; ++i) {
            auto & source{ mProcessor.getSources()[i] };
            auto const elevation{ isCubeMode ? source.getElevation() : MAX_ELEVATION };
            if (numOfSources <= 2) {
                source.setCoordinates(-azims2[i], elevation, distance, Source::OriginOfChange::userAnchorMove);
            } else if (numOfSources <= 4) {
                source.setCoordinates(-azims4[i], elevation, distance, Source::OriginOfChange::userAnchorMove);
            } else if (numOfSources <= 6) {
                source.setCoordinates(-azims6[i], elevation, distance, Source::OriginOfChange::userAnchorMove);
            } else {
                source.setCoordinates(-azims8[i], elevation, distance, Source::OriginOfChange::userAnchorMove);
            }
        }
        break;
    }
    case SourcePlacement::leftClockwise: {
        for (int i{}; i < numOfSources; ++i) {
            auto & source{ mProcessor.getSources()[i] };
            auto const azimuth{ Degrees{ 360.0f } / numOfSources * i - offset };
            auto const elevation{ isCubeMode ? source.getElevation() : MAX_ELEVATION };
            source.setCoordinates(azimuth, elevation, distance, Source::OriginOfChange::userAnchorMove);
        }
        break;
    }
    case SourcePlacement::leftCounterClockwise: {
        for (int i{}; i < numOfSources; ++i) {
            auto & source{ mProcessor.getSources()[i] };
            auto const azimuth{ Degrees{ 360.0f } / numOfSources * -i - offset };
            auto const elevation{ isCubeMode ? source.getElevation() : MAX_ELEVATION };
            mProcessor.getSources()[i].setCoordinates(azimuth,
                                                      elevation,
                                                      distance,
                                                      Source::OriginOfChange::userAnchorMove);
        }
        break;
    }
    case SourcePlacement::rightClockwise: {
        for (int i{}; i < numOfSources; ++i) {
            auto & source{ mProcessor.getSources()[i] };
            auto const azimuth{ Degrees{ 360.0f } / numOfSources * i + offset };
            auto const elevation{ isCubeMode ? source.getElevation() : MAX_ELEVATION };
            source.setCoordinates(azimuth, elevation, distance, Source::OriginOfChange::userAnchorMove);
        }
        break;
    }
    case SourcePlacement::rightCounterClockwise: {
        for (int i{}; i < numOfSources; ++i) {
            auto & source{ mProcessor.getSources()[i] };
            auto const azimuth{ Degrees{ 360.0f } / numOfSources * -i + offset };
            auto const elevation{ isCubeMode ? source.getElevation() : MAX_ELEVATION };
            source.setCoordinates(azimuth, elevation, distance, Source::OriginOfChange::userAnchorMove);
        }
        break;
    }
    case SourcePlacement::topClockwise: {
        for (int i{}; i < numOfSources; ++i) {
            auto & source{ mProcessor.getSources()[i] };
            auto const azimuth{ Degrees{ 360.0f } / numOfSources * i };
            auto const elevation{ isCubeMode ? source.getElevation() : MAX_ELEVATION };
            source.setCoordinates(azimuth, elevation, distance, Source::OriginOfChange::userAnchorMove);
        }
        break;
    }
    case SourcePlacement::topCounterClockwise: {
        for (int i{}; i < numOfSources; ++i) {
            auto & source{ mProcessor.getSources()[i] };
            auto const azimuth{ Degrees{ 360.0f } / numOfSources * -i };
            auto const elevation{ isCubeMode ? source.getElevation() : MAX_ELEVATION };
            source.setCoordinates(azimuth, elevation, distance, Source::OriginOfChange::userAnchorMove);
        }
        break;
    }
    case SourcePlacement::undefined:
        jassertfalse;
    }

    for (SourceIndex i{}; i < SourceIndex{ numOfSources }; ++i) {
        mProcessor.setSourceParameterValue(i,
                                           SourceParameter::azimuth,
                                           mProcessor.getSources()[i].getNormalizedAzimuth().toFloat());
        mProcessor.setSourceParameterValue(i,
                                           SourceParameter::elevation,
                                           mProcessor.getSources()[i].getNormalizedElevation().toFloat());
        mProcessor.setSourceParameterValue(i, SourceParameter::distance, mProcessor.getSources()[i].getDistance());
    }

    mSourceBox.updateSelectedSource(&mProcessor.getSources()[mSelectedSource],
                                    mSelectedSource,
                                    mProcessor.getSpatMode());

    mPositionTrajectoryManager.setTrajectoryType(mPositionTrajectoryManager.getTrajectoryType(),
                                                 mProcessor.getSources().getPrimarySource().getPos());

    repaint();
}

//==============================================================================
void ControlGrisAudioProcessorEditor::sourceBoxPositionChanged(SourceIndex const sourceIndex,
                                                               Radians angle,
                                                               float rayLen)
{
    auto & source{ mProcessor.getSources()[sourceIndex] };
    if (mProcessor.getSpatMode() == SpatMode::dome) {
        auto const elevation{ MAX_ELEVATION * rayLen };
        source.setCoordinates(angle, elevation, 1.0f, Source::OriginOfChange::userMove);
    } else {
        auto const currentElevation{ source.getElevation() };
        source.setCoordinates(angle, currentElevation, rayLen, Source::OriginOfChange::userMove);
    }
}

//==============================================================================
// ParametersBoxComponent::Listener callbacks.
void ControlGrisAudioProcessorEditor::parametersBoxParameterChanged(SourceParameter const sourceParameter,
                                                                    double const value)
{
    mProcessor.setSourceParameterValue(mSelectedSource, sourceParameter, static_cast<float>(value));

    mPositionField.repaint();
    if (mProcessor.getSpatMode() == SpatMode::cube) {
        mElevationField.repaint();
    }
}

//==============================================================================
void ControlGrisAudioProcessorEditor::parametersBoxSelectedSourceClicked()
{
    // increment source index
    mSelectedSource = SourceIndex{ (mSelectedSource.toInt() + 1) % mProcessor.getSources().size() };

    mParametersBox.setSelectedSource(&mProcessor.getSources()[mSelectedSource]);
    mPositionField.setSelectedSource(mSelectedSource);
    mElevationField.setSelectedSource(mSelectedSource);
    mSourceBox.updateSelectedSource(&mProcessor.getSources()[mSelectedSource],
                                    mSelectedSource,
                                    mProcessor.getSpatMode());
}

//==============================================================================
void ControlGrisAudioProcessorEditor::parametersBoxAzimuthSpanDragStarted()
{
    mProcessor.getChangeGestureManager().beginGesture(Automation::Ids::AZIMUTH_SPAN);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::parametersBoxAzimuthSpanDragEnded()
{
    mProcessor.getChangeGestureManager().endGesture(Automation::Ids::AZIMUTH_SPAN);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::parametersBoxElevationSpanDragStarted()
{
    mProcessor.getChangeGestureManager().beginGesture(Automation::Ids::ELEVATION_SPAN);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::parametersBoxElevationSpanDragEnded()
{
    mProcessor.getChangeGestureManager().endGesture(Automation::Ids::ELEVATION_SPAN);
}

//==============================================================================
// TrajectoryBoxComponent::Listener callbacks.
void ControlGrisAudioProcessorEditor::trajectoryBoxPositionSourceLinkChanged(PositionSourceLink const sourceLink)
{
    auto const howMany{ static_cast<float>(POSITION_SOURCE_LINK_TYPES.size() - 1) };
    auto const value{ (static_cast<float>(sourceLink) - 1.0f) / howMany };
    auto * parameter{ mAudioProcessorValueTreeState.getParameter(Automation::Ids::POSITION_SOURCE_LINK) };
    auto const gestureLock{ mProcessor.getChangeGestureManager().getScopedLock(Automation::Ids::POSITION_SOURCE_LINK) };
    parameter->setValueNotifyingHost(value);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::trajectoryBoxElevationSourceLinkChanged(ElevationSourceLink const sourceLink)
{
    auto const howMany{ static_cast<float>(ELEVATION_SOURCE_LINK_TYPES.size() - 1) };
    auto const value{ (static_cast<float>(sourceLink) - 1.0f) / howMany };
    auto * parameter{ mAudioProcessorValueTreeState.getParameter(Automation::Ids::ELEVATION_SOURCE_LINK) };
    auto const gestureLock{ mProcessor.getChangeGestureManager().getScopedLock(
        Automation::Ids::ELEVATION_SOURCE_LINK) };
    parameter->setValueNotifyingHost(value);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::trajectoryBoxPositionTrajectoryTypeChanged(PositionTrajectoryType value)
{
    mAudioProcessorValueTreeState.state.setProperty("trajectoryType", static_cast<int>(value), nullptr);
    mPositionTrajectoryManager.setTrajectoryType(value, mProcessor.getSources()[0].getPos());
    mPositionField.repaint();
}

//==============================================================================
void ControlGrisAudioProcessorEditor::trajectoryBoxElevationTrajectoryTypeChanged(ElevationTrajectoryType value)
{
    mAudioProcessorValueTreeState.state.setProperty("trajectoryTypeAlt", static_cast<int>(value), nullptr);
    mElevationTrajectoryManager.setTrajectoryType(value);
    mElevationField.repaint();
}

//==============================================================================
void ControlGrisAudioProcessorEditor::trajectoryBoxPositionBackAndForthChanged(bool value)
{
    mAudioProcessorValueTreeState.state.setProperty("backAndForth", value, nullptr);
    mPositionTrajectoryManager.setPositionBackAndForth(value);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::trajectoryBoxElevationBackAndForthChanged(bool value)
{
    mAudioProcessorValueTreeState.state.setProperty("backAndForthAlt", value, nullptr);
    mElevationTrajectoryManager.setPositionBackAndForth(value);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::trajectoryBoxPositionDampeningCyclesChanged(int value)
{
    mAudioProcessorValueTreeState.state.setProperty("dampeningCycles", value, nullptr);
    mPositionTrajectoryManager.setPositionDampeningCycles(value);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::trajectoryBoxElevationDampeningCyclesChanged(int value)
{
    mAudioProcessorValueTreeState.state.setProperty("dampeningCyclesAlt", value, nullptr);
    mElevationTrajectoryManager.setPositionDampeningCycles(value);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::trajectoryBoxDeviationPerCycleChanged(float degrees)
{
    mAudioProcessorValueTreeState.state.setProperty("deviationPerCycle", degrees, nullptr);
    mPositionTrajectoryManager.setDeviationPerCycle(Degrees{ degrees });
}

//==============================================================================
void ControlGrisAudioProcessorEditor::trajectoryBoxCycleDurationChanged(double duration, int mode)
{
    mAudioProcessorValueTreeState.state.setProperty("cycleDuration", duration, nullptr);
    double dur = duration;
    if (mode == 2) {
        dur = duration * 60.0 / mProcessor.getBpm();
    }
    mPositionTrajectoryManager.setPlaybackDuration(dur);
    mElevationTrajectoryManager.setPlaybackDuration(dur);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::trajectoryBoxDurationUnitChanged(double duration, int mode)
{
    mAudioProcessorValueTreeState.state.setProperty("durationUnit", mode, nullptr);
    double dur = duration;
    if (mode == 2) {
        dur = duration * 60.0 / mProcessor.getBpm();
    }
    mPositionTrajectoryManager.setPlaybackDuration(dur);
    mElevationTrajectoryManager.setPlaybackDuration(dur);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::trajectoryBoxPositionActivateChanged(bool value)
{
    mPositionTrajectoryManager.setPositionActivateState(value);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::trajectoryBoxElevationActivateChanged(bool value)
{
    mElevationTrajectoryManager.setPositionActivateState(value);
}

//==============================================================================
// Update the interface if anything has changed (mostly automations).
void ControlGrisAudioProcessorEditor::refresh()
{
    mParametersBox.setSelectedSource(&mProcessor.getSources()[mSelectedSource]);
    mSourceBox.updateSelectedSource(&mProcessor.getSources()[mSelectedSource],
                                    mSelectedSource,
                                    mProcessor.getSpatMode());

    mPositionField.setIsPlaying(mProcessor.isPlaying());
    mElevationField.setIsPlaying(mProcessor.isPlaying());

    if (mTrajectoryBox.getPositionActivateState() != mPositionTrajectoryManager.getPositionActivateState()) {
        mTrajectoryBox.setPositionActivateState(mPositionTrajectoryManager.getPositionActivateState());
    }
    if (mTrajectoryBox.getElevationActivateState() != mElevationTrajectoryManager.getPositionActivateState()) {
        mTrajectoryBox.setElevationActivateState(mElevationTrajectoryManager.getPositionActivateState());
    }
}

//==============================================================================
// FieldComponent::Listener callback.
void ControlGrisAudioProcessorEditor::fieldSourcePositionChanged(SourceIndex const sourceIndex, int whichField)
{
    mProcessor.sourcePositionChanged(sourceIndex, whichField);
    mSelectedSource = sourceIndex;
    mParametersBox.setSelectedSource(&mProcessor.getSources()[sourceIndex]);
    mPositionField.setSelectedSource(mSelectedSource);
    mElevationField.setSelectedSource(mSelectedSource);
    mSourceBox.updateSelectedSource(&mProcessor.getSources()[mSelectedSource],
                                    mSelectedSource,
                                    mProcessor.getSpatMode());
}

//==============================================================================
// PositionPresetComponent::Listener callback.
void ControlGrisAudioProcessorEditor::positionPresetChanged(int const presetNumber)
{
    //    MessageManagerLock mml{};

    mProcessor.getPresetsManager().forceLoad(presetNumber);

    auto * parameter{ mAudioProcessorValueTreeState.getParameter(Automation::Ids::POSITION_PRESET) };
    auto const newValue{ static_cast<float>(presetNumber) / static_cast<float>(NUMBER_OF_POSITION_PRESETS) };

    auto const gestureLock{ mProcessor.getChangeGestureManager().getScopedLock(Automation::Ids::POSITION_PRESET) };
    parameter->setValueNotifyingHost(newValue);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::positionPresetSaved(int presetNumber)
{
    mProcessor.getPresetsManager().save(presetNumber);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::positionPresetDeleted(int presetNumber)
{
    mProcessor.getPresetsManager().deletePreset(presetNumber);
}

//==============================================================================
// InterfaceBoxComponent::Listener callback.
void ControlGrisAudioProcessorEditor::oscOutputPluginIdChanged(int value)
{
    mProcessor.setOscOutputPluginId(value);
}

//==============================================================================
void ControlGrisAudioProcessorEditor::oscInputConnectionChanged(bool state, int oscPort)
{
    if (state) {
        mProcessor.createOscInputConnection(oscPort);
    } else {
        mProcessor.disconnectOscInput(oscPort);
    }
}

//==============================================================================
void ControlGrisAudioProcessorEditor::oscOutputConnectionChanged(bool state, String oscAddress, int oscPort)
{
    if (state) {
        mProcessor.createOscOutputConnection(oscAddress, oscPort);
    } else {
        mProcessor.disconnectOscOutput(oscAddress, oscPort);
    }
}

//==============================================================================
void ControlGrisAudioProcessorEditor::paint(Graphics & g)
{
    g.fillAll(mGrisLookAndFeel.findColour(ResizableWindow::backgroundColourId));
}

//==============================================================================
void ControlGrisAudioProcessorEditor::resized()
{
    auto const width{ getWidth() - 50 }; // Remove position preset space.
    auto const height{ getHeight() };

    auto const fieldSize{ std::max(width / 2, MIN_FIELD_WIDTH) };

    mMainBanner.setBounds(0, 0, fieldSize, 20);
    mPositionField.setBounds(0, 20, fieldSize, fieldSize);

    if (mProcessor.getSpatMode() == SpatMode::cube) {
        mMainBanner.setText("Azimuth - Distance", NotificationType::dontSendNotification);
        mElevationBanner.setVisible(true);
        mElevationField.setVisible(true);
        mElevationBanner.setBounds(fieldSize, 0, fieldSize, 20);
        mElevationField.setBounds(fieldSize, 20, fieldSize, fieldSize);
    } else {
        mMainBanner.setText("Azimuth - Elevation", NotificationType::dontSendNotification);
        mElevationBanner.setVisible(false);
        mElevationField.setVisible(false);
    }

    mParametersBox.setBounds(0, fieldSize + 20, width, 50);

    mTrajectoryBanner.setBounds(0, fieldSize + 70, width, 20);
    mTrajectoryBox.setBounds(0, fieldSize + 90, width, 160);

    mSettingsBanner.setBounds(0, fieldSize + 250, width, 20);
    mConfigurationComponent.setBounds(0, fieldSize + 270, width, 130);

    mLastUIWidth = getWidth();
    mLastUIHeight = getHeight();

    mPositionPresetBanner.setBounds(width, 0, 50, 20);
    mPositionPresetBox.setBounds(width, 20, 50, height - 20);
}
