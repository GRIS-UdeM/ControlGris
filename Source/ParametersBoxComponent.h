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
#include "Source.h"

class ParametersBoxComponent final
    : public Component
    , public Slider::Listener
{
public:
    ParametersBoxComponent();
    ~ParametersBoxComponent() final;

    void mouseDown(const MouseEvent & event) final;
    void sliderValueChanged(Slider * slider) final;
    void paint(Graphics &) final;
    void resized() final;

    void setSelectedSource(Source * source);
    void setDistanceEnabled(bool shouldBeEnabled);
    void setSpanLinkState(bool state);
    bool getSpanLinkState() const { return m_spanLinked; }

    struct Listener {
        virtual ~Listener() {}

        virtual void parametersBoxSelectedSourceClicked() = 0;
        virtual void parametersBoxParameterChanged(SourceParameter sourceId, double value) = 0;
    };

    void addListener(Listener * l) { listeners.add(l); }
    void removeListener(Listener * l) { listeners.remove(l); }

private:
    ListenerList<Listener> listeners;

    bool m_distanceEnabled;
    bool m_spanLinked;

    Source * selectedSource;

    Label azimuthLabel;
    Label elevationLabel;
    Slider azimuthSpan;
    Slider elevationSpan;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParametersBoxComponent)
};
