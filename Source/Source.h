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

#include "../JuceLibraryCode/JuceHeader.h"

#define SOURCE_ID_AZIMUTH 0
#define SOURCE_ID_ELEVATION 1
#define SOURCE_ID_DISTANCE 2
#define SOURCE_ID_X 3
#define SOURCE_ID_Y 4
#define SOURCE_ID_AZIMUTH_SPAN 5
#define SOURCE_ID_ELEVATION_SPAN 6

static const float kSourceRadius = 10;
static const float kSourceDiameter = kSourceRadius * 2;

class Source
{
public:
    Source();
    ~Source();

    void setId(int id);
    int getId();

    void setRadiusIsElevation(bool shouldBeElevation);

    void setAzimuth(float azimuth);
    void setNormalizedAzimuth(float value);
    float getAzimuth();
    float getNormalizedAzimuth();
    void setElevation(float elevation);
    void setNormalizedElevation(float value);
    float getElevation();
    float getNormalizedElevation();
    void setDistance(float distance);
    float getDistance();
    void setAzimuthSpan(float azimuthSpan);
    float getAzimuthSpan();
    void setElevationSpan(float elevationSpan);
    float getElevationSpan();

    void setCoordinates(float azimuth, float elevation, float distance);

    void setX(float x);
    float getX();
    void setY(float y);
    float getY();
    Point<float> getPos();

    void computeXY();
    void computeAzimuthElevation();

    void setColour(Colour col);
    Colour getColour();

private:
    int m_id;
    bool m_changed;
    bool m_radiusIsElevation;
    float m_azimuth;
    float m_elevation;
    float m_distance;
    float m_aziSpan;
    float m_eleSpan;
    float m_x;
    float m_y;
    Colour colour;

    inline double degreeToRadian(float degree) { return (degree / 360.0 * 2.0 * M_PI); }

};
