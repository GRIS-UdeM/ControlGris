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
#include "PluginProcessor.h"

#include "PluginEditor.h"

String getFixedPosSourceName(int index, int dimension)
{
    String result{};
    switch (dimension) {
    case 0:
        result = String("S") + String(index + 1) + String("_X");
        break;
    case 1:
        result = String("S") + String(index + 1) + String("_Y");
        break;
    case 2:
        result = String("S") + String(index + 1) + String("_Z");
        break;
    default:
        jassertfalse; // how did you get there?
    }
    return result;
}

// The parameter Layout creates the automatable mParameters.
AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    using Parameter = AudioProcessorValueTreeState::Parameter;

    std::vector<std::unique_ptr<Parameter>> mParameters;

    mParameters.push_back(std::make_unique<Parameter>(String("recordingTrajectory_x"),
                                                      String("Recording Trajectory X"),
                                                      String(),
                                                      NormalisableRange<float>(0.0f, 1.0f),
                                                      0.0f,
                                                      nullptr,
                                                      nullptr));
    mParameters.push_back(std::make_unique<Parameter>(String("recordingTrajectory_y"),
                                                      String("Recording Trajectory Y"),
                                                      String(),
                                                      NormalisableRange<float>(0.0f, 1.0f),
                                                      0.0f,
                                                      nullptr,
                                                      nullptr));
    mParameters.push_back(std::make_unique<Parameter>(String("recordingTrajectory_z"),
                                                      String("Recording Trajectory Z"),
                                                      String(),
                                                      NormalisableRange<float>(0.0f, 1.0f),
                                                      0.0f,
                                                      nullptr,
                                                      nullptr));

    mParameters.push_back(std::make_unique<Parameter>(
        String("sourceLink"),
        String("Source Link"),
        String(),
        NormalisableRange<float>(0.0f, static_cast<float>(POSITION_SOURCE_LINK_TYPES.size() - 1), 1.0f),
        0.0f,
        nullptr,
        nullptr,
        false,
        true,
        true));
    mParameters.push_back(std::make_unique<Parameter>(String("sourceLinkAlt"),
                                                      String("Source Link Alt"),
                                                      String(),
                                                      NormalisableRange<float>(0.0f, 4.0f, 1.0f),
                                                      0.0f,
                                                      nullptr,
                                                      nullptr,
                                                      false,
                                                      true,
                                                      true));

    mParameters.push_back(std::make_unique<Parameter>(String("positionPreset"),
                                                      String("Position Preset"),
                                                      String(),
                                                      NormalisableRange<float>(0.0f, 50.0f, 1.0f),
                                                      0.0f,
                                                      nullptr,
                                                      nullptr,
                                                      false,
                                                      true,
                                                      true));

    mParameters.push_back(std::make_unique<Parameter>(String("azimuthSpan"),
                                                      String("Azimuth Span"),
                                                      String(),
                                                      NormalisableRange<float>(0.0f, 1.0f),
                                                      0.0f,
                                                      nullptr,
                                                      nullptr));
    mParameters.push_back(std::make_unique<Parameter>(String("elevationSpan"),
                                                      String("Elevation Span"),
                                                      String(),
                                                      NormalisableRange<float>(0.0f, 1.0f),
                                                      0.0f,
                                                      nullptr,
                                                      nullptr));

    return { mParameters.begin(), mParameters.end() };
}

//==============================================================================
ControlGrisAudioProcessor::ControlGrisAudioProcessor()
    :
#ifndef JucePlugin_PreferredChannelConfigurations
    AudioProcessor(BusesProperties()
    #if !JucePlugin_IsMidiEffect
        #if !JucePlugin_IsSynth
                       .withInput("Input", AudioChannelSet::stereo(), true)
        #endif // JucePlugin_IsSynth
                       .withOutput("Output", AudioChannelSet::stereo(), true)
    #endif // JucePlugin_IsMidiEffect
                       )
    ,
#endif // JucePlugin_PreferredChannelConfigurations
    mParameters(*this, nullptr, Identifier(JucePlugin_Name), createParameterLayout())
    , mFixPositionData(FIXED_POSITION_DATA_TAG)
{
    // Size of the plugin window.
    mParameters.state.addChild({ "uiState", { { "width", 650 }, { "height", 700 } }, {} }, -1, nullptr);

    // Global setting mParameters.
    mParameters.state.setProperty("oscFormat", 0, nullptr);
    mParameters.state.setProperty("oscPortNumber", 18032, nullptr);
    mParameters.state.setProperty("oscConnected", true, nullptr);
    mParameters.state.setProperty("oscInputPortNumber", 9000, nullptr);
    mParameters.state.setProperty("oscInputConnected", false, nullptr);
    mParameters.state.setProperty("oscOutputAddress", "192.168.1.100", nullptr);
    mParameters.state.setProperty("oscOutputPortNumber", 8000, nullptr);
    mParameters.state.setProperty("oscOutputConnected", false, nullptr);
    mParameters.state.setProperty("numberOfSources", 2, nullptr);
    mParameters.state.setProperty("firstSourceId", 1, nullptr);
    mParameters.state.setProperty("oscOutputPluginId", 1, nullptr);

    // Trajectory box persitent settings.
    mParameters.state.setProperty("trajectoryType", static_cast<int>(PositionTrajectoryType::realtime), nullptr);
    mParameters.state.setProperty("trajectoryTypeAlt", static_cast<int>(ElevationTrajectoryType::realtime), nullptr);
    mParameters.state.setProperty("backAndForth", false, nullptr);
    mParameters.state.setProperty("backAndForthAlt", false, nullptr);
    mParameters.state.setProperty("dampeningCycles", 0, nullptr);
    mParameters.state.setProperty("dampeningCyclesAlt", 0, nullptr);
    mParameters.state.setProperty("deviationPerCycle", 0, nullptr);
    mParameters.state.setProperty("cycleDuration", 5, nullptr);
    mParameters.state.setProperty("durationUnit", 1, nullptr);

    // Per source mParameters. Because there is no attachment to the automatable
    // mParameters, we need to keep track of the current parameter values to be
    // able to reload the last state of the plugin when we close/open the UI.
    for (int i{}; i < MAX_NUMBER_OF_SOURCES; ++i) {
        String oscId(i);
        // Non-automatable, per source, mParameters.
        mParameters.state.setProperty(String("p_azimuth_") + oscId, i % 2 == 0 ? -90.0 : 90.0, nullptr);
        mParameters.state.setProperty(String("p_elevation_") + oscId, 0.0, nullptr);
        mParameters.state.setProperty(String("p_distance_") + oscId, 1.0, nullptr);
        mParameters.state.setProperty(String("p_x_") + oscId, 0.0, nullptr);
        mParameters.state.setProperty(String("p_y_") + oscId, 0.0, nullptr);

        // Gives the source an initial id...
        mSources.get(i).setId(SourceId{ i + mFirstSourceId.toInt() });
        // .. and coordinates.
        auto const azimuth{ i % 2 == 0 ? Degrees{ -45.0f } : Degrees{ 45.0f } };
        mSources.get(i).setCoordinates(azimuth, Degrees{ 90.0f }, 1.0f, SourceLinkNotification::notify);
    }

    mParameters.getParameter("recordingTrajectory_x")->setValue(mSources.getPrimarySource().getPos().x);
    mParameters.getParameter("recordingTrajectory_y")->setValue(mSources.getPrimarySource().getPos().y);

    // Automation values for the recording trajectory.
    mParameters.addParameterListener(String("recordingTrajectory_x"), this);
    mParameters.addParameterListener(String("recordingTrajectory_y"), this);
    mParameters.addParameterListener(String("recordingTrajectory_z"), this);
    mParameters.addParameterListener(String("sourceLink"), this);
    mParameters.addParameterListener(String("sourceLinkAlt"), this);
    mParameters.addParameterListener(String("positionPreset"), this);
    mParameters.addParameterListener(String("azimuthSpan"), this);
    mParameters.addParameterListener(String("elevationSpan"), this);

    mPositionAutomationManager.addListener(this);
    mElevationAutomationManager.addListener(this);

    // The timer's callback send OSC messages periodically.
    //-----------------------------------------------------
    startTimerHz(50);
}

