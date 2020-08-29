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

#include "SourceComponent.h"

#include "ControlGrisConstants.h"

//==============================================================================
SourceComponent::SourceComponent(Colour const colour, String const & icon) : mColour(colour), mIcon(icon)
{
    constexpr auto dimension = SOURCE_FIELD_COMPONENT_DIAMETER + MAX_LINE_THICKNESS * 2.0f;
    constexpr auto rounded{ static_cast<int>(dimension + 0.5f) };
    this->setSize(rounded, rounded);
}

//==============================================================================
void SourceComponent::setSelected(bool const selected)
{
    mSelected = selected;
    repaint();
}

//==============================================================================
void SourceComponent::paint(Graphics & g)
{
    auto const lineThickness{ static_cast<float>(mSelected ? MAX_LINE_THICKNESS : 1) };
    auto const saturation{ (mSelected) ? 1.0f : 0.8f };
    auto const colour{ mColour.withMultipliedSaturation(saturation) };
    auto const center{ getLocalBounds().getCentre().toFloat() };
    auto const area{ Rectangle<float>{ SOURCE_FIELD_COMPONENT_DIAMETER, SOURCE_FIELD_COMPONENT_DIAMETER }.withCentre(
        center) };

    g.setColour(colour);
    g.drawEllipse(area, lineThickness);
    g.setGradientFill(ColourGradient(colour.darker(0.8f), center, colour, center / 5.0f, true));
    g.fillEllipse(area);
    g.setColour(Colours::white);
    g.drawFittedText(mIcon, area.getSmallestIntegerContainer(), Justification::centred, 1);
}
