/*
  ==============================================================================

    Trajectory.h
    Created: 7 Jun 2020 4:18:45pm
    Author:  samuel

  ==============================================================================
*/

#pragma once

#include <variant>

#include "../JuceLibraryCode/JuceHeader.h"

#include "ConstrainedStrongTypes.h"
#include "ControlGrisConstants.h"

using AnyTrajectoryType = std::variant<PositionTrajectoryType, ElevationTrajectoryType>;

//=========
class Trajectory
{
protected:
    //=========
    Array<Point<float>> mPoints{};
    AnyTrajectoryType mTrajectoryType{ PositionTrajectoryType::realtime };

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
