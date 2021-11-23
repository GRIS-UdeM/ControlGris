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

#include <JuceHeader.h>

namespace gris
{
//==============================================================================
/**
 * A manager for change gestures that enables recursive locking.
 *
 * Because some hosts can crash when calling juce::AudioProcessorParameter::beginChangeGesture() or
 * juce::AudioProcessorParameter::endChangeGesture() twice, this class prevents such problems. A RAII-style scoped lock
 * is provided by the getScopedLock() method.
 */
class ChangeGesturesManager
{
public:
    //==============================================================================
    class ScopedLock
    {
        friend ChangeGesturesManager;
        //==============================================================================
        ChangeGesturesManager & mManager;
        juce::String mParameterName;

    public:
        //==============================================================================
        ScopedLock(ChangeGesturesManager & manager, juce::String const & parameterName);
        ScopedLock() = delete;
        ~ScopedLock();
        //==============================================================================
        ScopedLock(ScopedLock const &) = delete;
        ScopedLock(ScopedLock && other) noexcept;
        ScopedLock & operator=(ScopedLock const &) = delete;
        ScopedLock & operator=(ScopedLock &&) = delete;

    private:
        //==============================================================================
        JUCE_LEAK_DETECTOR(ScopedLock)

    }; // class ScopedLock

private:
    //==============================================================================
    juce::AudioProcessorValueTreeState & mAudioProcessorValueTreeState;
    juce::HashMap<juce::String, int> mGestureStates{};

public:
    //==============================================================================
    explicit ChangeGesturesManager(juce::AudioProcessorValueTreeState & audioProcessorValueTreeState);
    ChangeGesturesManager() = delete;
    ~ChangeGesturesManager() noexcept = default;
    //==============================================================================
    ChangeGesturesManager(ChangeGesturesManager const &) = delete;
    ChangeGesturesManager(ChangeGesturesManager &&) = delete;
    ChangeGesturesManager & operator=(ChangeGesturesManager const &) = delete;
    ChangeGesturesManager & operator=(ChangeGesturesManager &&) = delete;
    //==============================================================================
    void beginGesture(juce::String const & parameterName);
    void endGesture(juce::String const & parameterName);
    ScopedLock getScopedLock(juce::String const & parameterName);

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR(ChangeGesturesManager)

}; // class ChangeGesturesManager

} // namespace gris
