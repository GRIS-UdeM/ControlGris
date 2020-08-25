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
#include "ChangeGesturesManager.h"
#include "ControlGrisConstants.h"
#include "ControlGrisUtilities.h"
#include "PresetsManager.h"
#include "Source.h"
#include "SourceLinkEnforcer.h"

//==============================================================================
class ControlGrisAudioProcessor final
    : public AudioProcessor
    , public AudioProcessorValueTreeState::Listener
    , public AutomationManager::Listener
    , public Timer
    , private OSCReceiver::Listener<OSCReceiver::RealtimeCallback>
{
private:
    //==============================================================================
    SpatMode mSpatMode{ SpatMode::dome };
    bool mOscConnected{ false };
    bool mOscInputConnected{ false };
    bool mOscOutputConnected{ false };
    SourceId mFirstSourceId{ 1 };
    int mCurrentOSCPort{ 18032 };
    int mLastConnectedOSCPort{ -1 };
    int mCurrentOSCInputPort{ 8000 };
    int mCurrentOSCOutputPort{ 9000 };
    String mCurrentOSCOutputAddress{ "192.168.1.100" };
    bool mNeedsInitialization{ true };

    double mInitTimeOnPlay{ 0.0 };
    double mCurrentTime{ 0.0 };
    double mLastTime{ 10000000.0 };
    double mLastTimerTime{ 10000000.0 };

    bool mIsPlaying{ false };
    bool mCanStopActivate{ false };
    double mBpm{ 120 };

    OSCSender mOscSender;
    OSCSender mOscOutputSender;
    OSCReceiver mOscInputReceiver;

    XmlElement mFixPositionData{ FIXED_POSITION_DATA_TAG };

    Sources mSources{};
    SourceLinkEnforcer mPositionSourceLinkEnforcer{ mSources };
    SourceLinkEnforcer mElevationSourceLinkEnforcer{ mSources };

public:
    AudioProcessorValueTreeState mAudioProcessorValueTreeState;

private:
    ChangeGesturesManager mChangeGesturesManager{ mAudioProcessorValueTreeState };
    PresetsManager mPresetManager{ mFixPositionData,
                                   mSources,
                                   mPositionSourceLinkEnforcer,
                                   mElevationSourceLinkEnforcer };

public:
    //==============================================================================
    PositionAutomationManager mPositionAutomationManager{ *this, mSources.getPrimarySource() };
    ElevationAutomationManager mElevationAutomationManager{ *this, mSources.getPrimarySource() };
    //==============================================================================
    ControlGrisAudioProcessor();
    ~ControlGrisAudioProcessor() final;
    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) final;
    void releaseResources() final {}

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout & layouts) const final;
#endif

    void processBlock(AudioBuffer<float> &, MidiBuffer &) final;

    //==============================================================================
    AudioProcessorEditor * createEditor() final;
    bool hasEditor() const final { return true; } // (change this to false if you choose to not supply an editor)

    //==============================================================================
    String const getName() const final;

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
    void setCurrentProgram([[maybe_unused]] int index) final {}
    String const getProgramName([[maybe_unused]] int index) final { return {}; }
    void changeProgramName([[maybe_unused]] int index, [[maybe_unused]] String const & newName) final {}

    //==============================================================================
    void getStateInformation(MemoryBlock & destData) final;
    void setStateInformation(const void * data, int sizeInBytes) final;

    //==============================================================================
    void parameterChanged(String const & parameterID, float newValue) final;

    //==============================================================================
    void setSpatMode(SpatMode spatMode);
    SpatMode getSpatMode() const { return mSpatMode; }

    void setOscPortNumber(int oscPortNumber);
    int getOscPortNumber() const { return mCurrentOSCPort; }

    void setFirstSourceId(SourceId firstSourceId, bool propagate = true);
    auto getFirstSourceId() const { return mFirstSourceId; }

    void setNumberOfSources(int numOfSources, bool propagate = true);

    auto & getSources() { return mSources; }
    auto const & getSources() const { return mSources; }

    //==============================================================================
    bool createOscConnection(int oscPort);
    bool disconnectOSC();
    bool getOscConnected() const { return mOscConnected; }
    void handleOscConnection(bool state);
    void sendOscMessage();

    bool createOscInputConnection(int oscPort);
    bool disconnectOSCInput(int oscPort);
    bool getOscInputConnected() const { return mOscInputConnected; }
    void oscMessageReceived(const OSCMessage & message) final;
    void oscBundleReceived(const OSCBundle & bundle) final;

    bool createOscOutputConnection(String const & oscAddress, int oscPort);
    bool disconnectOSCOutput(String const & oscAddress, int oscPort);
    bool getOscOutputConnected() const { return mOscOutputConnected; }
    void sendOscOutputMessage();
    void setOscOutputPluginId(int pluginId);
    int getOscOutputPluginId() const;

    void timerCallback() final;

    //==============================================================================
    void setPluginState();

    void sourcePositionChanged(SourceIndex sourceIndex, int whichField);
    void setSourceParameterValue(SourceIndex sourceIndex, SourceParameter sourceParameter, float value);

    void initialize();

    double getInitTimeOnPlay() const { return std::max(mInitTimeOnPlay, 0.0); }
    double getCurrentTime() const { return std::max(mCurrentTime, 0.0); }

    bool isPlaying() const { return mIsPlaying; }
    double getBPM() const { return mBpm; }

    ChangeGesturesManager & getChangeGestureManager() { return mChangeGesturesManager; }

    void trajectoryPositionChanged(AutomationManager * manager, Point<float> position, Radians elevation) final;

    void setPositionSourceLink(PositionSourceLink value);
    void setElevationSourceLink(ElevationSourceLink value);

    PresetsManager & getPresetsManager() { return mPresetManager; }
    PresetsManager const & getPresetsManager() const { return mPresetManager; }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControlGrisAudioProcessor)
};
