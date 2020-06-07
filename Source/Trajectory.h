/*
  ==============================================================================

    Trajectory.h
    Created: 7 Jun 2020 4:18:45pm
    Author:  samuel

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "ControlGrisConstants.h"
#include "StrongTypes.h"

//=========
class Trajectory
{
protected:
    //=========
    Array<Point<float>> mPoints;

public:
    //=========
    Trajectory() = default;
    Trajectory(PositionTrajectoryType positionTrajectoryType, Point<float> const & startingPoint);
    Trajectory(ElevationTrajectoryType elevationTrajectoryType);

    Trajectory(Trajectory const &) = default;
    Trajectory(Trajectory &&) = default;

    ~Trajectory() = default;

    Trajectory & operator=(Trajectory const &) = default;
    Trajectory & operator=(Trajectory &&) = default;

    Point<float> const & operator[](int const index) const { return mPoints.getReference(index); }

    Point<float> const & getFirst() const { return mPoints.getReference(0); }
    Point<float> const & getLast() const { return mPoints.getReference(mPoints.size() - 1); }

    void clear() { mPoints.clear(); }
    void add(Point<float> const & point) { mPoints.add(point); }
    int size() const { return mPoints.size(); }

    Path createDrawablePath(float componentWidth) const;

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
    //=========
    JUCE_LEAK_DETECTOR(Trajectory);
};