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

ParameterComponent::~ParameterComponent() {
    setLookAndFeel(nullptr);
}

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
    slider.setValue(value, NotificationType::sendNotificationAsync);
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
    p_azimuthSpan(SOURCE_ID_AZIMUTH_SPAN, "Azimuth Span", this),
    p_elevationSpan(SOURCE_ID_ELEVATION_SPAN, "Elevation Span", this)
{
    m_distanceEnabled = false;
    m_spanLinked = false;
}

ParametersBoxComponent::~ParametersBoxComponent() {}

void ParametersBoxComponent::setSelectedSource(Source *source) {
    selectedSource = source;
    p_azimuthSpan.setValue(selectedSource->getAzimuthSpan());
    p_elevationSpan.setValue(selectedSource->getElevationSpan());
    repaint();
}

void ParametersBoxComponent::setDistanceEnabled(bool shouldBeEnabled) {
    m_distanceEnabled = shouldBeEnabled;
    resized();
}

bool ParametersBoxComponent::getLinkState(int parameterId) {
    if (parameterId == SOURCE_ID_AZIMUTH_SPAN) {
        return p_azimuthSpan.getLinkState();
    } else if (parameterId == SOURCE_ID_ELEVATION_SPAN) {
        return p_elevationSpan.getLinkState();
    } else {
        return false;
    }
}

void ParametersBoxComponent::setLinkState(int parameterId, bool state) {
    if (parameterId == SOURCE_ID_AZIMUTH_SPAN) {
        p_azimuthSpan.setLinkState(state);
    } else if (parameterId == SOURCE_ID_ELEVATION_SPAN) {
        p_elevationSpan.setLinkState(state);
    }
}

bool ParametersBoxComponent::getSpanLinkState() {
    return m_spanLinked;
}

void ParametersBoxComponent::setSpanLinkState(bool state) {
    m_spanLinked = state;
    repaint();
}

void ParametersBoxComponent::mouseDown(const MouseEvent &event) {
    float x = getWidth() - 35;
    float y = 15;

    // Area where the spanLinked arrow is shown.
    Rectangle<float> area = Rectangle<float>(245.f, 25.f, 45.f, 25.f);
    if (area.contains(event.getMouseDownPosition().toFloat())) {
        m_spanLinked = ! m_spanLinked;
        listeners.call([&] (Listener& l) { l.parametersBoxSpanLinkChanged(m_spanLinked); });
        repaint();
    }

    // Area where the selected source is shown.
    area = Rectangle<float>(x-5, y-5, 30, 30);
    if (area.contains(event.getMouseDownPosition().toFloat())) {
        listeners.call([&] (Listener& l) { l.parametersBoxSelectedSourceClicked(); });
    }
}

void ParametersBoxComponent::parameterChanged(int parameterId, double value) {
    listeners.call([&] (Listener& l) { l.parametersBoxParameterChanged(parameterId, value); });
    if (m_spanLinked) {
        if (parameterId == SOURCE_ID_AZIMUTH_SPAN) {
            p_elevationSpan.setValue(value);
            listeners.call([&] (Listener& l) { l.parametersBoxParameterChanged(SOURCE_ID_ELEVATION_SPAN, value); });
        } else if (parameterId == SOURCE_ID_ELEVATION_SPAN) {
            p_azimuthSpan.setValue(value);
            listeners.call([&] (Listener& l) { l.parametersBoxParameterChanged(SOURCE_ID_AZIMUTH_SPAN, value); });
        }
    }
}

void ParametersBoxComponent::parameterLinkChanged(int parameterId, bool value) {
    listeners.call([&] (Listener& l) { l.parametersBoxLinkChanged(parameterId, value); });
}

void ParametersBoxComponent::paint(Graphics& g) {
    float x = getWidth() - 35;
    float y = 15;

    GrisLookAndFeel *lookAndFeel;
    lookAndFeel = static_cast<GrisLookAndFeel *> (&getLookAndFeel());
    g.fillAll (lookAndFeel->findColour (ResizableWindow::backgroundColourId));

    if (m_spanLinked)
        g.setColour(Colours::orange);
    else
        g.setColour(Colours::black);
    g.drawArrow(Line<float>(285, 34, 245, 34), 4, 10, 10);
    g.drawArrow(Line<float>(250, 34, 290, 34), 4, 10, 10);

//    g.setColour(selectedSource->getColour());
    Rectangle<float> area(x, y, 20, 20);
    area.expand(3, 3);
    g.setColour(Colour(.2f, .2f, .2f, 1.f));
    g.drawEllipse(area.translated(.5f, .5f), 1.f);
    g.setGradientFill(ColourGradient(selectedSource->getColour().withSaturation(1.0f).darker(1.f), x + kSourceRadius, y + kSourceRadius,
                                     selectedSource->getColour().withSaturation(1.0f), x, y, true));
    g.fillEllipse(area);

    g.setColour(Colours::white);
    g.drawFittedText(String(selectedSource->getId()+1), area.getSmallestIntegerContainer(), Justification(Justification::centred), 1);
}

void ParametersBoxComponent::resized() {
    p_azimuthSpan.setBounds(5, 3, 225, 40);
    p_elevationSpan.setBounds(305, 3, 225, 40);
}

