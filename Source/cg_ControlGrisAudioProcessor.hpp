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

#include <JuceHeader.h>

#include "cg_ChangeGesturesManager.hpp"
#include "cg_PresetsManager.hpp"
#include "cg_Source.hpp"
#include "cg_SourceLinkEnforcer.hpp"
#include "cg_TrajectoryManager.hpp"
#include "cg_constants.hpp"

//==============================================================================
class ControlGrisAudioProcessor final
    : public AudioProcessor
    , public AudioProcessorValueTreeState::Listener
    //    , public TrajectoryManager::Listener
    , public Timer
    , private OSCReceiver::Listener<OSCReceiver::RealtimeCallback>
{
    //==============================================================================
    SpatMode mSpatMode{ SpatMode::dome };
    bool mOscConnected{ false };
    bool mOscInputConnected{ false };
    bool mOscOutputConnected{ false };
    SourceId mFirstSourceId{ 1 };
    int mCurrentOscPort{ 18032 };
    int mLastConnectedOscPort{ -1 };
    int mCurrentOscInputPort{ 8000 };
    int mCurrentOscOutputPort{ 9000 };
    String mCurrentOscOutputAddress{ "192.168.1.100" };
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
    SourceLinkEnforcer mPositionSourceLinkEnforcer{ mSources, PositionSourceLink::independent };
    SourceLinkEnforcer mElevationSourceLinkEnforcer{ mSources, ElevationSourceLink::independent };

    AudioProcessorValueTreeState mAudioProcessorValueTreeState;

    ChangeGesturesManager mChangeGesturesManager{ mAudioProcessorValueTreeState };
    PresetsManager mPresetManager{ mFixPositionData,
                                   mSources,
                                   mPositionSourceLinkEnforcer,
                                   mElevationSourceLinkEnforcer };

    PositionTrajectoryManager mPositionTrajectoryManager{ *this, mSources.getPrimarySource() };
    ElevationTrajectoryManager mElevationTrajectoryManager{ *this, mSources.getPrimarySource() };

public:
    //==============================================================================
    ControlGrisAudioProcessor();
    ~ControlGrisAudioProcessor() override;

    ControlGrisAudioProcessor(ControlGrisAudioProcessor const &) = delete;
    ControlGrisAudioProcessor(ControlGrisAudioProcessor &&) = delete;

    ControlGrisAudioProcessor & operator=(ControlGrisAudioProcessor const &) = delete;
    ControlGrisAudioProcessor & operator=(ControlGrisAudioProcessor &&) = delete;
    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(BusesLayout const & layouts) const override;
#endif

    void processBlock(AudioBuffer<float> &, MidiBuffer &) override;

    //==============================================================================
    AudioProcessorEditor * createEditor() override;
    bool hasEditor() const override { return true; } // (change this to false if you choose to not supply an editor)

    //==============================================================================
    String const getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override { return 0.0; }

    //==============================================================================
    int getNumPrograms() override
    {
        return 1;
    } // NB: some hosts don't cope very well if you tell them there are 0 programs,
      // so this should be at least 1, even if you're not really implementing programs.
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram([[maybe_unused]] int index) override {}
    String const getProgramName([[maybe_unused]] int index) override { return {}; }
    void changeProgramName([[maybe_unused]] int index, [[maybe_unused]] String const & newName) override {}

    //==============================================================================
    void getStateInformation(MemoryBlock & destData) override;
    void setStateInformation(const void * data, int sizeInBytes) override;

    //==============================================================================
    void parameterChanged(String const & parameterId, float newValue) override;

    //==============================================================================
    void setSpatMode(SpatMode spatMode);
    SpatMode getSpatMode() const { return mSpatMode; }

    void setOscPortNumber(int oscPortNumber);
    int getOscPortNumber() const { return mCurrentOscPort; }

    void setFirstSourceId(SourceId firstSourceId, bool propagate = true);
    auto getFirstSourceId() const { return mFirstSourceId; }

    void setNumberOfSources(int numOfSources, bool propagate = true);

    auto & getSources() { return mSources; }
    auto const & getSources() const { return mSources; }

    //==============================================================================
    AudioProcessorValueTreeState const & getValueTreeState() const { return mAudioProcessorValueTreeState; }
    AudioProcessorValueTreeState & getValueTreeState() { return mAudioProcessorValueTreeState; }
    //==============================================================================
    bool createOscConnection(int oscPort);
    bool disconnectOsc();
    bool isOscConnected() const { return mOscConnected; }
    void handleOscConnection(bool state);
    void sendOscMessage();

    bool createOscInputConnection(int oscPort);
    bool disconnectOscInput(int oscPort);
    bool getOscInputConnected() const { return mOscInputConnected; }
    void oscMessageReceived(const OSCMessage & message) override;
    void oscBundleReceived(const OSCBundle & bundle) override;

    bool createOscOutputConnection(String const & oscAddress, int oscPort);
    bool disconnectOscOutput(String const & oscAddress, int oscPort);
    bool getOscOutputConnected() const { return mOscOutputConnected; }
    void sendOscOutputMessage();
    void setOscOutputPluginId(int pluginId);
    int getOscOutputPluginId() const;

    void timerCallback() override;

    //==============================================================================
    void setPluginState();

    void sourcePositionChanged(SourceIndex sourceIndex, int whichField);
    void setSourceParameterValue(SourceIndex sourceIndex, SourceParameter sourceParameter, float value);

    void initialize();

    double getInitTimeOnPlay() const { return std::max(mInitTimeOnPlay, 0.0); }
    double getCurrentTime() const { return std::max(mCurrentTime, 0.0); }

    bool isPlaying() const { return mIsPlaying; }
    double getBpm() const { return mBpm; }

    ChangeGesturesManager & getChangeGestureManager() { return mChangeGesturesManager; }

    void setPositionSourceLink(PositionSourceLink value);
    void setElevationSourceLink(ElevationSourceLink value);

    PresetsManager & getPresetsManager() { return mPresetManager; }
    PresetsManager const & getPresetsManager() const { return mPresetManager; }

    void sourceChanged(Source & source, Source::ChangeType changeType, Source::OriginOfChange origin);
    void setSelectedSource(Source const & source);
    void updatePrimarySourceParameters(Source::ChangeType changeType);

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR(ControlGrisAudioProcessor)
};
