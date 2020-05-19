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
    selectedSourceNumber = 0;
    currentAngle = 0.0f;
    currentRayLength = 1.0f;

    sourcePlacementLabel.setText("Source Placement:", NotificationType::dontSendNotification);
    addAndMakeVisible(&sourcePlacementLabel);

    addAndMakeVisible(&sourcePlacementCombo);
    sourcePlacementCombo.setTextWhenNothingSelected("Choose a source placement...");
    sourcePlacementCombo.addItemList(SOURCE_PLACEMENT_SKETCH, 1);
    sourcePlacementCombo.onChange = [this] { 
            listeners.call([&] (Listener& l) { l.sourceBoxPlacementChanged(static_cast<SourcePlacement>(sourcePlacementCombo.getSelectedId()));
                                                sourcePlacementCombo.setSelectedId(0, NotificationType::dontSendNotification); });
        };

    sourceNumberLabel.setText("Source Number:", NotificationType::dontSendNotification);
    addAndMakeVisible(&sourceNumberLabel);

    addAndMakeVisible(&sourceNumberCombo);
    sourceNumberCombo.setTextWhenNothingSelected("Choose a source...");
    for (int i = 1; i <= 8; i++) {
        sourceNumberCombo.addItem(String(i), i);
    }
    sourceNumberCombo.setSelectedId(selectedSourceNumber);
    sourceNumberCombo.onChange = [this] { selectedSourceNumber = sourceNumberCombo.getSelectedItemIndex(); 
            listeners.call([&] (Listener& l) { l.sourceBoxSelectionChanged(selectedSourceNumber); });
        };

    rayLengthLabel.setText("Ray Length:", NotificationType::dontSendNotification);
    addAndMakeVisible(&rayLengthLabel);

    rayLengthSlider.setNormalisableRange(NormalisableRange<double>(0.0f, 1.0f, 0.01f));
    rayLengthSlider.setValue(1.0, NotificationType::dontSendNotification);
    rayLengthSlider.setTextBoxStyle(Slider::TextBoxRight, false, 40, 20);
    rayLengthSlider.setColour(Slider:: textBoxOutlineColourId, Colours::transparentBlack);
    addAndMakeVisible(&rayLengthSlider);
    rayLengthSlider.onValueChange = [this] { 
            currentRayLength = (float)rayLengthSlider.getValue();
            listeners.call([&] (Listener& l) { l.sourceBoxPositionChanged(selectedSourceNumber, currentAngle, currentRayLength); });
        };

    angleLabel.setText("Angle:", NotificationType::dontSendNotification);
    addAndMakeVisible(&angleLabel);

    angleSlider.setNormalisableRange(NormalisableRange<double>(0.0f, 360.0f, 0.01f));
    angleSlider.setValue(0.0, NotificationType::dontSendNotification);
    angleSlider.setTextBoxStyle(Slider::TextBoxRight, false, 40, 20);
    angleSlider.setColour(Slider:: textBoxOutlineColourId, Colours::transparentBlack);
    addAndMakeVisible(&angleSlider);
    angleSlider.onValueChange = [this] { 
            currentAngle = (float)angleSlider.getValue();
            listeners.call([&] (Listener& l) { l.sourceBoxPositionChanged(selectedSourceNumber, currentAngle, currentRayLength); });
        };
}

SourceBoxComponent::~SourceBoxComponent() {
    setLookAndFeel(nullptr);
}

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

void SourceBoxComponent::setNumberOfSources(int numOfSources, int firstSourceId) {
    sourceNumberCombo.clear();
    for (int i = firstSourceId; i < firstSourceId + numOfSources; i++) {
        sourceNumberCombo.addItem(String(i), i);
    }
    if (selectedSourceNumber >= numOfSources)
        selectedSourceNumber = 0;
    sourceNumberCombo.setSelectedItemIndex(selectedSourceNumber);
}

void SourceBoxComponent::updateSelectedSource(Source *source, int sourceIndex, SpatMode spatMode) {
    selectedSourceNumber = sourceIndex;
    sourceNumberCombo.setSelectedItemIndex(selectedSourceNumber);
    if (spatMode == SpatMode::LBAP) {
        currentAngle = source->getAzimuth();
        currentRayLength = source->getDistance();
    } else {
        currentAngle = source->getAzimuth();
        currentRayLength = 1.0f - source->getNormalizedElevation();
    }
    if (currentAngle < 0.0f) {
        currentAngle += 360.0f;
    }
    angleSlider.setValue(currentAngle, NotificationType::dontSendNotification);
    rayLengthSlider.setValue(currentRayLength, NotificationType::dontSendNotification);
}
