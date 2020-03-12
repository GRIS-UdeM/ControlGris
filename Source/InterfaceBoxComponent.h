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

class InterfaceBoxComponent : public Component,
                              public TextEditor::Listener
{
public:
    InterfaceBoxComponent();
    ~InterfaceBoxComponent();

    void textEditorReturnKeyPressed(TextEditor &editor) override;
    void paint(Graphics&) override;
    void resized() override;

    void setOscReceiveToggleState(bool state);
    void setOscReceiveInputPort(int port);

    void setOscSendToggleState(bool state);
    void setOscSendOutputAddress(String address);
    void setOscSendOutputPort(int port);

    struct Listener
    {
        virtual ~Listener() {}

        virtual void oscInputConnectionChanged(bool state, int oscPort) = 0;
        virtual void oscOutputConnectionChanged(bool state, String oscAddress, int oscPort) = 0;
    };

    void addListener(Listener* l) { listeners.add (l); }
    void removeListener(Listener* l) { listeners.remove (l); }

private:
    ListenerList<Listener> listeners;

/*
    Label           oscSourceLabel;
    ComboBox        oscSourceCombo;

    ToggleButton    enableJoystickToggle;
*/

    ToggleButton    oscReceiveToggle;
    ToggleButton    oscSendToggle;

    TextEditor      oscReceiveIpEditor;
    TextEditor      oscReceivePortEditor;
    TextEditor      oscSendIpEditor;
    TextEditor      oscSendPortEditor;

    int lastOscReceivePort;
    int lastOscSendPort;
    String lastOscSendAddress;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InterfaceBoxComponent)
};
