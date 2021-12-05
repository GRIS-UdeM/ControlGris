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

#include "cg_PositionTrajectoryType.hpp"

#include "cg_utilities.hpp"

namespace gris
{
//==============================================================================
juce::StringArray const POSITION_TRAJECTORY_TYPE_STRINGS{ "Drawing",
                                                          "Circle Clockwise",
                                                          "Circle Counter Clockwise",
                                                          "Ellipse Clockwise",
                                                          "Ellipse Counter Clockwise",
                                                          "Spiral Clockwise Out In",
                                                          "Spiral Counter Clockwise Out In",
                                                          "Spiral Clockwise In Out",
                                                          "Spiral Counter Clockwise In Out",
                                                          "Square Clockwise",
                                                          "Square Counter Clockwise",
                                                          "Triangle Clockwise",
                                                          "Triangle Counter Clockwise" };

//==============================================================================
std::optional<PositionTrajectoryType> toPositionTrajectoryType(juce::String const & string) noexcept(IS_RELEASE)
{
    return stringToEnum<PositionTrajectoryType>(string, POSITION_TRAJECTORY_TYPE_STRINGS);
}

//==============================================================================
juce::String const & toString(PositionTrajectoryType const & positionTrajectoryType) noexcept(IS_RELEASE)
{
    return enumToString(positionTrajectoryType, POSITION_TRAJECTORY_TYPE_STRINGS);
}
} // namespace gris
