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

#include "cg_Trajectory.hpp"

#include <cmath>

#include "cg_Source.hpp"

namespace gris
{
//==============================================================================
Trajectory::Trajectory(PositionTrajectoryType const trajectoryType, juce::Point<float> const & startingPoint) noexcept
    : mIsElevationDrawing(false)
{
    switch (trajectoryType) {
    case PositionTrajectoryType::circleClockwise:
        mPoints = getBasicCirclePoints();
        break;
    case PositionTrajectoryType::circleCounterClockwise:
        mPoints = getBasicCirclePoints();
        invertDirection();
        break;
    case PositionTrajectoryType::ellipseClockwise:
        mPoints = getBasicEllipsePoints();
        break;
    case PositionTrajectoryType::ellipseCounterClockwise:
        mPoints = getBasicEllipsePoints();
        invertDirection();
        break;
    case PositionTrajectoryType::spiralClockwiseInOut:
        mPoints = getBasicSpiralPoints();
        flipOnHorizontalAxis();
        break;
    case PositionTrajectoryType::spiralCounterClockwiseInOut:
        mPoints = getBasicSpiralPoints();
        break;
    case PositionTrajectoryType::spiralClockwiseOutIn:
        mPoints = getBasicSpiralPoints();
        flipOnHorizontalAxis();
        invertDirection();
        break;
    case PositionTrajectoryType::spiralCounterClockwiseOutIn:
        mPoints = getBasicSpiralPoints();
        invertDirection();
        break;
    case PositionTrajectoryType::squareClockwise:
        mPoints = getBasicSquarePoints();
        break;
    case PositionTrajectoryType::squareCounterClockwise:
        mPoints = getBasicSquarePoints();
        invertDirection();
        break;
    case PositionTrajectoryType::triangleClockwise:
        mPoints = getBasicTrianglePoints();
        break;
    case PositionTrajectoryType::triangleCounterClockwise:
        mPoints = getBasicTrianglePoints();
        invertDirection();
        break;
    case PositionTrajectoryType::drawing:
        break;
    case PositionTrajectoryType::realtime: // do not make a trajectory in realtime mode!
    case PositionTrajectoryType::undefined:
    default:
        jassertfalse;
    }
    Radians const angle{ std::atan2(startingPoint.getY(), startingPoint.getX()) };
    auto const radius{ startingPoint.getDistanceFromOrigin() };

    rotate(angle);
    scale(radius);
}

//==============================================================================
Trajectory::Trajectory(ElevationTrajectoryType const trajectoryType) noexcept
    : mIsElevationDrawing(trajectoryType == ElevationTrajectoryType::drawing)
{
    switch (trajectoryType) {
    case ElevationTrajectoryType::downUp:
        mPoints = getBasicDownUpPoints();
        break;
    case ElevationTrajectoryType::upDown:
        mPoints = getBasicDownUpPoints();
        flipOnHorizontalAxis();
        break;
    case ElevationTrajectoryType::drawing:
        mPoints.clear();
        break;
    case ElevationTrajectoryType::realtime: // do not make a trajectory in realtime mode!
    case ElevationTrajectoryType::undefined:
    default:
        jassertfalse;
    }
}

//==============================================================================
juce::Path Trajectory::getDrawablePath(juce::Rectangle<float> const & drawArea, SpatMode spatMode) const
{
    auto trajectoryPositionToComponentPosition = [&](juce::Point<float> const & trajectoryPosition) {
        auto const clippedPoint{ Source::clipPosition(trajectoryPosition, spatMode) };
        auto const normalizedPoint{ (clippedPoint + juce::Point<float>{ 1.0f, 1.0f }) / 2.0f };
        auto const x{ normalizedPoint.getX() * drawArea.getWidth() + drawArea.getX() };
        auto const y{ normalizedPoint.getY() * drawArea.getHeight() + drawArea.getY() };
        return juce::Point<float>{ x, y };
    };

    juce::Path result{};
    if (!mPoints.isEmpty()) {
        result.startNewSubPath(trajectoryPositionToComponentPosition(mPoints.getReference(0)));
        for (int i{ 1 }; i < mPoints.size(); ++i) {
            result.lineTo(trajectoryPositionToComponentPosition(mPoints.getReference(i)));
        }
    }
    return result;
}

//==============================================================================
juce::Point<float> Trajectory::getPosition(Normalized const normalized) const
{
    auto const nbPoints{ static_cast<float>(mPoints.size()) };
    auto const index_f{ (nbPoints - 1.0f) * normalized.get() };
    auto const index_a{ static_cast<int>(std::floor(index_f)) };
    auto const index_b{ static_cast<int>(std::ceil(index_f)) };
    auto const balance{ std::fmod(index_f, 1.0f) };

    auto const & point_a{ mPoints.getReference(index_a) };
    auto const & point_b{ mPoints.getReference(index_b) };

    auto const result{ point_a * (1.0f - balance) + point_b * balance };

    return result;
}

//==============================================================================
void Trajectory::addPoint(juce::Point<float> const & point)
{
    mPoints.add(point);

    // elevation drawing works differently. We need to space the points evenly on the x axis.
    if (mIsElevationDrawing && mPoints.size() > 1) {
        constexpr auto FIELD_WIDTH{ 2.0f }; // space between [ -1, 1 ]
        auto const distanceBetweenPoints{ FIELD_WIDTH / (static_cast<float>(mPoints.size() - 1)) };
        constexpr auto FIELD_X_START{ -1.0f };
        float x{ FIELD_X_START };
        for (auto & it : mPoints) {
            it.setX(x);
            x += distanceBetweenPoints;
        }
    }
}

//==============================================================================
void Trajectory::invertDirection()
{
    for (int front{}, back{ mPoints.size() - 1 }; front < back; ++front, --back) {
        mPoints.swap(front, back);
    }
}

//==============================================================================
void Trajectory::flipOnHorizontalAxis()
{
    for (auto & point : mPoints) {
        point.setY(-point.getY());
    }
}

//==============================================================================
void Trajectory::rotate(Radians const angle)
{
    for (auto & point : mPoints) {
        point = point.rotatedAboutOrigin(angle.getAsRadians());
    }
}

//==============================================================================
void Trajectory::scale(float magnitude)
{
    for (auto & point : mPoints) {
        point *= magnitude;
    }
}

//==============================================================================
juce::Array<juce::Point<float>> Trajectory::getBasicCirclePoints()
{
    constexpr int NB_POINTS = 300;

    juce::Array<juce::Point<float>> result{};
    result.ensureStorageAllocated(NB_POINTS);
    for (int i{}; i < NB_POINTS; ++i) {
        auto const angle{ static_cast<float>(i) / static_cast<float>(NB_POINTS) * juce::MathConstants<float>::twoPi };
        result.add(juce::Point<float>{ std::cos(angle), std::sin(angle) });
    }

    return result;
}

//==============================================================================
juce::Array<juce::Point<float>> Trajectory::getBasicEllipsePoints()
{
    // just squish a circle!
    juce::Array<juce::Point<float>> result( getBasicCirclePoints() );
    for (auto & point : result) {
        point.setY(point.getY() * 0.5f);
    }

    return result;
}

//==============================================================================
juce::Array<juce::Point<float>> Trajectory::getBasicSpiralPoints()
{
    constexpr int NB_ROTATIONS = 3;
    constexpr int NB_POINTS_PER_ROTATION = 100;
    constexpr int NB_POINTS = NB_ROTATIONS * NB_POINTS_PER_ROTATION;

    constexpr float angleStep{ juce::MathConstants<float>::twoPi / static_cast<float>(NB_POINTS_PER_ROTATION) };
    constexpr float radiusStep{ 1.0f / static_cast<float>(NB_POINTS) };

    juce::Array<juce::Point<float>> result{};
    result.ensureStorageAllocated(NB_POINTS);

    float radius{};
    float angle{};

    for (int i{}; i < NB_POINTS; ++i) {
        result.add(juce::Point<float>{ std::cos(angle) * radius, std::sin(angle) * radius });
        radius += radiusStep;
        angle += angleStep;
    }

    return result;
}

//==============================================================================
juce::Array<juce::Point<float>> Trajectory::getBasicSquarePoints()
{
    constexpr int NB_POINTS_PER_SIDE = 75;
    constexpr int NB_POINTS = NB_POINTS_PER_SIDE * 4;

    constexpr float STEP = 1.0f / NB_POINTS_PER_SIDE;

    constexpr juce::Point<float> startingPoint{ 1.0f, 0.0f };
    constexpr juce::Point<float> step_up_left{ -STEP, STEP };
    constexpr juce::Point<float> step_down_left{ -STEP, -STEP };
    constexpr juce::Point<float> step_down_right{ STEP, -STEP };
    constexpr juce::Point<float> step_up_right{ STEP, STEP };

    auto currentPoint{ startingPoint };

    juce::Array<juce::Point<float>> result{};
    result.ensureStorageAllocated(NB_POINTS);

    for (int i{}; i < NB_POINTS_PER_SIDE; ++i) {
        result.add(currentPoint);
        currentPoint += step_up_left;
    }
    for (int i{}; i < NB_POINTS_PER_SIDE; ++i) {
        result.add(currentPoint);
        currentPoint += step_down_left;
    }
    for (int i{}; i < NB_POINTS_PER_SIDE; ++i) {
        result.add(currentPoint);
        currentPoint += step_down_right;
    }
    for (int i{}; i < NB_POINTS_PER_SIDE; ++i) {
        result.add(currentPoint);
        currentPoint += step_up_right;
    }

    return result;
}

//==============================================================================
juce::Array<juce::Point<float>> Trajectory::getBasicTrianglePoints()
{
    constexpr int NB_POINTS_PER_SIDE = 100;
    constexpr int NB_POINTS = NB_POINTS_PER_SIDE * 3;

    constexpr float sqrt3{ 1.73205080757f };
    constexpr float sideLength{ sqrt3 }; // that's just how equilateral triangles work!
    constexpr juce::Point<float> initialPoint{ 1.0f, 0.0f };
    constexpr auto step{ sideLength / static_cast<float>(NB_POINTS_PER_SIDE) };

    constexpr auto up_left_slope{ Degrees{ 150.0f }.getAsRadians() };
    constexpr auto up_right_slope{ Degrees{ 30.0f }.getAsRadians() };

    juce::Point<float> const up_left_step{ std::cos(up_left_slope) * step, std::sin(up_left_slope) * step };
    juce::Point<float> const up_right_step{ std::cos(up_right_slope) * step, std::sin(up_right_slope) * step };
    constexpr juce::Point<float> down_step{ 0.0f, -step };

    juce::Array<juce::Point<float>> result{};
    result.ensureStorageAllocated(NB_POINTS);

    auto currentPoint{ initialPoint };
    for (int i{}; i < NB_POINTS_PER_SIDE; ++i) {
        result.add(currentPoint);
        currentPoint += up_left_step;
    }
    for (int i{}; i < NB_POINTS_PER_SIDE; ++i) {
        result.add(currentPoint);
        currentPoint += down_step;
    }
    for (int i{}; i < NB_POINTS_PER_SIDE; ++i) {
        result.add(currentPoint);
        currentPoint += up_right_step;
    }

    return result;
}

//==============================================================================
juce::Array<juce::Point<float>> Trajectory::getBasicDownUpPoints()
{
    constexpr int NB_POINTS = 200;

    juce::Array<juce::Point<float>> result{};
    result.ensureStorageAllocated(NB_POINTS);

    constexpr juce::Point<float> step{ 2.0f / (NB_POINTS - 1), 2.0f / (NB_POINTS - 1) };
    constexpr juce::Point<float> startingPoint{ -1.0f, -1.0f };

    juce::Point<float> currentPoint{ startingPoint };
    for (int i{}; i < NB_POINTS; ++i) {
        result.add(currentPoint);
        currentPoint += step;
    }

    return result;
}

} // namespace gris
