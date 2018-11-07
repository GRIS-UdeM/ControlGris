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

    radiusLabel.setText("Radius", NotificationType::dontSendNotification);
    addAndMakeVisible(&radiusLabel);
    radiusLabel.setEnabled(false);

    radiusLinkButton.setButtonText("Link");
    addAndMakeVisible(&radiusLinkButton);
    radiusLinkButton.setEnabled(false);

    radiusSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    radiusSlider.setRange(0.0, 1.0);
    radiusSlider.addListener(this);
    addAndMakeVisible(&radiusSlider);
    radiusSlider.setEnabled(false);

    //---------------------------------------------------------------------------------

    xLabel.setText("X", NotificationType::dontSendNotification);
    addChildComponent(&xLabel);

    xLinkButton.setButtonText("Link");
    addChildComponent(&xLinkButton);

    xSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    xSlider.addListener(this);
    addChildComponent(&xSlider);

    yLabel.setText("Y", NotificationType::dontSendNotification);
    addChildComponent(&yLabel);

    yLinkButton.setButtonText("Link");
    addChildComponent(&yLinkButton);

    ySlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    ySlider.addListener(this);
    addChildComponent(&ySlider);

    zLabel.setText("Z", NotificationType::dontSendNotification);
    addChildComponent(&zLabel);
    zLabel.setEnabled(false);

    zLinkButton.setButtonText("Link");
    addChildComponent(&zLinkButton);
    zLinkButton.setEnabled(false);

    zSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    zSlider.addListener(this);
    addChildComponent(&zSlider);
    zSlider.setEnabled(false);

    //---------------------------------------------------------------------------------

    azimuthSpanLabel.setText("Azimuth Span", NotificationType::dontSendNotification);
    addAndMakeVisible(&azimuthSpanLabel);

    azimuthSpanLinkButton.setButtonText("Link");
    addAndMakeVisible(&azimuthSpanLinkButton);

    azimuthSpanSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    azimuthSpanSlider.addListener(this);
    addAndMakeVisible(&azimuthSpanSlider);

    elevationSpanLabel.setText("Elevation Span", NotificationType::dontSendNotification);
    addAndMakeVisible(&elevationSpanLabel);

    elevationSpanLinkButton.setButtonText("Link");
    addAndMakeVisible(&elevationSpanLinkButton);

    elevationSpanSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
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
    repaint();
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
            radiusLabel.setVisible(false);
            radiusLinkButton.setVisible(false);
            radiusSlider.setVisible(false);
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
            radiusLabel.setVisible(true);
            radiusLinkButton.setVisible(true);
            radiusSlider.setVisible(true);
        }
    }
}

void ParametersBoxComponent::sliderValueChanged(Slider *slider) {
    int parameterId;
    if (slider == &azimuthSlider) {
        parameterId = 0;
    } else if (slider == &elevationSlider) {
        parameterId = 1;    
    } else if (slider == &radiusSlider) {
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
    float x = getWidth() - 30;
    float y = getHeight() - 30;

    g.setColour(selectedSource->getColour());
    g.drawEllipse(x, y, 20, 20, 3);
    g.setColour(Colours::white);
    g.drawText(String(selectedSource->getId()+1), x, y, 20, 20, Justification(Justification::centred), false);
}

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

    //---------------------------------------------------------------------------------

    xLabel.setBounds(5, 5, 150, 15);
    xLinkButton.setBounds(5, 20, 45, 20);
    xSlider.setBounds(55, 20, 150, 20);

    yLabel.setBounds(5, 45, 150, 15);
    yLinkButton.setBounds(5, 60, 45, 20);
    ySlider.setBounds(55, 60, 150, 20);

    zLabel.setBounds(5, 85, 150, 15);
    zLinkButton.setBounds(5, 100, 45, 20);
    zSlider.setBounds(55, 100, 150, 20);

    //---------------------------------------------------------------------------------

    azimuthSpanLabel.setBounds(240, 5, 150, 15);
    azimuthSpanLinkButton.setBounds(240, 20, 45, 20);
    azimuthSpanSlider.setBounds(290, 20, 150, 20);

    elevationSpanLabel.setBounds(240, 45, 150, 15);
    elevationSpanLinkButton.setBounds(240, 60, 45, 20);
    elevationSpanSlider.setBounds(290, 60, 150, 20);

    activatorXYZ.setBounds(width - 65, 5, 60, 20);
}
