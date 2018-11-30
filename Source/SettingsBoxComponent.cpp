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
#include "SettingsBoxComponent.h"

SettingsBoxComponent::SettingsBoxComponent() {
    setLookAndFeel(&mGrisFeel);

    oscFormatLabel.setText("OSC Format:", NotificationType::dontSendNotification);
    addAndMakeVisible(&oscFormatLabel);

    oscFormatCombo.addItem("VBAP - ServerGris", 1);
    oscFormatCombo.addItem("LBAP - ServerGris", 2);
    oscFormatCombo.onChange = [this] {
            listeners.call([&] (Listener& l) { l.oscFormatChanged(oscFormatCombo.getSelectedId()); });
        };
    oscFormatCombo.setSelectedId(1);
    addAndMakeVisible(&oscFormatCombo);

    oscPortLabel.setText("OSC Port:", NotificationType::dontSendNotification);
    addAndMakeVisible(&oscPortLabel);

    addAndMakeVisible(&oscPortCombo);

    numOfSourcesLabel.setText("Number of Sources:", NotificationType::dontSendNotification);
    addAndMakeVisible(&numOfSourcesLabel);

    numOfSourcesEditor.setText("2");
    numOfSourcesEditor.setSelectAllWhenFocused(true);
    numOfSourcesEditor.setInputRestrictions(1, "12345678");
    numOfSourcesEditor.onReturnKey = [this] {
            if (! numOfSourcesEditor.isEmpty()) {
                listeners.call([&] (Listener& l) { l.numberOfSourcesChanged(numOfSourcesEditor.getText().getIntValue()); });
            } else {
                listeners.call([&] (Listener& l) { l.numberOfSourcesChanged(1); numOfSourcesEditor.setText("1"); });
            }
            numOfSourcesEditor.moveKeyboardFocusToSibling(true);
        };
    addAndMakeVisible(&numOfSourcesEditor);

    firstSourceIdLabel.setText("First Source ID:", NotificationType::dontSendNotification);
    addAndMakeVisible(&firstSourceIdLabel);

    firstSourceIdEditor.setText("1");
    firstSourceIdEditor.setSelectAllWhenFocused(true);
    firstSourceIdEditor.setInputRestrictions(2, "0123456789");
    firstSourceIdEditor.onReturnKey = [this] {
            if (! firstSourceIdEditor.isEmpty()) {
                listeners.call([&] (Listener& l) { l.firstSourceIdChanged(firstSourceIdEditor.getText().getIntValue()); });
            } else {
                listeners.call([&] (Listener& l) { l.firstSourceIdChanged(1); firstSourceIdEditor.setText("1"); });
            }
            firstSourceIdEditor.moveKeyboardFocusToSibling(true);
        };
    addAndMakeVisible(&firstSourceIdEditor);
}

void SettingsBoxComponent::setNumberOfSources(int numOfSources) {
    numOfSourcesEditor.setText(String(numOfSources));
}

void SettingsBoxComponent::setFirstSourceId(int firstSourceId) {
    firstSourceIdEditor.setText(String(firstSourceId));
}

void SettingsBoxComponent::resized() {
    oscFormatLabel.setBounds(5, 10, 90, 15);
    oscFormatCombo.setBounds(95, 10, 150, 20);

    oscPortLabel.setBounds(5, 40, 90, 15);
    oscPortCombo.setBounds(95, 40, 150, 20);

    numOfSourcesLabel.setBounds(265, 10, 130, 15);
    numOfSourcesEditor.setBounds(395, 10, 40, 15);

    firstSourceIdLabel.setBounds(265, 40, 130, 15);
    firstSourceIdEditor.setBounds(395, 40, 40, 15);
}
