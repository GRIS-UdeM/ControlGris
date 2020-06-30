/*
  ==============================================================================

    ElevationDrawingTrajectory.h
    Created: 29 Jun 2020 9:17:05pm
    Author:  samuel

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "ConstrainedStrongTypes.h"

class ElevationDrawingTrajectory
{
private:
    Array<Radians> mValues;

public:
    void clear() { mValues.clear(); }
    void addValue(Radians const value) { mValues.add(value); }
    Radians getElevationAt(Normalized moment) const;
    Radians getStartElevation() const { return mValues.getFirst(); }
    Radians getEndElevation() const { return mValues.getLast(); }
    Path getDrawablePath(Rectangle<float> const & drawableArea) const;
};