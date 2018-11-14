#include "SourceBoxComponent.h"

SourceBoxComponent::SourceBoxComponent() {
    setLookAndFeel(&mGrisFeel);

    sourcePlacementLabel.setText("Source Placement:", NotificationType::dontSendNotification);
    addAndMakeVisible(&sourcePlacementLabel);

    addAndMakeVisible(&sourcePlacementCombo);

    sourceNumberLabel.setText("Source Number:", NotificationType::dontSendNotification);
    addAndMakeVisible(&sourceNumberLabel);

    addAndMakeVisible(&sourceNumberCombo);

    rayLengthLabel.setText("Ray Length:", NotificationType::dontSendNotification);
    addAndMakeVisible(&rayLengthLabel);

    rayLengthSlider.setNormalisableRange(NormalisableRange<double>(0.0f, 2.0f, 0.01f));
    rayLengthSlider.setValue(1.0, NotificationType::sendNotificationAsync);
    rayLengthSlider.setTextBoxStyle(Slider::TextBoxRight, false, 40, 20);
    rayLengthSlider.setColour(Slider:: textBoxOutlineColourId, Colours::transparentBlack);
    addAndMakeVisible(&rayLengthSlider);

    angleLabel.setText("Angle:", NotificationType::dontSendNotification);
    addAndMakeVisible(&angleLabel);

    angleSlider.setNormalisableRange(NormalisableRange<double>(0.0f, 360.0f, 0.01f));
    angleSlider.setValue(90.0, NotificationType::sendNotificationAsync);
    angleSlider.setTextBoxStyle(Slider::TextBoxRight, false, 40, 20);
    angleSlider.setColour(Slider:: textBoxOutlineColourId, Colours::transparentBlack);
    addAndMakeVisible(&angleSlider);
}

SourceBoxComponent::~SourceBoxComponent() {
    setLookAndFeel(nullptr);
}

void SourceBoxComponent::paint(Graphics& g) {}

void SourceBoxComponent::resized() {
    double width = getWidth();

    sourcePlacementLabel.setBounds(5, 10, 150, 15);
    sourcePlacementCombo.setBounds(130, 10, 150, 20);

    sourceNumberLabel.setBounds(5, 40, 150, 15);
    sourceNumberCombo.setBounds(130, 40, 150, 20);

    rayLengthLabel.setBounds(5, 70, 150, 15);
    rayLengthSlider.setBounds(125, 70, 200, 20);

    angleLabel.setBounds(5, 100, 150, 15);
    angleSlider.setBounds(125, 100, 200, 20);
}
