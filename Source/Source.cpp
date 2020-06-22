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

void Source::setAzimuth(Radians const azimuth, SourceLinkNotification const sourceLinkNotification)
{
    auto const balancedAzimuth{ azimuth.simplified() };
    if (balancedAzimuth != mAzimuth) {
        mAzimuth = balancedAzimuth;
        computeXY(sourceLinkNotification);
        sendNotifications(sourceLinkNotification);
    }
}

void Source::setAzimuth(Normalized const normalizedAzimuth, SourceLinkNotification const sourceLinkNotification)
{
    this->setAzimuth(twoPi * normalizedAzimuth - pi, sourceLinkNotification);
}

Normalized Source::getNormalizedAzimuth() const
{
    return Normalized{ (mAzimuth + pi) / twoPi };
}

void Source::setElevation(Radians const elevation, SourceLinkNotification const sourceLinkNotification)
{
    auto const clippedElevation{ clipElevation(elevation) };
    if (clippedElevation != mElevation) {
        mElevation = clippedElevation;
        computeXY(sourceLinkNotification);
        sendNotifications(sourceLinkNotification);
    }
}

void Source::setElevation(Normalized const normalizedElevation, SourceLinkNotification const sourceLinkNotification)
{
    setElevation(halfPi * static_cast<float>(normalizedElevation), sourceLinkNotification);
}

void Source::setDistance(float const distance, SourceLinkNotification const sourceLinkNotification)
{
    jassert(distance >= 0.0f);

    if (distance != mDistance) {
        mDistance = distance;
        computeXY(sourceLinkNotification);
        sendNotifications(sourceLinkNotification);
    }
}

void Source::setCoordinates(Radians const azimuth,
                            Radians const elevation,
                            float const distance,
                            SourceLinkNotification const sourceLinkNotification)
{
    auto const balancedAzimuth{ azimuth.simplified() };
    auto const clippedElevation{ clipElevation(elevation) };
    jassert(distance >= 0.0f);

    if (balancedAzimuth != mAzimuth || clippedElevation != mElevation || distance != mDistance) {
        mAzimuth = azimuth;
        mElevation = elevation;
        mDistance = distance;
        computeXY(sourceLinkNotification);
        sendNotifications(sourceLinkNotification);
    }
}

void Source::setAzimuthSpan(Normalized const azimuthSpan)
{
    if (mAzimuthSpan != azimuthSpan) {
        mAzimuthSpan = azimuthSpan;
        mGuiChangeBroadcaster.sendChangeMessage();
    }
}

void Source::setElevationSpan(Normalized const elevationSpan)
{
    if (mElevationSpan != elevationSpan) {
        mElevationSpan = elevationSpan;
        mGuiChangeBroadcaster.sendChangeMessage();
    }
}

void Source::setX(float const x, SourceLinkNotification const sourceLinkNotification)
{
    auto const clippedX{ clipCoordinate(x) };
    if (clippedX != mPosition.getX()) {
        mPosition.setX(clippedX);
        computeAzimuthElevation(sourceLinkNotification);
        sendNotifications(sourceLinkNotification);
    }
}

void Source::setY(float const y, SourceLinkNotification const sourceLinkNotification)
{
    auto const clippedY{ clipCoordinate(y) };
    if (y != mPosition.getY()) {
        mPosition.setY(clippedY);
        computeAzimuthElevation(sourceLinkNotification);
        sendNotifications(sourceLinkNotification);
    }
}

void Source::setPos(Point<float> const & position, SourceLinkNotification const sourceLinkNotification)
{
    auto const clippedPosition{ clipPosition(position) };
    if (mPosition != clippedPosition) {
        mPosition = clippedPosition;
        computeAzimuthElevation(sourceLinkNotification);
        sendNotifications(sourceLinkNotification);
    }
}

void Source::computeXY(SourceLinkNotification const sourceLinkNotification)
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
}

void Source::computeAzimuthElevation(SourceLinkNotification const sourceLinkNotification)
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
    } else { // azimuth - distance
        mDistance = radius;
    }
}

Normalized Source::getNormalizedElevation() const
{
    return Normalized{ mElevation / halfPi };
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
    auto const getAngle = [](Point<float> const & position) {
        if (position.getX() == 0.0f && position.getY() == 0.0f) {
            return 0.0f;
        }
        return std::atan2(position.getY(), position.getX());
    };
    Radians const angle{ getAngle(position) };
    auto const rotatedAngle{ angle + halfPi };
    return rotatedAngle;
}

void Source::sendNotifications(SourceLinkNotification const sourceLinkNotification)
{
    mGuiChangeBroadcaster.sendChangeMessage();
    if (sourceLinkNotification == SourceLinkNotification::notify) {
        mSourceLinkChangeBroadcaster.sendSynchronousChangeMessage();
    }
}

Radians Source::clipElevation(Radians const elevation)
{
    return Radians{ std::clamp(elevation, Radians{ 0.0f }, halfPi) };
}

float Source::clipCoordinate(float const coord)
{
    return std::clamp(coord, -1.0f, 1.0f);
}

Point<float> Source::clipPosition(Point<float> const & position) const
{
    if (mSpatMode == SpatMode::dome) {
        auto const radius{ position.getDistanceFromOrigin() };
        if (radius > 1.0f) {
            auto const angle{ getAngleFromPosition(position) };
            return getPositionFromAngle(angle, 1.0f);
        } else {
            return position;
        }
    } else {
        return Point<float>{ clipCoordinate(position.getX()), clipCoordinate(position.getY()) };
    }
}

void Source::setColorFromIndex(int const numTotalSources)
{
    auto hue{ static_cast<float>(mIndex.toInt()) / static_cast<float>(numTotalSources) + 0.577251f };
    if (hue > 1.0f) {
        hue -= 1.0f;
    }
    mColour = Colour::fromHSV(hue, 1.0f, 1.0f, 0.85f);
    mGuiChangeBroadcaster.sendChangeMessage();
}
