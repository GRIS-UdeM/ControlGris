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
const int FIELD_WIDTH = 300;
const int MAX_NUMBER_OF_SOURCES = 8;

// Configuration -> Source tab -> Source Placement popup choices.
//---------------------------------------------------------------
enum SOURCE_PLACEMENT_ENUM {    SOURCE_PLACEMENT_LEFT_ALTERNATE = 1,
                                SOURCE_PLACEMENT_RIGHT_ALTERNATE,
                                SOURCE_PLACEMENT_LEFT_CLOCKWISE,
                                SOURCE_PLACEMENT_LEFT_COUNTER_CLOCKWISE,
                                SOURCE_PLACEMENT_RIGHT_CLOCKWISE,
                                SOURCE_PLACEMENT_RIGHT_COUNTER_CLOCKWISE,
                                SOURCE_PLACEMENT_TOP_CLOCKWISE,
                                SOURCE_PLACEMENT_TOP_COUNTER_CLOCKWISE
                           };
const StringArray SOURCE_PLACEMENT_SKETCH {String("Left Alternate"), String("Right Alternate"),
                                           String("Left Clockwise"), String("Left Counter Clockwise"),
                                           String("Right Clockwise"), String("Right Counter Clockwise"),
                                           String("Top Clockwise"), String("Top Counter Clockwise")};

// Trajectories -> Source Link popup choices.
//-------------------------------------------
enum SOURCE_LINK_ENUM { SOURCE_LINK_INDEPENDANT = 1,
                        SOURCE_LINK_CIRCULAR,
                        SOURCE_LINK_CIRCULAR_FIXED_RADIUS,
                        SOURCE_LINK_CIRCULAR_FIXED_ANGLE,
                        SOURCE_LINK_CIRCULAR_FULLY_FIXED,
                        SOURCE_LINK_DELTA_LOCK
                      };
const StringArray SOURCE_LINK_TYPES       {String("Independant"), String("Circular"),
                                           String("Circular Fixed Radius"), String("Circular Fixed Angle"),
                                           String("Circular Fully Fixed"), String("Delta Lock")};

// Trajectories -> Trajectory Type popup choices.
//-----------------------------------------------
enum TRAJECTORY_TYPE_ENUM { TRAJECTORY_TYPE_DRAWING = 1,
                            TRAJECTORY_TYPE_CIRCLE_CLOCKWISE,
                            TRAJECTORY_TYPE_CIRCLE_COUNTER_CLOCKWISE,
                            TRAJECTORY_TYPE_ELLIPSE_CLOCKWISE,
                            TRAJECTORY_TYPE_ELLIPSE_COUNTER_CLOCKWISE,
                            TRAJECTORY_TYPE_SPIRAL_CLOCKWISE
                          };
const StringArray TRAJECTORY_TYPES        {String("Drawing"), String("Circle Clockwise"),
                                           String("Circle Counter Clockwise"), String("Ellipse Clockwise"),
                                           String("Ellipse Counter Clockwise"), String("Spiral Clockwise")};

// Fix position data headers.
//---------------------------
const StringArray FIXED_POSITION_DATA_HEADERS { String("Delete"), String("Time"),
                                                String("Src_1_X"), String("Src_1_Y"), String("Src_1_Z"),
                                                String("Src_2_X"), String("Src_2_Y"), String("Src_2_Z"),
                                                String("Src_3_X"), String("Src_3_Y"), String("Src_3_Z"),
                                                String("Src_4_X"), String("Src_4_Y"), String("Src_4_Z"),
                                                String("Src_5_X"), String("Src_5_Y"), String("Src_5_Z"),
                                                String("Src_6_X"), String("Src_6_Y"), String("Src_6_Z"),
                                                String("Src_7_X"), String("Src_7_Y"), String("Src_7_Z"),
                                                String("Src_8_X"), String("Src_8_Y"), String("Src_8_Z")};

const String FIXED_POSITION_DATA_TAG ("Fix_Position_Data");
