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

#include "cg_IsRelease.hpp"

#include <JuceHeader.h>

#include <optional>

namespace gris
{
// Configuration -> Source tab -> Source Placement popup choices.
enum class SourcePlacement {
    undefined,
    leftAlternate,
    rightAlternate,
    leftClockwise,
    leftCounterClockwise,
    rightClockwise,
    rightCounterClockwise,
    topClockwise,
    topCounterClockwise
};

//==============================================================================
extern juce::StringArray const SOURCE_PLACEMENT_STRINGS;
std::optional<SourcePlacement> toSourcePlacement(juce::String const & string) noexcept(IS_RELEASE);
juce::String const & toString(SourcePlacement const & sourcePlacement) noexcept(IS_RELEASE);
} // namespace gris