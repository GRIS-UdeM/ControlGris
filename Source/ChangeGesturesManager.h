/*
  ==============================================================================

    ChangeGesturesManager.h
    Created: 2 Jul 2020 10:11:57am
    Author:  samuel

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class ChangeGesturesManager
{
public:
    class ScopedLock
    {
    private:
        friend ChangeGesturesManager;

        ChangeGesturesManager & mManager;
        String mParameterName;

    public:
        ScopedLock(ChangeGesturesManager & manager, String const & parameterName);
        ~ScopedLock();

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScopedLock);
    };

private:
    AudioProcessorValueTreeState & mAudioProcessorValueTreeState;
    HashMap<String, bool> mGestureStates{};

public:
    ChangeGesturesManager(AudioProcessorValueTreeState & audioProcessorValueTreeState)
        : mAudioProcessorValueTreeState(audioProcessorValueTreeState)
    {
    }
    ~ChangeGesturesManager() noexcept = default;

    void beginGesture(String const & parameterName);
    void endGesture(String const & parameterName);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChangeGesturesManager);
};