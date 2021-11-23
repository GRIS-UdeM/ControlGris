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

#pragma once

#include "cg_ChangeGesturesManager.hpp"
#include "cg_PresetsManager.hpp"
#include "cg_Sources.hpp"
#include "cg_TrajectoryManager.hpp"

namespace gris
{
//==============================================================================
/** The main class that represents a plugin instance. */
class ControlGrisAudioProcessor final
    : public juce::AudioProcessor
    , public juce::AudioProcessorValueTreeState::Listener
    , public juce::Timer
    , private juce::OSCReceiver::Listener<juce::OSCReceiver::RealtimeCallback>
{
    //==============================================================================
    SpatMode mSpatMode{ DEFAULT_SPAT_MODE };
    bool mOscActivated{ DEFAULT_OSC_ACTIVE };
    bool mOscConnected{ true };
    bool mOscInputConnected{ false };
    bool mOscOutputConnected{ false };
    SourceId mFirstSourceId{ 1 };
    int mCurrentOscPort{ DEFAULT_OSC_PORT };
    juce::String mCurrentOscAddress{ DEFAULT_OSC_ADDRESS };
    int mLastConnectedOscPort{ -1 };
    int mCurrentOscInputPort{ DEFAULT_OSC_INPUT_PORT };
    int mCurrentOscOutputPort{ DEFAULT_OSC_OUTPUT_PORT };
    juce::String mCurrentOscOutputAddress{ DEFAULT_OSC_OUTPUT_ADDRESS };
    bool mNeedsInitialization{ true };

    double mInitTimeOnPlay{ 0.0 };
    double mCurrentTime{ 0.0 };
    double mLastTime{ 10000000.0 };
    double mLastTimerTime{ 10000000.0 };

    bool mIsPlaying{ false };
    bool mCanStopActivate{ false };
    double mBpm{ DEFAULT_BPM };

    juce::OSCSender mOscSender;
    juce::OSCSender mOscOutputSender;
    juce::OSCReceiver mOscInputReceiver;

    juce::XmlElement mFixPositionData{ FIXED_POSITION_DATA_TAG };

    Sources mSources{};
    SourceLinkEnforcer mPositionSourceLinkEnforcer{ mSources, PositionSourceLink::independent };
    SourceLinkEnforcer mElevationSourceLinkEnforcer{ mSources, ElevationSourceLink::independent };

    juce::AudioProcessorValueTreeState mAudioProcessorValueTreeState;

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
    //==============================================================================
    ControlGrisAudioProcessor(ControlGrisAudioProcessor const &) = delete;
    ControlGrisAudioProcessor(ControlGrisAudioProcessor &&) = delete;
    ControlGrisAudioProcessor & operator=(ControlGrisAudioProcessor const &) = delete;
    ControlGrisAudioProcessor & operator=(ControlGrisAudioProcessor &&) = delete;
    //==============================================================================
    void setSpatMode(SpatMode spatMode);
    [[nodiscard]] SpatMode getSpatMode() const noexcept { return mSpatMode; }
    void setOscPortNumber(int oscPortNumber);
    void setOscAddress(juce::String const & address);
    [[nodiscard]] int getOscPortNumber() const { return mCurrentOscPort; }
    [[nodiscard]] juce::String const & getOscAddress() const { return mCurrentOscAddress; }
    void setFirstSourceId(SourceId firstSourceId, bool propagate = true);
    [[nodiscard]] auto getFirstSourceId() const { return mFirstSourceId; }
    void setNumberOfSources(int numSources, bool sendOscMessageRightAway = true);
    [[nodiscard]] auto & getSources() { return mSources; }
    [[nodiscard]] auto const & getSources() const { return mSources; }
    //==============================================================================
    [[nodiscard]] auto const & getValueTreeState() const { return mAudioProcessorValueTreeState; }
    [[nodiscard]] auto & getValueTreeState() { return mAudioProcessorValueTreeState; }
    //==============================================================================
    [[nodiscard]] bool createOscConnection(juce::String const &, int oscPort);
    [[nodiscard]] bool disconnectOsc();
    [[nodiscard]] bool isOscConnected() const { return mOscConnected; }
    [[nodiscard]] bool isOscActive() const { return mOscActivated; }
    void setOscActive(bool state);
    void sendOscMessage();
    [[nodiscard]] bool createOscInputConnection(int oscPort);
    [[nodiscard]] bool disconnectOscInput(int oscPort);
    [[nodiscard]] bool getOscInputConnected() const { return mOscInputConnected; }
    void oscMessageReceived(const juce::OSCMessage & message) override;
    void oscBundleReceived(const juce::OSCBundle & bundle) override;
    [[nodiscard]] bool createOscOutputConnection(juce::String const & oscAddress, int oscPort);
    [[nodiscard]] bool disconnectOscOutput(juce::String const & oscAddress, int oscPort);
    [[nodiscard]] bool getOscOutputConnected() const { return mOscOutputConnected; }
    void sendOscOutputMessage();
    void setOscOutputPluginId(int pluginId);
    [[nodiscard]] int getOscOutputPluginId() const;
    //==============================================================================
    /** Creates an OSC connection and sets play start time. Usually called when playback is started or is about to
     * start. */
    void initialize();
    /** Loads values from the current AudioProcessorValueTreeState. */
    void setPluginState();
    /** Called when a value has been manually changed, either by the user or by an external OSC controller. */
    void sourcePositionChanged(SourceIndex sourceIndex, int whichField);
    void setSourceParameterValue(SourceIndex sourceIndex, SourceParameter sourceParameter, float value);
    [[nodiscard]] double getInitTimeOnPlay() const noexcept { return std::max(mInitTimeOnPlay, 0.0); }
    [[nodiscard]] double getCurrentTime() const noexcept { return std::max(mCurrentTime, 0.0); }
    [[nodiscard]] bool isPlaying() const noexcept { return mIsPlaying; }
    [[nodiscard]] double getBpm() const noexcept { return mBpm; }
    [[nodiscard]] ChangeGesturesManager & getChangeGestureManager() noexcept { return mChangeGesturesManager; }
    [[nodiscard]] PositionSourceLink getPositionSourceLink() const noexcept
    {
        return mPositionTrajectoryManager.getSourceLink();
    }
    void setPositionSourceLink(PositionSourceLink newSourceLink, SourceLinkEnforcer::OriginOfChange originOfChange);
    void setElevationSourceLink(ElevationSourceLink newSourceLink, SourceLinkEnforcer::OriginOfChange originOfChange);
    void setPositionTrajectoryType(PositionTrajectoryType trajectoryType);
    void setElevationTrajectoryType(ElevationTrajectoryType trajectoryType);
    [[nodiscard]] PresetsManager & getPresetsManager() noexcept { return mPresetManager; }
    [[nodiscard]] PresetsManager const & getPresetsManager() const noexcept { return mPresetManager; }
    void sourceChanged(Source & source, Source::ChangeType changeType, Source::OriginOfChange origin);
    void setSelectedSource(Source const & source);
    void updatePrimarySourceParameters(Source::ChangeType changeType);
    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;
    //==============================================================================
    /** This will get called regularly in order to send the OSC messages. */
    void timerCallback() override;
    //==============================================================================
    [[nodiscard]] juce::AudioProcessorEditor * createEditor() override;
    [[nodiscard]] bool hasEditor() const override { return true; }
    [[nodiscard]] juce::String const getName() const override { return JucePlugin_Name; }
    [[nodiscard]] bool acceptsMidi() const override { return false; }
    [[nodiscard]] bool producesMidi() const override { return false; }
    [[nodiscard]] bool isMidiEffect() const override { return false; }
    [[nodiscard]] double getTailLengthSeconds() const override { return 0.0; }
    /** NB: some hosts don't cope very well if you tell them there are 0 programs, so this should be at least 1, even if
     * you're not really implementing programs. */
    [[nodiscard]] int getNumPrograms() override { return 1; }
    [[nodiscard]] int getCurrentProgram() override { return 0; }
    void setCurrentProgram([[maybe_unused]] int index) override {}
    juce::String const getProgramName([[maybe_unused]] int index) override { return {}; }
    void changeProgramName([[maybe_unused]] int index, [[maybe_unused]] juce::String const & newName) override {}
    //==============================================================================
    void getStateInformation(juce::MemoryBlock & destData) override;
    void setStateInformation(const void * data, int sizeInBytes) override;
    //==============================================================================
    /** Called when a parameter in the AudioProcessorValueTreeState has changed */
    void parameterChanged(juce::String const & parameterId, float newValue) override;

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR(ControlGrisAudioProcessor)
};

} // namespace gris