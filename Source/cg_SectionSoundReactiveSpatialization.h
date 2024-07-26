/**************************************************************************
 * Copyright 2024 UdeM - GRIS - Gaël LANE LÉPINE                          *
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
 * <https://www.gnu.org/licenses/>.                                       *
 *************************************************************************/

#pragma once

#include <JuceHeader.h>

#include "cg_ControlGrisLookAndFeel.hpp"
#include "cg_constants.hpp"

namespace gris
{
//==============================================================================
class SectionSoundReactiveSpatialization final : public juce::Component
{
public:
    //==============================================================================
    struct Listener {
        virtual ~Listener() = default;

        //virtual void futureVirtualFunctionsHere() = 0;
    };

private:
    //==============================================================================
    GrisLookAndFeel & mGrisLookAndFeel;

    juce::ListenerList<Listener> mListeners;

public:
    //==============================================================================
    explicit SectionSoundReactiveSpatialization(GrisLookAndFeel & grisLookAndFeel);
    //==============================================================================
    SectionSoundReactiveSpatialization() = delete;
    ~SectionSoundReactiveSpatialization() override = default;

    SectionSoundReactiveSpatialization(SectionSoundReactiveSpatialization const &) = delete;
    SectionSoundReactiveSpatialization(SectionSoundReactiveSpatialization &&) = delete;

    SectionSoundReactiveSpatialization & operator=(SectionSoundReactiveSpatialization const &) = delete;
    SectionSoundReactiveSpatialization & operator=(SectionSoundReactiveSpatialization &&) = delete;
    //==============================================================================

    //==============================================================================
    // overrides
    void paint(juce::Graphics & g) override;
    void resized() override;

    //==============================================================================
    void addListener(Listener * l) { mListeners.add(l); }
    void removeListener(Listener * l) { mListeners.remove(l); }

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR(SectionSoundReactiveSpatialization)
};

} // namespace gris