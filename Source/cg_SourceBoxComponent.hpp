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

#include "cg_ControlGrisLookAndFeel.hpp"
#include "cg_Source.hpp"
#include "cg_constants.hpp"

//==============================================================================
class SourceBoxComponent final : public juce::Component
{
public:
    //==============================================================================
    struct Listener {
        virtual ~Listener() {}

        virtual void sourceBoxPlacementChanged(SourcePlacement value) = 0;
        virtual void sourceBoxSelectionChanged(SourceIndex sourceIndex) = 0;
        virtual void sourceBoxPositionChanged(SourceIndex sourceIndex, Radians angle, float rayLen) = 0;
    };

private:
    //==============================================================================
    GrisLookAndFeel & mGrisLookAndFeel;

    ListenerList<Listener> mListeners;

    SourceIndex mSelectedSource;
    Degrees mCurrentAngle;
    float mCurrentRayLength;

    Label mSourcePlacementLabel;
    ComboBox mSourcePlacementCombo;

    Label mSourceNumberLabel;
    ComboBox mSourceNumberCombo;

    Label mRayLengthLabel;
    Slider mRayLengthSlider;

    Label mAngleLabel;
    Slider mAngleSlider;

public:
    //==============================================================================
    explicit SourceBoxComponent(GrisLookAndFeel & grisLookAndFeel);
    //==============================================================================
    SourceBoxComponent() = delete;
    ~SourceBoxComponent() override = default;

    SourceBoxComponent(SourceBoxComponent const &) = delete;
    SourceBoxComponent(SourceBoxComponent &&) = delete;

    SourceBoxComponent & operator=(SourceBoxComponent const &) = delete;
    SourceBoxComponent & operator=(SourceBoxComponent &&) = delete;
    //==============================================================================
    void paint(Graphics &) override;
    void resized() override;

    void setNumberOfSources(int numOfSources, SourceId firstSourceId);
    void updateSelectedSource(Source * source, SourceIndex sourceIndex, SpatMode spatMode);

    void addListener(Listener * l) { mListeners.add(l); }
    void removeListener(Listener * l) { mListeners.remove(l); }

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR(SourceBoxComponent)
};
