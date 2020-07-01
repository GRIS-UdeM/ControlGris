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
    constexpr Radians() noexcept = default;
    constexpr explicit Radians(float value) noexcept : mValue(value)
    {
        if (std::isnan(value)) {
            jassertfalse;
        }
    }
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
};

constexpr Radians pi{ MathConstants<float>::pi };
constexpr Radians twoPi{ MathConstants<float>::twoPi };
constexpr Radians halfPi{ MathConstants<float>::halfPi };

class Degrees
{
    float mValue{};

public:
    constexpr Degrees() noexcept = default;
    constexpr explicit Degrees(float const value) noexcept : mValue(value)
    {
        if (std::isnan(value)) {
            jassertfalse;
        }
    }
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

    constexpr operator Radians() const { return Radians{ mValue / 360.0f * MathConstants<float>::twoPi }; }
    [[nodiscard]] constexpr float getAsRadians() const { return mValue / 360.0f * MathConstants<float>::twoPi; }
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
};

class SourceIndex
{
private:
    int mValue{};

public:
    constexpr SourceIndex() noexcept = default;
    ~SourceIndex() noexcept = default;

    constexpr SourceIndex(SourceIndex const &) = default;
    constexpr SourceIndex(SourceIndex &&) noexcept = default;

    constexpr SourceIndex & operator=(SourceIndex const &) = default;
    constexpr SourceIndex & operator=(SourceIndex &&) noexcept = default;

    constexpr explicit SourceIndex(int index) noexcept : mValue(index) {}

    constexpr int toInt() const { return mValue; }
    String toString() const { return String{ mValue }; }

    constexpr bool operator==(SourceIndex const & other) const { return mValue == other.mValue; }
    constexpr bool operator!=(SourceIndex const & other) const { return mValue != other.mValue; }
    constexpr bool operator<(SourceIndex const & other) const { return mValue < other.mValue; }
    constexpr bool operator>=(SourceIndex const & other) const { return mValue >= other.mValue; }

    constexpr SourceIndex & operator++()
    {
        ++mValue;
        return *this;
    }
    constexpr SourceIndex operator++(int)
    {
        SourceIndex const temp{ *this };
        ++mValue;
        return temp;
    }

    constexpr bool isValid() const { return mValue >= 0 && mValue < 8; } // TODO: remove magic number
};

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
};

#endif // STRONGTYPESTEST_TYPES_H
