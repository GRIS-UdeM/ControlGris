#include "ParametersBoxComponent.h"

ParametersBoxComponent::ParametersBoxComponent() {
    setLookAndFeel(&mGrisFeel);

    m_distanceEnabled = false;

    // Azimuth
    azimuthLabel.setText("Azimuth", NotificationType::dontSendNotification);
    addAndMakeVisible(&azimuthLabel);

    azimuthLinkButton.setButtonText("Link");
    addAndMakeVisible(&azimuthLinkButton);

    azimuthSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    azimuthSlider.setRange(0.0, 1.0);
    azimuthSlider.addListener(this);
    addAndMakeVisible(&azimuthSlider);

    // Elevation
    elevationLabel.setText("Elevation", NotificationType::dontSendNotification);
    addAndMakeVisible(&elevationLabel);

    elevationLinkButton.setButtonText("Link");
    addAndMakeVisible(&elevationLinkButton);

    elevationSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    elevationSlider.setRange(0.0, 1.0);
    elevationSlider.addListener(this);
    addAndMakeVisible(&elevationSlider);

    // Distance
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

    // X
    xLabel.setText("X", NotificationType::dontSendNotification);
    addChildComponent(&xLabel);

    xLinkButton.setButtonText("Link");
    addChildComponent(&xLinkButton);

    xSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    xSlider.setRange(0.0, 1.0);
    xSlider.addListener(this);
    addChildComponent(&xSlider);

    // Y
    yLabel.setText("Y", NotificationType::dontSendNotification);
    addChildComponent(&yLabel);

    yLinkButton.setButtonText("Link");
    addChildComponent(&yLinkButton);

    ySlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    ySlider.setRange(0.0, 1.0);
    ySlider.addListener(this);
    addChildComponent(&ySlider);

    //---------------------------------------------------------------------------------

    // Azimuth Span
    azimuthSpanLabel.setText("Azimuth Span", NotificationType::dontSendNotification);
    addAndMakeVisible(&azimuthSpanLabel);

    azimuthSpanLinkButton.setButtonText("Link");
    addAndMakeVisible(&azimuthSpanLinkButton);

    azimuthSpanSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    azimuthSpanSlider.setRange(0.0, 1.0);
    azimuthSpanSlider.addListener(this);
    addAndMakeVisible(&azimuthSpanSlider);

    // Elevation Span
    elevationSpanLabel.setText("Elevation Span", NotificationType::dontSendNotification);
    addAndMakeVisible(&elevationSpanLabel);

    elevationSpanLinkButton.setButtonText("Link");
    addAndMakeVisible(&elevationSpanLinkButton);

    elevationSpanSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    elevationSpanSlider.setRange(0.0, 1.0);
    elevationSpanSlider.addListener(this);
    addAndMakeVisible(&elevationSpanSlider);

    // Azimuth-Elevation / X-Y switch
    activatorXY.setButtonText("X-Y");
    activatorXY.addListener(this);
    addAndMakeVisible(&activatorXY);
}

ParametersBoxComponent::~ParametersBoxComponent() {
    setLookAndFeel(nullptr);
}

void ParametersBoxComponent::setSelectedSource(Source *source) {
    selectedSource = source;
    float normalizedAzimuth = selectedSource->getAzimuth()  / 360.0;
    normalizedAzimuth = normalizedAzimuth >= 0 ? normalizedAzimuth : normalizedAzimuth + 1.0;
    azimuthSlider.setValue(normalizedAzimuth, NotificationType::dontSendNotification);
    elevationSlider.setValue(selectedSource->getElevation() / 90.0f, NotificationType::dontSendNotification);
    distanceSlider.setValue(selectedSource->getDistance(), NotificationType::dontSendNotification);
    azimuthSpanSlider.setValue(selectedSource->getAzimuthSpan(), NotificationType::dontSendNotification);
    elevationSpanSlider.setValue(selectedSource->getElevationSpan(), NotificationType::dontSendNotification);
    xSlider.setValue(selectedSource->getX(), NotificationType::dontSendNotification);
    ySlider.setValue(selectedSource->getY(), NotificationType::dontSendNotification);
    repaint();
}

void ParametersBoxComponent::setDistanceEnabled(bool shouldBeEnabled) {
    distanceLabel.setEnabled(shouldBeEnabled);
    distanceLinkButton.setEnabled(shouldBeEnabled);
    distanceSlider.setEnabled(shouldBeEnabled);
    m_distanceEnabled = shouldBeEnabled;
    resized();
    activatorXY.triggerClick();
    activatorXY.triggerClick();
}

