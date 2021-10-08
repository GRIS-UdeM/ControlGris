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

#include "cg_constants.hpp"

namespace gris
{
//==============================================================================
// Plugin parameter tags

juce::String const parameters::dynamic::X{ "recordingTrajectory_x" };
juce::String const parameters::dynamic::Y{ "recordingTrajectory_y" };
juce::String const parameters::dynamic::Z{ "recordingTrajectory_z" };
juce::String const parameters::dynamic::POSITION_SOURCE_LINK{ "sourceLink" };
juce::String const parameters::dynamic::ELEVATION_SOURCE_LINK{ "sourceLinkAlt" };
juce::String const parameters::dynamic::AZIMUTH_SPAN{ "azimuthSpan" };
juce::String const parameters::dynamic::ELEVATION_SPAN{ "elevationSpan" };
juce::String const parameters::dynamic::POSITION_PRESET{ "positionPreset" };

juce::String const parameters::statics::OSC_FORMAT{ "oscFormat" };
juce::String const parameters::statics::OSC_PORT{ "oscPortNumber" };
juce::String const parameters::statics::OSC_ADDRESS{ "oscAddress" };
juce::String const parameters::statics::OSC_ACTIVE{ "oscConnected" };
juce::String const parameters::statics::OSC_INPUT_PORT{ "oscInputPortNumber" };
juce::String const parameters::statics::OSC_INPUT_CONNECTED{ "oscInputConnected" };
juce::String const parameters::statics::OSC_OUTPUT_ADDRESS{ "oscOutputAddress" };
juce::String const parameters::statics::OSC_OUTPUT_PORT{ "oscOutputPortNumber" };
juce::String const parameters::statics::OSC_OUTPUT_CONNECTED{ "oscOutputConnected" };
juce::String const parameters::statics::NUM_SOURCES{ "numberOfSources" };
juce::String const parameters::statics::FIRST_SOURCE_ID{ "firstSourceId" };
juce::String const parameters::statics::OSC_OUTPUT_PLUGIN_ID{ "oscOutputPluginId" };

juce::String const parameters::statics::POSITION_TRAJECTORY_TYPE{ "trajectoryType" };
juce::String const parameters::statics::ELEVATION_TRAJECTORY_TYPE{ "trajectoryTypeAlt" };
juce::String const parameters::statics::POSITION_BACK_AND_FORTH{ "backAndForth" };
juce::String const parameters::statics::ELEVATION_BACK_AND_FORTH{ "backAndForthAlt" };
juce::String const parameters::statics::POSITION_DAMPENING_CYCLES{ "dampeningCycles" };
juce::String const parameters::statics::ELEVATION_DAMPENING_CYCLES{ "dampeningCyclesAlt" };
juce::String const parameters::statics::DEVIATION_PER_CYCLE{ "deviationPerCycle" };
juce::String const parameters::statics::CYCLE_DURATION{ "cycleDuration" };
juce::String const parameters::statics::DURATION_UNIT{ "durationUnit" };

//==============================================================================
// Defaults

juce::String const DEFAULT_OSC_ADDRESS = "127.0.0.1";
juce::String const DEFAULT_OSC_OUTPUT_ADDRESS = "192.168.1.100";

//==============================================================================
// Other string values

juce::String const SOURCE_SELECTION_WARNING{ "This source link does not allow individual moves." };

juce::StringArray const SOURCE_PLACEMENT_SKETCH{
    juce::String("Left Alternate"),  juce::String("Right Alternate"),
    juce::String("Left Clockwise"),  juce::String("Left Counter Clockwise"),
    juce::String("Right Clockwise"), juce::String("Right Counter Clockwise"),
    juce::String("Top Clockwise"),   juce::String("Top Counter Clockwise")
};

juce::StringArray const POSITION_SOURCE_LINK_TYPES{
    juce::String("Independent"),           juce::String("Circular"),
    juce::String("Circular Fixed Radius"), juce::String("Circular Fixed Angle"),
    juce::String("Circular Fully Fixed"),  juce::String("Delta Lock"),
    juce::String("Symmetric X"),           juce::String("Symmetric Y")
};

juce::StringArray const ELEVATION_SOURCE_LINK_TYPES{ juce::String("Independent"),
                                                     juce::String("Equal Elevation"),
                                                     juce::String("Bottom-Top"),
                                                     juce::String("Top-Bottom"),
                                                     juce::String("Delta Lock") };

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

juce::StringArray const ELEVATION_TRAJECTORY_TYPE_TYPES{ juce::String("Realtime"),
                                                         juce::String("Drawing"),
                                                         juce::String("Up Down"),
                                                         juce::String("Down Up") };

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

} // namespace gris
