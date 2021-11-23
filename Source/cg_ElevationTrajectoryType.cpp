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

#include "cg_ElevationTrajectoryType.hpp"

#include "cg_utilities.hpp"

namespace gris
{
//==============================================================================
juce::StringArray const ELEVATION_TRAJECTORY_TYPE_STRINGS{ "Realtime", "Drawing", "Up Down", "Down Up" };

//==============================================================================
std::optional<ElevationTrajectoryType> toElevationTrajectoryType(juce::String const & string) noexcept(IS_RELEASE)
{
    return stringToEnum<ElevationTrajectoryType>(string, ELEVATION_TRAJECTORY_TYPE_STRINGS);
}

//==============================================================================
juce::String const & toString(ElevationTrajectoryType const & elevationTrajectoryType) noexcept(IS_RELEASE)
{
    return enumToString(elevationTrajectoryType, ELEVATION_TRAJECTORY_TYPE_STRINGS);
}
} // namespace gris
