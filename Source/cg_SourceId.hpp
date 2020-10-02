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

class SourceId
{
private:
    int mValue{};

public:
    constexpr SourceId() noexcept = default;
    ~SourceId() noexcept = default;

    constexpr SourceId(SourceId const &) = default;
    constexpr SourceId(SourceId &&) noexcept = default;

    constexpr SourceId & operator=(SourceId const &) = default;
    constexpr SourceId & operator=(SourceId &&) noexcept = default;

    constexpr explicit SourceId(int index) noexcept : mValue(index) {}

    constexpr int toInt() const { return mValue; }
    String toString() const { return String{ mValue }; }

    constexpr bool operator==(SourceId const & other) const { return mValue == other.mValue; }
    constexpr bool operator!=(SourceId const & other) const { return mValue != other.mValue; }
    constexpr bool operator<(SourceId const & other) const { return mValue < other.mValue; }
    constexpr bool operator>=(SourceId const & other) const { return mValue >= other.mValue; }

    constexpr SourceId operator+(SourceId const & other) const { return SourceId{ mValue + other.mValue }; }
    constexpr SourceId operator+(int const value) const { return SourceId{ mValue + value }; }

    constexpr SourceId & operator++()
    {
        ++mValue;
        return *this;
    }
    constexpr SourceId operator++(int)
    {
        SourceId const temp{ *this };
        ++mValue;
        return temp;
    }

    constexpr bool isValid() const { return mValue > 0 && mValue <= 128; } // TODO: remove magic number
};                                                                         // class SourceId
