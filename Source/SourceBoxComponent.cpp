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
#include "SourceBoxComponent.h"
#include "ControlGrisConstants.h"

SourceBoxComponent::SourceBoxComponent() {
    selectedSourceNumber = 1;
    currentAngle = 0.0f;
    currentRayLength = 1.0f;

    sourcePlacementLabel.setText("Source Placement:", NotificationType::dontSendNotification);
    addAndMakeVisible(&sourcePlacementLabel);

    addAndMakeVisible(&sourcePlacementCombo);
    sourcePlacementCombo.setTextWhenNothingSelected("Choose a source placement...");
    sourcePlacementCombo.addItemList(SOURCE_PLACEMENT_SKETCH, 1);
    sourcePlacementCombo.addListener(this);

    sourceNumberLabel.setText("Source Number:", NotificationType::dontSendNotification);
    addAndMakeVisible(&sourceNumberLabel);

    addAndMakeVisible(&sourceNumberCombo);
    sourceNumberCombo.setTextWhenNothingSelected("Choose a source...");
    for (int i = 1; i <= 8; i++) {
        sourceNumberCombo.addItem(String(i), i);
    }
    sourceNumberCombo.setSelectedId(selectedSourceNumber);
    sourceNumberCombo.addListener(this);

    rayLengthLabel.setText("Ray Length:", NotificationType::dontSendNotification);
    addAndMakeVisible(&rayLengthLabel);

    rayLengthSlider.setNormalisableRange(NormalisableRange<double>(0.0f, 1.0f, 0.01f));
    rayLengthSlider.setValue(1.0, NotificationType::sendNotificationAsync);
    rayLengthSlider.setTextBoxStyle(Slider::TextBoxRight, false, 40, 20);
    rayLengthSlider.setColour(Slider:: textBoxOutlineColourId, Colours::transparentBlack);
    rayLengthSlider.addListener(this);
    addAndMakeVisible(&rayLengthSlider);

    angleLabel.setText("Angle:", NotificationType::dontSendNotification);
    addAndMakeVisible(&angleLabel);

    angleSlider.setNormalisableRange(NormalisableRange<double>(0.0f, 360.0f, 0.01f));
    angleSlider.setValue(90.0, NotificationType::sendNotificationAsync);
    angleSlider.setTextBoxStyle(Slider::TextBoxRight, false, 40, 20);
    angleSlider.setColour(Slider:: textBoxOutlineColourId, Colours::transparentBlack);
    angleSlider.addListener(this);
    addAndMakeVisible(&angleSlider);
}

SourceBoxComponent::~SourceBoxComponent() {}

void SourceBoxComponent::paint(Graphics& g) {
    GrisLookAndFeel *lookAndFeel;
    lookAndFeel = static_cast<GrisLookAndFeel *> (&getLookAndFeel());
    g.fillAll (lookAndFeel->findColour (ResizableWindow::backgroundColourId));
}

void SourceBoxComponent::resized() {
    sourcePlacementLabel.setBounds(5, 10, 150, 15);
    sourcePlacementCombo.setBounds(130, 10, 150, 20);

    sourceNumberLabel.setBounds(305, 10, 150, 15);
    sourceNumberCombo.setBounds(430, 10, 150, 20);

    rayLengthLabel.setBounds(305, 40, 150, 15);
    rayLengthSlider.setBounds(380, 40, 200, 20);

    angleLabel.setBounds(305, 70, 150, 15);
    angleSlider.setBounds(380, 70, 200, 20);
}

void SourceBoxComponent::comboBoxChanged(ComboBox *combo) {
    if (combo == &sourcePlacementCombo) {
        listeners.call([&] (Listener& l) { l.sourcePlacementChanged(combo->getSelectedId()); });
    } else if (combo == &sourceNumberCombo) {
        selectedSourceNumber = combo->getSelectedId();
    }
}

void SourceBoxComponent::sliderValueChanged(Slider *slider) {
    if (slider == &rayLengthSlider) {
        currentRayLength = (float)slider->getValue();
    } else if (slider == &angleSlider) {
        currentAngle = (float)slider->getValue();
    }
    listeners.call([&] (Listener& l) { l.sourceNumberPositionChanged(selectedSourceNumber, currentAngle, currentRayLength); });
}

void SourceBoxComponent::setNumberOfSources(int numOfSources) {
    sourceNumberCombo.clear();
    for (int i = 1; i <= numOfSources; i++) {
        sourceNumberCombo.addItem(String(i), i);
    }
    if (selectedSourceNumber > numOfSources)
        selectedSourceNumber = 1;
    sourceNumberCombo.setSelectedId(selectedSourceNumber);
}
