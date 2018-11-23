#include "ParametersBoxComponent.h"

ParametersBoxComponent::ParametersBoxComponent() {
    setLookAndFeel(&mGrisFeel);

    azimuthLabel.setText("Azimuth", NotificationType::dontSendNotification);
    addAndMakeVisible(&azimuthLabel);

    azimuthLinkButton.setButtonText("Link");
    addAndMakeVisible(&azimuthLinkButton);

    azimuthSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    azimuthSlider.setRange(0.0, 1.0);
    azimuthSlider.addListener(this);
    addAndMakeVisible(&azimuthSlider);

    elevationLabel.setText("Elevation", NotificationType::dontSendNotification);
    addAndMakeVisible(&elevationLabel);

    elevationLinkButton.setButtonText("Link");
    addAndMakeVisible(&elevationLinkButton);

    elevationSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    elevationSlider.setRange(0.0, 1.0);
    elevationSlider.addListener(this);
    addAndMakeVisible(&elevationSlider);

    distanceLabel.setText("Distance", NotificationType::dontSendNotification);
    addAndMakeVisible(&distanceLabel);
    distanceLabel.setEnabled(false);

    distanceLinkButton.setButtonText("Link");
    addAndMakeVisible(&distanceLinkButton);
    distanceLinkButton.setEnabled(false);

    distanceSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    distanceSlider.setRange(0.0, 1.0);
    distanceSlider.addListener(this);
    addAndMakeVisible(&distanceSlider);
    distanceSlider.setEnabled(false);

    //---------------------------------------------------------------------------------

    xLabel.setText("X", NotificationType::dontSendNotification);
    addChildComponent(&xLabel);

    xLinkButton.setButtonText("Link");
    addChildComponent(&xLinkButton);

    xSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    xSlider.setRange(0.0, 1.0);
    xSlider.addListener(this);
    addChildComponent(&xSlider);

    yLabel.setText("Y", NotificationType::dontSendNotification);
    addChildComponent(&yLabel);

    yLinkButton.setButtonText("Link");
    addChildComponent(&yLinkButton);

    ySlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    ySlider.setRange(0.0, 1.0);
    ySlider.addListener(this);
    addChildComponent(&ySlider);

    zLabel.setText("Z", NotificationType::dontSendNotification);
    addChildComponent(&zLabel);
    zLabel.setEnabled(false);

    zLinkButton.setButtonText("Link");
    addChildComponent(&zLinkButton);
    zLinkButton.setEnabled(false);

    zSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    zSlider.setRange(0.0, 1.0);
    zSlider.addListener(this);
    addChildComponent(&zSlider);
    zSlider.setEnabled(false);

    //---------------------------------------------------------------------------------

    azimuthSpanLabel.setText("Azimuth Span", NotificationType::dontSendNotification);
    addAndMakeVisible(&azimuthSpanLabel);

    azimuthSpanLinkButton.setButtonText("Link");
    addAndMakeVisible(&azimuthSpanLinkButton);

    azimuthSpanSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    azimuthSpanSlider.setRange(0.0, 1.0);
    azimuthSpanSlider.addListener(this);
    addAndMakeVisible(&azimuthSpanSlider);

    elevationSpanLabel.setText("Elevation Span", NotificationType::dontSendNotification);
    addAndMakeVisible(&elevationSpanLabel);

    elevationSpanLinkButton.setButtonText("Link");
    addAndMakeVisible(&elevationSpanLinkButton);

    elevationSpanSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    elevationSpanSlider.setRange(0.0, 1.0);
    elevationSpanSlider.addListener(this);
    addAndMakeVisible(&elevationSpanSlider);

    activatorXYZ.setButtonText("X-Y-Z");
    activatorXYZ.addListener(this);
    addAndMakeVisible(&activatorXYZ);
}

ParametersBoxComponent::~ParametersBoxComponent() {
    setLookAndFeel(nullptr);
}

void ParametersBoxComponent::setSelectedSource(Source *source) {
    selectedSource = source;
    azimuthSlider.setValue(selectedSource->getAzimuth() / 360.0);
    elevationSlider.setValue(selectedSource->getElevation() / 90.0);
    distanceSlider.setValue(selectedSource->getDistance());
    azimuthSpanSlider.setValue(selectedSource->getAzimuthSpan());
    elevationSpanSlider.setValue(selectedSource->getElevationSpan());
    repaint();
}

void ParametersBoxComponent::setDistanceEnabled(bool shouldBeEnabled) {
    distanceLabel.setEnabled(shouldBeEnabled);
    distanceLinkButton.setEnabled(shouldBeEnabled);
    distanceSlider.setEnabled(shouldBeEnabled);
    zLabel.setEnabled(shouldBeEnabled);
    zLinkButton.setEnabled(shouldBeEnabled);
    zSlider.setEnabled(shouldBeEnabled);
}

