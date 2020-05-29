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

constexpr float kSourceRadius = 12.0f;
constexpr float kSourceDiameter = kSourceRadius * 2.0f;

class Source
{
public:
    Source() = default;
    ~Source() = default;

    void setId(int const id) { mId = id; }
    int getId() const { return mId; }

    void setRadiusIsElevation(bool const shouldBeElevation) { mRadiusIsElevation = shouldBeElevation; }

    void setAzimuth(float azimuth);
    void setNormalizedAzimuth(float value);
    float getAzimuth() const { return mAzimuth; }
    float getNormalizedAzimuth() const;
    void setElevationNoClip(float elevation);
    void setElevation(float elevation);
    void setNormalizedElevation(float const value) { setElevation(value * 90.0f); }
    float getElevation() const { return mElevation; }
    float getNormalizedElevation() const { return mElevation / 90.0f; }
    void setDistance(float distance);
    void setDistanceNoClip(float distance);
    float getDistance() const { return mDistance; }
    void setAzimuthSpan(float azimuthSpan);
    float getAzimuthSpan() const { return mAzimuthSpan; }
    void setElevationSpan(float elevationSpan);
    float getElevationSpan() const { return mElevationSpan; }

    void setCoordinates(float azimuth, float elevation, float distance);

    void setX(float x);
    float getX() const { return mX; }
    void setY(float y);
    float getY() const { return mY; }
    Point<float> getPos() const { return Point<float>{ mX, mY }; }
    void setPos(Point<float> const & pos);

    void computeXY();
    void computeAzimuthElevation();

    void setFixedPosition(float x, float y);
    void setFixedElevation(float const z) { mFixedElevation = 90.0f - z * 90.0f; }

    void setSymmetricX(float x, float y);
    void setSymmetricY(float x, float y);

    void fixSourcePosition(bool shouldBeFixed);
    void fixSourcePositionElevation(bool shouldBeFixed);

    float getDeltaX() const { return mX - mFixedX; }
    float getDeltaY() const { return mY - mFixedY; }
    float getDeltaAzimuth() const { return (mAzimuth - mFixedAzimuth); }
    float getDeltaElevation() const { return (mElevationNoClip - mFixedElevation) / 90.0f; }
    float getDeltaDistance() const { return (mDistance - mFixedDistance); }

    void setCoordinatesFromFixedSource(float deltaAzimuth, float deltaElevation, float deltaDistance);
    void setXYCoordinatesFromFixedSource(float deltaX, float deltaY);
    void setElevationFromFixedSource(float const deltaY) { setElevation(mFixedElevation + deltaY * 90.0f); }

    void setColour(Colour const & col) { mColour = col; }
    Colour getColour() const { return mColour; }

private:
    int mId{};
    bool mChanged{ false };
    bool mRadiusIsElevation{ true };

    float mAzimuth{};
    float mElevation{};
    float mElevationNoClip{};
    float mDistance{ 1.0f };
    float mDistanceNoClip{ 1.0f };
    float mAzimuthSpan{};
    float mElevationSpan{};
    float mX{};
    float mY{};

    Colour mColour{ Colours::black };

    float mFixedAzimuth{ -1.0f };
    float mFixedElevation{ -1.0f };
    float mFixedDistance{ -1.0f };
    float mFixedX{};
    float mFixedY{};
};
