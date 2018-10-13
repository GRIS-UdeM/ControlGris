#include "ParametersBoxComponent.h"

ParametersBoxComponent::ParametersBoxComponent() {
    setLookAndFeel(&mGrisFeel);

    azimuthLabel.setText("Azimuth", NotificationType::dontSendNotification);
    addAndMakeVisible(&azimuthLabel);

    azimuthLinkButton.setButtonText("Link");
    addAndMakeVisible(&azimuthLinkButton);

    azimuthSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible(&azimuthSlider);

    elevationLabel.setText("Elevation", NotificationType::dontSendNotification);
    addAndMakeVisible(&elevationLabel);

    elevationLinkButton.setButtonText("Link");
    addAndMakeVisible(&elevationLinkButton);

    elevationSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible(&elevationSlider);

    radiusLabel.setText("Radius", NotificationType::dontSendNotification);
    addAndMakeVisible(&radiusLabel);
    radiusLabel.setEnabled(false);

    radiusLinkButton.setButtonText("Link");
    addAndMakeVisible(&radiusLinkButton);
    radiusLinkButton.setEnabled(false);

    radiusSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible(&radiusSlider);
    radiusSlider.setEnabled(false);

    azimuthSpanLabel.setText("Azimuth Span", NotificationType::dontSendNotification);
    addAndMakeVisible(&azimuthSpanLabel);

    azimuthSpanLinkButton.setButtonText("Link");
    addAndMakeVisible(&azimuthSpanLinkButton);

    azimuthSpanSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible(&azimuthSpanSlider);

    elevationSpanLabel.setText("Elevation Span", NotificationType::dontSendNotification);
    addAndMakeVisible(&elevationSpanLabel);

    elevationSpanLinkButton.setButtonText("Link");
    addAndMakeVisible(&elevationSpanLinkButton);

    elevationSpanSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible(&elevationSpanSlider);

    activatorXYZ.setButtonText("X-Y-Z");
    addAndMakeVisible(&activatorXYZ);
}

ParametersBoxComponent::~ParametersBoxComponent() {
}

void ParametersBoxComponent::paint(Graphics& g) {}

void ParametersBoxComponent::resized() {
    double width = getWidth();

    azimuthLabel.setBounds(5, 5, 150, 15);
    azimuthLinkButton.setBounds(5, 20, 45, 20);
    azimuthSlider.setBounds(55, 20, 150, 20);

    elevationLabel.setBounds(5, 45, 150, 15);
    elevationLinkButton.setBounds(5, 60, 45, 20);
    elevationSlider.setBounds(55, 60, 150, 20);

    radiusLabel.setBounds(5, 85, 150, 15);
    radiusLinkButton.setBounds(5, 100, 45, 20);
    radiusSlider.setBounds(55, 100, 150, 20);

    azimuthSpanLabel.setBounds(240, 5, 150, 15);
    azimuthSpanLinkButton.setBounds(240, 20, 45, 20);
    azimuthSpanSlider.setBounds(290, 20, 150, 20);

    elevationSpanLabel.setBounds(240, 45, 150, 15);
    elevationSpanLinkButton.setBounds(240, 60, 45, 20);
    elevationSpanSlider.setBounds(290, 60, 150, 20);

    activatorXYZ.setBounds(width - 65, 5, 60, 20);
}