ControlGrisAudioProcessor::~ControlGrisAudioProcessor()
{
    disconnectOSC();
}

//==============================================================================
void ControlGrisAudioProcessor::parameterChanged(String const & parameterID, float newValue)
{
    if (std::isnan(newValue) || std::isinf(newValue)) {
        return;
    }

    if (parameterID.compare("recordingTrajectory_x") == 0) {
        mPositionAutomationManager.setPlaybackPositionX(newValue);
    } else if (parameterID.compare("recordingTrajectory_y") == 0) {
        mPositionAutomationManager.setPlaybackPositionY(newValue);
    } else if (parameterID.compare("recordingTrajectory_z") == 0 && mSelectedOscFormat == SpatMode::cube) {
        mElevationAutomationManager.setPlaybackPositionY(newValue);
    }

    if (parameterID.compare("sourceLink") == 0) {
        auto const val = static_cast<PositionSourceLink>(static_cast<int>(newValue) + 1);
        if (val != mPositionAutomationManager.getSourceLink()) {
            if (mSources.size() != 2
                && (val == PositionSourceLink::linkSymmetricX || val == PositionSourceLink::linkSymmetricY))
                return;
            mPositionAutomationManager.setSourceLink(val);
            //            mPositionAutomationManager.fixPrincipalSourcePosition();
            //            onSourceLinkChanged(val);
            auto * ed = dynamic_cast<ControlGrisAudioProcessorEditor *>(getActiveEditor());
            if (ed != nullptr) {
                ed->updateSourceLinkCombo(val);
            }
        }
    }

    if (parameterID.compare("sourceLinkAlt") == 0) {
        auto const val = static_cast<ElevationSourceLink>(static_cast<int>(newValue) + 1);
        if (val != mElevationAutomationManager.getSourceLink()) {
            mElevationAutomationManager.setSourceLink(val);
            //            mElevationAutomationManager.fixPrincipalSourcePosition();
            //            onElevationSourceLinkChanged(static_cast<ElevationSourceLink>(val));
            auto * ed = dynamic_cast<ControlGrisAudioProcessorEditor *>(getActiveEditor());
            if (ed != nullptr) {
                ed->updateElevationSourceLinkCombo(static_cast<ElevationSourceLink>(val));
            }
        }
    }

    if (parameterID.compare("positionPreset") == 0) {
        mNewPositionPreset = (int)newValue;
    }

    if (parameterID.startsWith("azimuthSpan")) {
        for (auto & source : mSources) {
            source.setAzimuthSpan(Normalized{ newValue });
        }
    } else if (parameterID.startsWith("elevationSpan")) {
        for (auto & source : mSources) {
            source.setElevationSpan(Normalized{ newValue });
        }
    }
}

//== Tools for sorting mSources based on azimuth values. ==
struct Sorter {
    int index;
    float value;
};

bool compareLessThan(const Sorter & a, const Sorter & b)
{
    return a.value <= b.value;
}
//========================================================
void ControlGrisAudioProcessor::setPositionSourceLink(PositionSourceLink newSourceLink)
{
    if (newSourceLink != mPositionAutomationManager.getSourceLink()) {
        if (mSources.size() != 2
            && (newSourceLink == PositionSourceLink::linkSymmetricX
                || newSourceLink == PositionSourceLink::linkSymmetricY)) {
            // TODO: the symmetric links should not be available when only 2 sources are present
            return;
        }

        if (mPositionAutomationManager.getTrajectoryType() != PositionTrajectoryType::drawing) {
            mPositionAutomationManager.setPrincipalSourceAndPlaybackPosition(mSources.getPrimarySource().getPos());
        }

        mPositionAutomationManager.setSourceLink(newSourceLink);
        //        mPositionAutomationManager.fixPrincipalSourcePosition();
        //        onSourceLinkChanged(value);

        auto const howMany{ static_cast<float>(POSITION_SOURCE_LINK_TYPES.size() - 1) };
        mParameters.getParameter("sourceLink")->beginChangeGesture();
        mParameters.getParameter("sourceLink")
            ->setValueNotifyingHost((static_cast<float>(newSourceLink) - 1.0f) / howMany);
        mParameters.getParameter("sourceLink")->endChangeGesture();
        auto * editor{ dynamic_cast<ControlGrisAudioProcessorEditor *>(getActiveEditor()) };
        if (editor != nullptr) {
            editor->updateSourceLinkCombo(newSourceLink);
        }

        mSourceLinkEnforcer.setSourceLink(newSourceLink);
    }
}

void ControlGrisAudioProcessor::setElevationSourceLink(ElevationSourceLink value)
{
    if (value != static_cast<ElevationSourceLink>(mElevationAutomationManager.getSourceLink())) {
        if (value == ElevationSourceLink::deltaLock
            && static_cast<ElevationTrajectoryType>(mElevationAutomationManager.getTrajectoryType())
                   != ElevationTrajectoryType::drawing) {
            mElevationAutomationManager.setPrincipalSourceAndPlaybackPosition(Point<float>{ 0.0f, 0.0f });
        }

        mElevationAutomationManager.setSourceLink(value);
        //        mElevationAutomationManager.fixPrincipalSourcePosition();
        //        onElevationSourceLinkChanged(value);

        mParameters.getParameter("sourceLinkAlt")->beginChangeGesture();
        mParameters.getParameter("sourceLinkAlt")->setValueNotifyingHost((static_cast<float>(value) - 1.0f) / 4.0f);
        mParameters.getParameter("sourceLinkAlt")->endChangeGesture();
        auto * ed = dynamic_cast<ControlGrisAudioProcessorEditor *>(getActiveEditor());
        if (ed != nullptr) {
            ed->updateElevationSourceLinkCombo(value);
        }
    }
}

