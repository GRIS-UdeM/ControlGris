/*
  ==============================================================================

    ConstrainedStrongTypes.cpp
    Created: 1 Jun 2020 10:08:44pm
    Author:  samuel

  ==============================================================================
*/

#include "ConstrainedStrongTypes.h"

constexpr SourcePosition::SourcePosition(Radians const & angle, float const radius)
    : mX(ClippedPosition{std::sin(static_cast<float>(angle)) * radius})
    , mY(ClippedPosition{std::cos(static_cast<float>(angle)) * radius})
{}