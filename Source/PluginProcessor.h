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
#include "Source.h"
#include "AutomationManager.h"
#include "ControlGrisConstants.h"
#include "ControlGrisUtilities.h"

class ControlGrisAudioProcessor  : public AudioProcessor,
                                   public AudioProcessorValueTreeState::Listener,
                                   public AutomationManager::Listener,
                                   public Timer,
                                   private OSCReceiver::Listener<OSCReceiver::RealtimeCallback>
{
public:
    //==============================================================================
    ControlGrisAudioProcessor();
    ~ControlGrisAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    void parameterChanged(const String &parameterID, float newValue) override;

    //==============================================================================
    void setOscFormat(SPAT_MODE_ENUM oscFormat);
    SPAT_MODE_ENUM getOscFormat();

    void setOscPortNumber(int oscPortNumber);
    int getOscPortNumber();

    void setFirstSourceId(int firstSourceId, bool propagate=true);
    int getFirstSourceId();

    void setSelectedSourceId(int id);

    void setNumberOfSources(int numOfSources, bool propagate=true);
    int getNumberOfSources();

    Source * getSources();

    //==============================================================================
    bool createOscConnection(int oscPort);
    bool disconnectOSC();
    bool getOscConnected();
    void handleOscConnection(bool state);
    void sendOscMessage();

    bool createOscInputConnection(int oscPort);
    bool disconnectOSCInput(int oscPort);
    bool getOscInputConnected();
    void oscMessageReceived (const OSCMessage& message) override;
    void oscBundleReceived(const OSCBundle& bundle) override;

    bool createOscOutputConnection(String oscAddress, int oscPort);
    bool disconnectOSCOutput(String oscAddress, int oscPort);
    bool getOscOutputConnected();
    void sendOscOutputMessage();

    void timerCallback() override;

    //==============================================================================
    void setPluginState();

    void setSourceParameterValue(int sourceId, int parameterId, double value);

    void initialize();

    double getInitTimeOnPlay();
    double getCurrentTime();

    bool getIsPlaying();
    double getBPM();

    void trajectoryPositionChanged(AutomationManager *manager, Point<float> position) override;

    void onSourceLinkChanged(int value);
    void onSourceLinkAltChanged(int value);

    void linkSourcePositions();
    void linkSourcePositionsAlt();

    void setPositionPreset(int presetNumber);

    void addNewFixedPosition(int id);
    bool recallFixedPosition(int id);
    void copyFixedPositionXmlElement(XmlElement *src, XmlElement *dest);
    XmlElement * getFixedPositionData(); // retrieve all data.
    void deleteFixedPosition(int id);

    //==============================================================================
    AudioProcessorValueTreeState parameters;

    AutomationManager automationManager;
    AutomationManager automationManagerAlt;

private:
    SPAT_MODE_ENUM m_selectedOscFormat;
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
    int m_lastSourceLink;
    int m_lastSourceLinkAlt;

    Source sources[MAX_NUMBER_OF_SOURCES];

    OSCSender oscSender;
    OSCSender oscOutputSender;
    OSCReceiver oscInputReceiver;

    XmlElement fixPositionData;
    XmlElement *currentFixPosition = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControlGrisAudioProcessor)
};
