#include "ParametersBoxComponent.h"

//-------------------------------------------------------------------
ParameterComponent::ParameterComponent(int parameterId, String label, Component *parent) {
    m_parameterId = parameterId;

    parameterLabel.setText(label, NotificationType::dontSendNotification);
    addAndMakeVisible(&parameterLabel);

    linkButton.setButtonText("Link");
    addAndMakeVisible(&linkButton);

    slider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    slider.setRange(0.0, 1.0);
    slider.addListener(this);
    addAndMakeVisible(&slider);

    addListener(dynamic_cast<ParameterComponent::Listener *>(parent));
    parent->addAndMakeVisible(this);
}

ParameterComponent::~ParameterComponent() {}

void ParameterComponent::paint(Graphics&) {}

void ParameterComponent::resized() {
    parameterLabel.setBounds(0, 0, 150, 20);
    linkButton.setBounds(0, 20, 45, 20);
    slider.setBounds(50, 20, 175, 20);
}

void ParameterComponent::sliderValueChanged(Slider *slider) {
    listeners.call([&] (Listener& l) { l.parameterChanged(m_parameterId, slider->getValue()); });
}

void ParameterComponent::setValue(double value) {
    slider.setValue(value, NotificationType::dontSendNotification);
}

bool ParameterComponent::getLinkState() {
    return linkButton.getToggleState();
}

//-------------------------------------------------------------------
ParametersBoxComponent::ParametersBoxComponent() :
    p_azimuth(SOURCE_ID_AZIMUTH, "Azimuth", this),
    p_elevation(SOURCE_ID_ELEVATION, "Elevation", this),
    p_distance(SOURCE_ID_DISTANCE, "Distance", this),
    p_x(SOURCE_ID_X, "X", this),
    p_y(SOURCE_ID_Y, "Y", this),
    p_azimuthSpan(SOURCE_ID_AZIMUTH_SPAN, "Azimuth Span", this),
    p_elevationSpan(SOURCE_ID_ELEVATION_SPAN, "Elevation Span", this)
{
    setLookAndFeel(&mGrisFeel);

    m_distanceEnabled = false;

    p_distance.setEnabled(false);
    p_x.setVisible(false);
    p_y.setVisible(false);

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
    p_azimuth.setValue(normalizedAzimuth);
    p_elevation.setValue(selectedSource->getElevation() / 90.0f);
    p_distance.setValue(selectedSource->getDistance());
    p_x.setValue(selectedSource->getX());
    p_y.setValue(selectedSource->getY());
    p_azimuthSpan.setValue(selectedSource->getAzimuthSpan());
    p_elevationSpan.setValue(selectedSource->getElevationSpan());
    repaint();
}

void ParametersBoxComponent::setDistanceEnabled(bool shouldBeEnabled) {
    p_distance.setEnabled(shouldBeEnabled);
    m_distanceEnabled = shouldBeEnabled;
    resized();
    activatorXY.triggerClick();
    activatorXY.triggerClick();
}

bool ParametersBoxComponent::getLinkState(int parameterId) {
    if (parameterId == SOURCE_ID_AZIMUTH) {
        return p_azimuth.getLinkState();
    } else if (parameterId == SOURCE_ID_ELEVATION) {
        return p_elevation.getLinkState();
    } else if (parameterId == SOURCE_ID_DISTANCE) {
        return p_distance.getLinkState();
    } else if (parameterId == SOURCE_ID_X) {
        return p_x.getLinkState();
    } else if (parameterId == SOURCE_ID_Y) {
        return p_y.getLinkState();
    } else if (parameterId == SOURCE_ID_AZIMUTH_SPAN) {
        return p_azimuthSpan.getLinkState();
    } else if (parameterId == SOURCE_ID_ELEVATION_SPAN) {
        return p_elevationSpan.getLinkState();
    }
}

void ParametersBoxComponent::buttonClicked(Button *button) {
    if (button == &activatorXY) {
        if (activatorXY.getToggleState()) {
            p_x.setVisible(true);
            p_y.setVisible(true);
            p_azimuth.setVisible(false);
            p_distance.setVisible(false);
            if (m_distanceEnabled) {
                p_elevation.setVisible(true);
            } else {
                p_elevation.setVisible(false);
            }
        } else {
            p_x.setVisible(false);
            p_y.setVisible(false);
            p_azimuth.setVisible(true);
            p_elevation.setVisible(true);
            p_distance.setVisible(true);
        }
    }
}

void ParametersBoxComponent::parameterChanged(int parameterId, double value) {
    listeners.call([&] (Listener& l) { l.parameterChanged(parameterId, value); });
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
    p_azimuth.setBounds(5, 20, 225, 40);
    if (m_distanceEnabled) {
        p_distance.setBounds(5, 70, 225, 40);
        p_elevation.setBounds(5, 120, 225, 40);
    } else {
        p_elevation.setBounds(5, 70, 225, 40);
        p_distance.setBounds(5, 120, 225, 40);
    }
    p_x.setBounds(5, 20, 225, 40);
    p_y.setBounds(5, 70, 225, 40);
    p_azimuthSpan.setBounds(5, 170, 225, 40);
    p_elevationSpan.setBounds(5, 220, 225, 40);
    activatorXY.setBounds(240, 5, 60, 20);
}
