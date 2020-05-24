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

InterfaceBoxComponent::InterfaceBoxComponent()
{
    /*
        oscSourceLabel.setText("OSC Source:", NotificationType::dontSendNotification);
        addAndMakeVisible(&oscSourceLabel);

        addAndMakeVisible(&oscSourceCombo);

        enableJoystickToggle.setButtonText("Enable Joystick");
        addAndMakeVisible(&enableJoystickToggle);
    */

    oscOutputPluginIdLabel.setText("OSC output plugin ID:", NotificationType::dontSendNotification);
    addAndMakeVisible(&oscOutputPluginIdLabel);

    oscOutputPluginIdEditor.setText(String(1));
    oscOutputPluginIdEditor.setInputRestrictions(3, "0123456789");
    oscOutputPluginIdEditor.addListener(this);
    oscOutputPluginIdEditor.onReturnKey = [this] { this->grabKeyboardFocus(); };
    oscOutputPluginIdEditor.onFocusLost = [this] {
        if (!oscOutputPluginIdEditor.isEmpty()) {
            listeners.call(
                [&](Listener & l) { l.oscOutputPluginIdChanged(oscOutputPluginIdEditor.getText().getIntValue()); });
        } else {
            listeners.call([&](Listener & l) {
                l.oscOutputPluginIdChanged(1);
                oscOutputPluginIdEditor.setText(String(1));
            });
        }
    };

    addAndMakeVisible(&oscOutputPluginIdEditor);

    oscReceiveToggle.setButtonText("Receive on port");
    oscReceiveToggle.setExplicitFocusOrder(1);
    addAndMakeVisible(&oscReceiveToggle);
    oscReceiveToggle.onClick = [this] {
        listeners.call([&](Listener & l) {
            l.oscInputConnectionChanged(oscReceiveToggle.getToggleState(),
                                        oscReceivePortEditor.getText().getIntValue());
        });
    };

    oscSendToggle.setButtonText("Send on port : IP");
    addAndMakeVisible(&oscSendToggle);
    oscSendToggle.onClick = [this] {
        listeners.call([&](Listener & l) {
            l.oscOutputConnectionChanged(oscSendToggle.getToggleState(),
                                         oscSendIpEditor.getText(),
                                         oscSendPortEditor.getText().getIntValue());
        });
    };

    oscReceiveIpEditor.setText(IPAddress::getLocalAddress().toString());
    oscReceiveIpEditor.setReadOnly(true);
    addAndMakeVisible(&oscReceiveIpEditor);

    lastOscReceivePort = 9000;
    oscReceivePortEditor.setText(String(lastOscReceivePort));
    oscReceivePortEditor.setInputRestrictions(5, "0123456789");
    oscReceivePortEditor.addListener(this);
    oscReceivePortEditor.onReturnKey = [this] { this->grabKeyboardFocus(); };
    oscReceivePortEditor.onFocusLost = [this] {
        if (!oscReceivePortEditor.isEmpty()) {
            listeners.call([&](Listener & l) {
                l.oscInputConnectionChanged(oscReceiveToggle.getToggleState(),
                                            oscReceivePortEditor.getText().getIntValue());
            });
        } else {
            listeners.call([&](Listener & l) {
                l.oscInputConnectionChanged(oscReceiveToggle.getToggleState(), lastOscReceivePort);
                oscReceivePortEditor.setText(String(lastOscReceivePort));
            });
        }
    };

    addAndMakeVisible(&oscReceivePortEditor);

    lastOscSendAddress = String("192.168.1.100");
    oscSendIpEditor.setText(lastOscSendAddress);
    oscSendIpEditor.setInputRestrictions(16, ".0123456789");
    oscSendIpEditor.addListener(this);
    oscSendIpEditor.onReturnKey = [this] { this->grabKeyboardFocus(); };
    oscSendIpEditor.onFocusLost = [this] {
        if (!oscSendIpEditor.isEmpty()) {
            listeners.call([&](Listener & l) {
                l.oscOutputConnectionChanged(oscSendToggle.getToggleState(),
                                             oscSendIpEditor.getText(),
                                             oscSendPortEditor.getText().getIntValue());
            });
        } else {
            listeners.call([&](Listener & l) {
                l.oscOutputConnectionChanged(oscSendToggle.getToggleState(),
                                             lastOscSendAddress,
                                             oscSendPortEditor.getText().getIntValue());
                oscSendIpEditor.setText(String(lastOscSendAddress));
            });
        }
    };

    addAndMakeVisible(&oscSendIpEditor);

    lastOscSendPort = 8000;
    oscSendPortEditor.setText(String(lastOscSendPort));
    oscSendPortEditor.setInputRestrictions(5, "0123456789");
    oscSendPortEditor.addListener(this);
    oscSendPortEditor.onReturnKey = [this] { this->grabKeyboardFocus(); };
    oscSendPortEditor.onFocusLost = [this] {
        if (!oscSendPortEditor.isEmpty()) {
            listeners.call([&](Listener & l) {
                l.oscOutputConnectionChanged(oscSendToggle.getToggleState(),
                                             oscSendIpEditor.getText(),
                                             oscSendPortEditor.getText().getIntValue());
            });
        } else {
            listeners.call([&](Listener & l) {
                l.oscOutputConnectionChanged(oscSendToggle.getToggleState(),
                                             oscSendIpEditor.getText(),
                                             lastOscSendPort);
                oscSendPortEditor.setText(String(lastOscSendPort));
            });
        }
    };

    addAndMakeVisible(&oscSendPortEditor);
}

