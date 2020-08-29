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

#include "ControlGrisConstants.h"

String const Automation::Ids::x{ "recordingTrajectory_x" };
String const Automation::Ids::y{ "recordingTrajectory_y" };
String const Automation::Ids::z{ "recordingTrajectory_z" };
String const Automation::Ids::positionSourceLink{ "sourceLink" };
String const Automation::Ids::elevationSourceLink{ "sourceLinkAlt" };
String const Automation::Ids::azimuthSpan{ "azimuthSpan" };
String const Automation::Ids::elevationSpan{ "elevationSpan" };
String const Automation::Ids::positionPreset{ "positionPreset" };

Automation::Enum Automation::idToEnum(const String & name)
{
    if (name.compare(Ids::x) == 0) {
        return Enum::x;
    }
    if (name.compare(Ids::y) == 0) {
        return Enum::y;
    }
    if (name.compare(Ids::z) == 0) {
        return Enum::z;
    }
    if (name.compare(Ids::positionSourceLink) == 0) {
        return Enum::positionSourceLink;
    }
    if (name.compare(Ids::elevationSourceLink) == 0) {
        return Enum::elevationSourceLink;
    }
    if (name.compare(Ids::azimuthSpan) == 0) {
        return Enum::azimuthSpan;
    }
    if (name.compare(Ids::elevationSpan) == 0) {
        return Enum::elevationSpan;
    }
    if (name.compare(Ids::positionPreset) == 0) {
        return Enum::positionPreset;
    }

    jassertfalse;
    return {};
}
