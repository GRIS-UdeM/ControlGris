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
#include "Source.h"

Source::Source() {
    m_changed = false;
    m_radiusIsElevation = true;
    m_azimuth = 0.0;
    m_elevation = 0.0;
    m_distance = 1.0;
    m_aziSpan = 0.0;
    m_eleSpan = 0.0;
    m_x = 0.0;
    m_y = 0.0;
    colour = Colours::black;
}

Source::~Source() {}

void Source::setId(int id) {
    m_id = id;
}

int Source::getId() {
    return m_id;
}

void Source::setRadiusIsElevation(bool shouldBeElevation) {
    m_radiusIsElevation = shouldBeElevation;
}

void Source::setAzimuth(float azimuth) {
    m_azimuth = azimuth;
    computeXY();
}

void Source::setNormalizedAzimuth(float value) {
    if (value <= 0.5) {
        m_azimuth = value * 360;
    } else {
        m_azimuth = (value - 1.0) * 360;
    }
    computeXY();
}

float Source::getAzimuth() {
    return m_azimuth;
}

float Source::getNormalizedAzimuth() {
    float azimuth = m_azimuth  / 360.0;
    return azimuth >= 0 ? azimuth : azimuth + 1.0;
}

void Source::setElevation(float elevation) {
    if (elevation < 0.0) {
        m_elevation = 0.0;
    } else if (elevation > 90.0) {
        m_elevation = 90.0;
    } else {
        m_elevation = elevation;
    }
    computeXY();
}

void Source::setNormalizedElevation(float value) {
    setElevation(value * 90.0);
}

float Source::getElevation() {
    return m_elevation;
}

float Source::getNormalizedElevation() {
    return m_elevation / 90.0;
}

void Source::setDistance(float distance) {
    m_distance = distance;
    computeXY();
}

float Source::getDistance() {
    return m_distance;
}

void Source::setAzimuthSpan(float azimuthSpan) {
    m_aziSpan = azimuthSpan;
    m_changed = true;
}

float Source::getAzimuthSpan() {
    return m_aziSpan;
}

void Source::setElevationSpan(float elevationSpan) {
    m_eleSpan = elevationSpan;
    m_changed = true;
}

float Source::getElevationSpan() {
    return m_eleSpan;
}

void Source::setX(float x) {
    m_x = x;
    computeAzimuthElevation();
}

float Source::getX() {
    return m_x;
}

void Source::setY(float y) {
    m_y = y;
    computeAzimuthElevation();
}

float Source::getY() {
    return m_y;
}

Point<float> Source::getPos() {
    return Point<float> {m_x, m_y};
}

void Source::computeXY() {
    float radius;
    if (m_radiusIsElevation) {  // azimuth - elevation
        radius = (90.0 - m_elevation) / 90.0;
    } else {                    // azimuth - distance
        radius = m_distance;
    }
    m_x = radius * sinf(degreeToRadian(m_azimuth));
    m_x = -m_x * 0.5 + 0.5;
    m_y = radius * cosf(degreeToRadian(m_azimuth));
    m_y = m_y * 0.5 + 0.5;
    m_changed = true;
}

void Source::computeAzimuthElevation() {
    float x = m_x * 2.0 - 1.0;
    float y = m_y * 2.0 - 1.0;
    float ang = atan2f(x, y) / M_PI * 180.0;
    if (ang <= -180) {
        ang += 360.0;
    }
    m_azimuth = -ang;
    float rad = sqrtf(x*x + y*y);
    if (m_radiusIsElevation) {  // azimuth - elevation
        rad = rad < 0.0 ? 0.0 : rad > 1.0 ? 1.0 : rad;
        m_elevation = 90.0 - rad * 90.0;
    } else {                    // azimuth - distance
        m_distance = rad;
    }
    m_changed = true;
}

void Source::setColour(Colour col) {
    colour = col;
}

Colour Source::getColour() {
    return colour;
}
