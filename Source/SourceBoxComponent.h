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
#include "GrisLookAndFeel.h"
#include "ControlGrisConstants.h"
#include "Source.h"

class SourceBoxComponent : public Component
{
public:
    SourceBoxComponent();
    ~SourceBoxComponent();

    void paint(Graphics&) override;
    void resized() override;

    void setNumberOfSources(int numOfSources, int firstSourceId);
    void updateSelectedSource(Source *source, int sourceIndex, SpatMode spatMode);

    struct Listener
    {
        virtual ~Listener() {}

        virtual void sourceBoxPlacementChanged(SourcePlacement value) = 0;
        virtual void sourceBoxSelectionChanged(int sourceNum) = 0;
        virtual void sourceBoxPositionChanged(int sourceNum, float angle, float rayLen) = 0;
    };

    void addListener(Listener* l) { listeners.add (l); }
    void removeListener(Listener* l) { listeners.remove (l); }

private:
    ListenerList<Listener> listeners;

    int             selectedSourceNumber;
    float           currentAngle;
    float           currentRayLength;

    Label           sourcePlacementLabel;
    ComboBox        sourcePlacementCombo;

    Label           sourceNumberLabel;
    ComboBox        sourceNumberCombo;

    Label           rayLengthLabel;
    Slider          rayLengthSlider;

    Label           angleLabel;
    Slider          angleSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SourceBoxComponent)
};