//==============================================================================
void ControlGrisAudioProcessor::setOscFormat(SpatMode const oscFormat)
{
    mSelectedOscFormat = oscFormat;
    mParameters.state.setProperty("oscFormat", static_cast<int>(mSelectedOscFormat), nullptr);
    for (int i{}; i < MAX_NUMBER_OF_SOURCES; ++i) {
        mSources.get(i).setSpatMode(oscFormat);
    }
}

void ControlGrisAudioProcessor::setOscPortNumber(int oscPortNumber)
{
    mCurrentOSCPort = oscPortNumber;
    mParameters.state.setProperty("oscPortNumber", mCurrentOSCPort, nullptr);
}

void ControlGrisAudioProcessor::setFirstSourceId(SourceId const firstSourceId, bool propagate)
{
    mFirstSourceId = firstSourceId;
    mParameters.state.setProperty("firstSourceId", mFirstSourceId.toInt(), nullptr);
    for (int i{}; i < MAX_NUMBER_OF_SOURCES; ++i) {
        mSources.get(i).setId(SourceId{ i + mFirstSourceId.toInt() });
    }

    if (propagate)
        sendOscMessage();
}

void ControlGrisAudioProcessor::setNumberOfSources(int const numOfSources, bool const propagate)
{
    mSources.setSize(numOfSources);
    mParameters.state.setProperty("numberOfSources", mSources.size(), nullptr);

    for (auto & source : mSources) {
        source.setColorFromIndex(numOfSources);
    }

    mSourceLinkEnforcer.numberOfSourcesChanged();

    if (propagate) {
        sendOscMessage();
    }
}

void ControlGrisAudioProcessor::setSelectedSource(SourceIndex const index)
{
    mSelectedSource = index;
}

//==============================================================================
bool ControlGrisAudioProcessor::createOscConnection(int oscPort)
{
    disconnectOSC();

    mOscConnected = mOscSender.connect("127.0.0.1", oscPort);
    if (!mOscConnected)
        std::cout << "Error: could not connect to UDP port " << oscPort << "." << std::endl;
    else
        mLastConnectedOSCPort = oscPort;

    return mOscConnected;
}

bool ControlGrisAudioProcessor::disconnectOSC()
{
    if (mOscConnected) {
        if (mOscSender.disconnect()) {
            mOscConnected = false;
            mLastConnectedOSCPort = -1;
        }
    }
    return !mOscConnected;
}

void ControlGrisAudioProcessor::handleOscConnection(bool state)
{
    if (state) {
        if (mLastConnectedOSCPort != mCurrentOSCPort) {
            createOscConnection(mCurrentOSCPort);
        }
    } else {
        disconnectOSC();
    }
    mParameters.state.setProperty("oscConnected", getOscConnected(), nullptr);
}

void ControlGrisAudioProcessor::sendOscMessage()
{
    if (!mOscConnected)
        return;

    OSCAddressPattern oscPattern("/spat/serv");
    OSCMessage message(oscPattern);

    for (auto const & source : mSources) {
        float const azimuth{ source.getAzimuth().getAsRadians() };
        float const elevation{ source.getElevation().getAsRadians() };
        float const azimuthSpan{ source.getAzimuthSpan() * 2.0f };
        float const elevationSpan{ source.getElevationSpan() * 0.5f };
        float const distance{ mSelectedOscFormat == SpatMode::cube ? source.getDistance() / 0.6f
                                                                   : source.getDistance() };

        // std::cout << "Sending osc for source #" << i << ":\n\tazimuth: " << azimuth << "\n\televation: " << elevation
        //           << "\n\tazimuthSpan: " << azimuthSpan << "\n\televationSpan" << elevationSpan
        //           << "\n\tdistance: " << distance << "\n\n";

        message.clear();
        message.addInt32(source.getId().toInt() - 1); // osc id starts at 0
        message.addFloat32(azimuth);
        message.addFloat32(elevation);
        message.addFloat32(azimuthSpan);
        message.addFloat32(elevationSpan);
        message.addFloat32(distance);
        message.addFloat32(0.0);

        auto const success{ mOscSender.send(message) };
        jassert(success);
    }
}

//==============================================================================
bool ControlGrisAudioProcessor::createOscInputConnection(int oscPort)
{
    disconnectOSCInput(oscPort);

    mOscInputConnected = mOscInputReceiver.connect(oscPort);
    if (!mOscInputConnected) {
        std::cout << "Error: could not connect to UDP input port " << oscPort << "." << std::endl;
    } else {
        mOscInputReceiver.addListener(this);
        mCurrentOSCInputPort = oscPort;
        mParameters.state.setProperty("oscInputPortNumber", oscPort, nullptr);
    }

    mParameters.state.setProperty("oscInputConnected", getOscInputConnected(), nullptr);

    return mOscInputConnected;
}

bool ControlGrisAudioProcessor::disconnectOSCInput(int oscPort)
{
    if (mOscInputConnected) {
        if (mOscInputReceiver.disconnect()) {
            mOscInputConnected = false;
        }
    }

    mParameters.state.setProperty("oscInputPortNumber", oscPort, nullptr);
    mParameters.state.setProperty("oscInputConnected", getOscInputConnected(), nullptr);

    return !mOscInputConnected;
}

void ControlGrisAudioProcessor::oscBundleReceived(const OSCBundle & bundle)
{
    for (auto const & element : bundle) {
        if (element.isMessage())
            oscMessageReceived(element.getMessage());
        else if (element.isBundle())
            oscBundleReceived(element.getBundle());
    }
}

