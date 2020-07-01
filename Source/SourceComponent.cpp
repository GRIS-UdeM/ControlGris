/*
  ==============================================================================

    FieldComponentClickableItem.cpp
    Created: 8 Jun 2020 10:11:39pm
    Author:  samuel

  ==============================================================================
*/

#include "SourceComponent.h"

SourceComponent::SourceComponent(Colour const colour, String const & icon) : mColour(colour), mIcon(icon)
{
    constexpr auto dimension = SOURCE_FIELD_COMPONENT_DIAMETER + MAX_LINE_THICKNESS * 2;
    this->setSize(dimension, dimension);
}

void SourceComponent::setSelected(bool const selected)
{
    mSelected = selected;
    repaint();
}

void SourceComponent::paint(Graphics & g)
{
    auto const lineThickness{ static_cast<float>(mSelected ? MAX_LINE_THICKNESS : 1) };
    auto const saturation{ (mSelected) ? 1.0f : 0.75f };
    auto const colour{ mColour.withSaturation(saturation) };
    auto const center{ getLocalBounds().getCentre().toFloat() };
    auto const area{ Rectangle<float>{ SOURCE_FIELD_COMPONENT_DIAMETER, SOURCE_FIELD_COMPONENT_DIAMETER }.withCentre(
        center) };

    //    g.setColour(Colour(.2f, .2f, .2f, 1.0f));
    g.setColour(colour);
    g.drawEllipse(area, lineThickness);
    g.setGradientFill(ColourGradient(colour.darker(0.8f), center, colour, center / 5.0f, true));
    g.fillEllipse(area);
    g.setColour(Colours::white);
    g.drawFittedText(mIcon, area.getSmallestIntegerContainer(), Justification::centred, 1);
}