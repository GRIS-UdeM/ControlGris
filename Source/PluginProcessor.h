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
#include "ControlGrisConstants.h"
#include "ControlGrisUtilities.h"
#include "Source.h"

class ControlGrisAudioProcessor final
    : public AudioProcessor
    , public AudioProcessorValueTreeState::Listener
    , public AutomationManager::Listener
    , public Timer
    , private OSCReceiver::Listener<OSCReceiver::RealtimeCallback>
{
public:
    //==============================================================================
    ControlGrisAudioProcessor();
    ~ControlGrisAudioProcessor() final;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) final;
    void releaseResources() final;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout & layouts) const final;
#endif

    void processBlock(AudioBuffer<float> &, MidiBuffer &) final;

    //==============================================================================
    AudioProcessorEditor * createEditor() final;
    bool hasEditor() const final { return true; } // (change this to false if you choose to not supply an editor)

    //==============================================================================
    const String getName() const final;

    bool acceptsMidi() const final;
    bool producesMidi() const final;
    bool isMidiEffect() const final;
    double getTailLengthSeconds() const final { return 0.0; }

    //==============================================================================
    int getNumPrograms() final
    {
        return 1;
    } // NB: some hosts don't cope very well if you tell them there are 0 programs,
      // so this should be at least 1, even if you're not really implementing programs.
    int getCurrentProgram() final { return 0; }
    void setCurrentProgram(int index) final {}
    const String getProgramName(int index) final { return {}; }
    void changeProgramName(int index, const String & newName) final {}

    //==============================================================================
    void getStateInformation(MemoryBlock & destData) final;
    void setStateInformation(const void * data, int sizeInBytes) final;

    //==============================================================================
    void parameterChanged(const String & parameterID, float newValue) final;

    //==============================================================================
    void setOscFormat(SpatMode oscFormat);
    SpatMode getOscFormat() const { return m_selectedOscFormat; }

    void setOscPortNumber(int oscPortNumber);
    int getOscPortNumber() const { return m_currentOSCPort; }

    void setFirstSourceId(int firstSourceId, bool propagate = true);
    int getFirstSourceId() const { return m_firstSourceId; }

    void setSelectedSourceId(int id);

    void setNumberOfSources(int numOfSources, bool propagate = true);
    int getNumberOfSources() const { return m_numOfSources; }

    Source * getSources() { return sources; }
    Source const * getSources() const { return sources; }

    //==============================================================================
    bool createOscConnection(int oscPort);
    bool disconnectOSC();
    bool getOscConnected() const { return m_oscConnected; }
    void handleOscConnection(bool state);
    void sendOscMessage();

    bool createOscInputConnection(int oscPort);
    bool disconnectOSCInput(int oscPort);
    bool getOscInputConnected() const { return m_oscInputConnected; }
    void oscMessageReceived(const OSCMessage & message) final;
    void oscBundleReceived(const OSCBundle & bundle) final;

    bool createOscOutputConnection(String oscAddress, int oscPort);
    bool disconnectOSCOutput(String oscAddress, int oscPort);
    bool getOscOutputConnected() const { return m_oscOutputConnected; }
    void sendOscOutputMessage();
    void setOscOutputPluginId(int pluginId);
    int getOscOutputPluginId() const;

    void timerCallback() final;

    //==============================================================================
    void setPluginState();

    void sourcePositionChanged(int sourceId, int whichField);
    void setSourceParameterValue(int sourceId, SourceParameter sourceParameter, double value);

    void initialize();

    double getInitTimeOnPlay() const { return m_initTimeOnPlay >= 0.0 ? m_initTimeOnPlay : 0.0; }
    double getCurrentTime() const { return m_currentTime >= 0.0 ? m_currentTime : 0.0; }

    bool getIsPlaying() const { return m_isPlaying; }
    double getBPM() const { return m_bpm; }

    void trajectoryPositionChanged(AutomationManager * manager, Point<float> position) final;

    void setSourceLink(SourceLink value);
    void setElevationSourceLink(ElevationSourceLink value);
    void onSourceLinkChanged(SourceLink value);
    void onElevationSourceLinkChanged(ElevationSourceLink value);

    void linkSourcePositions();
    void linkSourcePositionsAlt();

    // These are called after a source has changed from mouse movement in a field (or from an OSC message).
    void validateSourcePositions();
    void validateSourcePositionsAlt();

    void setPositionPreset(int presetNumber);

    void addNewFixedPosition(int id);
    bool recallFixedPosition(int id);
    void copyFixedPositionXmlElement(XmlElement * src, XmlElement * dest);
    XmlElement * getFixedPositionData() { return &fixPositionData; } // retrieve all data.
    XmlElement const * getFixedPositionData() const { return &fixPositionData; }
    void deleteFixedPosition(int id);

    //==============================================================================
    AudioProcessorValueTreeState parameters;

    AutomationManager automationManager;
    AutomationManager automationManagerAlt;

private:
    SpatMode m_selectedOscFormat;
    bool m_oscConnected;
    bool m_oscInputConnected;
    bool m_oscOutputConnected;
    int m_firstSourceId;
    int m_numOfSources;
    int m_selectedSourceId;
    int m_currentOSCPort;
    int m_lastConnectedOSCPort;
    int m_currentOSCInputPort;
    int m_currentOSCOutputPort;
    String m_currentOSCOutputAddress;
    bool m_needInitialization;

    double m_initTimeOnPlay;
    double m_currentTime;
    double m_lastTime;
    double m_lastTimerTime;

    bool m_isPlaying;
    bool m_canStopActivate;
    double m_bpm;

    int m_currentPositionPreset;
    int m_newPositionPreset;

    // Filtering variables for OSC controller output.
    int m_lastPositionPreset;
    float m_lastTrajectory1x;
    float m_lastTrajectory1y;
    float m_lastTrajectory1z;
    float m_lastAzispan;
    float m_lastElespan;
    SourceLink m_lastSourceLink;
    ElevationSourceLink m_lastElevationSourceLink;

    Source sources[MAX_NUMBER_OF_SOURCES];

    OSCSender oscSender;
    OSCSender oscOutputSender;
    OSCReceiver oscInputReceiver;

    XmlElement fixPositionData;
    XmlElement * currentFixPosition = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControlGrisAudioProcessor)
};