void ControlGrisAudioProcessor::oscMessageReceived(const OSCMessage & message)
{
    PositionSourceLink positionSourceLinkToProcess{ PositionSourceLink::undefined };
    ElevationSourceLink elevationSourceLinkToProcess{ ElevationSourceLink::undefined };
    float x{ -1.0f };
    float y{ -1.0f };
    float z{ -1.0f };
    String const address{ message.getAddressPattern().toString() };
    String const pluginInstance{ String("/controlgris/") + String(getOscOutputPluginId()) };
    if ((address == String(pluginInstance + "/traj/1/x") || address == String(pluginInstance + "/traj/1/xyz/1"))
        && mPositionAutomationManager.getTrajectoryType() == PositionTrajectoryType::realtime) {
        x = message[0].getFloat32();
    } else if ((address == String(pluginInstance + "/traj/1/y") || address == String(pluginInstance + "/traj/1/xyz/2"))
               && mPositionAutomationManager.getTrajectoryType() == PositionTrajectoryType::realtime) {
        y = message[0].getFloat32();
    } else if ((address == String(pluginInstance + "/traj/1/z") || address == String(pluginInstance + "/traj/1/xyz/3"))
               && static_cast<ElevationTrajectoryType>(mElevationAutomationManager.getTrajectoryType())
                      == ElevationTrajectoryType::realtime) {
        z = message[0].getFloat32();
    } else if (address == String(pluginInstance + "/traj/1/xy")
               && mPositionAutomationManager.getTrajectoryType() == PositionTrajectoryType::realtime) {
        x = message[0].getFloat32();
        y = message[1].getFloat32();
    } else if (address == String(pluginInstance + "/traj/1/xyz")) {
        if (mPositionAutomationManager.getTrajectoryType() == PositionTrajectoryType::realtime) {
            x = message[0].getFloat32();
            y = message[1].getFloat32();
        }
        if (static_cast<ElevationTrajectoryType>(mElevationAutomationManager.getTrajectoryType())
            == ElevationTrajectoryType::realtime) {
            z = message[2].getFloat32();
        }
    } else if (address == String(pluginInstance + "/azispan")) {
        for (auto & source : mSources)
            source.setAzimuthSpan(Normalized{ message[0].getFloat32() });
        mParameters.getParameter("azimuthSpan")->beginChangeGesture();
        mParameters.getParameter("azimuthSpan")->setValueNotifyingHost(message[0].getFloat32());
        mParameters.getParameter("azimuthSpan")->endChangeGesture();
    } else if (address == String(pluginInstance + "/elespan")) {
        for (auto & source : mSources)
            source.setElevationSpan(Normalized{ message[0].getFloat32() });
        mParameters.getParameter("elevationSpan")->beginChangeGesture();
        mParameters.getParameter("elevationSpan")->setValueNotifyingHost(message[0].getFloat32());
        mParameters.getParameter("elevationSpan")->endChangeGesture();
    } else if (address == String(pluginInstance + "/sourcelink/1/1")) {
        if (message[0].getFloat32() == 1)
            positionSourceLinkToProcess = static_cast<PositionSourceLink>(1);
    } else if (address == String(pluginInstance + "/sourcelink/2/1")) {
        if (message[0].getFloat32() == 1)
            positionSourceLinkToProcess = static_cast<PositionSourceLink>(2);
    } else if (address == String(pluginInstance + "/sourcelink/3/1")) {
        if (message[0].getFloat32() == 1)
            positionSourceLinkToProcess = static_cast<PositionSourceLink>(3);
    } else if (address == String(pluginInstance + "/sourcelink/4/1")) {
        if (message[0].getFloat32() == 1)
            positionSourceLinkToProcess = static_cast<PositionSourceLink>(4);
    } else if (address == String(pluginInstance + "/sourcelink/5/1")) {
        if (message[0].getFloat32() == 1)
            positionSourceLinkToProcess = static_cast<PositionSourceLink>(5);
    } else if (address == String(pluginInstance + "/sourcelink/6/1")) {
        if (message[0].getFloat32() == 1)
            positionSourceLinkToProcess = static_cast<PositionSourceLink>(6);
    } else if (address == String(pluginInstance + "/sourcelink")) {
        positionSourceLinkToProcess = static_cast<PositionSourceLink>(message[0].getFloat32()); // 1 -> 6
    } else if (address == String(pluginInstance + "/sourcelinkalt/1/1")) {
        if (message[0].getFloat32() == 1)
            elevationSourceLinkToProcess = static_cast<ElevationSourceLink>(1);
    } else if (address == String(pluginInstance + "/sourcelinkalt/2/1")) {
        if (message[0].getFloat32() == 1)
            elevationSourceLinkToProcess = static_cast<ElevationSourceLink>(2);
    } else if (address == String(pluginInstance + "/sourcelinkalt/3/1")) {
        if (message[0].getFloat32() == 1)
            elevationSourceLinkToProcess = static_cast<ElevationSourceLink>(3);
    } else if (address == String(pluginInstance + "/sourcelinkalt/4/1")) {
        if (message[0].getFloat32() == 1)
            elevationSourceLinkToProcess = static_cast<ElevationSourceLink>(4);
    } else if (address == String(pluginInstance + "/sourcelinkalt/5/1")) {
        if (message[0].getFloat32() == 1)
            elevationSourceLinkToProcess = static_cast<ElevationSourceLink>(5);
    } else if (address == String(pluginInstance + "/sourcelinkalt")) {
        elevationSourceLinkToProcess = static_cast<ElevationSourceLink>(message[0].getFloat32()); // 1 -> 5
    } else if (address == String(pluginInstance + "/presets")) {
        int newPreset = (int)message[0].getFloat32(); // 1 -> 50
        setPositionPreset(newPreset);
        ControlGrisAudioProcessorEditor * ed = dynamic_cast<ControlGrisAudioProcessorEditor *>(getActiveEditor());
        if (ed != nullptr) {
            ed->updatePositionPreset(newPreset);
        }
    }

    if (x != -1.0f && y != -1.0f) {
        mSources.getPrimarySource().setPos(Point<float>{ x, y }, SourceLinkNotification::notify);
        sourcePositionChanged(SourceIndex{ 0 }, 0);
        setPositionPreset(0);
    } else if (y != -1.0f) {
        mSources.getPrimarySource().setY(y, SourceLinkNotification::notify);
        sourcePositionChanged(SourceIndex{ 0 }, 0);
        setPositionPreset(0);
    } else if (x != -1.0f) {
        mSources.getPrimarySource().setX(x, SourceLinkNotification::notify);
        sourcePositionChanged(SourceIndex{ 0 }, 0);
        setPositionPreset(0);
    }

    if (z != -1.0f) {
        mSources.getPrimarySource().setY(z, SourceLinkNotification::notify);
        mElevationAutomationManager.sendTrajectoryPositionChangedEvent();
        setPositionPreset(0);
    }

    if (static_cast<bool>(positionSourceLinkToProcess)) {
        setPositionSourceLink(positionSourceLinkToProcess);
    }

    if (static_cast<bool>(elevationSourceLinkToProcess)) {
        setElevationSourceLink(elevationSourceLinkToProcess);
    }
}

//==============================================================================
bool ControlGrisAudioProcessor::createOscOutputConnection(String const & oscAddress, int const oscPort)
{
    disconnectOSCOutput(oscAddress, oscPort);

    mOscOutputConnected = mOscOutputSender.connect(oscAddress, oscPort);
    if (!mOscOutputConnected)
        std::cout << "Error: could not connect to UDP output port " << oscPort << " on address " << oscAddress << "."
                  << std::endl;
    else {
        mCurrentOSCOutputPort = oscPort;
        mCurrentOSCOutputAddress = oscAddress;
        mParameters.state.setProperty("oscOutputPortNumber", oscPort, nullptr);
        mParameters.state.setProperty("oscOutputAddress", oscAddress, nullptr);
    }

    mParameters.state.setProperty("oscOutputConnected", getOscOutputConnected(), nullptr);

    return mOscOutputConnected;
}

bool ControlGrisAudioProcessor::disconnectOSCOutput(String oscAddress, int oscPort)
{
    if (mOscOutputConnected) {
        if (mOscOutputSender.disconnect()) {
            mOscOutputConnected = false;
        }
    }

    mParameters.state.setProperty("oscOutputPortNumber", oscPort, nullptr);
    mParameters.state.setProperty("oscOutputAddress", oscAddress, nullptr);
    mParameters.state.setProperty("oscOutputConnected", getOscOutputConnected(), nullptr);

    return !mOscOutputConnected;
}

