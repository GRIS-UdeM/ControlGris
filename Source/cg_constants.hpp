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

#include "cg_SpatMode.hpp"
#include "cg_StrongTypes.hpp"

namespace gris
{
//==============================================================================
constexpr int MIN_FIELD_WIDTH_PIXELS = 300;
constexpr int MAX_NUMBER_OF_SOURCES = 8;
constexpr int NUMBER_OF_POSITION_PRESETS = 50;
constexpr float SOURCE_FIELD_COMPONENT_RADIUS_PIXELS = 12.0f;
constexpr float SOURCE_FIELD_COMPONENT_DIAMETER = SOURCE_FIELD_COMPONENT_RADIUS_PIXELS * 2.0f;
/** Considering that the radius of a dome is 1, how far can a source or speaker go in any direction in and CUBE? */
constexpr auto LBAP_FAR_FIELD_RATIO = 1.666666667f;

constexpr Radians MIN_ELEVATION = Degrees{ 0.0f };
constexpr Radians MAX_ELEVATION = Degrees{ 90.0f };

//==============================================================================
// Defaults
constexpr int DEFAULT_OSC_PORT = 18032;
constexpr int DEFAULT_OSC_INPUT_PORT = 8000;
constexpr int DEFAULT_OSC_OUTPUT_PORT = 9000;
constexpr int DEFAULT_NUM_SOURCES = 2;
constexpr int DEFAULT_FIRST_SOURCE_ID = 1;
constexpr int DEFAULT_OSC_OUTPUT_PLUGIN_ID = 1;
constexpr SpatMode DEFAULT_SPAT_MODE = SpatMode::dome;
constexpr double DEFAULT_BPM = 120.0;
constexpr bool DEFAULT_OSC_ACTIVE = true;

extern juce::String const DEFAULT_OSC_ADDRESS;
extern juce::String const DEFAULT_OSC_OUTPUT_ADDRESS;

//==============================================================================
extern juce::String const SOURCE_SELECTION_WARNING;
//==============================================================================
// Fix position data headers.
extern juce::StringArray const FIXED_POSITION_DATA_HEADERS;
extern juce::String const FIXED_POSITION_DATA_TAG;

/** The parameters that are part of a ControlGRIS state. */
namespace parameters
{
/** The parameters that CAN be automated in the DAW. */
namespace dynamic
{
juce::String extern const X;
juce::String extern const Y;
juce::String extern const Z;
juce::String extern const POSITION_SOURCE_LINK;
juce::String extern const ELEVATION_SOURCE_LINK;
juce::String extern const AZIMUTH_SPAN;
juce::String extern const ELEVATION_SPAN;
juce::String extern const POSITION_PRESET;
} // namespace dynamic

/** The parameters that CANNOT be automated in the DAW. */
namespace statics
{
juce::String extern const OSC_FORMAT;
juce::String extern const OSC_PORT;
juce::String extern const OSC_ADDRESS;
juce::String extern const OSC_ACTIVE;
juce::String extern const OSC_INPUT_PORT;
juce::String extern const OSC_INPUT_CONNECTED;
juce::String extern const OSC_OUTPUT_ADDRESS;
juce::String extern const OSC_OUTPUT_PORT;
juce::String extern const OSC_OUTPUT_CONNECTED;
juce::String extern const NUM_SOURCES;
juce::String extern const FIRST_SOURCE_ID;
juce::String extern const OSC_OUTPUT_PLUGIN_ID;

juce::String extern const POSITION_TRAJECTORY_TYPE;
juce::String extern const ELEVATION_TRAJECTORY_TYPE;
juce::String extern const POSITION_BACK_AND_FORTH;
juce::String extern const ELEVATION_BACK_AND_FORTH;
juce::String extern const POSITION_DAMPENING_CYCLES;
juce::String extern const ELEVATION_DAMPENING_CYCLES;
juce::String extern const DEVIATION_PER_CYCLE;
juce::String extern const CYCLE_DURATION;
juce::String extern const DURATION_UNIT;
} // namespace statics
} // namespace parameters
} // namespace gris
