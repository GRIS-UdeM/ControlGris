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

void Source::setAzimuth(float const azimuth)
{
    mAzimuth = azimuth;
    computeXY();
}

void Source::setNormalizedAzimuth(float const value)
{
    if (value <= 0.5f) {
        mAzimuth = value * 360.f;
    } else {
        mAzimuth = (value - 1.f) * 360.f;
    }
    computeXY();
}

float Source::getNormalizedAzimuth() const
{
    auto const azimuth{ mAzimuth / 360.f };
    return azimuth >= 0.f ? azimuth : azimuth + 1.f;
}

void Source::setElevationNoClip(float elevation)
{
    mElevationNoClip = elevation;
    setElevation(mElevationNoClip);
}

void Source::setElevation(float const elevation)
{
    if (elevation < 0.f) {
        mElevation = 0.f;
    } else if (elevation > 90.f) {
        mElevation = 90.f;
    } else {
        mElevationNoClip = mElevation = elevation;
    }
    computeXY();
}

void Source::setDistanceNoClip(float const distance)
{
    mDistanceNoClip = distance;
    setDistance(mDistanceNoClip);
}

void Source::setDistance(float const distance)
{
    if (distance < 0.f) {
        mDistance = 0.f;
    } else {
        mDistanceNoClip = mDistance = distance;
    }
    computeXY();
}

void Source::setCoordinates(float const azimuth, float const elevation, float const distance)
{
    mAzimuth = azimuth;
    mElevation = elevation;
    mDistance = distance;
    computeXY();
    computeAzimuthElevation();
}

void Source::setAzimuthSpan(float const azimuthSpan)
{
    mAzimuthSpan = azimuthSpan;
    mChanged = true;
}

void Source::setElevationSpan(float const elevationSpan)
{
    mElevationSpan = elevationSpan;
    mChanged = true;
}

void Source::setX(float const x)
{
    mX = x;
    computeAzimuthElevation();
}

void Source::setY(float const y)
{
    mY = y;
    computeAzimuthElevation();
}

void Source::setPos(Point<float> const & pos)
{
    mX = pos.x;
    mY = pos.y;
    computeAzimuthElevation();
}

void Source::computeXY()
{
    float radius;
    if (mRadiusIsElevation) { // azimuth - elevation
        radius = (90.f - mElevation) / 90.f;
    } else { // azimuth - distance
        radius = mDistance;
    }
    mX = radius * sinf(degreeToRadian(mAzimuth));
    mX = -mX * 0.5f + 0.5f;
    mX = std::clamp(mX, 0.f, 1.f);
    mY = radius * cosf(degreeToRadian(mAzimuth));
    mY = mY * 0.5f + 0.5f;
    mY = std::clamp(mY, 0.f, 1.f);
    mChanged = true;
}

void Source::computeAzimuthElevation()
{
    auto const x{ mX * 2.f - 1.f };
    auto const y{ mY * 2.f - 1.f };
    if (x != 0.0 || y != 0.0) {
        auto ang{ atan2f(x, y) / MathConstants<float>::pi * 180.f };
        if (ang <= -180.f) {
            ang += 360.f;
        }
        mAzimuth = -ang;
    }
    float rad{ sqrtf(x * x + y * y) };
    if (mRadiusIsElevation) { // azimuth - elevation
        rad = rad < 0.f ? 0.f : rad > 1.f ? 1.f : rad;
        mElevationNoClip = mElevation = 90.f - rad * 90.f;
    } else { // azimuth - distance
        rad = rad < 0.f ? 0.f : rad;
        mDistanceNoClip = mDistance = rad;
    }
    mChanged = true;
}

void Source::fixSourcePosition(bool const shouldBeFixed)
{
    if (shouldBeFixed) {
        mFixedAzimuth = mAzimuth;
        mFixedElevation = mElevationNoClip;
        mFixedDistance = mDistanceNoClip;
        mFixedX = mX;
        mFixedY = mY;
    } else {
        mFixedAzimuth = -1.f;
        mFixedElevation = -1.f;
        mFixedDistance = -1.f;
        mFixedX = -1.f;
        mFixedY = -1.f;
    }
}

void Source::fixSourcePositionElevation(bool const shouldBeFixed)
{
    if (shouldBeFixed) {
        mFixedElevation = mElevation;
    } else {
        mFixedElevation = -1.f;
    }
}

void Source::setFixedPosition(float const x, float const y)
{
    mFixedX = x;
    mFixedY = y;
    auto const fx{ mFixedX * 2.f - 1.f };
    auto const fy{ mFixedY * 2.f - 1.f };
    auto ang{ atan2f(fx, fy) / MathConstants<float>::pi * 180.f };
    if (ang <= -180.f) {
        ang += 360.f;
    }
    mFixedAzimuth = -ang;
    auto rad{ sqrtf(fx * fx + fy * fy) };
    if (mRadiusIsElevation) { // azimuth - elevation
        rad = std::clamp(rad, 0.f, 1.f);
        mFixedElevation = 90.f - rad * 90.f;
    } else { // azimuth - distance
        rad = rad < 0.f ? 0.f : rad;
        mFixedDistance = rad;
    }
}

void Source::setCoordinatesFromFixedSource(float const deltaAzimuth,
                                           float const deltaElevation,
                                           float const deltaDistance)
{
    if (mRadiusIsElevation) { // azimuth - elevation
        setAzimuth(mFixedAzimuth + deltaAzimuth);
        setElevationNoClip(mFixedElevation + deltaElevation * 90.f);
    } else { // azimuth - distance
        setAzimuth(mFixedAzimuth + deltaAzimuth);
        setDistanceNoClip(mFixedDistance + deltaDistance);
    }
    computeXY();
}

void Source::setSymmetricX(float const x, float const y)
{
    setX(x);
    setY(1.f - y);
}

void Source::setSymmetricY(float const x, float const y)
{
    setX(1.f - x);
    setY(y);
}

void Source::setXYCoordinatesFromFixedSource(float const deltaX, float const deltaY)
{
    auto const x{ std::clamp(mFixedX + deltaX, 0.f, 1.f) };
    auto const y{ std::clamp(mFixedY + deltaY, 0.f, 1.f) };
    setX(x);
    setY(y);
    computeAzimuthElevation();
}