void ControlGrisAudioProcessor::setOscOutputPluginId(int pluginId)
{
    mParameters.state.setProperty("oscOutputPluginId", pluginId, nullptr);
}

int ControlGrisAudioProcessor::getOscOutputPluginId() const
{
    return mParameters.state.getProperty("oscOutputPluginId", 1);
}

void ControlGrisAudioProcessor::sendOscOutputMessage()
{
    if (!mOscOutputConnected)
        return;

    OSCMessage message(OSCAddressPattern("/tmp"));

    String pluginInstance = String("/controlgris/") + String(getOscOutputPluginId());

    auto const trajectoryHandlePosition{ mSources.getPrimarySource().getPos() };
    float trajectory1x = trajectoryHandlePosition.getX();
    float trajectory1y = trajectoryHandlePosition.getY();
    float trajectory1z = trajectoryHandlePosition.getY();

    if (mLastTrajectory1x != trajectory1x) {
        message.setAddressPattern(OSCAddressPattern(pluginInstance + "/traj/1/x"));
        message.addFloat32(trajectory1x);
        mOscOutputSender.send(message);
        message.clear();

        message.setAddressPattern(OSCAddressPattern(pluginInstance + "/traj/1/xyz/1"));
        message.addFloat32(trajectory1x);
        mOscOutputSender.send(message);
        message.clear();
    }

    if (mLastTrajectory1y != trajectory1y) {
        message.setAddressPattern(OSCAddressPattern(pluginInstance + "/traj/1/y"));
        message.addFloat32(trajectory1y);
        mOscOutputSender.send(message);
        message.clear();

        message.setAddressPattern(OSCAddressPattern(pluginInstance + "/traj/1/xyz/2"));
        message.addFloat32(trajectory1y);
        mOscOutputSender.send(message);
        message.clear();
    }

    if (mLastTrajectory1z != trajectory1z) {
        message.setAddressPattern(OSCAddressPattern(pluginInstance + "/traj/1/z"));
        message.addFloat32(trajectory1z);
        mOscOutputSender.send(message);
        message.clear();

        message.setAddressPattern(OSCAddressPattern(pluginInstance + "/traj/1/xyz/3"));
        message.addFloat32(trajectory1z);
        mOscOutputSender.send(message);
        message.clear();
    }

    if (mLastTrajectory1x != trajectory1x || mLastTrajectory1y != trajectory1y || mLastTrajectory1z != trajectory1z) {
        message.setAddressPattern(OSCAddressPattern(pluginInstance + "/traj/1/xy"));
        message.addFloat32(trajectory1x);
        message.addFloat32(trajectory1y);
        mOscOutputSender.send(message);
        message.clear();

        message.setAddressPattern(OSCAddressPattern(pluginInstance + "/traj/1/xyz"));
        message.addFloat32(trajectory1x);
        message.addFloat32(trajectory1y);
        message.addFloat32(trajectory1z);
        mOscOutputSender.send(message);
        message.clear();
    }

    mLastTrajectory1x = trajectory1x;
    mLastTrajectory1y = trajectory1y;
    mLastTrajectory1z = trajectory1z;

    if (mLastAzispan != mSources.getPrimarySource().getAzimuthSpan()) {
        message.setAddressPattern(OSCAddressPattern(pluginInstance + "/azispan"));
        message.addFloat32(mSources.getPrimarySource().getAzimuthSpan());
        mOscOutputSender.send(message);
        message.clear();
        mLastAzispan = mSources.getPrimarySource().getAzimuthSpan();
    }

    if (mLastElespan != mSources.getPrimarySource().getElevationSpan()) {
        message.setAddressPattern(OSCAddressPattern(pluginInstance + "/elespan"));
        message.addFloat32(mSources.getPrimarySource().getElevationSpan());
        mOscOutputSender.send(message);
        message.clear();
        mLastElespan = mSources.getPrimarySource().getElevationSpan();
    }

    if (mPositionAutomationManager.getSourceLink() != mLastSourceLink) {
        message.setAddressPattern(OSCAddressPattern(pluginInstance + "/sourcelink"));
        message.addInt32(static_cast<int32>(mPositionAutomationManager.getSourceLink()));
        mOscOutputSender.send(message);
        message.clear();

        String pattern = pluginInstance + String("/sourcelink/")
                         + String(static_cast<int>(mPositionAutomationManager.getSourceLink())) + String("/1");
        message.setAddressPattern(OSCAddressPattern(pattern));
        message.addInt32(1);
        mOscOutputSender.send(message);
        message.clear();

        mLastSourceLink = mPositionAutomationManager.getSourceLink();
    }

    if (static_cast<ElevationSourceLink>(mElevationAutomationManager.getSourceLink()) != mLastElevationSourceLink) {
        message.setAddressPattern(OSCAddressPattern(pluginInstance + "/sourcelinkalt"));
        message.addInt32(static_cast<int32>(mElevationAutomationManager.getSourceLink()));
        mOscOutputSender.send(message);
        message.clear();

        String patternAlt = pluginInstance + String("/sourcelinkalt/")
                            + String(static_cast<int>(mElevationAutomationManager.getSourceLink())) + String("/1");
        message.setAddressPattern(OSCAddressPattern(patternAlt));
        message.addInt32(1);
        mOscOutputSender.send(message);
        message.clear();

        mLastElevationSourceLink = static_cast<ElevationSourceLink>(mElevationAutomationManager.getSourceLink());
    }

    if (mCurrentPositionPreset != mLastPositionPreset) {
        message.setAddressPattern(OSCAddressPattern(pluginInstance + "/presets"));
        message.addInt32(mCurrentPositionPreset);
        mOscOutputSender.send(message);
        message.clear();

        mLastPositionPreset = mCurrentPositionPreset;
    }
}

