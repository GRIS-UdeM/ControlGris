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

class Normalized {
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

class ClippedPosition {
    float mValue{};
public:
    constexpr ClippedPosition() = default;
    constexpr ClippedPosition(ClippedPosition const &) = default;
    constexpr ClippedPosition(ClippedPosition &&) = default;

    constexpr explicit ClippedPosition(float const value) : mValue(value) { clip(); }
    constexpr explicit ClippedPosition(Normalized const value) : mValue(value * 2.0f - 1.0f) { clip(); }

    ~ClippedPosition() = default;

    constexpr ClippedPosition & operator=(ClippedPosition const &) = default;
    constexpr ClippedPosition & operator=(ClippedPosition &&) = default;

    constexpr bool operator<(ClippedPosition const rhs) const { return mValue < rhs.mValue; }
    constexpr bool operator>(ClippedPosition const rhs) const { return mValue > rhs.mValue; }
    constexpr bool operator<=(ClippedPosition const rhs) const { return mValue <= rhs.mValue; }
    constexpr bool operator>=(ClippedPosition const rhs) const { return mValue >= rhs.mValue; }
    constexpr bool operator==(ClippedPosition const rhs) const { return mValue == rhs.mValue; }
    constexpr bool operator!=(ClippedPosition const rhs) const { return mValue != rhs.mValue; }
    constexpr bool operator<(float const rhs) const { return mValue < rhs; }
    constexpr bool operator>(float const rhs) const { return mValue > rhs; }
    constexpr bool operator<=(float const rhs) const { return mValue <= rhs; }
    constexpr bool operator>=(float const rhs) const { return mValue >= rhs; }
    constexpr bool operator==(float const rhs) const { return mValue == rhs; }
    constexpr bool operator!=(float const rhs) const { return mValue != rhs; }

    constexpr ClippedPosition operator-() const { return ClippedPosition{ -mValue }; }
    constexpr float operator+(ClippedPosition const rhs) const { return mValue + rhs.mValue; }
    constexpr float operator-(ClippedPosition const rhs) const { return mValue - rhs.mValue; }
    constexpr float operator+(float const rhs) const { return mValue + rhs; }
    constexpr float operator-(float const rhs) const { return mValue - rhs; }
    constexpr float operator*(float const rhs) const { return mValue * rhs; }
    constexpr float operator/(float const rhs) const { return mValue / rhs; }

    constexpr operator float() const { return mValue; }
private:
    void clip() { mValue = std::clamp(mValue, -1.0f, 1.0f); }
};

class SourcePosition {
    ClippedPosition mX{};
    ClippedPosition mY{};
public:
    constexpr SourcePosition() = default;
    constexpr SourcePosition(SourcePosition const &) = default;
    constexpr SourcePosition(SourcePosition &&) = default;

    constexpr explicit SourcePosition(ClippedPosition const x, ClippedPosition const y) : mX(x), mY(y) {}
    constexpr explicit SourcePosition(float const x, float const y) : mX(x), mY(y) {}
    constexpr explicit SourcePosition(Point<float> const & point) : mX(point.getX()), mY(point.getY()) {}
    constexpr explicit SourcePosition(Radians const & angle, float radius);

    ~SourcePosition() = default;

    constexpr SourcePosition & operator=(SourcePosition const &) = default;
    constexpr SourcePosition & operator=(SourcePosition &&) = default;

    constexpr bool operator==(SourcePosition const & other) const { return mX == other.mX && mY == other.mY; }
    constexpr bool operator!=(SourcePosition const & other) const { return mX != other.mX || mY != other.mY; }

    constexpr Point<float> operator+(SourcePosition const & rhs) const { return Point<float>{ mX + rhs.mX, mY + rhs.mY }; }
    constexpr Point<float> operator+(Point<float> const & rhs) const { return Point<float>{ mX + rhs.getX(), mY + rhs.getY() }; }
    constexpr Point<float> operator-(SourcePosition const & rhs) const { return Point<float>{ mX - rhs.getX(), mY - rhs.getY() }; }
    constexpr Point<float> operator-(Point<float> const & rhs) const { return Point<float>{ mX - rhs.getX(), mY - rhs.getY() }; }
    constexpr Point<float> operator*(float const rhs) const { return Point<float>{ mX, mY} * rhs; }
    constexpr Point<float> operator/(float const rhs) const { return Point<float>{ mX, mY} / rhs; }

    constexpr ClippedPosition const getX() const { return mX; }
    constexpr ClippedPosition const getY() const { return mY; }
    constexpr void setX(ClippedPosition const x) { mX = x; }
    constexpr void setY(ClippedPosition const y) { mY = y; }

    constexpr operator Point<float>() const { return Point<float>{ static_cast<float>(mX), static_cast<float>(mY) }; }
    
    float getDistanceFromOrigin() const { return std::hypotf(static_cast<float>(mX), static_cast<float>(mY)); }
    Radians getAngle() const { return Radians{ std::atan2(mY, mX) }; }
};

class Azimuth {
    Radians mValue{};
public:
    constexpr Azimuth() = default;
    constexpr Azimuth(Radians const & radians) : mValue(radians) { balance(); }
    constexpr Azimuth(Degrees const & degrees) : Azimuth(static_cast<Radians>(degrees)) { balance(); }
    constexpr Azimuth(Normalized const & normalized) : mValue(Radians{static_cast<float>(normalized) * MathConstants<float>::twoPi - MathConstants<float>::pi}) {}
    constexpr explicit Azimuth(SourcePosition const & sourcePosition) : mValue(Radians{std::atan2(sourcePosition.getY(), sourcePosition.getX())}) { balance(); }
    ~Azimuth() = default;

    constexpr Azimuth(Azimuth const &) = default;
    constexpr Azimuth(Azimuth &&) = default;

    constexpr explicit operator Normalized() const { return Normalized{(static_cast<float>(mValue) + MathConstants<float>::pi) / MathConstants<float>::twoPi}; }

    constexpr Azimuth & operator=(Azimuth const & other) = default;
    constexpr Azimuth & operator=(Azimuth && other) = default;

    bool operator!=(Azimuth const other) const { return mValue != other.mValue; }

    constexpr Azimuth operator+(Azimuth const rhs) const { return Azimuth{ mValue + rhs.mValue }; }
    constexpr Azimuth operator-(Azimuth const rhs) const { return Azimuth{ mValue - rhs.mValue }; }

    constexpr operator Radians() const { return mValue; }
    constexpr operator float() const { return static_cast<float>(mValue); }
private:
    void balance() {
        while(mValue < Radians{-MathConstants<float>::pi}) {
            mValue += Radians{MathConstants<float>::twoPi};
        }
        while (mValue > Radians{ MathConstants<float>::pi}) {
            mValue -= Radians{MathConstants<float>::twoPi};
        }
    }
};