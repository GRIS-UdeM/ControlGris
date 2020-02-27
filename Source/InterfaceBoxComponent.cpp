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
#include "InterfaceBoxComponent.h"

InterfaceBoxComponent::InterfaceBoxComponent() {
    oscSourceLabel.setText("OSC Source:", NotificationType::dontSendNotification);
    addAndMakeVisible(&oscSourceLabel);

    addAndMakeVisible(&oscSourceCombo);

    enableLeapToggle.setButtonText("Enable Leap");
    addAndMakeVisible(&enableLeapToggle);

    enableJoystickToggle.setButtonText("Enable Joystick");
    addAndMakeVisible(&enableJoystickToggle);

    oscReceiveToggle.setButtonText("Receive on port");
    addAndMakeVisible(&oscReceiveToggle);
    oscReceiveToggle.onClick = [this] {
            listeners.call([&] (Listener& l) { l.oscInputConnectionChanged(oscReceiveToggle.getToggleState(), oscReceivePortEditor.getText().getIntValue()); }); };

    oscSendToggle.setButtonText("Send on port : IP");
    addAndMakeVisible(&oscSendToggle);
    oscSendToggle.onClick = [this] {
            listeners.call([&] (Listener& l) { l.oscOutputConnectionChanged(oscSendToggle.getToggleState(), oscSendIpEditor.getText(),
                                                                            oscSendPortEditor.getText().getIntValue()); });
        };

    oscReceiveIpEditor.setText(IPAddress::getLocalAddress().toString());
    oscReceiveIpEditor.setReadOnly(true);
    addAndMakeVisible(&oscReceiveIpEditor);

    lastOscReceivePort = 8000;
    oscReceivePortEditor.setText(String(lastOscReceivePort));
    oscReceivePortEditor.setInputRestrictions(5, "0123456789");
    oscReceivePortEditor.addListener(this);
    oscReceivePortEditor.onReturnKey = [this] {
            oscSourceCombo.grabKeyboardFocus();
        };
    oscReceivePortEditor.onFocusLost = [this] {
            if (! oscReceivePortEditor.isEmpty()) {
                listeners.call([&] (Listener& l) { l.oscInputConnectionChanged(oscReceiveToggle.getToggleState(), oscReceivePortEditor.getText().getIntValue()); });
            } else {
                listeners.call([&] (Listener& l) { l.oscInputConnectionChanged(oscReceiveToggle.getToggleState(), lastOscReceivePort);
                                                   oscReceivePortEditor.setText(String(lastOscReceivePort)); });
            }
        };

    addAndMakeVisible(&oscReceivePortEditor);

    lastOscSendAddress = String("192.168.1.100");
    oscSendIpEditor.setText(lastOscSendAddress);
    oscSendIpEditor.setInputRestrictions(16, ".0123456789");
    oscSendIpEditor.addListener(this);
    oscSendIpEditor.onReturnKey = [this] {
            oscSourceCombo.grabKeyboardFocus();
        };
    oscSendIpEditor.onFocusLost = [this] {
            if (! oscSendIpEditor.isEmpty()) {
                listeners.call([&] (Listener& l) { l.oscOutputConnectionChanged(oscSendToggle.getToggleState(), oscSendIpEditor.getText(),
                                                                                oscSendPortEditor.getText().getIntValue()); });
            } else {
                listeners.call([&] (Listener& l) { l.oscOutputConnectionChanged(oscSendToggle.getToggleState(), lastOscSendAddress,
                                                                                oscSendPortEditor.getText().getIntValue());
                                                   oscSendIpEditor.setText(String(lastOscSendAddress)); });
            }
        };

    addAndMakeVisible(&oscSendIpEditor);

    lastOscSendPort = 9000;
    oscSendPortEditor.setText(String(lastOscSendPort));
    oscSendPortEditor.setInputRestrictions(5, "0123456789");
    oscSendPortEditor.addListener(this);
    oscSendPortEditor.onReturnKey = [this] {
            oscSourceCombo.grabKeyboardFocus();
        };
    oscSendPortEditor.onFocusLost = [this] {
            if (! oscSendPortEditor.isEmpty()) {
                listeners.call([&] (Listener& l) { l.oscOutputConnectionChanged(oscSendToggle.getToggleState(), oscSendIpEditor.getText(),
                                                                                oscSendPortEditor.getText().getIntValue()); });
            } else {
                listeners.call([&] (Listener& l) { l.oscOutputConnectionChanged(oscSendToggle.getToggleState(), oscSendIpEditor.getText(), lastOscSendPort);
                                                   oscSendPortEditor.setText(String(lastOscSendPort)); });
            }
        };

    addAndMakeVisible(&oscSendPortEditor);
}

InterfaceBoxComponent::~InterfaceBoxComponent() {
    setLookAndFeel(nullptr);
}

void InterfaceBoxComponent::textEditorReturnKeyPressed(TextEditor &editor) {
    unfocusAllComponents();
}

//==============================================================================
void InterfaceBoxComponent::setOscReceiveToggleState(bool state) {
    oscReceiveToggle.setToggleState(state, NotificationType::dontSendNotification);
}

void InterfaceBoxComponent::setOscReceiveInputPort(int port) {
    lastOscReceivePort = port;
    oscReceivePortEditor.setText(String(port));
}

//==============================================================================
void InterfaceBoxComponent::setOscSendToggleState(bool state) {
    oscSendToggle.setToggleState(state, NotificationType::dontSendNotification);
}

void InterfaceBoxComponent::setOscSendOutputPort(int port) {
    lastOscSendPort = port;
    oscSendPortEditor.setText(String(port));
}

void InterfaceBoxComponent::setOscSendOutputAddress(String address) {
    lastOscSendAddress = address;
    oscSendIpEditor.setText(address);
}

//==============================================================================
void InterfaceBoxComponent::paint(Graphics& g) {
    GrisLookAndFeel *lookAndFeel;
    lookAndFeel = static_cast<GrisLookAndFeel *> (&getLookAndFeel());
    g.fillAll (lookAndFeel->findColour (ResizableWindow::backgroundColourId));
}

void InterfaceBoxComponent::resized() {
    oscSourceLabel.setBounds(5, 10, 90, 20);
    oscSourceCombo.setBounds(95, 10, 150, 20);

    enableLeapToggle.setBounds(5, 35, 120, 20);
    enableJoystickToggle.setBounds(125, 35, 150, 20);

    oscReceiveToggle.setBounds(255, 10, 200, 20);
    oscReceivePortEditor.setBounds(400, 10, 60, 20);
    oscReceiveIpEditor.setBounds(470, 10, 120, 20);

    oscSendToggle.setBounds(255, 35, 200, 20);
    oscSendPortEditor.setBounds(400, 35, 60, 20);
    oscSendIpEditor.setBounds(470, 35, 120, 20);
}
