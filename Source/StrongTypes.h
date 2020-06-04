//
// Created by samuel on 2020-06-01.
//

#ifndef STRONGTYPESTEST_TYPES_H
#define STRONGTYPESTEST_TYPES_H

#include "../JuceLibraryCode/JuceHeader.h"

#include <algorithm>

class Degrees;

class Radians {
    float mValue{};
public:
    constexpr Radians() = default;
    constexpr explicit Radians(float radians) : mValue(radians) {}
    constexpr Radians(Degrees const & degrees);
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

    constexpr Radians operator-() const { return Radians{ -mValue };}
    constexpr Radians operator+(Radians const & other) const { return Radians{ mValue + other.mValue }; }
    constexpr Radians operator-(Radians const & other) const { return Radians{ mValue - other.mValue }; }
    constexpr Radians operator*(float const value) const { return Radians{ mValue * value };}
    constexpr Radians operator/(float const value) const { return Radians{ mValue / value };}
    constexpr float operator/(Radians const other) const { return mValue / other.mValue; }

    constexpr Radians & operator+=(Radians const & other) { mValue += other.mValue; return *this;}
    constexpr Radians & operator-=(Radians const & other) { mValue -= other.mValue; return *this;}
    constexpr Radians & operator*=(float const value) { mValue *= value; return *this;}
    constexpr Radians & operator/=(float const value) { mValue /= value; return *this;}

    constexpr operator float() const { return mValue; }
};

constexpr Radians pi{ MathConstants<float>::pi };
constexpr Radians twoPi{ MathConstants<float>::twoPi };
constexpr Radians halfPi{ MathConstants<float>::halfPi };

class Degrees {
    float mValue{};
public:
    constexpr Degrees() = default;
    constexpr explicit Degrees(float const value) : mValue(value) {}
    constexpr Degrees(Radians const radians) : mValue(static_cast<float>(radians) / MathConstants<float>::twoPi * 360.0f) {}
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

    constexpr Degrees operator-() const { return Degrees{ -mValue };}
    constexpr Degrees operator+(Degrees const & other) const { return Degrees{ mValue + other.mValue }; }
    constexpr Degrees operator-(Degrees const & other) const { return Degrees{ mValue - other.mValue }; }
    constexpr Degrees operator*(float const value) const { return Degrees{ mValue * value };}
    constexpr Degrees operator/(float const value) const { return Degrees{ mValue / value };}
    constexpr float operator/(Degrees const other) const { return mValue / other.mValue; }

    constexpr Degrees & operator+=(Degrees const & other) { mValue += other.mValue; return *this;}
    constexpr Degrees & operator-=(Degrees const & other) { mValue -= other.mValue; return *this;}
    constexpr Degrees & operator*=(float const value) { mValue *= value; return *this;}
    constexpr Degrees & operator/=(float const value) { mValue /= value; return *this;}

    constexpr operator float() const { return mValue; }
};


#endif //STRONGTYPESTEST_TYPES_H