bool ParametersBoxComponent::getLinkState(int parameterId) {
    if (parameterId == SOURCE_ID_AZIMUTH) {
        return azimuthLinkButton.getToggleState();
    } else if (parameterId == SOURCE_ID_ELEVATION) {
        return elevationLinkButton.getToggleState();
    } else if (parameterId == SOURCE_ID_DISTANCE) {
        return distanceLinkButton.getToggleState();
    } else if (parameterId == SOURCE_ID_X) {
        return xLinkButton.getToggleState();
    } else if (parameterId == SOURCE_ID_Y) {
        return yLinkButton.getToggleState();
    } else if (parameterId == SOURCE_ID_AZIMUTH_SPAN) {
        return azimuthSpanLinkButton.getToggleState();
    } else if (parameterId == SOURCE_ID_ELEVATION_SPAN) {
        return elevationSpanLinkButton.getToggleState();
    }
}

void ParametersBoxComponent::buttonClicked(Button *button) {
    if (button == &activatorXY) {
        if (activatorXY.getToggleState()) {
            xLabel.setVisible(true);
            xLinkButton.setVisible(true);
            xSlider.setVisible(true);
            yLabel.setVisible(true);
            yLinkButton.setVisible(true);
            ySlider.setVisible(true);
            azimuthLabel.setVisible(false);
            azimuthLinkButton.setVisible(false);
            azimuthSlider.setVisible(false);
            distanceLabel.setVisible(false);
            distanceLinkButton.setVisible(false);
            distanceSlider.setVisible(false);
            if (m_distanceEnabled) {
                elevationLabel.setVisible(true);
                elevationLinkButton.setVisible(true);
                elevationSlider.setVisible(true);
            } else {
                elevationLabel.setVisible(false);
                elevationLinkButton.setVisible(false);
                elevationSlider.setVisible(false);
            }
        } else {
            xLabel.setVisible(false);
            xLinkButton.setVisible(false);
            xSlider.setVisible(false);
            yLabel.setVisible(false);
            yLinkButton.setVisible(false);
            ySlider.setVisible(false);
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
        parameterId = SOURCE_ID_AZIMUTH;
    } else if (slider == &elevationSlider) {
        parameterId = SOURCE_ID_ELEVATION;
    } else if (slider == &distanceSlider) {
        parameterId = SOURCE_ID_DISTANCE;
    } else if (slider == &xSlider) {
        parameterId = SOURCE_ID_X;
    } else if (slider == &ySlider) {
        parameterId = SOURCE_ID_Y;
    } else if (slider == &azimuthSpanSlider) {
        parameterId = SOURCE_ID_AZIMUTH_SPAN;
    } else if (slider == &elevationSpanSlider) {
        parameterId = SOURCE_ID_ELEVATION_SPAN;
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

    if (m_distanceEnabled) {
        distanceLabel.setBounds(5, 70, 150, 20);
        distanceLinkButton.setBounds(5, 90, 45, 20);
        distanceSlider.setBounds(55, 90, 175, 20);

        elevationLabel.setBounds(5, 120, 150, 20);
        elevationLinkButton.setBounds(5, 140, 45, 20);
        elevationSlider.setBounds(55, 140, 175, 20);
    } else {
        elevationLabel.setBounds(5, 70, 150, 20);
        elevationLinkButton.setBounds(5, 90, 45, 20);
        elevationSlider.setBounds(55, 90, 175, 20);

        distanceLabel.setBounds(5, 120, 150, 20);
        distanceLinkButton.setBounds(5, 140, 45, 20);
        distanceSlider.setBounds(55, 140, 175, 20);
    }

    //---------------------------------------------------------------------------------

    xLabel.setBounds(5, 20, 150, 20);
    xLinkButton.setBounds(5, 40, 45, 20);
    xSlider.setBounds(55, 40, 175, 20);

    yLabel.setBounds(5, 70, 150, 20);
    yLinkButton.setBounds(5, 90, 45, 20);
    ySlider.setBounds(55, 90, 175, 20);

    //---------------------------------------------------------------------------------

    azimuthSpanLabel.setBounds(5, 170, 150, 20);
    azimuthSpanLinkButton.setBounds(5, 190, 45, 20);
    azimuthSpanSlider.setBounds(55, 190, 175, 20);

    elevationSpanLabel.setBounds(5, 220, 150, 20);
    elevationSpanLinkButton.setBounds(5, 240, 45, 20);
    elevationSpanSlider.setBounds(55, 240, 175, 20);

    activatorXY.setBounds(240, 5, 60, 20);
}