//==============================================================================
void ControlGrisAudioProcessor::timerCallback()
{
    auto * editor{ dynamic_cast<ControlGrisAudioProcessorEditor *>(getActiveEditor()) };

    if (mNewPositionPreset != 0 && mNewPositionPreset != mCurrentPositionPreset) {
        if (recallFixedPosition(mNewPositionPreset)) {
            mCurrentPositionPreset = mNewPositionPreset;
            if (editor != nullptr) {
                editor->updatePositionPreset(mCurrentPositionPreset);
            }
        }
    }

    // MainField automation.
    if (mPositionAutomationManager.getPositionActivateState()) {
        if (mPositionAutomationManager.getTrajectoryType() == PositionTrajectoryType::realtime) {
            //...
        } else if (mLastTimerTime != getCurrentTime()) {
            mPositionAutomationManager.setTrajectoryDeltaTime(getCurrentTime() - getInitTimeOnPlay());
        }
    } else if (mIsPlaying && mPositionAutomationManager.getPlaybackPosition().has_value()) {
        mSources.getPrimarySource().setPos(mPositionAutomationManager.getPlaybackPosition().value(),
                                           SourceLinkNotification::notify);
    } else if (mPositionAutomationManager.getPlaybackPosition().has_value()
               && mSources.getPrimarySource().getPos() != mPositionAutomationManager.getPlaybackPosition()) {
        auto const preset{ static_cast<int>(mParameters.getParameterAsValue("positionPreset").getValue()) };
        recallFixedPosition(preset);
        mSources.getPrimarySource().setPos(mPositionAutomationManager.getPlaybackPosition().value(),
                                           SourceLinkNotification::notify);
        //        linkPositionSourcePositions();
    }

    // ElevationField automation.
    if (getOscFormat() == SpatMode::cube && mElevationAutomationManager.getPositionActivateState()) {
        if (static_cast<ElevationTrajectoryType>(mElevationAutomationManager.getTrajectoryType())
            == ElevationTrajectoryType::realtime) {
            //...
        } else if (mLastTimerTime != getCurrentTime()) {
            mElevationAutomationManager.setTrajectoryDeltaTime(getCurrentTime() - getInitTimeOnPlay());
        }
    } else if (mIsPlaying && mElevationAutomationManager.getPlaybackPosition().has_value()) {
        mSources.getPrimarySource().setPos(mElevationAutomationManager.getPlaybackPosition().value(),
                                           SourceLinkNotification::notify);
    } else if (mElevationAutomationManager.getPlaybackPosition().has_value()
               && mSources.getPrimarySource().getPos() != mElevationAutomationManager.getPlaybackPosition().value()) {
        int preset = (int)mParameters.getParameterAsValue("positionPreset").getValue();
        recallFixedPosition(preset);
        mSources.getPrimarySource().setPos(mElevationAutomationManager.getPlaybackPosition().value(),
                                           SourceLinkNotification::notify);
        //        linkElevationSourcePositions();
    }

    mLastTimerTime = getCurrentTime();

    if (mCanStopActivate && !mIsPlaying) {
        if (mPositionAutomationManager.getPositionActivateState())
            mPositionAutomationManager.setPositionActivateState(false);
        if (mElevationAutomationManager.getPositionActivateState())
            mElevationAutomationManager.setPositionActivateState(false);
        mCanStopActivate = false;

        if (editor != nullptr) {
            editor->updateSpanLinkButton(false);
        }
    }

    if (editor != nullptr) {
        editor->refresh();
    }

    sendOscMessage();
    sendOscOutputMessage();
}

//==============================================================================
void ControlGrisAudioProcessor::setPluginState()
{
    // If no preset is loaded, try to restore the last saved positions.
    if (mCurrentPositionPreset == 0) {
        for (auto & source : mSources) {
            auto const index{ source.getIndex().toString() };
            source.setAzimuth(Normalized{ mParameters.state.getProperty(String("p_azimuth_") + index) },
                              SourceLinkNotification::notify);
            source.setElevation(Normalized{ mParameters.state.getProperty(String("p_elevation_") + index) },
                                SourceLinkNotification::notify);
            source.setDistance(mParameters.state.getProperty(String("p_distance_") + index),
                               SourceLinkNotification::notify);
        }
    }

    auto * editor{ dynamic_cast<ControlGrisAudioProcessorEditor *>(getActiveEditor()) };
    if (editor != nullptr) {
        editor->setPluginState();
    }

    sendOscMessage();
}

//==============================================================================
void ControlGrisAudioProcessor::sourcePositionChanged(SourceIndex sourceIndex, int whichField)
{
    if (whichField == 0) {
        if (getOscFormat() == SpatMode::cube) {
            setSourceParameterValue(sourceIndex,
                                    SourceParameter::azimuth,
                                    mSources[sourceIndex].getNormalizedAzimuth());
            setSourceParameterValue(sourceIndex, SourceParameter::distance, mSources[sourceIndex].getDistance());
        } else {
            setSourceParameterValue(sourceIndex,
                                    SourceParameter::azimuth,
                                    mSources[sourceIndex].getNormalizedAzimuth());
            setSourceParameterValue(sourceIndex,
                                    SourceParameter::elevation,
                                    mSources[sourceIndex].getNormalizedElevation());
        }
    } else {
        setSourceParameterValue(sourceIndex,
                                SourceParameter::elevation,
                                mSources[sourceIndex].getNormalizedElevation());
    }

    if (whichField == 0) {
        validatePositionSourcePositions();
        if (mPositionAutomationManager.getTrajectoryType() >= PositionTrajectoryType::circleClockwise) {
            mPositionAutomationManager.setTrajectoryType(mPositionAutomationManager.getTrajectoryType(),
                                                         mSources.getPrimarySource().getPos());
        }
    }
    if (whichField == 1 && getOscFormat() == SpatMode::cube) {
        if (sourceIndex != SourceIndex{ 0 }) {
            auto const sourceElevation{ mSources[sourceIndex].getElevation() };
            auto const offset{ Degrees{ 60.0f } / mSources.size() * sourceIndex.toInt() };
        }
        validateElevationSourcePositions();
    }
}

// Called whenever a source has changed.
//--------------------------------------
void ControlGrisAudioProcessor::setSourceParameterValue(SourceIndex const sourceIndex,
                                                        SourceParameter const parameterId,
                                                        float const value)
{
    Normalized const normalized{ static_cast<float>(value) };
    auto const param_id{ sourceIndex.toString() };
    switch (parameterId) {
    case SourceParameter::azimuth:
        mSources[sourceIndex].setAzimuth(normalized, SourceLinkNotification::notify);
        mParameters.state.setProperty("p_azimuth_" + param_id, value, nullptr);
        break;
    case SourceParameter::elevation:
        mSources[sourceIndex].setElevation(normalized, SourceLinkNotification::notify);
        mParameters.state.setProperty(String("p_elevation_") + param_id, value, nullptr);
        break;
    case SourceParameter::distance:
        mSources[sourceIndex].setDistance(value, SourceLinkNotification::notify);
        mParameters.state.setProperty(String("p_distance_") + param_id, value, nullptr);
        break;
    case SourceParameter::x:
        mSources[sourceIndex].setX(value, SourceLinkNotification::notify);
        break;
    case SourceParameter::y:
        mSources[sourceIndex].setY(value, SourceLinkNotification::notify);
        break;
    case SourceParameter::azimuthSpan:
        for (auto & source : mSources) {
            source.setAzimuthSpan(normalized);
        }
        mParameters.getParameter("azimuthSpan")->beginChangeGesture();
        mParameters.getParameter("azimuthSpan")->setValueNotifyingHost(value);
        mParameters.getParameter("azimuthSpan")->endChangeGesture();
        break;
    case SourceParameter::elevationSpan:
        for (auto & source : mSources) {
            source.setElevationSpan(normalized);
        }
        mParameters.getParameter("elevationSpan")->beginChangeGesture();
        mParameters.getParameter("elevationSpan")->setValueNotifyingHost(value);
        mParameters.getParameter("elevationSpan")->endChangeGesture();
        break;
    }
}

