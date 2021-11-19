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

#pragma once

#include <JuceHeader.h>

namespace gris
{
//==============================================================================
/** A classical [x,y,z] coordinate. */
struct CartesianVector {
    float x;
    float y;
    float z;
    //==============================================================================
    [[nodiscard]] constexpr bool operator==(CartesianVector const & other) const noexcept;
    //==============================================================================
    [[nodiscard]] constexpr CartesianVector operator-() const noexcept;
    [[nodiscard]] constexpr CartesianVector operator-(CartesianVector const & other) const noexcept;
    [[nodiscard]] constexpr CartesianVector operator/(float scalar) const noexcept;
    //==============================================================================
    /** @return the Euclidean distance. */
    [[nodiscard]] float length() const noexcept;
    /** @return the Euclidean distance squared. */
    [[nodiscard]] constexpr float length2() const noexcept;
    [[nodiscard]] constexpr CartesianVector mean(CartesianVector const & other) const noexcept;
    [[nodiscard]] constexpr float dotProduct(CartesianVector const & other) const noexcept;
    [[nodiscard]] float angleWith(CartesianVector const & other) const noexcept;
    [[nodiscard]] juce::Point<float> discardZ() const noexcept;
    [[nodiscard]] CartesianVector crossProduct(CartesianVector const & other) const noexcept;
};

//==============================================================================
constexpr bool CartesianVector::operator==(CartesianVector const & other) const noexcept
{
    return x == other.x && y == other.y && z == other.z;
}

//==============================================================================
constexpr CartesianVector CartesianVector::operator-(CartesianVector const & other) const noexcept
{
    CartesianVector const result{ x - other.x, y - other.y, z - other.z };
    return result;
}

//==============================================================================
constexpr CartesianVector CartesianVector::operator/(float const scalar) const noexcept
{
    CartesianVector const result{ x / scalar, y / scalar, z / scalar };
    return result;
}

//==============================================================================
constexpr float CartesianVector::length2() const noexcept
{
    return x * x + y * y + z * z;
}

//==============================================================================
constexpr CartesianVector CartesianVector::operator-() const noexcept
{
    return CartesianVector{ -x, -y, -z };
}

//==============================================================================
constexpr float CartesianVector::dotProduct(CartesianVector const & other) const noexcept
{
    return x * other.x + y * other.y + z * other.z;
}

//==============================================================================
constexpr CartesianVector CartesianVector::mean(CartesianVector const & other) const noexcept
{
    auto const newX{ (x + other.x) * 0.5f };
    auto const newY{ (y + other.y) * 0.5f };
    auto const newZ{ (z + other.z) * 0.5f };

    CartesianVector const result{ newX, newY, newZ };
    return result;
}
} // namespace gris