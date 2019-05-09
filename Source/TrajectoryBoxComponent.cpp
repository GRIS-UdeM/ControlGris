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
#include "TrajectoryBoxComponent.h"
#include "ControlGrisConstants.h"

TrajectoryBoxComponent::TrajectoryBoxComponent() {
    sourceLinkLabel.setText("Source Link:", NotificationType::dontSendNotification);
    addAndMakeVisible(&sourceLinkLabel);

    sourceLinkCombo.addItemList(SOURCE_LINK_TYPES, 1);
    sourceLinkCombo.setSelectedId(1);
    addAndMakeVisible(&sourceLinkCombo);

    trajectoryTypeLabel.setText("Trajectory Type:", NotificationType::dontSendNotification);
    addAndMakeVisible(&trajectoryTypeLabel);

    trajectoryTypeCombo.addItemList(TRAJECTORY_TYPES, 1);
    trajectoryTypeCombo.setSelectedId(1);
    addAndMakeVisible(&trajectoryTypeCombo);

    durationLabel.setText("Duration:", NotificationType::dontSendNotification);
    addAndMakeVisible(&durationLabel);

    addAndMakeVisible(&durationEditor);
    durationEditor.setTextToShowWhenEmpty ("1", Colours::white);
    durationEditor.setText("5", false);
    durationEditor.setInputRestrictions (10, "0123456789.");
    durationEditor.onReturnKey = [this] { durationUnitCombo.grabKeyboardFocus(); };

    addAndMakeVisible(&durationUnitCombo);
    durationUnitCombo.addItem("Second(s)", 1);
    durationUnitCombo.addItem("Beat(s)", 2);
    durationUnitCombo.setSelectedId(1);

    cycleSpeedLabel.setText("Cycle Speed:", NotificationType::dontSendNotification);
    addAndMakeVisible(&cycleSpeedLabel);

    cycleSpeedSlider.setNormalisableRange(NormalisableRange<double>(-2.0f, 2.0f, 0.01f));
    cycleSpeedSlider.setValue(1.0, NotificationType::sendNotificationAsync);
    cycleSpeedSlider.setTextBoxStyle(Slider::TextBoxRight, false, 40, 20);
    cycleSpeedSlider.setColour(Slider:: textBoxOutlineColourId, Colours::transparentBlack);
    addAndMakeVisible(&cycleSpeedSlider);

    activateButton.setButtonText("Activate");
    activateButton.setClickingTogglesState(true);
    addAndMakeVisible(&activateButton);
}

TrajectoryBoxComponent::~TrajectoryBoxComponent() {}

void TrajectoryBoxComponent::paint(Graphics& g) {
    GrisLookAndFeel *lookAndFeel;
    lookAndFeel = static_cast<GrisLookAndFeel *> (&getLookAndFeel());
    g.fillAll (lookAndFeel->findColour (ResizableWindow::backgroundColourId));
}

void TrajectoryBoxComponent::resized() {
    sourceLinkLabel.setBounds(5, 5, 150, 20);
    sourceLinkCombo.setBounds(120, 5, 150, 20);

    trajectoryTypeLabel.setBounds(5, 35, 150, 20);
    trajectoryTypeCombo.setBounds(120, 35, 150, 20);

    durationLabel.setBounds(5, 65, 150, 20);
    durationEditor.setBounds(120, 65, 100, 20);
    durationUnitCombo.setBounds(225, 65, 100, 20);

    cycleSpeedLabel.setBounds(5, 95, 150, 20);
    cycleSpeedSlider.setBounds(115, 95, 165, 20);

    activateButton.setBounds(300, 5, 150, 20);
}
