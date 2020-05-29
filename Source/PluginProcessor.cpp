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

// The parameter Layout creates the automatable parameters.
AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    using Parameter = AudioProcessorValueTreeState::Parameter;

    std::vector<std::unique_ptr<Parameter>> parameters;

    parameters.push_back(std::make_unique<Parameter>(String("recordingTrajectory_x"),
                                                     String("Recording Trajectory X"),
                                                     String(),
                                                     NormalisableRange<float>(0.f, 1.f),
                                                     0.0f,
                                                     nullptr,
                                                     nullptr));
    parameters.push_back(std::make_unique<Parameter>(String("recordingTrajectory_y"),
                                                     String("Recording Trajectory Y"),
                                                     String(),
                                                     NormalisableRange<float>(0.f, 1.f),
                                                     0.0f,
                                                     nullptr,
                                                     nullptr));
    parameters.push_back(std::make_unique<Parameter>(String("recordingTrajectory_z"),
                                                     String("Recording Trajectory Z"),
                                                     String(),
                                                     NormalisableRange<float>(0.f, 1.f),
                                                     0.0f,
                                                     nullptr,
                                                     nullptr));

    parameters.push_back(std::make_unique<Parameter>(
        String("sourceLink"),
        String("Source Link"),
        String(),
        NormalisableRange<float>(0.f, static_cast<float>(POSITION_SOURCE_LINK_TYPES.size() - 1), 1.f),
        0.f,
        nullptr,
        nullptr,
        false,
        true,
        true));
    parameters.push_back(std::make_unique<Parameter>(String("sourceLinkAlt"),
                                                     String("Source Link Alt"),
                                                     String(),
                                                     NormalisableRange<float>(0.f, 4.f, 1.f),
                                                     0.f,
                                                     nullptr,
                                                     nullptr,
                                                     false,
                                                     true,
                                                     true));

    parameters.push_back(std::make_unique<Parameter>(String("positionPreset"),
                                                     String("Position Preset"),
                                                     String(),
                                                     NormalisableRange<float>(0.f, 50.f, 1.f),
                                                     0.f,
                                                     nullptr,
                                                     nullptr,
                                                     false,
                                                     true,
                                                     true));

    parameters.push_back(std::make_unique<Parameter>(String("azimuthSpan"),
                                                     String("Azimuth Span"),
                                                     String(),
                                                     NormalisableRange<float>(0.f, 1.f),
                                                     0.f,
                                                     nullptr,
                                                     nullptr));
    parameters.push_back(std::make_unique<Parameter>(String("elevationSpan"),
                                                     String("Elevation Span"),
                                                     String(),
                                                     NormalisableRange<float>(0.f, 1.f),
                                                     0.f,
                                                     nullptr,
                                                     nullptr));

    return { parameters.begin(), parameters.end() };
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
    parameters(*this, nullptr, Identifier(JucePlugin_Name), createParameterLayout())
    , fixPositionData(FIXED_POSITION_DATA_TAG)
{
    m_numOfSources = 2;
    m_firstSourceId = 1;
    m_selectedSourceId = 1;
    m_selectedOscFormat = SpatMode::VBAP;
    m_currentOSCPort = 18032;
    m_lastConnectedOSCPort = -1;
    m_oscConnected = true;
    m_oscInputConnected = false;
    m_currentOSCInputPort = 8000;
    m_oscOutputConnected = false;
    m_currentOSCOutputPort = 9000;
    m_currentOSCOutputAddress = String("192.168.1.100");

    m_initTimeOnPlay = m_currentTime = 0.0;
    m_lastTime = m_lastTimerTime = 10000000.0;

    m_bpm = 120;

    m_newPositionPreset = m_currentPositionPreset = m_lastPositionPreset = 0;

    m_lastTrajectory1x = m_lastTrajectory1y = m_lastTrajectory1z = -1;
    m_lastAzispan = m_lastElespan = -1;

    m_lastSourceLink = PositionSourceLink::undefined;
    m_lastElevationSourceLink = ElevationSourceLink::undefined;

    m_canStopActivate = false;

    // Size of the plugin window.
    parameters.state.addChild({ "uiState", { { "width", 650 }, { "height", 700 } }, {} }, -1, nullptr);

    // Global setting parameters.
    parameters.state.setProperty("oscFormat", 0, nullptr);
    parameters.state.setProperty("oscPortNumber", 18032, nullptr);
    parameters.state.setProperty("oscConnected", true, nullptr);
    parameters.state.setProperty("oscInputPortNumber", 9000, nullptr);
    parameters.state.setProperty("oscInputConnected", false, nullptr);
    parameters.state.setProperty("oscOutputAddress", "192.168.1.100", nullptr);
    parameters.state.setProperty("oscOutputPortNumber", 8000, nullptr);
    parameters.state.setProperty("oscOutputConnected", false, nullptr);
    parameters.state.setProperty("numberOfSources", 2, nullptr);
    parameters.state.setProperty("firstSourceId", 1, nullptr);
    parameters.state.setProperty("oscOutputPluginId", 1, nullptr);

    // Trajectory box persitent settings.
    parameters.state.setProperty("trajectoryType", 1, nullptr);
    parameters.state.setProperty("trajectoryTypeAlt", 1, nullptr);
    parameters.state.setProperty("backAndForth", false, nullptr);
    parameters.state.setProperty("backAndForthAlt", false, nullptr);
    parameters.state.setProperty("dampeningCycles", 0, nullptr);
    parameters.state.setProperty("dampeningCyclesAlt", 0, nullptr);
    parameters.state.setProperty("deviationPerCycle", 0, nullptr);
    parameters.state.setProperty("cycleDuration", 5, nullptr);
    parameters.state.setProperty("durationUnit", 1, nullptr);

    // Per source parameters. Because there is no attachment to the automatable
    // parameters, we need to keep track of the current parameter values to be
    // able to reload the last state of the plugin when we close/open the UI.
    for (int i{}; i < MAX_NUMBER_OF_SOURCES; ++i) {
        String id(i);
        // Non-automatable, per source, parameters.
        parameters.state.setProperty(String("p_azimuth_") + id, i % 2 == 0 ? -90.0 : 90.0, nullptr);
        parameters.state.setProperty(String("p_elevation_") + id, 0.0, nullptr);
        parameters.state.setProperty(String("p_distance_") + id, 1.0, nullptr);
        parameters.state.setProperty(String("p_x_") + id, 0.0, nullptr);
        parameters.state.setProperty(String("p_y_") + id, 0.0, nullptr);

        // Gives the source an initial id...
        sources[i].setId(i + m_firstSourceId - 1);
        // .. and coordinates.
        sources[i].setAzimuth(i % 2 == 0 ? 90.0 : -90.0);
        sources[i].setElevation(0.0);
        sources[i].setDistance(1.0);
    }

    mPositionAutomationManager.setSourcePosition(sources[0].getPos());
    parameters.getParameter("recordingTrajectory_x")->setValue(sources[0].getPos().x);
    parameters.getParameter("recordingTrajectory_y")->setValue(sources[0].getPos().y);

    // Automation values for the recording trajectory.
    parameters.addParameterListener(String("recordingTrajectory_x"), this);
    parameters.addParameterListener(String("recordingTrajectory_y"), this);
    parameters.addParameterListener(String("recordingTrajectory_z"), this);
    parameters.addParameterListener(String("sourceLink"), this);
    parameters.addParameterListener(String("sourceLinkAlt"), this);
    parameters.addParameterListener(String("positionPreset"), this);
    parameters.addParameterListener(String("azimuthSpan"), this);
    parameters.addParameterListener(String("elevationSpan"), this);

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
void ControlGrisAudioProcessor::parameterChanged(const String & parameterID, float newValue)
{
    if (std::isnan(newValue) || std::isinf(newValue)) {
        return;
    }

    bool needToLinkSourcePositions = false;
    if (parameterID.compare("recordingTrajectory_x") == 0) {
        mPositionAutomationManager.setPlaybackPositionX(newValue);
        needToLinkSourcePositions = true;
    } else if (parameterID.compare("recordingTrajectory_y") == 0) {
        mPositionAutomationManager.setPlaybackPositionY(newValue);
        needToLinkSourcePositions = true;
    } else if (parameterID.compare("recordingTrajectory_z") == 0 && m_selectedOscFormat == SpatMode::LBAP) {
        mElevationAutomationManager.setPlaybackPositionY(newValue);
        linkElevationSourcePositions();
    }

    if (needToLinkSourcePositions) {
        linkPositionSourcePositions();
    }

    if (parameterID.compare("sourceLink") == 0) {
        auto const val = static_cast<PositionSourceLink>(static_cast<int>(newValue) + 1);
        if (val != mPositionAutomationManager.getSourceLink()) {
            if (m_numOfSources != 2
                && (val == PositionSourceLink::linkSymmetricX || val == PositionSourceLink::linkSymmetricY))
                return;
            mPositionAutomationManager.setPostionSourceLink(val);
            mPositionAutomationManager.fixSourcePosition();
            onSourceLinkChanged(val);
            ControlGrisAudioProcessorEditor * ed = dynamic_cast<ControlGrisAudioProcessorEditor *>(getActiveEditor());
            if (ed != nullptr) {
                ed->updateSourceLinkCombo(val);
            }
        }
    }

    if (parameterID.compare("sourceLinkAlt") == 0) {
        auto const val = static_cast<PositionSourceLink>(static_cast<int>(newValue) + 1);
        if (val != mElevationAutomationManager.getSourceLink()) {
            mElevationAutomationManager.setPostionSourceLink(val);
            mElevationAutomationManager.fixSourcePosition();
            onElevationSourceLinkChanged(static_cast<ElevationSourceLink>(val));
            ControlGrisAudioProcessorEditor * ed = dynamic_cast<ControlGrisAudioProcessorEditor *>(getActiveEditor());
            if (ed != nullptr) {
                ed->updateElevationSourceLinkCombo(static_cast<ElevationSourceLink>(val));
            }
        }
    }

    if (parameterID.compare("positionPreset") == 0) {
        m_newPositionPreset = (int)newValue;
    }

    if (parameterID.startsWith("azimuthSpan")) {
        for (int i{}; i < m_numOfSources; ++i) {
            sources[i].setAzimuthSpan(newValue);
        }
    } else if (parameterID.startsWith("elevationSpan")) {
        for (int i{}; i < m_numOfSources; ++i) {
            sources[i].setElevationSpan(newValue);
        }
    }
}

//== Tools for sorting sources based on azimuth values. ==
struct Sorter {
    int index;
    float value;
};

bool compareLessThan(const Sorter & a, const Sorter & b)
{
    return a.value <= b.value;
}
//========================================================
void ControlGrisAudioProcessor::setPostionSourceLink(PositionSourceLink value)
{
    if (value != mPositionAutomationManager.getSourceLink()) {
        if (m_numOfSources != 2
            && (value == PositionSourceLink::linkSymmetricX || value == PositionSourceLink::linkSymmetricY))
            return;

        if (mPositionAutomationManager.getTrajectoryType() != PositionTrajectoryType::drawing) {
            if (value == PositionSourceLink::circularDeltaLock) {
                mPositionAutomationManager.setSourceAndPlaybackPosition(Point<float>(0.5, 0.5));
            } else {
                mPositionAutomationManager.setSourceAndPlaybackPosition(sources[0].getPos());
            }
        }

        mPositionAutomationManager.setPostionSourceLink(value);
        mPositionAutomationManager.fixSourcePosition();

        onSourceLinkChanged(value);

        float howMany = static_cast<float>(POSITION_SOURCE_LINK_TYPES.size() - 1);
        parameters.getParameter("sourceLink")->beginChangeGesture();
        parameters.getParameter("sourceLink")->setValueNotifyingHost(((float)value - 1.f) / howMany);
        parameters.getParameter("sourceLink")->endChangeGesture();
        ControlGrisAudioProcessorEditor * ed = dynamic_cast<ControlGrisAudioProcessorEditor *>(getActiveEditor());
        if (ed != nullptr) {
            ed->updateSourceLinkCombo(value);
        }
    }
}

void ControlGrisAudioProcessor::setElevationSourceLink(ElevationSourceLink value)
{
    if (value != static_cast<ElevationSourceLink>(mElevationAutomationManager.getSourceLink())) {
        if (value == ElevationSourceLink::deltaLock
            && static_cast<ElevationTrajectoryType>(mElevationAutomationManager.getTrajectoryType())
                   != ElevationTrajectoryType::drawing) {
            mElevationAutomationManager.setSourceAndPlaybackPosition(Point<float>(0., 0.5));
        }

        mElevationAutomationManager.setPostionSourceLink(static_cast<PositionSourceLink>(value));
        mElevationAutomationManager.fixSourcePosition();

        onElevationSourceLinkChanged(value);

        parameters.getParameter("sourceLinkAlt")->beginChangeGesture();
        parameters.getParameter("sourceLinkAlt")->setValueNotifyingHost(((float)value - 1.f) / 4.f);
        parameters.getParameter("sourceLinkAlt")->endChangeGesture();
        ControlGrisAudioProcessorEditor * ed = dynamic_cast<ControlGrisAudioProcessorEditor *>(getActiveEditor());
        if (ed != nullptr) {
            ed->updateElevationSourceLinkCombo(value);
        }
    }
}

void ControlGrisAudioProcessor::onSourceLinkChanged(PositionSourceLink value)
{
    if (value == PositionSourceLink::circularFixedRadius || value == PositionSourceLink::circularFullyFixed) {
        if (m_selectedOscFormat == SpatMode::LBAP) {
            for (int i{ 1 }; i < m_numOfSources; ++i) {
                sources[i].setDistance(sources[0].getDistance());
            }
        } else {
            for (int i{ 1 }; i < m_numOfSources; ++i) {
                sources[i].setElevation(sources[0].getElevation());
            }
        }
    }

    if (value == PositionSourceLink::circularFixedAngle || value == PositionSourceLink::circularFullyFixed) {
        Sorter tosort[m_numOfSources];
        for (int i{}; i < m_numOfSources; ++i) {
            tosort[i].index = i;
            tosort[i].value = sources[i].getAzimuth();
        }
        std::sort(tosort, tosort + m_numOfSources, compareLessThan);

        int posOfFirstSource;
        for (int i{}; i < m_numOfSources; ++i) {
            if (tosort[i].index == 0) {
                posOfFirstSource = i;
            }
        }

        float currentPos = sources[0].getAzimuth();
        for (int i{}; i < m_numOfSources; ++i) {
            float newPos = 360.0 / m_numOfSources * i + currentPos;
            int ioff = (i + posOfFirstSource) % m_numOfSources;
            sources[tosort[ioff].index].setAzimuth(newPos);
        }
    }

    if (value == PositionSourceLink::linkSymmetricX && m_numOfSources == 2) {
        sources[1].setSymmetricX(sources[0].getX(), sources[0].getY());
    }

    if (value == PositionSourceLink::linkSymmetricY && m_numOfSources == 2) {
        sources[1].setSymmetricY(sources[0].getX(), sources[0].getY());
    }

    bool shouldBeFixed = value != PositionSourceLink::independent;
    for (int i{}; i < m_numOfSources; ++i) {
        sources[i].fixSourcePosition(shouldBeFixed);
    }
}

void ControlGrisAudioProcessor::onElevationSourceLinkChanged(ElevationSourceLink value)
{
    if (getOscFormat() == SpatMode::LBAP) {
        // Fixed elevation.
        if (value == ElevationSourceLink::fixedElevation) {
            for (int i{ 1 }; i < m_numOfSources; ++i) {
                sources[i].setElevation(sources[0].getElevation());
            }
        }

        // Linear min.
        if (value == ElevationSourceLink::linearMin) {
            for (int i{}; i < m_numOfSources; ++i) {
                sources[i].setElevation(60.0 / m_numOfSources * i);
            }
        }

        // Linear max.
        if (value == ElevationSourceLink::linearMax) {
            for (int i{}; i < m_numOfSources; ++i) {
                sources[i].setElevation(90.0 - (60.0 / m_numOfSources * i));
            }
        }

        bool shouldBeFixed = value != ElevationSourceLink::independent;
        for (int i{}; i < m_numOfSources; ++i) {
            sources[i].fixSourcePositionElevation(shouldBeFixed);
        }
    }
}

//==============================================================================
void ControlGrisAudioProcessor::setOscFormat(SpatMode oscFormat)
{
    m_selectedOscFormat = oscFormat;
    parameters.state.setProperty("oscFormat", static_cast<int>(m_selectedOscFormat), nullptr);
    for (int i{}; i < MAX_NUMBER_OF_SOURCES; ++i) {
        sources[i].setRadiusIsElevation(m_selectedOscFormat != SpatMode::LBAP);
    }
}

void ControlGrisAudioProcessor::setOscPortNumber(int oscPortNumber)
{
    m_currentOSCPort = oscPortNumber;
    parameters.state.setProperty("oscPortNumber", m_currentOSCPort, nullptr);
}

void ControlGrisAudioProcessor::setFirstSourceId(int firstSourceId, bool propagate)
{
    m_firstSourceId = firstSourceId;
    parameters.state.setProperty("firstSourceId", m_firstSourceId, nullptr);
    for (int i{}; i < MAX_NUMBER_OF_SOURCES; ++i) {
        sources[i].setId(i + m_firstSourceId - 1);
    }

    if (propagate)
        sendOscMessage();
}

void ControlGrisAudioProcessor::setNumberOfSources(int numOfSources, bool propagate)
{
    m_numOfSources = numOfSources;
    parameters.state.setProperty("numberOfSources", m_numOfSources, nullptr);

    if (propagate)
        sendOscMessage();
}

void ControlGrisAudioProcessor::setSelectedSourceId(int id)
{
    m_selectedSourceId = id;
}

//==============================================================================
bool ControlGrisAudioProcessor::createOscConnection(int oscPort)
{
    disconnectOSC();

    m_oscConnected = oscSender.connect("127.0.0.1", oscPort);
    if (!m_oscConnected)
        std::cout << "Error: could not connect to UDP port " << oscPort << "." << std::endl;
    else
        m_lastConnectedOSCPort = oscPort;

    return m_oscConnected;
}

bool ControlGrisAudioProcessor::disconnectOSC()
{
    if (m_oscConnected) {
        if (oscSender.disconnect()) {
            m_oscConnected = false;
            m_lastConnectedOSCPort = -1;
        }
    }
    return !m_oscConnected;
}

void ControlGrisAudioProcessor::handleOscConnection(bool state)
{
    if (state) {
        if (m_lastConnectedOSCPort != m_currentOSCPort) {
            createOscConnection(m_currentOSCPort);
        }
    } else {
        disconnectOSC();
    }
    parameters.state.setProperty("oscConnected", getOscConnected(), nullptr);
}

void ControlGrisAudioProcessor::sendOscMessage()
{
    if (!m_oscConnected)
        return;

    OSCAddressPattern oscPattern("/spat/serv");
    OSCMessage message(oscPattern);

    for (int i{}; i < m_numOfSources; ++i) {
        message.clear();
        float const azim{ -sources[i].getAzimuth() / 180.f * MathConstants<float>::pi };
        float const elev{ MathConstants<float>::halfPi
                          - sources[i].getElevation() / 360.f * MathConstants<float>::twoPi };
        message.addInt32(sources[i].getId());
        message.addFloat32(azim);
        message.addFloat32(elev);
        message.addFloat32(sources[i].getAzimuthSpan() * 2.0);
        message.addFloat32(sources[i].getElevationSpan() * 0.5);
        if (m_selectedOscFormat == SpatMode::LBAP) {
            message.addFloat32(sources[i].getDistance() / 0.6);
        } else {
            message.addFloat32(sources[i].getDistance());
        }
        message.addFloat32(0.0);

        if (!oscSender.send(message)) {
            // std::cout << "Error: could not send OSC message." << std::endl;
            return;
        }
    }
}

//==============================================================================
bool ControlGrisAudioProcessor::createOscInputConnection(int oscPort)
{
    disconnectOSCInput(oscPort);

    m_oscInputConnected = oscInputReceiver.connect(oscPort);
    if (!m_oscInputConnected) {
        std::cout << "Error: could not connect to UDP input port " << oscPort << "." << std::endl;
    } else {
        oscInputReceiver.addListener(this);
        m_currentOSCInputPort = oscPort;
        parameters.state.setProperty("oscInputPortNumber", oscPort, nullptr);
    }

    parameters.state.setProperty("oscInputConnected", getOscInputConnected(), nullptr);

    return m_oscInputConnected;
}

bool ControlGrisAudioProcessor::disconnectOSCInput(int oscPort)
{
    if (m_oscInputConnected) {
        if (oscInputReceiver.disconnect()) {
            m_oscInputConnected = false;
        }
    }

    parameters.state.setProperty("oscInputPortNumber", oscPort, nullptr);
    parameters.state.setProperty("oscInputConnected", getOscInputConnected(), nullptr);

    return !m_oscInputConnected;
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
    float x{ -1.f };
    float y{ -1.f };
    float z{ -1.f };
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
        for (int i{}; i < m_numOfSources; ++i)
            sources[i].setAzimuthSpan(message[0].getFloat32());
        parameters.getParameter("azimuthSpan")->beginChangeGesture();
        parameters.getParameter("azimuthSpan")->setValueNotifyingHost(message[0].getFloat32());
        parameters.getParameter("azimuthSpan")->endChangeGesture();
    } else if (address == String(pluginInstance + "/elespan")) {
        for (int i{}; i < m_numOfSources; ++i)
            sources[i].setElevationSpan(message[0].getFloat32());
        parameters.getParameter("elevationSpan")->beginChangeGesture();
        parameters.getParameter("elevationSpan")->setValueNotifyingHost(message[0].getFloat32());
        parameters.getParameter("elevationSpan")->endChangeGesture();
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

    if (x != -1.f && y != -1.f) {
        if (mPositionAutomationManager.getSourceLink() == PositionSourceLink::circularDeltaLock) {
            mPositionAutomationManager.setSourcePositionX(x);
            mPositionAutomationManager.setSourcePositionY(y);
            mPositionAutomationManager.sendTrajectoryPositionChangedEvent();
        } else {
            sources[0].setX(x);
            sources[0].setY(y);
            sourcePositionChanged(0, 0);
        }
        setPositionPreset(0);
    } else if (y != -1.f) {
        if (mPositionAutomationManager.getSourceLink() == PositionSourceLink::circularDeltaLock) {
            mPositionAutomationManager.setSourcePositionY(y);
            mPositionAutomationManager.sendTrajectoryPositionChangedEvent();
        } else {
            sources[0].setY(y);
            sourcePositionChanged(0, 0);
        }
        setPositionPreset(0);
    } else if (x != -1.f) {
        if (mPositionAutomationManager.getSourceLink() == PositionSourceLink::circularDeltaLock) {
            mPositionAutomationManager.setSourcePositionX(x);
            mPositionAutomationManager.sendTrajectoryPositionChangedEvent();
        } else {
            sources[0].setX(x);
            sourcePositionChanged(0, 0);
        }
        setPositionPreset(0);
    }

    if (z != -1.f) {
        mElevationAutomationManager.setSourcePositionY(z);
        mElevationAutomationManager.sendTrajectoryPositionChangedEvent();
        setPositionPreset(0);
    }

    if (static_cast<bool>(positionSourceLinkToProcess))
        setPostionSourceLink(positionSourceLinkToProcess);

    if (static_cast<bool>(elevationSourceLinkToProcess))
        setElevationSourceLink(elevationSourceLinkToProcess);
}

//==============================================================================
bool ControlGrisAudioProcessor::createOscOutputConnection(String oscAddress, int oscPort)
{
    disconnectOSCOutput(oscAddress, oscPort);

    m_oscOutputConnected = oscOutputSender.connect(oscAddress, oscPort);
    if (!m_oscOutputConnected)
        std::cout << "Error: could not connect to UDP output port " << oscPort << " on address " << oscAddress << "."
                  << std::endl;
    else {
        m_currentOSCOutputPort = oscPort;
        m_currentOSCOutputAddress = oscAddress;
        parameters.state.setProperty("oscOutputPortNumber", oscPort, nullptr);
        parameters.state.setProperty("oscOutputAddress", oscAddress, nullptr);
    }

    parameters.state.setProperty("oscOutputConnected", getOscOutputConnected(), nullptr);

    return m_oscOutputConnected;
}

bool ControlGrisAudioProcessor::disconnectOSCOutput(String oscAddress, int oscPort)
{
    if (m_oscOutputConnected) {
        if (oscOutputSender.disconnect()) {
            m_oscOutputConnected = false;
        }
    }

    parameters.state.setProperty("oscOutputPortNumber", oscPort, nullptr);
    parameters.state.setProperty("oscOutputAddress", oscAddress, nullptr);
    parameters.state.setProperty("oscOutputConnected", getOscOutputConnected(), nullptr);

    return !m_oscOutputConnected;
}

void ControlGrisAudioProcessor::setOscOutputPluginId(int pluginId)
{
    parameters.state.setProperty("oscOutputPluginId", pluginId, nullptr);
}

int ControlGrisAudioProcessor::getOscOutputPluginId() const
{
    return parameters.state.getProperty("oscOutputPluginId", 1);
}

void ControlGrisAudioProcessor::sendOscOutputMessage()
{
    if (!m_oscOutputConnected)
        return;

    OSCMessage message(OSCAddressPattern("/tmp"));

    String pluginInstance = String("/controlgris/") + String(getOscOutputPluginId());

    float trajectory1x = mPositionAutomationManager.getSourcePosition().x;
    float trajectory1y = mPositionAutomationManager.getSourcePosition().y;
    float trajectory1z = mElevationAutomationManager.getSourcePosition().y;

    if (m_lastTrajectory1x != trajectory1x) {
        message.setAddressPattern(OSCAddressPattern(pluginInstance + "/traj/1/x"));
        message.addFloat32(trajectory1x);
        oscOutputSender.send(message);
        message.clear();

        message.setAddressPattern(OSCAddressPattern(pluginInstance + "/traj/1/xyz/1"));
        message.addFloat32(trajectory1x);
        oscOutputSender.send(message);
        message.clear();
    }

    if (m_lastTrajectory1y != trajectory1y) {
        message.setAddressPattern(OSCAddressPattern(pluginInstance + "/traj/1/y"));
        message.addFloat32(trajectory1y);
        oscOutputSender.send(message);
        message.clear();

        message.setAddressPattern(OSCAddressPattern(pluginInstance + "/traj/1/xyz/2"));
        message.addFloat32(trajectory1y);
        oscOutputSender.send(message);
        message.clear();
    }

    if (m_lastTrajectory1z != trajectory1z) {
        message.setAddressPattern(OSCAddressPattern(pluginInstance + "/traj/1/z"));
        message.addFloat32(trajectory1z);
        oscOutputSender.send(message);
        message.clear();

        message.setAddressPattern(OSCAddressPattern(pluginInstance + "/traj/1/xyz/3"));
        message.addFloat32(trajectory1z);
        oscOutputSender.send(message);
        message.clear();
    }

    if (m_lastTrajectory1x != trajectory1x || m_lastTrajectory1y != trajectory1y
        || m_lastTrajectory1z != trajectory1z) {
        message.setAddressPattern(OSCAddressPattern(pluginInstance + "/traj/1/xy"));
        message.addFloat32(trajectory1x);
        message.addFloat32(trajectory1y);
        oscOutputSender.send(message);
        message.clear();

        message.setAddressPattern(OSCAddressPattern(pluginInstance + "/traj/1/xyz"));
        message.addFloat32(trajectory1x);
        message.addFloat32(trajectory1y);
        message.addFloat32(trajectory1z);
        oscOutputSender.send(message);
        message.clear();
    }

    m_lastTrajectory1x = trajectory1x;
    m_lastTrajectory1y = trajectory1y;
    m_lastTrajectory1z = trajectory1z;

    if (m_lastAzispan != sources[0].getAzimuthSpan()) {
        message.setAddressPattern(OSCAddressPattern(pluginInstance + "/azispan"));
        message.addFloat32(sources[0].getAzimuthSpan());
        oscOutputSender.send(message);
        message.clear();
        m_lastAzispan = sources[0].getAzimuthSpan();
    }

    if (m_lastElespan != sources[0].getElevationSpan()) {
        message.setAddressPattern(OSCAddressPattern(pluginInstance + "/elespan"));
        message.addFloat32(sources[0].getElevationSpan());
        oscOutputSender.send(message);
        message.clear();
        m_lastElespan = sources[0].getElevationSpan();
    }

    if (mPositionAutomationManager.getSourceLink() != m_lastSourceLink) {
        message.setAddressPattern(OSCAddressPattern(pluginInstance + "/sourcelink"));
        message.addInt32(static_cast<int32>(mPositionAutomationManager.getSourceLink()));
        oscOutputSender.send(message);
        message.clear();

        String pattern = pluginInstance + String("/sourcelink/")
                         + String(static_cast<int>(mPositionAutomationManager.getSourceLink())) + String("/1");
        message.setAddressPattern(OSCAddressPattern(pattern));
        message.addInt32(1);
        oscOutputSender.send(message);
        message.clear();

        m_lastSourceLink = mPositionAutomationManager.getSourceLink();
    }

    if (static_cast<ElevationSourceLink>(mElevationAutomationManager.getSourceLink()) != m_lastElevationSourceLink) {
        message.setAddressPattern(OSCAddressPattern(pluginInstance + "/sourcelinkalt"));
        message.addInt32(static_cast<int32>(mElevationAutomationManager.getSourceLink()));
        oscOutputSender.send(message);
        message.clear();

        String patternAlt = pluginInstance + String("/sourcelinkalt/")
                            + String(static_cast<int>(mElevationAutomationManager.getSourceLink())) + String("/1");
        message.setAddressPattern(OSCAddressPattern(patternAlt));
        message.addInt32(1);
        oscOutputSender.send(message);
        message.clear();

        m_lastElevationSourceLink = static_cast<ElevationSourceLink>(mElevationAutomationManager.getSourceLink());
    }

    if (m_currentPositionPreset != m_lastPositionPreset) {
        message.setAddressPattern(OSCAddressPattern(pluginInstance + "/presets"));
        message.addInt32(m_currentPositionPreset);
        oscOutputSender.send(message);
        message.clear();

        m_lastPositionPreset = m_currentPositionPreset;
    }
}

//==============================================================================
void ControlGrisAudioProcessor::timerCallback()
{
    if (m_newPositionPreset != 0 && m_newPositionPreset != m_currentPositionPreset) {
        if (recallFixedPosition(m_newPositionPreset)) {
            m_currentPositionPreset = m_newPositionPreset;
            ControlGrisAudioProcessorEditor * ed = dynamic_cast<ControlGrisAudioProcessorEditor *>(getActiveEditor());
            if (ed != nullptr) {
                ed->updatePositionPreset(m_currentPositionPreset);
            }
        }
    }

    // MainField automation.
    if (mPositionAutomationManager.getPositionActivateState()) {
        if (mPositionAutomationManager.getTrajectoryType() == PositionTrajectoryType::realtime) {
            //...
        } else if (m_lastTimerTime != getCurrentTime()) {
            mPositionAutomationManager.setTrajectoryDeltaTime(getCurrentTime() - getInitTimeOnPlay());
        }
    } else if (m_isPlaying && mPositionAutomationManager.hasValidPlaybackPosition()) {
        mPositionAutomationManager.setSourcePosition(mPositionAutomationManager.getPlaybackPosition());
    } else if (mPositionAutomationManager.hasValidPlaybackPosition()
               && mPositionAutomationManager.getSourcePosition() != mPositionAutomationManager.getPlaybackPosition()) {
        int preset = (int)parameters.getParameterAsValue("positionPreset").getValue();
        recallFixedPosition(preset);
        mPositionAutomationManager.setSourcePosition(mPositionAutomationManager.getPlaybackPosition());
        linkPositionSourcePositions();
    }

    // ElevationField automation.
    if (getOscFormat() == SpatMode::LBAP && mElevationAutomationManager.getPositionActivateState()) {
        if (static_cast<ElevationTrajectoryType>(mElevationAutomationManager.getTrajectoryType())
            == ElevationTrajectoryType::realtime) {
            //...
        } else if (m_lastTimerTime != getCurrentTime()) {
            mElevationAutomationManager.setTrajectoryDeltaTime(getCurrentTime() - getInitTimeOnPlay());
        }
    } else if (m_isPlaying && mElevationAutomationManager.hasValidPlaybackPosition()) {
        mElevationAutomationManager.setSourcePosition(mElevationAutomationManager.getPlaybackPosition());
    } else if (mElevationAutomationManager.hasValidPlaybackPosition()
               && mElevationAutomationManager.getSourcePosition()
                      != mElevationAutomationManager.getPlaybackPosition()) {
        int preset = (int)parameters.getParameterAsValue("positionPreset").getValue();
        recallFixedPosition(preset);
        mElevationAutomationManager.setSourcePosition(mElevationAutomationManager.getPlaybackPosition());
        linkElevationSourcePositions();
    }

    m_lastTimerTime = getCurrentTime();

    ControlGrisAudioProcessorEditor * editor = dynamic_cast<ControlGrisAudioProcessorEditor *>(getActiveEditor());

    if (m_canStopActivate && !m_isPlaying) {
        if (mPositionAutomationManager.getPositionActivateState())
            mPositionAutomationManager.setPositionActivateState(false);
        if (mElevationAutomationManager.getPositionActivateState())
            mElevationAutomationManager.setPositionActivateState(false);
        m_canStopActivate = false;
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
    if (m_currentPositionPreset == 0) {
        for (int i{}; i < m_numOfSources; ++i) {
            String id(i);
            sources[i].setNormalizedAzimuth(parameters.state.getProperty(String("p_azimuth_") + id));
            sources[i].setNormalizedElevation(parameters.state.getProperty(String("p_elevation_") + id));
            sources[i].setDistance(parameters.state.getProperty(String("p_distance_") + id));
            if (i == 0) {
                mPositionAutomationManager.setSourcePosition(sources[0].getPos());
            }
        }
    }

    ControlGrisAudioProcessorEditor * editor = dynamic_cast<ControlGrisAudioProcessorEditor *>(getActiveEditor());
    if (editor != nullptr) {
        editor->setPluginState();
    }

    sendOscMessage();
}

//==============================================================================
void ControlGrisAudioProcessor::sourcePositionChanged(int sourceId, int whichField)
{
    if (whichField == 0) {
        if (getOscFormat() == SpatMode::LBAP) {
            setSourceParameterValue(sourceId, SourceParameter::azimuth, sources[sourceId].getNormalizedAzimuth());
            setSourceParameterValue(sourceId, SourceParameter::distance, sources[sourceId].getDistance());
        } else {
            setSourceParameterValue(sourceId, SourceParameter::azimuth, sources[sourceId].getNormalizedAzimuth());
            setSourceParameterValue(sourceId, SourceParameter::elevation, sources[sourceId].getNormalizedElevation());
        }
    } else {
        setSourceParameterValue(sourceId, SourceParameter::elevation, sources[sourceId].getNormalizedElevation());
    }

    if (whichField == 0) {
        if (sourceId != 0 && mPositionAutomationManager.getSourceLink() == PositionSourceLink::circularDeltaLock) {
            float deltaX = sources[sourceId].getDeltaX();
            float deltaY = sources[sourceId].getDeltaY();
            sources[0].setXYCoordinatesFromFixedSource(deltaX, deltaY);
        }
        validatePositionSourcePositions();
        if (mPositionAutomationManager.getTrajectoryType() >= PositionTrajectoryType::circleClockwise) {
            mPositionAutomationManager.setTrajectoryType(mPositionAutomationManager.getTrajectoryType(),
                                                         sources[0].getPos());
        }
    }
    if (whichField == 1 && getOscFormat() == SpatMode::LBAP) {
        if (sourceId != 0) {
            float sourceElevation = sources[sourceId].getElevation();
            float offset = 60.f / m_numOfSources * sourceId;
            switch (static_cast<ElevationSourceLink>(mElevationAutomationManager.getSourceLink())) {
            case ElevationSourceLink::fixedElevation:
                sources[0].setNormalizedElevation(sources[sourceId].getNormalizedElevation());
                break;
            case ElevationSourceLink::linearMin:
                sources[0].setElevation(sourceElevation - offset);
                break;
            case ElevationSourceLink::linearMax:
                sources[0].setElevation(sourceElevation + offset);
                break;
            case ElevationSourceLink::deltaLock: {
                float const deltaElevation = sources[sourceId].getDeltaElevation();
                sources[0].setElevationFromFixedSource(deltaElevation);
            } break;
            case ElevationSourceLink::independent:
            case ElevationSourceLink::undefined:
                jassertfalse;
            }
        }
        validateElevationSourcePositions();
    }
}

// Called whenever a source has changed.
//--------------------------------------
void ControlGrisAudioProcessor::setSourceParameterValue(int const sourceId,
                                                        SourceParameter const parameterId,
                                                        double const value)
{
    String id(sourceId);
    switch (parameterId) {
    case SourceParameter::azimuth:
        sources[sourceId].setNormalizedAzimuth(value);
        parameters.state.setProperty("p_azimuth_" + id, value, nullptr);
        break;
    case SourceParameter::elevation:
        sources[sourceId].setNormalizedElevation(value);
        parameters.state.setProperty(String("p_elevation_") + id, value, nullptr);
        break;
    case SourceParameter::distance:
        sources[sourceId].setDistance(value);
        parameters.state.setProperty(String("p_distance_") + id, value, nullptr);
        break;
    case SourceParameter::x:
        sources[sourceId].setX(value);
        break;
    case SourceParameter::y:
        sources[sourceId].setY(value);
        break;
    case SourceParameter::azimuthSpan:
        for (int i{}; i < m_numOfSources; ++i) {
            sources[i].setAzimuthSpan(value);
        }
        parameters.getParameter("azimuthSpan")->beginChangeGesture();
        parameters.getParameter("azimuthSpan")->setValueNotifyingHost(value);
        parameters.getParameter("azimuthSpan")->endChangeGesture();
        break;
    case SourceParameter::elevationSpan:
        for (int i{}; i < m_numOfSources; ++i) {
            sources[i].setElevationSpan(value);
        }
        parameters.getParameter("elevationSpan")->beginChangeGesture();
        parameters.getParameter("elevationSpan")->setValueNotifyingHost(value);
        parameters.getParameter("elevationSpan")->endChangeGesture();
        break;
    }
}

void ControlGrisAudioProcessor::trajectoryPositionChanged(AutomationManager * manager, Point<float> position)
{
    if (manager == &mPositionAutomationManager) {
        if (!getIsPlaying()) {
            mPositionAutomationManager.setSourceAndPlaybackPosition(Point<float>(position.x, position.y));
            parameters.getParameter("recordingTrajectory_x")->setValue(position.x);
            parameters.getParameter("recordingTrajectory_y")->setValue(position.y);
        }
        parameters.getParameter("recordingTrajectory_x")->beginChangeGesture();
        parameters.getParameter("recordingTrajectory_x")->setValueNotifyingHost(position.x);
        parameters.getParameter("recordingTrajectory_x")->endChangeGesture();
        parameters.getParameter("recordingTrajectory_y")->beginChangeGesture();
        parameters.getParameter("recordingTrajectory_y")->setValueNotifyingHost(position.y);
        parameters.getParameter("recordingTrajectory_y")->endChangeGesture();
        linkPositionSourcePositions();
    } else if (manager == &mElevationAutomationManager) {
        if (!getIsPlaying()) {
            mElevationAutomationManager.setSourceAndPlaybackPosition(Point<float>(0.f, position.y));
            parameters.getParameter("recordingTrajectory_z")->setValue(position.y);
        }
        parameters.getParameter("recordingTrajectory_z")->beginChangeGesture();
        parameters.getParameter("recordingTrajectory_z")->setValueNotifyingHost(position.y);
        parameters.getParameter("recordingTrajectory_z")->endChangeGesture();
        linkElevationSourcePositions();
    }
}

void ControlGrisAudioProcessor::linkPositionSourcePositions()
{
    float delta = kSourceDiameter / mPositionAutomationManager.getFieldWidth();
    Point<float> autopos = mPositionAutomationManager.getSourcePosition() - Point<float>(0.5, 0.5);
    float mag = sqrtf(autopos.x * autopos.x + autopos.y * autopos.y);
    float ang = atan2f(autopos.y, autopos.x);
    float const x = (mag + (mag * delta)) * cosf(ang) + 0.5;
    float const y = (mag + (mag * delta)) * sinf(ang) + 0.5;
    Point<float> const correctedPosition{ x, y };

    float deltaX = 0.f, deltaY = 0.f;
    switch (mPositionAutomationManager.getSourceLink()) {
    case PositionSourceLink::independent:
        sources[0].setPos(mPositionAutomationManager.getSourcePosition());
        break;
    case PositionSourceLink::circular:
    case PositionSourceLink::circularFixedRadius:
    case PositionSourceLink::circularFixedAngle:
    case PositionSourceLink::circularFullyFixed:
        sources[0].setPos(correctedPosition);
        if (getOscFormat() == SpatMode::LBAP) {
            float deltaAzimuth = sources[0].getDeltaAzimuth();
            float deltaDistance = sources[0].getDeltaDistance();
            for (int i{ 1 }; i < m_numOfSources; ++i) {
                sources[i].setCoordinatesFromFixedSource(deltaAzimuth, 0.0, deltaDistance);
            }
        } else {
            float deltaAzimuth = sources[0].getDeltaAzimuth();
            float deltaElevation = sources[0].getDeltaElevation();
            for (int i{ 1 }; i < m_numOfSources; ++i) {
                sources[i].setCoordinatesFromFixedSource(deltaAzimuth, deltaElevation, 0.0);
            }
        }
        break;
    case PositionSourceLink::circularDeltaLock:
        deltaX = mPositionAutomationManager.getSource().getDeltaX();
        deltaY = mPositionAutomationManager.getSource().getDeltaY();
        for (int i{}; i < m_numOfSources; ++i) {
            sources[i].setXYCoordinatesFromFixedSource(deltaX, deltaY);
        }
        break;
    case PositionSourceLink::linkSymmetricX:
        sources[0].setPos(mPositionAutomationManager.getSourcePosition());
        if (m_numOfSources == 2)
            sources[1].setSymmetricX(sources[0].getX(), sources[0].getY());
        break;
    case PositionSourceLink::linkSymmetricY:
        sources[0].setPos(mPositionAutomationManager.getSourcePosition());
        if (m_numOfSources == 2)
            sources[1].setSymmetricY(sources[0].getX(), sources[0].getY());
        break;
    default:
        jassertfalse;
    }
}

void ControlGrisAudioProcessor::linkElevationSourcePositions()
{
    float deltaY = 0.0f;

    switch (static_cast<ElevationSourceLink>(mElevationAutomationManager.getSourceLink())) {
    case ElevationSourceLink::independent:
        sources[0].setNormalizedElevation(mElevationAutomationManager.getSourcePosition().y);
        break;
    case ElevationSourceLink::fixedElevation:
        for (int i{}; i < m_numOfSources; ++i) {
            sources[i].setNormalizedElevation(mElevationAutomationManager.getSourcePosition().y);
        }
        break;
    case ElevationSourceLink::linearMin:
        for (int i{}; i < m_numOfSources; ++i) {
            float offset = mElevationAutomationManager.getSourcePosition().y * 90.0;
            sources[i].setElevation(60.0 / m_numOfSources * i + offset);
        }
        break;
    case ElevationSourceLink::linearMax:
        for (int i{}; i < m_numOfSources; ++i) {
            float offset = 90.0 - mElevationAutomationManager.getSourcePosition().y * 90.0;
            sources[i].setElevation(90.0 - (60.0 / m_numOfSources * i) - offset);
        }
        break;
    case ElevationSourceLink::deltaLock:
        deltaY = mElevationAutomationManager.getSource().getDeltaY();
        for (int i{}; i < m_numOfSources; ++i) {
            sources[i].setElevationFromFixedSource(deltaY);
        }
        break;
    default:
        jassertfalse;
    }
}

//==============================================================================
void ControlGrisAudioProcessor::validatePositionSourcePositions()
{
    auto const sourceLink{ mPositionAutomationManager.getSourceLink() };
    auto const trajectoryType{ mPositionAutomationManager.getTrajectoryType() };

    if (!getIsPlaying()) {
        if (sourceLink != PositionSourceLink::circularDeltaLock && trajectoryType != PositionTrajectoryType::drawing) {
            mPositionAutomationManager.setSourceAndPlaybackPosition(sources[0].getPos());
        } else {
            mPositionAutomationManager.setPlaybackPositionX(-1.0f);
            mPositionAutomationManager.setPlaybackPositionY(-1.0f);
        }
    }

    // Nothing to do for independent mode.

    // All circular modes.
    if (sourceLink >= PositionSourceLink::circular && sourceLink < PositionSourceLink::circularDeltaLock) {
        if (getOscFormat() == SpatMode::LBAP) {
            float deltaAzimuth = sources[0].getDeltaAzimuth();
            float deltaDistance = sources[0].getDeltaDistance();
            for (int i{ 1 }; i < m_numOfSources; ++i) {
                sources[i].setCoordinatesFromFixedSource(deltaAzimuth, 0.0, deltaDistance);
            }
        } else {
            float deltaAzimuth = sources[0].getDeltaAzimuth();
            float deltaElevation = sources[0].getDeltaElevation();
            for (int i{ 1 }; i < m_numOfSources; ++i) {
                sources[i].setCoordinatesFromFixedSource(deltaAzimuth, deltaElevation, 0.0);
            }
        }
    }
    // Delta Lock mode.
    else if (sourceLink == PositionSourceLink::circularDeltaLock) {
        float deltaX = sources[0].getDeltaX();
        float deltaY = sources[0].getDeltaY();
        for (int i{ 1 }; i < m_numOfSources; ++i) {
            sources[i].setXYCoordinatesFromFixedSource(deltaX, deltaY);
        }
    }
    // Symmetric X.
    else if (sourceLink == PositionSourceLink::linkSymmetricX) {
        if (m_numOfSources == 2) {
            float x = sources[0].getX();
            float y = sources[0].getY();
            sources[1].setSymmetricX(x, y);
        }
    }
    // Symmetric Y.
    else if (sourceLink == PositionSourceLink::linkSymmetricY) {
        if (m_numOfSources == 2) {
            float x = sources[0].getX();
            float y = sources[0].getY();
            sources[1].setSymmetricY(x, y);
        }
    }

    // Fix source positions.
    mPositionAutomationManager.fixSourcePosition();
    bool shouldBeFixed = sourceLink != PositionSourceLink::independent;
    if (static_cast<int>(sourceLink) >= 2 && static_cast<int>(sourceLink) < 6) {
        for (int i{}; i < m_numOfSources; ++i) {
            sources[i].fixSourcePosition(shouldBeFixed);
        }
    }
}

void ControlGrisAudioProcessor::validateElevationSourcePositions()
{
    auto const sourceLink{ static_cast<ElevationSourceLink>(mElevationAutomationManager.getSourceLink()) };
    auto const trajectoryType{ mElevationAutomationManager.getTrajectoryType() };

    if (!getIsPlaying()) {
        if (sourceLink != ElevationSourceLink::deltaLock
            && static_cast<ElevationTrajectoryType>(trajectoryType) != ElevationTrajectoryType::drawing) {
            mElevationAutomationManager.setSourceAndPlaybackPosition(
                Point<float>(0.f, sources[0].getNormalizedElevation()));
        } else {
            mElevationAutomationManager.setPlaybackPositionX(-1.f);
            mElevationAutomationManager.setPlaybackPositionY(-1.f);
        }
    }

    // Fixed elevation.
    if (sourceLink == ElevationSourceLink::fixedElevation) {
        for (int i{ 1 }; i < m_numOfSources; ++i) {
            sources[i].setElevation(sources[0].getElevation());
        }
    }
    // Linear min.
    else if (sourceLink == ElevationSourceLink::linearMin) {
        for (int i{ 1 }; i < m_numOfSources; ++i) {
            float offset = sources[0].getElevation();
            sources[i].setElevation(60.0 / m_numOfSources * i + offset);
        }
    }
    // Linear max.
    else if (sourceLink == ElevationSourceLink::linearMax) {
        for (int i{ 1 }; i < m_numOfSources; ++i) {
            float offset = 90.0 - sources[0].getElevation();
            sources[i].setElevation(90.0 - (60.0 / m_numOfSources * i) - offset);
        }
    }
    // Delta lock.
    else if (sourceLink == ElevationSourceLink::deltaLock) {
        float deltaY = sources[0].getDeltaElevation();
        for (int i{ 1 }; i < m_numOfSources; ++i) {
            sources[i].setElevationFromFixedSource(deltaY);
        }
    }

    // Fix source positions.
    mElevationAutomationManager.fixSourcePosition(); // not sure...
    bool const shouldBeFixed{ sourceLink != ElevationSourceLink::independent };
    if (static_cast<int>(sourceLink) >= 2 && static_cast<int>(sourceLink) < 5) { // TODO: what is going on here?
        for (int i{}; i < m_numOfSources; ++i) {
            sources[i].fixSourcePositionElevation(shouldBeFixed);
        }
    }
}

//==============================================================================
void ControlGrisAudioProcessor::setPositionPreset(int const presetNumber)
{
    if (presetNumber != m_currentPositionPreset) {
        if (presetNumber == 0) {
            m_newPositionPreset = m_currentPositionPreset = 0;
            parameters.getParameter("positionPreset")->beginChangeGesture();
            parameters.getParameter("positionPreset")->setValueNotifyingHost(0.0f);
            parameters.getParameter("positionPreset")->endChangeGesture();
        } else if (recallFixedPosition(presetNumber)) {
            m_currentPositionPreset = presetNumber;
            auto const value{ presetNumber / static_cast<float>(NUMBER_OF_POSITION_PRESETS + 1) };
            parameters.getParameter("positionPreset")->beginChangeGesture();
            parameters.getParameter("positionPreset")->setValueNotifyingHost(value);
            parameters.getParameter("positionPreset")->endChangeGesture();
            mPositionAutomationManager.setTrajectoryType(mPositionAutomationManager.getTrajectoryType(),
                                                         sources[0].getPos());
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
        newData->setAttribute(getFixedPosSourceName(i, 0), sources[i].getX());
        newData->setAttribute(getFixedPosSourceName(i, 1), sources[i].getY());
        if (m_selectedOscFormat == SpatMode::LBAP) {
            newData->setAttribute(getFixedPosSourceName(i, 2), sources[i].getNormalizedElevation());
        }
    }

    // Replace an element if the new one has the same ID as one already saved.
    bool found{ false };
    XmlElement * fpos = fixPositionData.getFirstChildElement();
    while (fpos) {
        if (fpos->getIntAttribute("ID") == id) {
            found = true;
            break;
        }
        fpos = fpos->getNextElement();
    }

    if (found) {
        fixPositionData.replaceChildElement(fpos, newData);
    } else {
        fixPositionData.addChildElement(newData);
    }

    XmlElementDataSorter sorter("ID", true);
    fixPositionData.sortChildElements(sorter);

    recallFixedPosition(id);
}

bool ControlGrisAudioProcessor::recallFixedPosition(int id)
{
    bool found = false;
    XmlElement * fpos = fixPositionData.getFirstChildElement();
    while (fpos) {
        if (fpos->getIntAttribute("ID") == id) {
            found = true;
            break;
        }
        fpos = fpos->getNextElement();
    }

    if (!found) {
        return false;
    }

    currentFixPosition = fpos;
    float x, y, z = 0.0;
    for (int i{}; i < m_numOfSources; ++i) {
        x = currentFixPosition->getDoubleAttribute(getFixedPosSourceName(i, 0));
        y = currentFixPosition->getDoubleAttribute(getFixedPosSourceName(i, 1));
        sources[i].setPos(Point<float>(x, y));
        sources[i].setFixedPosition(x, y);
        if (m_selectedOscFormat == SpatMode::LBAP) {
            z = currentFixPosition->getDoubleAttribute(getFixedPosSourceName(i, 2));
            sources[i].setFixedElevation(z);
            sources[i].setNormalizedElevation(z);
        }
    }

    return true;
}

void ControlGrisAudioProcessor::copyFixedPositionXmlElement(XmlElement * src, XmlElement * dest)
{
    if (dest == nullptr)
        dest = new XmlElement(FIXED_POSITION_DATA_TAG);

    forEachXmlChildElement(*src, element)
    {
        XmlElement * newData = new XmlElement("ITEM");
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
    XmlElement * fpos = fixPositionData.getFirstChildElement();
    while (fpos) {
        if (fpos->getIntAttribute("ID") == id) {
            found = true;
            break;
        }
        fpos = fpos->getNextElement();
    }

    if (found) {
        fixPositionData.removeChildElement(fpos, true);
        XmlElementDataSorter sorter("ID", true);
        fixPositionData.sortChildElements(sorter);
    }
}

//==============================================================================
const String ControlGrisAudioProcessor::getName() const
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
    m_needInitialization = true;
    m_lastTime = m_lastTimerTime = 10000000.0;
    m_canStopActivate = true;

    // If a preset is actually selected, we always recall it on initialize because
    // the automation won't trigger parameterChanged if it stays on the same value.
    if (m_currentPositionPreset != 0) {
        if (recallFixedPosition(m_currentPositionPreset)) {
            ControlGrisAudioProcessorEditor * ed = dynamic_cast<ControlGrisAudioProcessorEditor *>(getActiveEditor());
            if (ed != nullptr) {
                ed->updatePositionPreset(m_currentPositionPreset);
            }
        }
    }
}

void ControlGrisAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    if (m_isPlaying == 0)
        initialize();
}

void ControlGrisAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
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
    int started = m_isPlaying;
    AudioPlayHead * phead = getPlayHead();
    if (phead != nullptr) {
        AudioPlayHead::CurrentPositionInfo playposinfo;
        phead->getCurrentPosition(playposinfo);
        m_isPlaying = playposinfo.isPlaying;
        m_bpm = playposinfo.bpm;
        if (m_needInitialization) {
            m_initTimeOnPlay = m_currentTime = playposinfo.timeInSeconds < 0.0 ? 0.0 : playposinfo.timeInSeconds;
            m_needInitialization = false;
        } else {
            m_currentTime = playposinfo.timeInSeconds;
        }
    }

    if (!started && m_isPlaying) { // Initialization here only for Logic (also Reaper and Live), which are not
        PluginHostType hostType;   // calling prepareToPlay every time the sequence starts.
        if (hostType.isLogic() || hostType.isReaper() || hostType.isAbletonLive()) {
            initialize();
        }
    }
    m_lastTime = m_currentTime;
}

//==============================================================================
AudioProcessorEditor * ControlGrisAudioProcessor::createEditor()
{
    return new ControlGrisAudioProcessorEditor(*this,
                                               parameters,
                                               mPositionAutomationManager,
                                               mElevationAutomationManager);
}

//==============================================================================
void ControlGrisAudioProcessor::getStateInformation(MemoryBlock & destData)
{
    for (int i{}; i < MAX_NUMBER_OF_SOURCES; ++i) {
        String id(i);
        parameters.state.setProperty(String("p_azimuth_") + id, sources[i].getNormalizedAzimuth(), nullptr);
        parameters.state.setProperty(String("p_elevation_") + id, sources[i].getNormalizedElevation(), nullptr);
        parameters.state.setProperty(String("p_distance_") + id, sources[i].getDistance(), nullptr);
    }

    auto state = parameters.copyState();

    std::unique_ptr<XmlElement> xmlState(state.createXml());

    if (xmlState.get() != nullptr) {
        XmlElement * childExist = xmlState->getChildByName(FIXED_POSITION_DATA_TAG);
        if (childExist) {
            xmlState->removeChildElement(childExist, true);
        }
        if (fixPositionData.getNumChildElements() > 0) {
            XmlElement * positionData = xmlState->createNewChildElement(FIXED_POSITION_DATA_TAG);
            copyFixedPositionXmlElement(&fixPositionData, positionData);
        }
        copyXmlToBinary(*xmlState, destData);
    }
}

void ControlGrisAudioProcessor::setStateInformation(const void * data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr) {
        // Set global settings values.
        //----------------------------
        ValueTree valueTree = ValueTree::fromXml(*xmlState);
        setOscFormat((SpatMode)(int)valueTree.getProperty("oscFormat", 0));
        setOscPortNumber(valueTree.getProperty("oscPortNumber", 18032));
        handleOscConnection(valueTree.getProperty("oscConnected", true));
        setNumberOfSources(valueTree.getProperty("numberOfSources", 1), false);
        setFirstSourceId(valueTree.getProperty("firstSourceId", 1));
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
            fixPositionData.deleteAllChildElements();
            copyFixedPositionXmlElement(positionData, &fixPositionData);
        }
        // Replace the state and call automated parameter current values.
        //---------------------------------------------------------------
        parameters.replaceState(ValueTree::fromXml(*xmlState));
    }

    setPluginState();
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor * JUCE_CALLTYPE createPluginFilter()
{
    return new ControlGrisAudioProcessor();
}
