/*
  ==============================================================================

    ConstrainedStrongTypes.h
    Created: 1 Jun 2020 10:08:44pm
    Author:  samuel

  ==============================================================================
*/

#pragma once

#include <cmath>

#include "StrongTypes.h"

class Azimuth;

class Normalized
{
    float mValue{};

public:
    constexpr Normalized() = default;
    constexpr explicit Normalized(float const value) : mValue(value) { clip(); }

    constexpr Normalized(Normalized const &) = default;
    constexpr Normalized(Normalized &&) = default;

    ~Normalized() = default;

    Normalized & operator=(Normalized const &) = default;
    Normalized & operator=(Normalized &&) = default;

    constexpr float operator*(float const rhs) const { return mValue * rhs; }

    constexpr operator float() const { return mValue; }

private:
    void clip() { mValue = std::clamp(mValue, 0.0f, 1.0f); }
};
