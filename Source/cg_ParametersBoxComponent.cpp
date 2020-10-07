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

#include "cg_ParametersBoxComponent.hpp"

namespace gris
{
//==============================================================================
ParametersBoxComponent::ParametersBoxComponent(GrisLookAndFeel & grisLookAndFeel) : mGrisLookAndFeel(grisLookAndFeel)
{
    mAzimuthLabel.setText("Azimuth Span", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(&mAzimuthLabel);

    mElevationLabel.setText("Elevation Span", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(&mElevationLabel);

    mAzimuthSpan.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    mAzimuthSpan.setRange(0.0, 1.0);
    mAzimuthSpan.addListener(this);
    mAzimuthSpan.onDragStart
        = [&]() { mListeners.call([&](Listener & l) { l.parametersBoxAzimuthSpanDragStarted(); }); };
    mAzimuthSpan.onDragEnd = [&]() { mListeners.call([&](Listener & l) { l.parametersBoxAzimuthSpanDragEnded(); }); };
    mElevationSpan.onDragStart
        = [&]() { mListeners.call([&](Listener & l) { l.parametersBoxElevationSpanDragStarted(); }); };
    mElevationSpan.onDragEnd
        = [&]() { mListeners.call([&](Listener & l) { l.parametersBoxElevationSpanDragEnded(); }); };
    addAndMakeVisible(&mAzimuthSpan);

    mElevationSpan.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    mElevationSpan.setRange(0.0, 1.0);
    mElevationSpan.addListener(this);
    addAndMakeVisible(&mElevationSpan);
}

//==============================================================================
void ParametersBoxComponent::setSelectedSource(Source * source)
{
    if (mSelectedSource != source || source->getAzimuthSpan().toFloat() != mAzimuthSpan.getValue()
        || mElevationSpan.getValue() != source->getElevationSpan().toFloat()) {
        mSelectedSource = source;
        mAzimuthSpan.setValue(source->getAzimuthSpan().toFloat(), juce::NotificationType::dontSendNotification);
        mElevationSpan.setValue(source->getElevationSpan().toFloat(), juce::NotificationType::dontSendNotification);
        repaint();
    }
}

//==============================================================================
void ParametersBoxComponent::setDistanceEnabled(bool const distanceEnabled)
{
    mDistanceEnabled = distanceEnabled;
    resized();
}

//==============================================================================
void ParametersBoxComponent::setSpanLinkState(bool const spanLinkState)
{
    mSpanLinked = spanLinkState;
    repaint();
}

//==============================================================================
void ParametersBoxComponent::mouseDown(juce::MouseEvent const & event)
{
    float const x{ getWidth() - 35.0f };
    float constexpr y = 15;

    // Area where the spanLinked arrow is shown.
    juce::Rectangle<float> const spanLinkedArrowArea{ 245.0f, 25.0f, 45.0f, 25.0f };
    if (spanLinkedArrowArea.contains(event.getMouseDownPosition().toFloat())) {
        mSpanLinked = !mSpanLinked;
        repaint();
    }

    // Area where the selected source is shown.
    juce::Rectangle<float> const selectedSourceArea{ x - 5.0f, y - 5.0f, 30.0f, 30.0f };
    if (selectedSourceArea.contains(event.getMouseDownPosition().toFloat())) {
        mListeners.call([&](Listener & l) { l.parametersBoxSelectedSourceClicked(); });
    }
}

//==============================================================================
void ParametersBoxComponent::sliderValueChanged(juce::Slider * slider)
{
    auto const value{ slider->getValue() };
    auto const parameterId{ (slider == &mAzimuthSpan) ? SourceParameter::azimuthSpan : SourceParameter::elevationSpan };

    mListeners.call([&](Listener & l) { l.parametersBoxParameterChanged(parameterId, value); });

    if (mSpanLinked) {
        if (parameterId == SourceParameter::azimuthSpan) {
            mElevationSpan.setValue(value, juce::NotificationType::sendNotificationAsync);
            mListeners.call(
                [&](Listener & l) { l.parametersBoxParameterChanged(SourceParameter::elevationSpan, value); });
        } else if (parameterId == SourceParameter::elevationSpan) {
            mAzimuthSpan.setValue(value, juce::NotificationType::sendNotificationAsync);
            mListeners.call(
                [&](Listener & l) { l.parametersBoxParameterChanged(SourceParameter::azimuthSpan, value); });
        }
    }
}

//==============================================================================
void ParametersBoxComponent::paint(juce::Graphics & g)
{
    auto constexpr y = 15.0f;
    auto const x{ getWidth() - 35.0f };

    g.fillAll(mGrisLookAndFeel.findColour(juce::ResizableWindow::backgroundColourId));

    if (mSpanLinked)
        g.setColour(juce::Colours::orange);
    else
        g.setColour(juce::Colours::black);
    g.drawArrow(juce::Line<float>(285.0f, 34.0f, 245.0f, 34.0f), 4, 10, 10);
    g.drawArrow(juce::Line<float>(250.0f, 34.0f, 290.0f, 34.0f), 4, 10, 10);

    juce::Rectangle<float> area{ x, y, 20, 20 };
    area.expand(3, 3);
    g.setColour(juce::Colour(.2f, .2f, .2f, 1.0f));
    g.drawEllipse(area.translated(.5f, .5f), 1.0f);
    g.setGradientFill(juce::ColourGradient(mSelectedSource->getColour().withSaturation(1.0f).darker(1.0f),
                                           x + SOURCE_FIELD_COMPONENT_RADIUS,
                                           y + SOURCE_FIELD_COMPONENT_RADIUS,
                                           mSelectedSource->getColour().withSaturation(1.0f),
                                           x,
                                           y,
                                           true));
    g.fillEllipse(area);

    g.setColour(juce::Colours::white);
    g.drawFittedText(mSelectedSource->getId().toString(),
                     area.getSmallestIntegerContainer(),
                     juce::Justification::centred,
                     1);
}

//==============================================================================
void ParametersBoxComponent::resized()
{
    mAzimuthLabel.setBounds(5, 3, 225, 20);
    mElevationLabel.setBounds(305, 3, 225, 20);
    mAzimuthSpan.setBounds(5, 23, 225, 20);
    mElevationSpan.setBounds(305, 23, 225, 20);
}

} // namespace gris