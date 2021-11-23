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

#include "cg_SourcePlacement.hpp"

#include "cg_utilities.hpp"

namespace gris
{
//==============================================================================
juce::StringArray const SOURCE_PLACEMENT_STRINGS{ "Left Alternate",  "Right Alternate",
                                                  "Left Clockwise",  "Left Counter Clockwise",
                                                  "Right Clockwise", "Right Counter Clockwise",
                                                  "Top Clockwise",   "Top Counter Clockwise" };

//==============================================================================
std::optional<SourcePlacement> toSourcePlacement(juce::String const & string) noexcept(IS_RELEASE)
{
    return stringToEnum<SourcePlacement>(string, SOURCE_PLACEMENT_STRINGS);
}

//==============================================================================
juce::String const & toString(SourcePlacement const & sourcePlacement) noexcept(IS_RELEASE)
{
    return enumToString(sourcePlacement, SOURCE_PLACEMENT_STRINGS);
}

} // namespace gris
