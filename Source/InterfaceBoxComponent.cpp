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

    oscReceiveToggle.setButtonText("Receive on IP : port");
    addAndMakeVisible(&oscReceiveToggle);

    oscSendToggle.setButtonText("Send on IP : port");
    addAndMakeVisible(&oscSendToggle);

    oscReceiveIpEditor.setText("192.168.1.2");
    oscReceiveIpEditor.setInputRestrictions(16, ".0123456789");
    oscReceiveIpEditor.addListener(this);
    addAndMakeVisible(&oscReceiveIpEditor);

    oscReceivePortEditor.setText("8000");
    oscReceivePortEditor.setInputRestrictions(5, "0123456789");
    oscReceivePortEditor.addListener(this);
    addAndMakeVisible(&oscReceivePortEditor);

    oscSendIpEditor.setText("192.168.1.100");
    oscSendIpEditor.setInputRestrictions(16, ".0123456789");
    oscSendIpEditor.addListener(this);
    addAndMakeVisible(&oscSendIpEditor);

    oscSendPortEditor.setText("9000");
    oscSendPortEditor.setInputRestrictions(5, "0123456789");
    oscSendPortEditor.addListener(this);
    addAndMakeVisible(&oscSendPortEditor);
}

InterfaceBoxComponent::~InterfaceBoxComponent() {
    setLookAndFeel(nullptr);
}

void InterfaceBoxComponent::textEditorReturnKeyPressed(TextEditor &editor) {
    unfocusAllComponents();
}

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
    oscReceiveIpEditor.setBounds(400, 10, 120, 20);
    oscReceivePortEditor.setBounds(530, 10, 60, 20);

    oscSendToggle.setBounds(255, 35, 200, 20);
    oscSendIpEditor.setBounds(400, 35, 120, 20);
    oscSendPortEditor.setBounds(530, 35, 60, 20);
}
