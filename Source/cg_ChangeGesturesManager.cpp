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

#include "cg_ChangeGesturesManager.hpp"

//==============================================================================
ChangeGesturesManager::ScopedLock::ScopedLock(ChangeGesturesManager & manager, juce::String const & parameterName)
    : mManager(manager)
    , mParameterName(parameterName)
{
    manager.beginGesture(parameterName);
}

//==============================================================================
ChangeGesturesManager::ScopedLock::~ScopedLock()
{
    if (!mParameterName.isEmpty()) { // might have been moved
        mManager.endGesture(mParameterName);
    }
}

//==============================================================================
ChangeGesturesManager::ScopedLock::ScopedLock(ChangeGesturesManager::ScopedLock && other) noexcept
    : mManager(other.mManager)
    , mParameterName(std::move(other.mParameterName))
{
    other.mParameterName.clear();
}

//==============================================================================
void ChangeGesturesManager::beginGesture(juce::String const & parameterName)
{
    if (!mGestureStates.contains(parameterName)) {
        mGestureStates.set(parameterName, 0);
    }

    if (mGestureStates.getReference(parameterName)++ == 0) {
        auto * parameter{ mAudioProcessorValueTreeState.getParameter(parameterName) };
        if (parameter != nullptr) {
            parameter->beginChangeGesture();
        } else {
            jassertfalse;
        }
    }
}

//==============================================================================
void ChangeGesturesManager::endGesture(juce::String const & parameterName)
{
    jassert(mGestureStates.contains(parameterName));

    if (--mGestureStates.getReference(parameterName) == 0) {
        auto * parameter{ mAudioProcessorValueTreeState.getParameter(parameterName) };
        if (parameter) {
            parameter->endChangeGesture();
        } else {
            jassertfalse;
        }
    }

    jassert(mGestureStates[parameterName] >= 0);
}

ChangeGesturesManager::ScopedLock ChangeGesturesManager::getScopedLock(const juce::String & parameterName)
{
    return ChangeGesturesManager::ScopedLock{ *this, parameterName };
}
