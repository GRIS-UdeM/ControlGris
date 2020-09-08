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

#include <JuceHeader.h>

#include "ConstrainedStrongTypes.h"
#include "ControlGrisConstants.h"

//=========
class Trajectory
{
protected:
    //=========
    Array<Point<float>> mPoints{};
    bool mIsElevationDrawing{ false };

public:
    //=========
    Trajectory(PositionTrajectoryType positionTrajectoryType, Point<float> const & startingPoint) noexcept;
    Trajectory(ElevationTrajectoryType elevationTrajectoryType) noexcept;

    Trajectory(Trajectory const &) = default;
    Trajectory(Trajectory &&) noexcept = default;

    ~Trajectory() noexcept = default;

    Trajectory & operator=(Trajectory const &) = default;
    Trajectory & operator=(Trajectory &&) noexcept = default;

    Point<float> const & getStartPosition() const { return mPoints.getReference(0); }
    Point<float> const & getEndPosition() const { return mPoints.getReference(mPoints.size() - 1); }
    Point<float> getPosition(Normalized normalized) const;

    void clear() { mPoints.clear(); }
    void addPoint(Point<float> const & point);
    int size() const { return mPoints.size(); }

    Path getDrawablePath(Rectangle<float> const & drawArea, SpatMode spatMode) const;

private:
    //=========
    void invertDirection();
    void flipOnHorizontalAxis();
    void rotate(Radians angle);
    void scale(float magnitude);

    static Array<Point<float>> getBasicCirclePoints();
    static Array<Point<float>> getBasicEllipsePoints();
    static Array<Point<float>> getBasicSpiralPoints();
    static Array<Point<float>> getBasicSquarePoints();
    static Array<Point<float>> getBasicTrianglePoints();
    static Array<Point<float>> getBasicDownUpPoints();
    //=========
    JUCE_LEAK_DETECTOR(Trajectory);
};
