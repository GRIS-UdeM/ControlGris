/*
  ==============================================================================

    ElevationDrawingTrajectory.cpp
    Created: 29 Jun 2020 9:17:05pm
    Author:  samuel

  ==============================================================================
*/

#include "ElevationDrawingTrajectory.h"

#include "ControlGrisConstants.h"

Radians ElevationDrawingTrajectory::getElevationAt(Normalized moment) const
{
    auto const nbOfValues{ static_cast<float>(mValues.size()) };
    auto const index_f{ moment.toFloat() * (nbOfValues - 1.0f) };
    int const index_low{ static_cast<int>(std::floor(index_f)) };
    int const index_hi{ static_cast<int>(std::ceil(index_f)) };

    auto const & value_low{ mValues.getReference(index_low) };
    auto const & value_hi{ mValues.getReference(index_hi) };

    auto const ratio{ std::fmod(index_f, 1.0f) };

    auto const result{ value_low * (1.0f - ratio) + value_hi * ratio };
    return result;
}

Path ElevationDrawingTrajectory::getDrawablePath(Rectangle<float> const & drawableArea) const
{
    Path result{};

    if (mValues.size() > 1) {
        auto const distanceBetweenPoints{ drawableArea.getWidth() / static_cast<float>(mValues.size() - 1) };

        auto getPoint = [&](int const index, Radians const elevation) {
            auto const x{ distanceBetweenPoints * static_cast<float>(index) + drawableArea.getX() };
            auto const y{ elevation / MAX_ELEVATION * drawableArea.getHeight() + drawableArea.getY() };
            return Point<float>{ x, y };
        };

        result.startNewSubPath(getPoint(0, mValues.getFirst()));

        for (int index{ 1 }; index < mValues.size(); ++index) {
            result.lineTo(getPoint(index, mValues.getUnchecked(index)));
        }
    }

    return result;
}