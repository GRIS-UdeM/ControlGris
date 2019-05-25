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

const int MaxNumberOfSources = 8;

class ControlGrisAudioProcessor  : public AudioProcessor,
                                   public AudioProcessorValueTreeState::Listener,
                                   public AutomationManager::Listener,
                                   public Timer
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
    void setOscFormat(int oscFormat);
    int getOscFormat();

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

    void timerCallback() override;

    //==============================================================================
    void setPluginState();

    void setSourceParameterValue(int sourceId, int parameterId, double value);
    void setLinkedParameterValue(int sourceId, int parameterId);

    void newEventConsumed();
    bool isSomethingChanged();

    double getInitTimeOnPlay();
    double getCurrentTime();

    bool getIsPlaying();

    void trajectoryPositionChanged(Point<float> position) override;

    //==============================================================================
    AudioProcessorValueTreeState parameters;

    AutomationManager automationManager;

private:
    bool m_oscConnected;
    int m_firstSourceId;
    int m_numOfSources;
    int m_selectedSourceId;
    int m_selectedOscFormat;
    int m_currentOSCPort;
    int m_lastConnectedOSCPort;
    bool m_somethingChanged;

    bool m_needInitialization;
    double m_initTimeOnPlay;
    double m_currentTime;

    bool m_isPlaying;

    Source sources[MaxNumberOfSources];

    OSCSender oscSender;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControlGrisAudioProcessor)
};
