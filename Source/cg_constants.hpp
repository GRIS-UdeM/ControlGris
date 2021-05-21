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

#include "cg_StrongTypes.hpp"

namespace gris
{
//==============================================================================
// Global variables.
constexpr int MIN_FIELD_WIDTH = 300;
constexpr int MAX_NUMBER_OF_SOURCES = 8;
constexpr int NUMBER_OF_POSITION_PRESETS = 50;
constexpr float SOURCE_FIELD_COMPONENT_RADIUS = 12.0f;
constexpr float SOURCE_FIELD_COMPONENT_DIAMETER = SOURCE_FIELD_COMPONENT_RADIUS * 2.0f;
constexpr auto LBAP_FAR_FIELD = 1.666666667f;

constexpr Radians MIN_ELEVATION = Degrees{ 0.0f };
constexpr Radians MAX_ELEVATION = Degrees{ 90.0f };

//==============================================================================
// Spatialisation modes.
enum class SpatMode { dome, cube };

static juce::String const SOURCE_SELECTION_WARNING{ "This source link does not allow individual moves." };

//==============================================================================
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
juce::StringArray const SOURCE_PLACEMENT_SKETCH{
    juce::String("Left Alternate"),  juce::String("Right Alternate"),
    juce::String("Left Clockwise"),  juce::String("Left Counter Clockwise"),
    juce::String("Right Clockwise"), juce::String("Right Counter Clockwise"),
    juce::String("Top Clockwise"),   juce::String("Top Counter Clockwise")
};

//==============================================================================
// Trajectories -> Source Link popup choices.
enum class PositionSourceLink {
    undefined,
    independent,
    circular,
    circularFixedRadius,
    circularFixedAngle,
    circularFullyFixed,
    deltaLock,
    symmetricX,
    symmetricY
};
juce::StringArray const POSITION_SOURCE_LINK_TYPES{
    juce::String("Independent"),           juce::String("Circular"),
    juce::String("Circular Fixed Radius"), juce::String("Circular Fixed Angle"),
    juce::String("Circular Fully Fixed"),  juce::String("Delta Lock"),
    juce::String("Symmetric X"),           juce::String("Symmetric Y")
};

enum class ElevationSourceLink { undefined, independent, fixedElevation, linearMin, linearMax, deltaLock };
juce::StringArray const ELEVATION_SOURCE_LINK_TYPES{ juce::String("Independent"),
                                                     juce::String("Fixed Elevation"),
                                                     juce::String("Linear Min"),
                                                     juce::String("Linear Max"),
                                                     juce::String("Delta Lock") };

//==============================================================================
// Trajectories -> Trajectory Type popup choices.
enum class PositionTrajectoryType {
    undefined,
    realtime,
    drawing,
    circleClockwise,
    circleCounterClockwise,
    ellipseClockwise,
    ellipseCounterClockwise,
    spiralClockwiseOutIn,
    spiralCounterClockwiseOutIn,
    spiralClockwiseInOut,
    spiralCounterClockwiseInOut,
    squareClockwise,
    squareCounterClockwise,
    triangleClockwise,
    triangleCounterClockwise
};
juce::StringArray const POSITION_TRAJECTORY_TYPE_TYPES{ juce::String("Realtime"),
                                                        juce::String("Drawing"),
                                                        juce::String("Circle Clockwise"),
                                                        juce::String("Circle Counter Clockwise"),
                                                        juce::String("Ellipse Clockwise"),
                                                        juce::String("Ellipse Counter Clockwise"),
                                                        juce::String("Spiral Clockwise Out In"),
                                                        juce::String("Spiral Counter Clockwise Out In"),
                                                        juce::String("Spiral Clockwise In Out"),
                                                        juce::String("Spiral Counter Clockwise In Out"),
                                                        juce::String("Square Clockwise"),
                                                        juce::String("Square Counter Clockwise"),
                                                        juce::String("Triangle Clockwise"),
                                                        juce::String("Triangle Counter Clockwise") };

enum class ElevationTrajectoryType { undefined, realtime, drawing, downUp, upDown };
juce::StringArray const ELEVATION_TRAJECTORY_TYPE_TYPES{ juce::String("Realtime"),
                                                         juce::String("Drawing"),
                                                         juce::String("Up Down"),
                                                         juce::String("Down Up") };

//==============================================================================
// Fix position data headers.
juce::StringArray const FIXED_POSITION_DATA_HEADERS{
    // TODO: is this useful?
    juce::String("ID"),   juce::String("S1_X"), juce::String("S1_Y"), juce::String("S1_Z"), juce::String("S2_X"),
    juce::String("S2_Y"), juce::String("S2_Z"), juce::String("S3_X"), juce::String("S3_Y"), juce::String("S3_Z"),
    juce::String("S4_X"), juce::String("S4_Y"), juce::String("S4_Z"), juce::String("S5_X"), juce::String("S5_Y"),
    juce::String("S5_Z"), juce::String("S6_X"), juce::String("S6_Y"), juce::String("S6_Z"), juce::String("S7_X"),
    juce::String("S7_Y"), juce::String("S7_Z"), juce::String("S8_X"), juce::String("S8_Y"), juce::String("S8_Z"),
    juce::String("T1_X"), juce::String("T1_Y"), juce::String("T1_Z"), juce::String("T2_X"), juce::String("T2_Y"),
    juce::String("T2_Z")
};

juce::String const FIXED_POSITION_DATA_TAG("Fix_Position_Data");

namespace Automation
{
namespace Ids
{
juce::String extern const X;
juce::String extern const Y;
juce::String extern const Z;
juce::String extern const POSITION_SOURCE_LINK;
juce::String extern const ELEVATION_SOURCE_LINK;
juce::String extern const AZIMUTH_SPAN;
juce::String extern const ELEVATION_SPAN;
juce::String extern const POSITION_PRESET;
} // namespace Ids

enum class Enum { x, y, z, positionSourceLink, elevationSourceLink, azimuthSpan, elevationSpan, positionPreset };

Enum idToEnum(juce::String const & name);

} // namespace Automation

} // namespace gris