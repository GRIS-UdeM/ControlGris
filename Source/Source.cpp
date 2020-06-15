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

#include <algorithm>

void Source::setAzimuth(Radians const azimuth)
{
    auto const balancedAzimuth{ azimuth.simplified() };
    if (balancedAzimuth != mAzimuth) {
        mAzimuth = balancedAzimuth;
        computeXY();
    }
}

void Source::setAzimuth(Normalized const normalizedAzimuth)
{
    this->setAzimuth(twoPi * normalizedAzimuth - pi);
}

Normalized Source::getNormalizedAzimuth() const
{
    return Normalized{ (mAzimuth + pi) / twoPi };
}

void Source::setElevationNoClip(Radians const elevation)
{
    mElevationNoClip = elevation;
    setElevation(mElevationNoClip);
}

void Source::setElevation(Radians const elevation)
{
    auto const clippedElevation{ clipElevation(elevation) };
    if (clippedElevation != mElevation) {
        if (clippedElevation != elevation) {
            mElevation = clippedElevation;
        } else {
            mElevation = clippedElevation;
            mElevationNoClip = elevation;
        }
        computeXY();
    }
}

void Source::setElevation(Normalized const normalizedElevation)
{
    setElevation(halfPi * static_cast<float>(normalizedElevation));
}

void Source::setDistanceNoClip(float const distance)
{
    mDistanceNoClip = distance;
    setDistance(mDistanceNoClip);
}

void Source::setDistance(float const distance)
{
    jassert(distance >= 0.0f);

    if (distance != mDistance) {
        mDistance = distance;
        mDistanceNoClip = distance;
        computeXY();
    }
}

void Source::setCoordinates(Radians const azimuth, Radians const elevation, float const distance)
{
    auto const balancedAzimuth{ azimuth.simplified() };
    auto const clippedElevation{ clipElevation(elevation) };
    jassert(distance >= 0.0f);

    if (balancedAzimuth != mAzimuth || clippedElevation != mElevation || distance != mDistance) {
        mAzimuth = azimuth;
        mElevation = elevation;
        mDistance = distance;
        computeXY();
        // computeAzimuthElevation(); // usefull?
    }
}

void Source::setAzimuthSpan(Normalized const azimuthSpan)
{
    mAzimuthSpan = azimuthSpan;
}

void Source::setElevationSpan(Normalized const elevationSpan)
{
    mElevationSpan = elevationSpan;
}

void Source::setX(float const x)
{
    auto const clippedX{ clipCoordinate(x) };
    if (clippedX != mPosition.getX()) {
        mPosition.setX(clippedX);
        computeAzimuthElevation();
    }
}

void Source::setY(float const y)
{
    auto const clippedY{ clipCoordinate(y) };
    if (y != mPosition.getY()) {
        mPosition.setY(clippedY);
        computeAzimuthElevation();
    }
}

void Source::setPos(Point<float> const & position)
{
    auto const clippedPosition{ clipPosition(position) };
    if (mPosition != clippedPosition) {
        mPosition = clippedPosition;
        computeAzimuthElevation();
    }
}

void Source::computeXY()
{
    float const radius{ ([&] {
        if (mSpatMode == SpatMode::dome) { // azimuth - elevation
            auto const result{ mElevation / halfPi };
            jassert(result >= 0.0f && result <= 1.0f);
            return result;
        }
        return mDistance;
    }()) };

    mPosition = getPositionFromAngle(mAzimuth, radius);
    this->sendChangeMessage();
}

void Source::computeAzimuthElevation()
{
    if (mPosition.getX() != 0.0f || mPosition.getY() != 0.0f) {
        mAzimuth = getAngleFromPosition(mPosition).simplified();
    }

    auto const radius{ mPosition.getDistanceFromOrigin() };
    if (mSpatMode == SpatMode::dome) { // azimuth - elevation
        auto const clippedRadius{ std::min(radius, 1.0f) };
        if (clippedRadius < radius) {
            mPosition = getPositionFromAngle(mAzimuth, clippedRadius);
        }
        auto const elevation{ halfPi * clippedRadius };
        mElevation = elevation;
        mElevationNoClip = elevation;
    } else { // azimuth - distance
        mDistance = radius;
        mDistanceNoClip = radius;
    }

    this->sendChangeMessage();
}

void Source::fixSourcePosition(bool const shouldBeFixed)
{
    if (shouldBeFixed) {
        mFixedAzimuth = mAzimuth;
        mFixedElevation = mElevationNoClip;
        mFixedDistance = mDistanceNoClip;
        mFixedPosition = mPosition;
    } else {
        mFixedAzimuth.reset();
        mFixedElevation.reset();
        mFixedDistance.reset();
        mFixedPosition.reset();
    }
}

