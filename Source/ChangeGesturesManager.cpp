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

#include "ChangeGesturesManager.h"

//==============================================================================
ChangeGesturesManager::ScopedLock::ScopedLock(ChangeGesturesManager & manager, String const & parameterName)
    : mManager(manager)
    , mParameterName(parameterName)
{
    manager.beginGesture(parameterName);
}

//==============================================================================
ChangeGesturesManager::ScopedLock::~ScopedLock()
{
    mManager.endGesture(mParameterName);
}

//==============================================================================
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

//==============================================================================
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