void ControlGrisAudioProcessor::trajectoryPositionChanged(AutomationManager * manager, Point<float> position)
{
    if (manager == &mPositionAutomationManager) {
        if (!isPlaying()) {
            mPositionAutomationManager.setPrincipalSourceAndPlaybackPosition(Point<float>(position.x, position.y));
            mParameters.getParameter("recordingTrajectory_x")->setValue(position.x);
            mParameters.getParameter("recordingTrajectory_y")->setValue(position.y);
        }
        mParameters.getParameter("recordingTrajectory_x")->beginChangeGesture();
        mParameters.getParameter("recordingTrajectory_x")->setValueNotifyingHost(position.x);
        mParameters.getParameter("recordingTrajectory_x")->endChangeGesture();
        mParameters.getParameter("recordingTrajectory_y")->beginChangeGesture();
        mParameters.getParameter("recordingTrajectory_y")->setValueNotifyingHost(position.y);
        mParameters.getParameter("recordingTrajectory_y")->endChangeGesture();
        //        linkPositionSourcePositions();
    } else if (manager == &mElevationAutomationManager) {
        if (!isPlaying()) {
            mElevationAutomationManager.setPrincipalSourceAndPlaybackPosition(Point<float>(0.0f, position.y));
            mParameters.getParameter("recordingTrajectory_z")->setValue(position.y);
        }
        mParameters.getParameter("recordingTrajectory_z")->beginChangeGesture();
        mParameters.getParameter("recordingTrajectory_z")->setValueNotifyingHost(position.y);
        mParameters.getParameter("recordingTrajectory_z")->endChangeGesture();
        //        linkElevationSourcePositions();
    }
}

//==============================================================================
void ControlGrisAudioProcessor::validatePositionSourcePositions()
{
    auto const sourceLink{ mPositionAutomationManager.getSourceLink() };
    auto const trajectoryType{ mPositionAutomationManager.getTrajectoryType() };

    if (!isPlaying()) {
        if (sourceLink != PositionSourceLink::deltaLock && trajectoryType != PositionTrajectoryType::drawing) {
            mPositionAutomationManager.setPrincipalSourceAndPlaybackPosition(mSources.getPrimarySource().getPos());
        } else {
            mPositionAutomationManager.resetPlaybackPosition();
        }
    }
}

void ControlGrisAudioProcessor::validateElevationSourcePositions()
{
    auto const sourceLink{ static_cast<ElevationSourceLink>(mElevationAutomationManager.getSourceLink()) };
    auto const trajectoryType{ mElevationAutomationManager.getTrajectoryType() };

    if (!isPlaying()) {
        if (sourceLink != ElevationSourceLink::deltaLock
            && static_cast<ElevationTrajectoryType>(trajectoryType) != ElevationTrajectoryType::drawing) {
            mElevationAutomationManager.setPrincipalSourceAndPlaybackPosition(
                Point<float>(0.0f, mSources.getPrimarySource().getNormalizedElevation()));
        } else {
            mElevationAutomationManager.resetPlaybackPosition();
        }
    }
}

//==============================================================================
void ControlGrisAudioProcessor::setPositionPreset(int const presetNumber)
{
    if (presetNumber != mCurrentPositionPreset) {
        if (presetNumber == 0) {
            mNewPositionPreset = mCurrentPositionPreset = 0;
            mParameters.getParameter("positionPreset")->beginChangeGesture();
            mParameters.getParameter("positionPreset")->setValueNotifyingHost(0.0f);
            mParameters.getParameter("positionPreset")->endChangeGesture();
        } else if (recallFixedPosition(presetNumber)) {
            mCurrentPositionPreset = presetNumber;
            auto const value{ presetNumber / static_cast<float>(NUMBER_OF_POSITION_PRESETS + 1) };
            mParameters.getParameter("positionPreset")->beginChangeGesture();
            mParameters.getParameter("positionPreset")->setValueNotifyingHost(value);
            mParameters.getParameter("positionPreset")->endChangeGesture();
            mPositionAutomationManager.setTrajectoryType(mPositionAutomationManager.getTrajectoryType(),
                                                         mSources.getPrimarySource().getPos());
        }
    }
}

//==============================================================================
void ControlGrisAudioProcessor::addNewFixedPosition(int const id)
{
    // Build a new fixed position element.
    XmlElement * newData = new XmlElement("ITEM");
    newData->setAttribute("ID", id);
    for (int i{}; i < MAX_NUMBER_OF_SOURCES; ++i) {
        newData->setAttribute(getFixedPosSourceName(i, 0), mSources[i].getX());
        newData->setAttribute(getFixedPosSourceName(i, 1), mSources[i].getY());
        if (mSelectedOscFormat == SpatMode::cube) {
            newData->setAttribute(getFixedPosSourceName(i, 2), mSources[i].getNormalizedElevation());
        }
    }

    // Replace an element if the new one has the same ID as one already saved.
    bool found{ false };
    XmlElement * fpos = mFixPositionData.getFirstChildElement();
    while (fpos) {
        if (fpos->getIntAttribute("ID") == id) {
            found = true;
            break;
        }
        fpos = fpos->getNextElement();
    }

    if (found) {
        mFixPositionData.replaceChildElement(fpos, newData);
    } else {
        mFixPositionData.addChildElement(newData);
    }

    XmlElementDataSorter sorter("ID", true);
    mFixPositionData.sortChildElements(sorter);

    recallFixedPosition(id);
}

bool ControlGrisAudioProcessor::recallFixedPosition(int id)
{
    // TODO

    //    bool found = false;
    //    XmlElement * fpos = mFixPositionData.getFirstChildElement();
    //    while (fpos) {
    //        if (fpos->getIntAttribute("ID") == id) {
    //            found = true;
    //            break;
    //        }
    //        fpos = fpos->getNextElement();
    //    }
    //
    //    if (!found) {
    //        return false;
    //    }
    //
    //    mCurrentFixPosition = fpos;
    //    int index{};
    //    for (auto & source : mSources) {
    //        Point<float> const position{
    //            static_cast<float>(mCurrentFixPosition->getDoubleAttribute(getFixedPosSourceName(index, 0))),
    //            static_cast<float>(mCurrentFixPosition->getDoubleAttribute(getFixedPosSourceName(index, 1)))
    //        };
    //        source.setPos(position);
    //        source.setFixedPosition(position);
    //        if (mSelectedOscFormat == SpatMode::cube) {
    //            float const z{ static_cast<float>(
    //                mCurrentFixPosition->getDoubleAttribute(getFixedPosSourceName(index, 2))) };
    //            source.setFixedElevation(Degrees{ 90.0f } - Degrees{ 90.0f } * z);
    //            source.setElevation(Normalized{ z });
    //        }
    //        ++index;
    //    }
    //
    //    return true;

    return true;
}