InterfaceBoxComponent::~InterfaceBoxComponent()
{
    setLookAndFeel(nullptr);
}

void InterfaceBoxComponent::textEditorReturnKeyPressed(TextEditor & editor)
{
    unfocusAllComponents();
}

//==============================================================================
void InterfaceBoxComponent::setOscOutputPluginId(int id)
{
    oscOutputPluginIdEditor.setText(String(id));
}

//==============================================================================
void InterfaceBoxComponent::setOscReceiveToggleState(bool state)
{
    oscReceiveToggle.setToggleState(state, NotificationType::dontSendNotification);
}

void InterfaceBoxComponent::setOscReceiveInputPort(int port)
{
    lastOscReceivePort = port;
    oscReceivePortEditor.setText(String(port));
}

//==============================================================================
void InterfaceBoxComponent::setOscSendToggleState(bool state)
{
    oscSendToggle.setToggleState(state, NotificationType::dontSendNotification);
}

void InterfaceBoxComponent::setOscSendOutputPort(int port)
{
    lastOscSendPort = port;
    oscSendPortEditor.setText(String(port));
}

void InterfaceBoxComponent::setOscSendOutputAddress(String address)
{
    lastOscSendAddress = address;
    oscSendIpEditor.setText(address);
}

//==============================================================================
void InterfaceBoxComponent::paint(Graphics & g)
{
    GrisLookAndFeel * lookAndFeel;
    lookAndFeel = static_cast<GrisLookAndFeel *>(&getLookAndFeel());
    g.fillAll(lookAndFeel->findColour(ResizableWindow::backgroundColourId));
}

void InterfaceBoxComponent::resized()
{
    /*
        oscSourceLabel.setBounds(5, 10, 90, 20);
        oscSourceCombo.setBounds(95, 10, 150, 20);

        enableJoystickToggle.setBounds(5, 35, 120, 20);
    */

    oscOutputPluginIdLabel.setBounds(5, 10, 135, 20);
    oscOutputPluginIdEditor.setBounds(140, 10, 70, 20);

    oscReceiveToggle.setBounds(255, 10, 200, 20);
    oscReceivePortEditor.setBounds(400, 10, 60, 20);
    oscReceiveIpEditor.setBounds(470, 10, 120, 20);

    oscSendToggle.setBounds(255, 35, 200, 20);
    oscSendPortEditor.setBounds(400, 35, 60, 20);
    oscSendIpEditor.setBounds(470, 35, 120, 20);
}
