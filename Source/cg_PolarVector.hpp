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

#include "cg_CartesianVector.hpp"
#include "cg_StrongTypes.hpp"

namespace gris
{
static constexpr Radians DEFAULT_ELEVATION_COMPARE_TOLERANCE{ Degrees{ 5.0f } };
//==============================================================================
/** A polar coordinate.  */
struct PolarVector {
    Radians azimuth;
    Radians elevation;
    float length;
    //==============================================================================
    [[nodiscard]] constexpr bool operator==(PolarVector const & other) const noexcept;
    [[nodiscard]] CartesianVector toCartesian() const noexcept;
    [[nodiscard]] constexpr PolarVector normalized() const noexcept;
    [[nodiscard]] constexpr bool isOnSameElevation(PolarVector const & other,
                                                   Radians tolerance
                                                   = DEFAULT_ELEVATION_COMPARE_TOLERANCE) const noexcept;
    [[nodiscard]] static PolarVector fromCartesian(CartesianVector const & pos) noexcept;
};

//==============================================================================
constexpr bool PolarVector::operator==(PolarVector const & other) const noexcept
{
    return azimuth == other.azimuth && elevation == other.elevation && length == other.length;
}

//==============================================================================
constexpr PolarVector PolarVector::normalized() const noexcept
{
    return PolarVector{ azimuth, elevation, 1.0f };
}

//==============================================================================
constexpr bool PolarVector::isOnSameElevation(PolarVector const & other, Radians const tolerance) const noexcept
{
    return elevation > other.elevation - tolerance && elevation < other.elevation + tolerance;
}
} // namespace gris