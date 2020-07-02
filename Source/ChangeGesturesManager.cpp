/*
  ==============================================================================

    ChangeGesturesManager.cpp
    Created: 2 Jul 2020 10:11:57am
    Author:  samuel

  ==============================================================================
*/

#include "ChangeGesturesManager.h"

ChangeGesturesManager::ScopedLock::ScopedLock(ChangeGesturesManager & manager, String const & parameterName)
    : mManager(manager)
    , mParameterName(parameterName)
{
    manager.beginGesture(parameterName);
}

ChangeGesturesManager::ScopedLock::~ScopedLock()
{
    mManager.endGesture(mParameterName);
}

void ChangeGesturesManager::beginGesture(String const & parameterName)
{
    bool shouldBeginGesture{ false };

    if (!mGestureStates.contains(parameterName)) {
        shouldBeginGesture = true;
    } else if (!mGestureStates[parameterName]) {
        shouldBeginGesture = true;
    }

    if (shouldBeginGesture) {
        auto * parameter{ mAudioProcessorValueTreeState.getParameter(parameterName) };
        if (parameter != nullptr) {
            parameter->beginChangeGesture();
            mGestureStates.set(parameterName, true);
        }
    }
}

void ChangeGesturesManager::endGesture(String const & parameterName)
{
    bool shouldEndGesture{ false };

    if (mGestureStates.contains(parameterName)) {
        if (mGestureStates[parameterName]) {
            shouldEndGesture = true;
        }
    }

    if (shouldEndGesture) {
        auto * parameter{ mAudioProcessorValueTreeState.getParameter(parameterName) };
        if (parameter != nullptr) {
            parameter->endChangeGesture();
            mGestureStates.set(parameterName, false);
        }
    }
}
