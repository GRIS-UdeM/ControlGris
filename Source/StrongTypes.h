//
// Created by samuel on 2020-06-01.
//

#ifndef STRONGTYPESTEST_TYPES_H
#define STRONGTYPESTEST_TYPES_H

#include "../JuceLibraryCode/JuceHeader.h"

#include <algorithm>

class Degrees;

class Radians
{
    float mValue{};

public:
    constexpr Radians() = default;
    constexpr explicit Radians(float radians) : mValue(radians) {}
    // constexpr Radians(Degrees const & degrees);
    ~Radians() = default;

    constexpr Radians(Radians const & other) = default;
    constexpr Radians(Radians && other) = default;

    constexpr Radians & operator=(Radians const &) = default;
    constexpr Radians & operator=(Radians &&) = default;

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
    constexpr Radians operator*(int const value) const { return Radians{ mValue * static_cast<float>(value) }; }
    constexpr Radians operator/(float const value) const { return Radians{ mValue / value }; }
    constexpr Radians operator/(int const value) const { return Radians{ mValue / static_cast<float>(value) }; }
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
};

constexpr Radians pi{ MathConstants<float>::pi };
constexpr Radians twoPi{ MathConstants<float>::twoPi };
constexpr Radians halfPi{ MathConstants<float>::halfPi };

class Degrees
{
    float mValue{};

public:
    constexpr Degrees() = default;
    constexpr explicit Degrees(float const value) : mValue(value) {}
    constexpr Degrees(Radians const radians) : mValue(radians.getAsDegrees()) {}
    ~Degrees() = default;

    constexpr Degrees(Degrees const & other) = default;
    constexpr Degrees(Degrees && other) = default;

    constexpr Degrees & operator=(Degrees const &) = default;
    constexpr Degrees & operator=(Degrees &&) = default;

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

    constexpr operator Radians() const { return Radians{ mValue / 360.0f * MathConstants<float>::twoPi }; }
    [[nodiscard]] constexpr float getAsRadians() const { return mValue / 360.0f * MathConstants<float>::twoPi; }
    [[nodiscard]] constexpr float getAsDegrees() const { return mValue; }
};

#endif // STRONGTYPESTEST_TYPES_H