void ControlGrisAudioProcessor::copyFixedPositionXmlElement(XmlElement * src, XmlElement * dest)
{
    if (dest == nullptr)
        dest = new XmlElement(FIXED_POSITION_DATA_TAG);

    forEachXmlChildElement(*src, element)
    {
        auto * newData = new XmlElement("ITEM");
        newData->setAttribute("ID", element->getIntAttribute("ID"));
        for (int i{}; i < MAX_NUMBER_OF_SOURCES; ++i) {
            newData->setAttribute(getFixedPosSourceName(i, 0),
                                  element->getDoubleAttribute(getFixedPosSourceName(i, 0)));
            newData->setAttribute(getFixedPosSourceName(i, 1),
                                  element->getDoubleAttribute(getFixedPosSourceName(i, 1)));
            newData->setAttribute(getFixedPosSourceName(i, 2),
                                  element->getDoubleAttribute(getFixedPosSourceName(i, 2)));
        }

        dest->addChildElement(newData);
    }
}

void ControlGrisAudioProcessor::deleteFixedPosition(int id)
{
    bool found = false;
    XmlElement * fpos = mFixPositionData.getFirstChildElement();
    while (fpos) {
        if (fpos->getIntAttribute("ID") == id) {
            found = true;
            break;
        }
        fpos = fpos->getNextElement();
    }

    if (found) {
        mFixPositionData.removeChildElement(fpos, true);
        XmlElementDataSorter sorter("ID", true);
        mFixPositionData.sortChildElements(sorter);
    }
}

//==============================================================================
String const ControlGrisAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ControlGrisAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool ControlGrisAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool ControlGrisAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

//==============================================================================
void ControlGrisAudioProcessor::initialize()
{
    mNeedsInitialization = true;
    mLastTime = mLastTimerTime = 10000000.0;
    mCanStopActivate = true;

    // If a preset is actually selected, we always recall it on initialize because
    // the automation won't trigger parameterChanged if it stays on the same value.
    if (mCurrentPositionPreset != 0) {
        if (recallFixedPosition(mCurrentPositionPreset)) {
            auto * ed = dynamic_cast<ControlGrisAudioProcessorEditor *>(getActiveEditor());
            if (ed != nullptr) {
                ed->updatePositionPreset(mCurrentPositionPreset);
            }
        }
    }
}

void ControlGrisAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    if (mIsPlaying == 0)
        initialize();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ControlGrisAudioProcessor::isBusesLayoutSupported(const BusesLayout & layouts) const
{
    #if JucePlugin_IsMidiEffect
    ignoreUnused(layouts);
    return true;
    #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

            // This checks if the input layout matches the output layout
        #if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
        #endif

    return true;
    #endif
}
#endif

void ControlGrisAudioProcessor::processBlock(AudioBuffer<float> & buffer, MidiBuffer & midiMessages)
{
    int started = mIsPlaying;
    AudioPlayHead * audioPlayHead = getPlayHead();
    if (audioPlayHead != nullptr) {
        AudioPlayHead::CurrentPositionInfo currentPositionInfo{};
        audioPlayHead->getCurrentPosition(currentPositionInfo);
        mIsPlaying = currentPositionInfo.isPlaying;
        mBpm = currentPositionInfo.bpm;
        if (mNeedsInitialization) {
            mInitTimeOnPlay = mCurrentTime
                = currentPositionInfo.timeInSeconds < 0.0 ? 0.0 : currentPositionInfo.timeInSeconds;
            mNeedsInitialization = false;
        } else {
            mCurrentTime = currentPositionInfo.timeInSeconds;
        }
    }

    if (!started && mIsPlaying) { // Initialization here only for Logic (also Reaper and Live), which are not
        PluginHostType hostType;  // calling prepareToPlay every time the sequence starts.
        if (hostType.isLogic() || hostType.isReaper() || hostType.isAbletonLive()) {
            initialize();
        }
    }
    mLastTime = mCurrentTime;
}

//==============================================================================
AudioProcessorEditor * ControlGrisAudioProcessor::createEditor()
{
    return new ControlGrisAudioProcessorEditor(*this,
                                               mParameters,
                                               mPositionAutomationManager,
                                               mElevationAutomationManager);
}

//==============================================================================
void ControlGrisAudioProcessor::getStateInformation(MemoryBlock & destData)
{
    for (int i{}; i < MAX_NUMBER_OF_SOURCES; ++i) {
        String id(i);
        mParameters.state.setProperty(String("p_azimuth_") + id,
                                      static_cast<float>(mSources[i].getNormalizedAzimuth()),
                                      nullptr);
        mParameters.state.setProperty(String("p_elevation_") + id,
                                      static_cast<float>(mSources[i].getNormalizedElevation()),
                                      nullptr);
        mParameters.state.setProperty(String("p_distance_") + id, mSources[i].getDistance(), nullptr);
    }

    auto state = mParameters.copyState();

    auto xmlState{ state.createXml() };

    if (xmlState != nullptr) {
        auto * childExist = xmlState->getChildByName(FIXED_POSITION_DATA_TAG);
        if (childExist) {
            xmlState->removeChildElement(childExist, true);
        }
        if (mFixPositionData.getNumChildElements() > 0) {
            auto * positionData = xmlState->createNewChildElement(FIXED_POSITION_DATA_TAG);
            copyFixedPositionXmlElement(&mFixPositionData, positionData);
        }
        copyXmlToBinary(*xmlState, destData);
    }
}

void ControlGrisAudioProcessor::setStateInformation(const void * data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr) {
        // Set global settings values.
        //----------------------------
        ValueTree valueTree = ValueTree::fromXml(*xmlState);
        setOscFormat((SpatMode)(int)valueTree.getProperty("oscFormat", 0));
        setOscPortNumber(valueTree.getProperty("oscPortNumber", 18032));
        handleOscConnection(valueTree.getProperty("oscConnected", true));
        setNumberOfSources(valueTree.getProperty("numberOfSources", 1), false);
        setFirstSourceId(SourceId{ valueTree.getProperty("firstSourceId", 1) });
        setOscOutputPluginId(valueTree.getProperty("oscOutputPluginId", 1));

        if (valueTree.getProperty("oscInputConnected", false)) {
            createOscInputConnection(valueTree.getProperty("oscInputPortNumber", 9000));
        }

        if (valueTree.getProperty("oscOutputConnected", false)) {
            createOscOutputConnection(valueTree.getProperty("oscOutputAddress", "192.168.1.100"),
                                      valueTree.getProperty("oscOutputPortNumber", 8000));
        }

        // Load saved fixed positions.
        //----------------------------
        XmlElement * positionData = xmlState->getChildByName(FIXED_POSITION_DATA_TAG);
        if (positionData) {
            mFixPositionData.deleteAllChildElements();
            copyFixedPositionXmlElement(positionData, &mFixPositionData);
        }
        // Replace the state and call automated parameter current values.
        //---------------------------------------------------------------
        mParameters.replaceState(ValueTree::fromXml(*xmlState));
    }

    setPluginState();
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor * JUCE_CALLTYPE createPluginFilter()
{
    return new ControlGrisAudioProcessor();
}
