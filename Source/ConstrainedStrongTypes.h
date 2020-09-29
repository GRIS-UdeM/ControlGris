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

class Normalized
{
    float mValue{};

public:
    constexpr Normalized() noexcept = default;
    ~Normalized() noexcept = default;

    constexpr Normalized(Normalized const &) = default;
    constexpr Normalized(Normalized &&) noexcept = default;

    Normalized & operator=(Normalized const &) = default;
    Normalized & operator=(Normalized &&) noexcept = default;

    constexpr explicit Normalized(float const value) noexcept : mValue(value) {}

    constexpr Normalized operator-(Normalized const other) const { return Normalized{ mValue - other.mValue }; }
    constexpr Normalized operator+(Normalized const other) const { return Normalized{ mValue + other.mValue }; }

    constexpr bool operator!=(Normalized const other) const { return mValue != other.mValue; }

    constexpr float toFloat() const { return mValue; }

    constexpr float operator*(float const rhs) const { return mValue * rhs; }
};
