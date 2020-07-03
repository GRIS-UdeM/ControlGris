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