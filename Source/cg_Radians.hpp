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

#include <JuceHeader.h>

#include <algorithm>

class Degrees;

class Radians
{
    using Type = float;
    Type mValue{};

public:
    constexpr Radians() noexcept = default;
    constexpr explicit Radians(Type value) noexcept : mValue(value) {}
    ~Radians() noexcept = default;

    constexpr Radians(Radians const & other) = default;
    constexpr Radians(Radians && other) noexcept = default;

    constexpr Radians & operator=(Radians const &) = default;
    constexpr Radians & operator=(Radians &&) noexcept = default;

    constexpr bool operator<(Radians const other) const { return mValue < other.mValue; }
    constexpr bool operator>(Radians const other) const { return mValue > other.mValue; }
    constexpr bool operator<=(Radians const other) const { return mValue <= other.mValue; }
    constexpr bool operator>=(Radians const other) const { return mValue >= other.mValue; }
    constexpr bool operator==(Radians const other) const { return mValue == other.mValue; }
    constexpr bool operator!=(Radians const other) const { return mValue != other.mValue; }

    constexpr Radians operator-() const { return Radians{ -mValue }; }
    constexpr Radians operator+(Radians const & other) const { return Radians{ mValue + other.mValue }; }
    constexpr Radians operator-(Radians const & other) const { return Radians{ mValue - other.mValue }; }
    constexpr Radians operator*(float const value) const { return Radians{ mValue * value }; }
    constexpr Radians operator*(double const value) const { return Radians{ mValue * static_cast<Type>(value) }; }
    constexpr Radians operator*(int const value) const { return Radians{ mValue * static_cast<Type>(value) }; }
    constexpr Radians operator/(float const value) const { return Radians{ mValue / static_cast<Type>(value) }; }
    constexpr Radians operator/(int const value) const { return Radians{ mValue / static_cast<Type>(value) }; }
    constexpr float operator/(Radians const other) const { return mValue / other.mValue; }

    constexpr Radians & operator+=(Radians const & other)
    {
        mValue += other.mValue;
        return *this;
    }
    constexpr Radians & operator-=(Radians const & other)
    {
        mValue -= other.mValue;
        return *this;
    }
    constexpr Radians & operator*=(float const value)
    {
        mValue *= value;
        return *this;
    }
    constexpr Radians & operator/=(float const value)
    {
        mValue /= value;
        return *this;
    }

    [[nodiscard]] constexpr float getAsRadians() const { return mValue; }
    [[nodiscard]] constexpr float getAsDegrees() const { return mValue / MathConstants<float>::twoPi * 360.0f; }
    [[nodiscard]] constexpr Radians simplified() const
    {
        if (mValue < -MathConstants<float>::pi) {
            return Radians{ mValue + MathConstants<float>::twoPi };
        }
        if (mValue > MathConstants<float>::pi) {
            return Radians{ mValue - MathConstants<float>::twoPi };
        }
        return *this;
    }
    [[nodiscard]] constexpr Radians clamp(Radians const min, Radians const max) const
    {
        return Radians{ std::clamp(mValue, min.mValue, max.mValue) };
    }
    static Radians fromPoint(Point<float> const & point)
    {
        if (point.getX() == 0.0f && point.getY() == 0.0f) {
            return Radians{};
        }
        return Radians{ std::atan2(point.getY(), point.getX()) };
    }
}; // class Radians

constexpr Radians pi{ MathConstants<float>::pi };
constexpr Radians twoPi{ MathConstants<float>::twoPi };
constexpr Radians halfPi{ MathConstants<float>::halfPi };