void Source::fixSourcePositionElevation(bool const shouldBeFixed)
{
    if (shouldBeFixed) {
        mFixedElevation = mElevation;
    } else {
        mFixedElevation.reset();
    }
}

float Source::getDeltaX() const
{
    jassert(mFixedPosition.has_value());
    return mPosition.getX() - mFixedPosition.value().getX();
}

float Source::getDeltaY() const
{
    jassert(mFixedPosition.has_value());
    return mPosition.getY() - mFixedPosition.value().getY();
}

Point<float> Source::getDeltaPosition() const
{
    jassert(mFixedPosition.has_value());
    return mPosition - mFixedPosition.value();
}

Radians Source::getDeltaAzimuth() const
{
    jassert(mFixedAzimuth.has_value());
    return mAzimuth - mFixedAzimuth.value();
}

Radians Source::getDeltaElevation() const
{
    jassert(mFixedElevation.has_value());
    return mElevationNoClip - mFixedElevation.value();
}

float Source::getDeltaDistance() const
{
    jassert(mFixedElevation.has_value());
    return mDistance - mFixedDistance.value();
}

void Source::setFixedPosition(Point<float> const & position)
{
    auto const clippedPosition{ clipPosition(position) };
    mFixedPosition = clippedPosition;
    mFixedAzimuth = getAngleFromPosition(clippedPosition);
    auto const radius{ clippedPosition.getDistanceFromOrigin() };
    if (mSpatMode == SpatMode::dome) { // azimuth - elevation
        auto const clippedRadius{ std::min(radius, 1.0f) };
        if (clippedRadius < radius) {
            mFixedPosition = getPositionFromAngle(*mFixedAzimuth, clippedRadius);
        }
        mFixedElevation = halfPi * clippedRadius;
    } else { // azimuth - distance
        mFixedDistance = radius;
    }
}

void Source::setCoordinatesFromFixedSource(Radians const deltaAzimuth,
                                           Radians const deltaElevation,
                                           float const deltaDistance)
{
    jassert(mFixedAzimuth.has_value());
    if (mSpatMode == SpatMode::dome) { // azimuth - elevation
        setAzimuth(mFixedAzimuth.value() + deltaAzimuth);
        jassert(mFixedElevation.has_value());
        setElevationNoClip(mFixedElevation.value() + deltaElevation);
    } else { // azimuth - distance
        setAzimuth(mFixedAzimuth.value() + deltaAzimuth);
        jassert(mFixedDistance.has_value());
        setDistanceNoClip(mFixedDistance.value() + deltaDistance);
    }
}

Normalized Source::getNormalizedElevation() const
{
    return Normalized{ mElevation / halfPi };
}

void Source::setSymmetricX(Point<float> const & position)
{
    setPos(Point<float>{ position.getX(), 1.0f - position.getY() });
}

void Source::setSymmetricY(Point<float> const & position)
{
    setPos(Point<float>{ 1.0f - position.getX(), position.getY() });
}

void Source::setXYCoordinatesFromFixedSource(Point<float> const & deltaPosition)
{
    setPos(mPosition + deltaPosition);
}

Point<float> Source::getPositionFromAngle(Radians const angle, float const radius)
{
    auto const rotatedAngle{ angle - halfPi };
    Point<float> const result{ std::cos(rotatedAngle.getAsRadians()) * radius,
                               std::sin(rotatedAngle.getAsRadians()) * radius };
    return result;
}

Radians Source::getAngleFromPosition(Point<float> const & position)
{
    Radians const angle{ std::atan2(position.getY(), position.getX()) };
    auto const rotatedAngle{ angle + halfPi };
    return rotatedAngle;
}

void Source::setFixedElevation(Radians const fixedElevation)
{
    mFixedElevation = clipElevation(fixedElevation);
}

Radians Source::clipElevation(Radians const elevation)
{
    return Radians{ std::clamp(elevation, Radians{ 0.0f }, halfPi) };
}

float Source::clipCoordinate(float const coord)
{
    return std::clamp(coord, -1.0f, 1.0f);
}

Point<float> Source::clipPosition(Point<float> const & position)
{
    return Point<float>{ std::clamp(position.getX(), -1.0f, 1.0f), std::clamp(position.getY(), -1.0f, 1.0f) };
}

void Source::setColorFromId(int const numTotalSources)
{
    auto hue{ static_cast<float>(mId) / static_cast<float>(numTotalSources)
              + 0.577251f }; // TODO: why is the Euler–Mascheroni constant appearing here???
    if (hue > 1.0f) {
        hue -= 1.0f;
    }
    mColour = Colour::fromHSV(hue, 1.0f, 1.0f, 0.85f);
}
