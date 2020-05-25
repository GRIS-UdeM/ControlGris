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

enum class SourceParameter { azimuth, elevation, distance, x, y, azimuthSpan, elevationSpan };

constexpr float kSourceRadius = 12.f;
constexpr float kSourceDiameter = kSourceRadius * 2.f;

class Source
{
public:
    Source();
    ~Source();

    void setId(int id);
    int getId() const { return m_id; }

    void setRadiusIsElevation(bool shouldBeElevation);

    void setAzimuth(float azimuth);
    void setNormalizedAzimuth(float value);
    float getAzimuth() const { return m_azimuth; }
    float getNormalizedAzimuth() const;
    void setElevationNoClip(float elevation);
    void setElevation(float elevation);
    void setNormalizedElevation(float value);
    float getElevation() const { return m_elevation; }
    float getNormalizedElevation() const { return m_elevation / 90.0; }
    void setDistance(float distance);
    void setDistanceNoClip(float distance);
    float getDistance() const { return m_distance; }
    void setAzimuthSpan(float azimuthSpan);
    float getAzimuthSpan() const { return m_aziSpan; }
    void setElevationSpan(float elevationSpan);
    float getElevationSpan() const { return m_eleSpan; }

    void setCoordinates(float azimuth, float elevation, float distance);

    void setX(float x);
    float getX() const { return m_x; }
    void setY(float y);
    float getY() const { return m_y; }
    Point<float> getPos() const { return Point<float>{ m_x, m_y }; }
    void setPos(Point<float> pos);

    void computeXY();
    void computeAzimuthElevation();

    void setFixedPosition(float x, float y);
    void setFixedElevation(float z);

    void setSymmetricX(float x, float y);
    void setSymmetricY(float x, float y);

    void fixSourcePosition(bool shouldBeFixed);
    void fixSourcePositionElevation(bool shouldBeFixed);

    float getDeltaX() const { return m_x - fixedX; }
    float getDeltaY() const { return m_y - fixedY; }
    float getDeltaAzimuth() const { return (m_azimuth - fixedAzimuth); }
    float getDeltaElevation() const { return (m_elevationNoClip - fixedElevation) / 90.f; }
    float getDeltaDistance() const { return (m_distance - fixedDistance); }

    void setCoordinatesFromFixedSource(float deltaAzimuth, float deltaElevation, float deltaDistance);
    void setXYCoordinatesFromFixedSource(float deltaX, float deltaY);
    void setElevationFromFixedSource(float deltaY);

    void setColour(Colour col);
    Colour getColour() const { return colour; }

private:
    int m_id;
    bool m_changed;
    bool m_radiusIsElevation;

    float m_azimuth;
    float m_elevation;
    float m_elevationNoClip;
    float m_distance;
    float m_distanceNoClip;
    float m_aziSpan;
    float m_eleSpan;
    float m_x;
    float m_y;

    Colour colour;

    float fixedAzimuth;
    float fixedElevation;
    float fixedDistance;
    float fixedX;
    float fixedY;

    inline double degreeToRadian(float degree) { return (degree / 360.0 * 2.0 * M_PI); }
};
