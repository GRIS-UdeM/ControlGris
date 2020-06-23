/*
  ==============================================================================

    ConstrainedStrongTypes.h
    Created: 1 Jun 2020 10:08:44pm
    Author:  samuel

  ==============================================================================
*/

#pragma once

#include "StrongTypes.h"

class Azimuth;

class Normalized
{
private:
    float mValue{};

public:
    constexpr Normalized() noexcept = default;
    ~Normalized() noexcept = default;

    constexpr Normalized(Normalized const &) = default;
    constexpr Normalized(Normalized &&) noexcept = default;

    Normalized & operator=(Normalized const &) = default;
    Normalized & operator=(Normalized &&) noexcept = default;

    constexpr explicit Normalized(float const value) noexcept : mValue(value) { clip(); }

    constexpr operator float() const { return mValue; }
    constexpr float toFloat() const { return mValue; }

    constexpr float operator*(float const rhs) const { return mValue * rhs; }

private:
    void clip() { mValue = std::clamp(mValue, 0.0f, 1.0f); }
};
