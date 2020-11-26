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

#pragma once

#include "cg_Radians.hpp"

namespace gris
{
class Degrees
{
    float mValue{};

public:
    constexpr Degrees() noexcept = default;

    constexpr explicit Degrees(float const value) noexcept : mValue(value) {}

    constexpr Degrees(Radians const radians) noexcept : Degrees(radians.getAsDegrees()) {}

    ~Degrees() noexcept = default;

    constexpr Degrees(Degrees const & other) = default;

    constexpr Degrees(Degrees && other) noexcept = default;

    constexpr Degrees & operator=(Degrees const &) = default;

    constexpr Degrees & operator=(Degrees &&) noexcept = default;

    constexpr bool operator<(Degrees const other) const { return mValue < other.mValue; }

    constexpr bool operator>(Degrees const other) const { return mValue > other.mValue; }

    constexpr bool operator<=(Degrees const other) const { return mValue <= other.mValue; }

    constexpr bool operator>=(Degrees const other) const { return mValue >= other.mValue; }

    constexpr bool operator==(Degrees const other) const { return mValue == other.mValue; }

    constexpr bool operator!=(Degrees const other) const { return mValue != other.mValue; }

    constexpr Degrees operator-() const { return Degrees{ -mValue }; }

    constexpr Degrees operator+(Degrees const & other) const { return Degrees{ mValue + other.mValue }; }

    constexpr Degrees operator-(Degrees const & other) const { return Degrees{ mValue - other.mValue }; }

    constexpr Degrees operator*(float const value) const { return Degrees{ mValue * value }; }

    constexpr Degrees operator*(int const value) const { return Degrees{ mValue * static_cast<float>(value) }; }

    constexpr Degrees operator/(float const value) const { return Degrees{ mValue / value }; }

    constexpr Degrees operator/(int const value) const { return Degrees{ mValue / static_cast<float>(value) }; }

    constexpr float operator/(Degrees const other) const { return mValue / other.mValue; }

    constexpr Degrees & operator+=(Degrees const & other)
    {
        mValue += other.mValue;
        return *this;
    }

    constexpr Degrees & operator-=(Degrees const & other)
    {
        mValue -= other.mValue;
        return *this;
    }

    constexpr Degrees & operator*=(float const value)
    {
        mValue *= value;
        return *this;
    }

    constexpr Degrees & operator/=(float const value)
    {
        mValue /= value;
        return *this;
    }

    constexpr operator Radians() const { return Radians{ mValue / 360.0f * juce::MathConstants<float>::twoPi }; }

    [[nodiscard]] constexpr float getAsRadians() const { return mValue / 360.0f * juce::MathConstants<float>::twoPi; }

    [[nodiscard]] constexpr float getAsDegrees() const { return mValue; }

    [[nodiscard]] constexpr Degrees simplified() const
    {
        constexpr auto circle{ 360.0f };
        constexpr auto halfCircle{ circle / 2.0f };
        if (mValue < -halfCircle) {
            return Degrees{ mValue + circle };
        }
        if (mValue > halfCircle) {
            return Degrees{ mValue - circle };
        }
        return *this;
    }
}; // class Degrees

} // namespace gris