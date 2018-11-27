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

TrajectoryBoxComponent::TrajectoryBoxComponent() {
    setLookAndFeel(&mGrisFeel);

    sourceLinkLabel.setText("Source Link:", NotificationType::dontSendNotification);
    addAndMakeVisible(&sourceLinkLabel);

    addAndMakeVisible(&sourceLinkCombo);

    trajectoryTypeLabel.setText("Trajectory Type:", NotificationType::dontSendNotification);
    addAndMakeVisible(&trajectoryTypeLabel);

    addAndMakeVisible(&trajectoryTypeCombo);

    timeScaleLabel.setText("Time Scale:", NotificationType::dontSendNotification);
    addAndMakeVisible(&timeScaleLabel);

    addAndMakeVisible(&timeScaleCombo);

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

TrajectoryBoxComponent::~TrajectoryBoxComponent() {
    setLookAndFeel(nullptr);
}

void TrajectoryBoxComponent::paint(Graphics& g) {
    double width = getWidth();
    double height = getHeight();

    g.setColour(mGrisFeel.getEditBackgroundColour().withAlpha(0.25f));
    g.fillRect(290, 5, width - 295, height - 10);
}

void TrajectoryBoxComponent::resized() {
    sourceLinkLabel.setBounds(5, 5, 150, 20);
    sourceLinkCombo.setBounds(120, 5, 150, 20);

    trajectoryTypeLabel.setBounds(5, 35, 150, 20);
    trajectoryTypeCombo.setBounds(120, 35, 150, 20);

    timeScaleLabel.setBounds(5, 65, 150, 20);
    timeScaleCombo.setBounds(120, 65, 150, 20);

    cycleSpeedLabel.setBounds(5, 95, 150, 20);
    cycleSpeedSlider.setBounds(115, 95, 165, 20);

    activateButton.setBounds(8, 125, 150, 20);
}
