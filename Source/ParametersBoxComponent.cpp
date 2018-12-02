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
#include "ParametersBoxComponent.h"

//-------------------------------------------------------------------
ParameterComponent::ParameterComponent(int parameterId, String label, Component *parent) {
    m_parameterId = parameterId;

    parameterLabel.setText(label, NotificationType::dontSendNotification);
    addAndMakeVisible(&parameterLabel);

    linkButton.setButtonText("Link");
    linkButton.addListener(this);
    addAndMakeVisible(&linkButton);

    slider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    slider.setRange(0.0, 1.0);
    slider.addListener(this);
    addAndMakeVisible(&slider);

    addListener(dynamic_cast<ParameterComponent::Listener *>(parent));
    parent->addAndMakeVisible(this);
}

ParameterComponent::~ParameterComponent() {}

void ParameterComponent::paint(Graphics& g) {
    GrisLookAndFeel *lookAndFeel;
    lookAndFeel = static_cast<GrisLookAndFeel *> (&getLookAndFeel());
    g.fillAll (lookAndFeel->findColour (ResizableWindow::backgroundColourId));
}

void ParameterComponent::resized() {
    parameterLabel.setBounds(0, 0, 150, 20);
    linkButton.setBounds(0, 20, 45, 20);
    slider.setBounds(50, 20, 175, 20);
}

void ParameterComponent::buttonClicked(Button *button) {
    listeners.call([&] (Listener& l) { l.parameterLinkChanged(m_parameterId, button->getToggleState()); });
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

void ParameterComponent::setLinkState(bool state) {
    linkButton.setToggleState(state, NotificationType::dontSendNotification);
    listeners.call([&] (Listener& l) { l.parameterLinkChanged(m_parameterId, state); });
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
    m_distanceEnabled = false;

    p_distance.setEnabled(false);
    p_x.setVisible(false);
    p_y.setVisible(false);

    activatorXY.setButtonText("X-Y");
    activatorXY.addListener(this);
    addAndMakeVisible(&activatorXY);
}

ParametersBoxComponent::~ParametersBoxComponent() {}

void ParametersBoxComponent::setSelectedSource(Source *source) {
    selectedSource = source;

    p_azimuth.setValue(selectedSource->getNormalizedAzimuth());
    p_elevation.setValue(selectedSource->getNormalizedElevation());
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

void ParametersBoxComponent::setLinkState(int parameterId, bool state) {
    if (parameterId == SOURCE_ID_AZIMUTH) {
        p_azimuth.setLinkState(state);
    } else if (parameterId == SOURCE_ID_ELEVATION) {
        p_elevation.setLinkState(state);
    } else if (parameterId == SOURCE_ID_DISTANCE) {
        p_distance.setLinkState(state);
    } else if (parameterId == SOURCE_ID_X) {
        p_x.setLinkState(state);
    } else if (parameterId == SOURCE_ID_Y) {
        p_y.setLinkState(state);
    } else if (parameterId == SOURCE_ID_AZIMUTH_SPAN) {
        p_azimuthSpan.setLinkState(state);
    } else if (parameterId == SOURCE_ID_ELEVATION_SPAN) {
        p_elevationSpan.setLinkState(state);
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

void ParametersBoxComponent::mouseDown(const MouseEvent &event) {
    // Area where the selected source is shown.
    Rectangle<float> area = Rectangle<float>(245, 245, 30, 30);
    if (area.contains(event.getMouseDownPosition().toFloat())) {
        listeners.call([&] (Listener& l) { l.selectedSourceClicked(); });
    }
}

void ParametersBoxComponent::parameterChanged(int parameterId, double value) {
    listeners.call([&] (Listener& l) { l.parameterChanged(parameterId, value); });
}

void ParametersBoxComponent::parameterLinkChanged(int parameterId, bool value) {
    listeners.call([&] (Listener& l) { l.parameterLinkChanged(parameterId, value); });
}

void ParametersBoxComponent::paint(Graphics& g) {
    float x = 250;
    float y = 250;

    GrisLookAndFeel *lookAndFeel;
    lookAndFeel = static_cast<GrisLookAndFeel *> (&getLookAndFeel());
    g.fillAll (lookAndFeel->findColour (ResizableWindow::backgroundColourId));

    g.setColour(selectedSource->getColour());
    g.drawEllipse(x, y, 20, 20, 3);

    g.setColour(Colours::white);
    g.drawFittedText(String(selectedSource->getId()+1), x, y, kSourceDiameter - 2, kSourceDiameter,
                     Justification(Justification::centred), 1);
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