void ParametersBoxComponent::buttonClicked(Button *button) {
    if (button == &activatorXYZ) {
        if (activatorXYZ.getToggleState()) {
            xLabel.setVisible(true);
            xLinkButton.setVisible(true);
            xSlider.setVisible(true);
            yLabel.setVisible(true);
            yLinkButton.setVisible(true);
            ySlider.setVisible(true);
            zLabel.setVisible(true);
            zLinkButton.setVisible(true);
            zSlider.setVisible(true);
            azimuthLabel.setVisible(false);
            azimuthLinkButton.setVisible(false);
            azimuthSlider.setVisible(false);
            elevationLabel.setVisible(false);
            elevationLinkButton.setVisible(false);
            elevationSlider.setVisible(false);
            distanceLabel.setVisible(false);
            distanceLinkButton.setVisible(false);
            distanceSlider.setVisible(false);
        } else {
            xLabel.setVisible(false);
            xLinkButton.setVisible(false);
            xSlider.setVisible(false);
            yLabel.setVisible(false);
            yLinkButton.setVisible(false);
            ySlider.setVisible(false);
            zLabel.setVisible(false);
            zLinkButton.setVisible(false);
            zSlider.setVisible(false);
            azimuthLabel.setVisible(true);
            azimuthLinkButton.setVisible(true);
            azimuthSlider.setVisible(true);
            elevationLabel.setVisible(true);
            elevationLinkButton.setVisible(true);
            elevationSlider.setVisible(true);
            distanceLabel.setVisible(true);
            distanceLinkButton.setVisible(true);
            distanceSlider.setVisible(true);
        }
    }
}

void ParametersBoxComponent::sliderValueChanged(Slider *slider) {
    int parameterId;
    if (slider == &azimuthSlider) {
        parameterId = 0;
    } else if (slider == &elevationSlider) {
        parameterId = 1;    
    } else if (slider == &distanceSlider) {
        parameterId = 2;
    } else if (slider == &xSlider) {
        parameterId = 3;
    } else if (slider == &ySlider) {
        parameterId = 4;
    } else if (slider == &zSlider) {
        parameterId = 5;
    } else if (slider == &azimuthSpanSlider) {
        parameterId = 6;
    } else if (slider == &elevationSpanSlider) {
        parameterId = 7;
    }

    listeners.call([&] (Listener& l) { l.parameterChanged(parameterId, slider->getValue()); });
}

void ParametersBoxComponent::paint(Graphics& g) {
    float x = 270;
    float y = 270;

    g.setColour(selectedSource->getColour());
    g.drawEllipse(x, y, 20, 20, 3);
    g.setColour(Colours::white);
    g.drawText(String(selectedSource->getId()+1), x, y, 20, 20, Justification(Justification::centred), false);
}

void ParametersBoxComponent::resized() {
    azimuthLabel.setBounds(5, 20, 150, 20);
    azimuthLinkButton.setBounds(5, 40, 45, 20);
    azimuthSlider.setBounds(55, 40, 175, 20);

    elevationLabel.setBounds(5, 70, 150, 20);
    elevationLinkButton.setBounds(5, 90, 45, 20);
    elevationSlider.setBounds(55, 90, 175, 20);

    distanceLabel.setBounds(5, 120, 150, 20);
    distanceLinkButton.setBounds(5, 140, 45, 20);
    distanceSlider.setBounds(55, 140, 175, 20);

    //---------------------------------------------------------------------------------

    xLabel.setBounds(5, 20, 150, 20);
    xLinkButton.setBounds(5, 40, 45, 20);
    xSlider.setBounds(55, 40, 175, 20);

    yLabel.setBounds(5, 70, 150, 20);
    yLinkButton.setBounds(5, 90, 45, 20);
    ySlider.setBounds(55, 90, 175, 20);

    zLabel.setBounds(5, 120, 150, 20);
    zLinkButton.setBounds(5, 140, 45, 20);
    zSlider.setBounds(55, 140, 175, 20);

    //---------------------------------------------------------------------------------

    azimuthSpanLabel.setBounds(5, 170, 150, 20);
    azimuthSpanLinkButton.setBounds(5, 190, 45, 20);
    azimuthSpanSlider.setBounds(55, 190, 175, 20);

    elevationSpanLabel.setBounds(5, 220, 150, 20);
    elevationSpanLinkButton.setBounds(5, 240, 45, 20);
    elevationSpanSlider.setBounds(55, 240, 175, 20);

    activatorXYZ.setBounds(240, 5, 60, 20);
}
