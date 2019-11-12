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

ParametersBoxComponent::ParametersBoxComponent()
{
    m_distanceEnabled = false;
    m_spanLinked = false;

    azimuthLabel.setText("Azimuth Span", NotificationType::dontSendNotification);
    addAndMakeVisible(&azimuthLabel);

    elevationLabel.setText("Elevation Span", NotificationType::dontSendNotification);
    addAndMakeVisible(&elevationLabel);

    azimuthSpan.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    azimuthSpan.setRange(0.0, 1.0);
    azimuthSpan.addListener(this);
    addAndMakeVisible(&azimuthSpan);

    elevationSpan.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    elevationSpan.setRange(0.0, 1.0);
    elevationSpan.addListener(this);
    addAndMakeVisible(&elevationSpan);
}

ParametersBoxComponent::~ParametersBoxComponent() {}

void ParametersBoxComponent::setSelectedSource(Source *source) {
    selectedSource = source;
    azimuthSpan.setValue(selectedSource->getAzimuthSpan(), NotificationType::dontSendNotification);
    elevationSpan.setValue(selectedSource->getElevationSpan(), NotificationType::dontSendNotification);
    repaint();
}

void ParametersBoxComponent::setDistanceEnabled(bool shouldBeEnabled) {
    m_distanceEnabled = shouldBeEnabled;
    resized();
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

void ParametersBoxComponent::sliderValueChanged(Slider *slider) {
    float value = slider->getValue();
    int parameterId = (slider == &azimuthSpan) ? SOURCE_ID_AZIMUTH_SPAN : SOURCE_ID_ELEVATION_SPAN;

    listeners.call([&] (Listener& l) { l.parametersBoxParameterChanged(parameterId, value); });

    if (m_spanLinked) {
        if (parameterId == SOURCE_ID_AZIMUTH_SPAN) {
            elevationSpan.setValue(value, NotificationType::sendNotificationAsync);
            listeners.call([&] (Listener& l) { l.parametersBoxParameterChanged(SOURCE_ID_ELEVATION_SPAN, value); });
        } else if (parameterId == SOURCE_ID_ELEVATION_SPAN) {
            azimuthSpan.setValue(value, NotificationType::sendNotificationAsync);
            listeners.call([&] (Listener& l) { l.parametersBoxParameterChanged(SOURCE_ID_AZIMUTH_SPAN, value); });
        }
    }
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
    azimuthLabel.setBounds(5, 3, 225, 20);
    elevationLabel.setBounds(305, 3, 225, 20);
    azimuthSpan.setBounds(5, 23, 225, 20);
    elevationSpan.setBounds(305, 23, 225, 20);
}
