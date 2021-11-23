/**************************************************************************
 * Copyright 2021 UdeM - GRIS - Samuel Béland & Olivier Belanger          *
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

#include "cg_ElevationTrajectoryType.hpp"
#include "cg_PositionTrajectoryType.hpp"
#include "cg_constants.hpp"

namespace gris
{
//==============================================================================
class Trajectory
{
protected:
    //==============================================================================
    juce::Array<juce::Point<float>> mPoints{};
    bool mIsElevationDrawing{ false };

public:
    //==============================================================================
    explicit Trajectory(ElevationTrajectoryType elevationTrajectoryType) noexcept;
    Trajectory(PositionTrajectoryType positionTrajectoryType, juce::Point<float> const & startingPoint) noexcept;
    ~Trajectory() noexcept = default;
    //==============================================================================
    Trajectory(Trajectory const &) = default;
    Trajectory(Trajectory &&) noexcept = default;
    Trajectory & operator=(Trajectory const &) = default;
    Trajectory & operator=(Trajectory &&) noexcept = default;
    //==============================================================================
    [[nodiscard]] int size() const { return mPoints.size(); }
    void addPoint(juce::Point<float> const & point);
    void clear() { mPoints.clear(); }
    //==============================================================================
    [[nodiscard]] juce::Point<float> const & getStartPosition() const { return mPoints.getReference(0); }
    [[nodiscard]] juce::Point<float> const & getEndPosition() const { return mPoints.getReference(mPoints.size() - 1); }
    [[nodiscard]] juce::Point<float> getPosition(Normalized normalized) const;
    [[nodiscard]] juce::Path getDrawablePath(juce::Rectangle<float> const & drawArea, SpatMode spatMode) const;

private:
    //==============================================================================
    void invertDirection();
    void flipOnHorizontalAxis();
    void rotate(Radians angle);
    void scale(float magnitude);
    //==============================================================================
    [[nodiscard]] static juce::Array<juce::Point<float>> getBasicCirclePoints();
    [[nodiscard]] static juce::Array<juce::Point<float>> getBasicEllipsePoints();
    [[nodiscard]] static juce::Array<juce::Point<float>> getBasicSpiralPoints();
    [[nodiscard]] static juce::Array<juce::Point<float>> getBasicSquarePoints();
    [[nodiscard]] static juce::Array<juce::Point<float>> getBasicTrianglePoints();
    [[nodiscard]] static juce::Array<juce::Point<float>> getBasicDownUpPoints();
    //==============================================================================
    JUCE_LEAK_DETECTOR(Trajectory);
};

} // namespace gris