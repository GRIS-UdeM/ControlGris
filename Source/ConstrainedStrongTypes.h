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

    constexpr explicit Normalized(float const value) noexcept : mValue(value)
    {
        if (std::isnan(value)) {
            jassertfalse;
        }
    }

    constexpr Normalized operator-(Normalized const other) const { return Normalized{ mValue - other.mValue }; }
    constexpr Normalized operator+(Normalized const other) const { return Normalized{ mValue + other.mValue }; }

    constexpr bool operator!=(Normalized const other) const { return mValue != other.mValue; }

    constexpr float toFloat() const { return mValue; }

    constexpr float operator*(float const rhs) const { return mValue * rhs; }
};
