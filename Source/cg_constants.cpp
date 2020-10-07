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

juce::String const Automation::Ids::X{ "recordingTrajectory_x" };
juce::String const Automation::Ids::Y{ "recordingTrajectory_y" };
juce::String const Automation::Ids::Z{ "recordingTrajectory_z" };
juce::String const Automation::Ids::POSITION_SOURCE_LINK{ "sourceLink" };
juce::String const Automation::Ids::ELEVATION_SOURCE_LINK{ "sourceLinkAlt" };
juce::String const Automation::Ids::AZIMUTH_SPAN{ "azimuthSpan" };
juce::String const Automation::Ids::ELEVATION_SPAN{ "elevationSpan" };
juce::String const Automation::Ids::POSITION_PRESET{ "positionPreset" };

Automation::Enum Automation::idToEnum(const juce::String & name)
{
    if (name.compare(Ids::X) == 0) {
        return Enum::x;
    }
    if (name.compare(Ids::Y) == 0) {
        return Enum::y;
    }
    if (name.compare(Ids::Z) == 0) {
        return Enum::z;
    }
    if (name.compare(Ids::POSITION_SOURCE_LINK) == 0) {
        return Enum::positionSourceLink;
    }
    if (name.compare(Ids::ELEVATION_SOURCE_LINK) == 0) {
        return Enum::elevationSourceLink;
    }
    if (name.compare(Ids::AZIMUTH_SPAN) == 0) {
        return Enum::azimuthSpan;
    }
    if (name.compare(Ids::ELEVATION_SPAN) == 0) {
        return Enum::elevationSpan;
    }
    if (name.compare(Ids::POSITION_PRESET) == 0) {
        return Enum::positionPreset;
    }

    jassertfalse;
    return {};
}
