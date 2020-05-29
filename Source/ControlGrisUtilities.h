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
#pragma once

#include <cmath>
#include <type_traits>

#include "../JuceLibraryCode/JuceHeader.h"

template<typename Float>
constexpr Float getFloatPrecision(Float const value, size_t const precision)
{
    static_assert(std::is_floating_point_v<Float>);
    return std::floor((value * std::pow(10, precision) + static_cast<Float>(0.5))) / std::pow(10, precision);
}

template<typename Float>
constexpr Float degreeToRadian(Float const degree)
{
    static_assert(std::is_floating_point_v<Float>);
    return (degree / static_cast<Float>(360.0) * MathConstants<Float>::twoPi);
}

//==============================================================================
class XmlElementDataSorter
{
    String attributeToSort;
    int direction;

public:
    //==============================================================================
    XmlElementDataSorter(String const & attributeToSortBy, bool forwards)
        : attributeToSort(attributeToSortBy)
        , direction(forwards ? 1 : -1)
    {
    }
    //==============================================================================
    int compareElements(XmlElement * first, XmlElement * second) const
    {
        int result;
        if (first->getDoubleAttribute(attributeToSort) < second->getDoubleAttribute(attributeToSort))
            result = -1;
        else if (first->getDoubleAttribute(attributeToSort) > second->getDoubleAttribute(attributeToSort))
            result = 1;
        else
            result = 0;

        return direction * result;
    }
};
