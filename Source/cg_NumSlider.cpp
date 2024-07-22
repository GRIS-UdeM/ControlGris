/**************************************************************************
 * Copyright 2024 UdeM - GRIS - Gaël LANE LÉPINE                          *
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
 * <https://www.gnu.org/licenses/>.                                       *
 *************************************************************************/

#include "cg_NumSlider.h"

#include "cg_ControlGrisLookAndFeel.hpp"
namespace gris
{

//==============================================================================
NumSlider::NumSlider(GrisLookAndFeel & grisLookAndFeel)
    : mGrisLookAndFeel(grisLookAndFeel)
{
    setLookAndFeel(&grisLookAndFeel);
    setTitle("NumSlider");
    setRange(0.0, 1.0, 0.001);
    setSliderStyle(juce::Slider::LinearBar);
    setSliderSnapsToMousePosition(false);
    setNumDecimalPlacesToDisplay(3);
    setScrollWheelEnabled(true);
    setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, getWidth(), getHeight());
    setTextBoxIsEditable(false);
    setDoubleClickReturnValue(true, 0.0);
}

//==============================================================================
void NumSlider::mouseWheelMove(const juce::MouseEvent & event, const juce::MouseWheelDetails & wheel)
{
    juce::Time currentTime = juce::Time::getCurrentTime();
    double timeDiff = (currentTime - mLastTime).inMilliseconds();
    double valueDiff = wheel.deltaY * getInterval();
    double velocity = valueDiff / timeDiff * 1000;
    double newValue = mLastValue - velocity;

    setValue(newValue);

    mLastTime = currentTime;
    mLastValue = newValue;
}

//==============================================================================
void NumSlider::paint(juce::Graphics & g)
{
    auto rangeVals{ getRange() };
    auto val{ getValue() };
    auto bounds{ getLocalBounds() };

    if (val > 0) {
        juce::Rectangle<float> drawRec;
        g.setColour(mGrisLookAndFeel.getOnColor());
        drawRec = juce::Rectangle<float>{ 0,
                                          0,
                                          static_cast<float>(bounds.getWidth() * (val / rangeVals.getLength())),
                                          static_cast<float>(bounds.getHeight()) };
        g.fillRect(drawRec);

        g.setColour(mGrisLookAndFeel.getLightColor());
        drawRec = juce::Rectangle<float>{ static_cast<float>(bounds.getWidth() * (val / rangeVals.getLength())),
                                          0,
                                          static_cast<float>(bounds.getWidth()),
                                          static_cast<float>(bounds.getHeight()) };
        g.fillRect(drawRec);
    } else {
        g.setColour(mGrisLookAndFeel.getLightColor());
        g.fillRect(bounds);
    }

    g.setColour(mGrisLookAndFeel.getDarkColor());
    g.drawText(getTextFromValue(getValue()), bounds, juce::Justification::centred);
}

//==============================================================================
void NumSlider::valueChanged()
{
    mLastValue = getValue();
}

//==============================================================================
void NumSlider::mouseUp(const juce::MouseEvent & event)
{
    if (event.mods.isRightButtonDown()) {
        auto sliderEditor{ std::make_unique<juce::TextEditor>("SliderEditor") };
        sliderEditor->addListener(this);
        sliderEditor->setColour(juce::ColourSelector::backgroundColourId, juce::Colours::transparentBlack);
        sliderEditor->setInputRestrictions(5, "0123456789,.");
        sliderEditor->setMultiLine(false);
        sliderEditor->setJustification(juce::Justification::centred);
        sliderEditor->setSize(60, 20);

        juce::CallOutBox::launchAsynchronously(std::move(sliderEditor), getScreenBounds(), nullptr);
    }

    Slider::mouseUp(event);
}

//==============================================================================
void NumSlider::textEditorReturnKeyPressed(juce::TextEditor & ed)
{
    if (!ed.getText().isEmpty()) {
        auto val = ed.getText().replace(",", ".").getDoubleValue();
        mLastValue = val;
        setValue(val);
    }

    auto callOutBox = dynamic_cast<juce::CallOutBox *>(ed.getParentComponent());

    if (callOutBox != nullptr) {
        callOutBox->dismiss();
    }
}

//==============================================================================
void NumSlider::textEditorEscapeKeyPressed(juce::TextEditor & ed)
{
    auto callOutBox = dynamic_cast<juce::CallOutBox *>(ed.getParentComponent());

    if (callOutBox != nullptr) {
        callOutBox->dismiss();
    }
}

} // namespace gris