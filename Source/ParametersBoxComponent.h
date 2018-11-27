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

// This file defines the classes used to construct the Parameters panel.
// The Parameters panel allow the user to control the coordinates of 
// the currently selected source.
// Classes:
//   ParameterComponent : The base class for a parameter. It contains the
//                        label, the link button and the slider.
//   ParametersBoxComponent : This is the Parameters panel that is shown
//                            in the interface. It is the bridge for the
//                            communication between the plugin and the 
//                            parameters.
 
//-------------------------------------------------------------------
class ParameterComponent : public Component,
                           public Button::Listener,
                           public Slider::Listener
{
public:
    ParameterComponent(int ParameterId, String label, Component *parent);
    ~ParameterComponent();

    void buttonClicked(Button *button) override;
    void sliderValueChanged(Slider *slider) override;
    void paint(Graphics&) override;
    void resized() override;

    void setValue(double value);
    void setLinkState(bool state);
    bool getLinkState();

    struct Listener
    {
        virtual ~Listener() {}

        virtual void parameterChanged(int parameterId, double value) = 0;
        virtual void parameterLinkChanged(int parameterId, bool value) = 0;
    };

    void addListener(Listener* l) { listeners.add (l); }
    void removeListener(Listener* l) { listeners.remove (l); }

private:
    ListenerList<Listener> listeners;

    int m_parameterId;

    Label           parameterLabel;
    ToggleButton    linkButton;
    Slider          slider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterComponent)
};

//-----------------------------------------------------------------
class ParametersBoxComponent : public Component,
                               public Button::Listener,
                               public ParameterComponent::Listener
{
public:
    ParametersBoxComponent();
    ~ParametersBoxComponent();

    void buttonClicked(Button *button) override;
    void parameterChanged(int parameterId, double value) override;
    void parameterLinkChanged(int parameterId, bool value) override;
    void paint(Graphics&) override;
    void resized() override;

    void setSelectedSource(Source *source);
    void setDistanceEnabled(bool shouldBeEnabled);
    void setLinkState(int parameterId, bool state);
    bool getLinkState(int parameterId);

    struct Listener
    {
        virtual ~Listener() {}

        virtual void parameterChanged(int parameterId, double value) = 0;
        virtual void parameterLinkChanged(int parameterId, bool value) = 0;
    };

    void addListener(Listener* l) { listeners.add (l); }
    void removeListener(Listener* l) { listeners.remove (l); }

private:
    ListenerList<Listener> listeners;

    GrisLookAndFeel mGrisFeel;

    bool m_distanceEnabled;

    Source *selectedSource;

    ParameterComponent p_azimuth;
    ParameterComponent p_elevation;
    ParameterComponent p_distance;
    ParameterComponent p_x;
    ParameterComponent p_y;
    ParameterComponent p_azimuthSpan;
    ParameterComponent p_elevationSpan;

    ToggleButton activatorXY;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParametersBoxComponent)
};
