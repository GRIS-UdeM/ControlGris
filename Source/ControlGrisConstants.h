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

// Global variables.
//------------------
constexpr int MIN_FIELD_WIDTH = 300;
constexpr int MAX_NUMBER_OF_SOURCES = 8;
constexpr int NUMBER_OF_POSITION_PRESETS = 50;

// Spatialisation modes.
//----------------------
enum class SpatMode {
    VBAP = 0,
    LBAP
};

const String WARNING_CIRCULAR_SOURCE_SELECTION ("Only source 1 can be selected \nin circular or symmetric links!");

// Configuration -> Source tab -> Source Placement popup choices.
//---------------------------------------------------------------
enum class SourcePlacement { 
    undefined,
    leftAlternate = 1,
    rightAlternate,
    leftClockwise,
    leftCounterClockwise,
    rightClockwise,
    rightCounterClockwise,
    topClockwise,
    topCounterClockwise
};
const StringArray SOURCE_PLACEMENT_SKETCH {String("Left Alternate"), String("Right Alternate"),
                                           String("Left Clockwise"), String("Left Counter Clockwise"),
                                           String("Right Clockwise"), String("Right Counter Clockwise"),
                                           String("Top Clockwise"), String("Top Counter Clockwise")};

// Trajectories -> Source Link popup choices.
//-------------------------------------------
enum class SourceLink { 
    undefined,
    independent = 1,
    circular,
    circularFixedRadius,
    circularFixedAngle,
    circularFullyFixed,
    circularDeltaLock,
    linkSymmetricX,
    linkSymmetricY
};
const StringArray SOURCE_LINK_TYPES       {String("Independent"), String("Circular"),
                                           String("Circular Fixed Radius"), String("Circular Fixed Angle"),
                                           String("Circular Fully Fixed"), String("Delta Lock"),
                                           String("Symmetric X"), String("Symmetric Y")};

enum class SourceLinkAlt { 
    undefined,
    independent = 1,
    fixedElevation,
    linearMin,
    linearMax,
    deltaLock
};
const StringArray SOURCE_LINK_ALT_TYPES   {String("Independent"), String("Fixed Elevation"),
                                           String("Linear Min"), String("Linear Max"), String("Delta Lock")};

// Trajectories -> Trajectory Type popup choices.
//-----------------------------------------------
enum class TrajectoryType { 
    undefined,
    realtime = 1,
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
const StringArray TRAJECTORY_TYPE_TYPES   {String("Realtime"), String("Drawing"), String("Circle Clockwise"),
                                           String("Circle Counter Clockwise"), String("Ellipse Clockwise"),
                                           String("Ellipse Counter Clockwise"), String("Spiral Clockwise Out In"),
                                           String("Spiral Counter Clockwise Out In"), String("Spiral Clockwise In Out"),
                                           String("Spiral Counter Clockwise In Out"), String("Square Clockwise"),
                                           String("Square Counter Clockwise"), String("Triangle Clockwise"),
                                           String("Triangle Counter Clockwise")};

enum class TrajectoryTypeAlt { undefined, realtime = 1,
                               drawing,
                               downUp,
                               upDown
                              };
const StringArray TRAJECTORY_TYPE_ALT_TYPES    {String("Realtime"), String("Drawing"), String("Up Down"),
                                                String("Down Up")};

// Fix position data headers.
//---------------------------
const StringArray FIXED_POSITION_DATA_HEADERS { String("ID"),
                                                String("S1_X"), String("S1_Y"), String("S1_Z"),
                                                String("S2_X"), String("S2_Y"), String("S2_Z"),
                                                String("S3_X"), String("S3_Y"), String("S3_Z"),
                                                String("S4_X"), String("S4_Y"), String("S4_Z"),
                                                String("S5_X"), String("S5_Y"), String("S5_Z"),
                                                String("S6_X"), String("S6_Y"), String("S6_Z"),
                                                String("S7_X"), String("S7_Y"), String("S7_Z"),
                                                String("S8_X"), String("S8_Y"), String("S8_Z"),
                                                String("T1_X"), String("T1_Y"), String("T1_Z"),
                                                String("T2_X"), String("T2_Y"), String("T2_Z")};

const String FIXED_POSITION_DATA_TAG ("Fix_Position_Data");
