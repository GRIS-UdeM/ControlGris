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

//==============================================================================
class ChangeGesturesManager
{
public:
    //==============================================================================
    class ScopedLock
    {
        friend ChangeGesturesManager;
        //==============================================================================
        ChangeGesturesManager & mManager;
        String mParameterName;

    public:
        //==============================================================================
        ScopedLock() = delete;
        ~ScopedLock();

        ScopedLock(ScopedLock const &) = delete;
        ScopedLock(ScopedLock && other) noexcept;

        ScopedLock & operator=(ScopedLock const &) = delete;
        ScopedLock & operator=(ScopedLock &&) = delete;
        //==============================================================================
        ScopedLock(ChangeGesturesManager & manager, String const & parameterName);

    private:
        //==============================================================================
        JUCE_LEAK_DETECTOR(ScopedLock);

    }; // class ScopedLock

private:
    //==============================================================================
    AudioProcessorValueTreeState & mAudioProcessorValueTreeState;
    HashMap<String, bool> mGestureStates{};

public:
    //==============================================================================
    ChangeGesturesManager() = delete;
    ~ChangeGesturesManager() noexcept = default;

    ChangeGesturesManager(ChangeGesturesManager const &) = delete;
    ChangeGesturesManager(ChangeGesturesManager &&) = delete;

    ChangeGesturesManager & operator=(ChangeGesturesManager const &) = delete;
    ChangeGesturesManager & operator=(ChangeGesturesManager &&) = delete;
    //==============================================================================
    explicit ChangeGesturesManager(AudioProcessorValueTreeState & audioProcessorValueTreeState)
        : mAudioProcessorValueTreeState(audioProcessorValueTreeState)
    {
    }
    //==============================================================================

    void beginGesture(String const & parameterName);
    void endGesture(String const & parameterName);
    ScopedLock getScopedLock(String const & parameterName);

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR(ChangeGesturesManager);

}; // class ChangeGesturesManager