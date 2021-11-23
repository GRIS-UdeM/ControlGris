/**************************************************************************
 * Copyright 2021 UdeM - GRIS - Samuel Béland & Olivier Belanger          *
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

#include "cg_CartesianVector.hpp"

namespace gris
{
//==============================================================================
float CartesianVector::length() const noexcept
{
    return std::sqrt(length2());
}

//==============================================================================
CartesianVector CartesianVector::crossProduct(CartesianVector const & other) const noexcept
{
    auto const newX = (y * other.z) - (z * other.y);
    auto const newY = (z * other.x) - (x * other.z);
    auto const newZ = (x * other.y) - (y * other.x);
    CartesianVector const unscaledResult{ newX, newY, newZ };

    auto const length = unscaledResult.length();
    auto const result{ unscaledResult / length };

    return result;
}

//==============================================================================
juce::Point<float> CartesianVector::discardZ() const noexcept
{
    return juce::Point<float>{ x, y };
}
} // namespace gris