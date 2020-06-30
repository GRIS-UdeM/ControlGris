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
    int const lineThickness{ (mSelected) ? MAX_LINE_THICKNESS : 1 };
    float const saturation{ (mSelected) ? 1.0f : 0.75f };
    Rectangle<float> area{ MAX_LINE_THICKNESS,
                           MAX_LINE_THICKNESS,
                           SOURCE_FIELD_COMPONENT_DIAMETER,
                           SOURCE_FIELD_COMPONENT_DIAMETER };
    area.expand(lineThickness, lineThickness);
    g.setColour(Colour(.2f, .2f, .2f, 1.0f));
    g.drawEllipse(area.translated(.5f, .5f), 1.0f);
    g.setGradientFill(ColourGradient(mColour.withSaturation(saturation).darker(1.0f),
                                     MAX_LINE_THICKNESS + SOURCE_FIELD_COMPONENT_RADIUS,
                                     MAX_LINE_THICKNESS + SOURCE_FIELD_COMPONENT_RADIUS,
                                     mColour.withSaturation(saturation),
                                     MAX_LINE_THICKNESS,
                                     MAX_LINE_THICKNESS,
                                     true));
    g.fillEllipse(area);
    g.setColour(Colours::white);
    g.drawFittedText(mIcon, area.getSmallestIntegerContainer(), Justification::centred, 1);
}