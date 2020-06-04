/**************************************************************************
 * Copyright 2018 UdeM - GRIS - Olivier Belanger                          *
 *                                                                        *
 * This file is part of ControlGris, a multi-mSource spatialization plugin *
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
#include "AutomationManager.h"

#include <algorithm>

constexpr auto MAGIC_1 = 300;
constexpr auto MAGIC_2 = 300.0f;
constexpr auto MAGIC_3 = 99;
constexpr auto MAGIC_4 = 200;
constexpr auto MAGIC_5 = 199;
constexpr auto MAGIC_6 = 199.0f;

AutomationManager::AutomationManager()
{
    mCurrentTrajectoryPoint = Point<float>{ mFieldWidth / 2.0f, mFieldWidth / 2.0f };
    mSource.setX(0.0f);
    mSource.setY(0.0f);
}

void AutomationManager::setFieldWidth(float const newFieldWidth)
{
    float const factor{ newFieldWidth / mFieldWidth };
    mFieldWidth = newFieldWidth;
    AffineTransform const t{ AffineTransform::scale(factor) };
    for (auto & p : mTrajectoryPoints) {
        p.applyTransform(t);
    }
}

void AutomationManager::setPositionActivateState(bool const newState)
{
    mActivateState = newState;
    if (newState == false) {
        mPlaybackPosition = Point<float>{ INVALID_POSITION };
    } else {
        mTrajectoryDeltaTime = 0.0;
        mLastTrajectoryDeltaTime = 0.0;
        mBackAndForthDirection = Direction::forward;
        mDampeningCycleCount = 0;
        mDampeningLastDelta = 0.0;
        mCurrentPlaybackDuration = mPlaybackDuration;
        mCurrentDegreeOfDeviation = 0.0;
        mDeviationCycleCount = 0;
    }
}

void AutomationManager::resetRecordingTrajectory(Point<float> const currentPosition)
{
    mPlaybackPosition = Point<float>{ INVALID_POSITION };
    mTrajectoryPoints.clear();
    mTrajectoryPoints.add(currentPosition);
    mLastRecordingPoint = currentPosition;
    setSourcePosition(Point<float>{ currentPosition.x / mFieldWidth, 1.0f - currentPosition.y / mFieldWidth });
}

Point<float> AutomationManager::smoothRecordingPosition(Point<float> const & pos)
{
    constexpr auto smoothingFactor = 0.8f;

    mLastRecordingPoint = (mLastRecordingPoint - pos) * smoothingFactor + pos;
    return mLastRecordingPoint;
}

void AutomationManager::createRecordingPath(Path & path)
{
    path.startNewSubPath(mTrajectoryPoints[0]);
    for (int i{ 1 }; i < mTrajectoryPoints.size(); ++i) {
        path.lineTo(mTrajectoryPoints[i]);
    }
}

void AutomationManager::setTrajectoryDeltaTime(double const relativeTimeFromPlay)
{
    mTrajectoryDeltaTime = relativeTimeFromPlay / mCurrentPlaybackDuration;
    mTrajectoryDeltaTime = std::fmod(mTrajectoryDeltaTime, 1.0f);
    computeCurrentTrajectoryPoint();
}

void AutomationManager::compressTrajectoryXValues(int maxValue) // TODO: make this function useless asap
{
    auto const offset{ static_cast<int>(10.0f + SOURCE_FIELD_COMPONENT_RADIUS) };
    maxValue -= offset;
    int const size{ getRecordingTrajectorySize() };
    auto const delta{ static_cast<float>(maxValue) / (static_cast<float>(size) + 1.0f) };
    for (int i{}; i < size; ++i) {
        mTrajectoryPoints.data()[i].setX(static_cast<float>(i) * delta + offset);
    }
}

void AutomationManager::computeCurrentTrajectoryPoint()
{
    int const dampeningCyclesTimes2{ mDampeningCycles * 2 };
    double currentScaleMin{ 0.0 };
    double currentScaleMax{ 0.0 };

    if (mTrajectoryPoints.size() > 0) {
        if (mTrajectoryDeltaTime < mLastTrajectoryDeltaTime) {
            if (mIsBackAndForth) {
                this->invertBackAndForthDirection();
                mDampeningCycleCount++;
                if (mDampeningCycleCount >= dampeningCyclesTimes2) {
                    mDampeningCycleCount = dampeningCyclesTimes2;
                }
            }
            mDeviationCycleCount++;
        }
        mLastTrajectoryDeltaTime = mTrajectoryDeltaTime;

        double trajectoryPhase;
        if (mIsBackAndForth && mDampeningCycles > 0) {
            if (mTrajectoryDeltaTime <= 0.5) {
                trajectoryPhase = std::pow(mTrajectoryDeltaTime * 2.0, 2.0) * 0.5;
            } else {
                trajectoryPhase = 1.0 - std::pow(1.0 - ((mTrajectoryDeltaTime - 0.5) * 2.0), 2.0) * 0.5;
            }
        } else {
            trajectoryPhase = mTrajectoryDeltaTime;
        }

        double delta{ trajectoryPhase * mTrajectoryPoints.size() };

        if (mBackAndForthDirection == Direction::backward) {
            delta = mTrajectoryPoints.size() - delta;
        }

        delta = std::clamp(delta, 0.0, static_cast<double>(mTrajectoryPoints.size()));

        if (mIsBackAndForth && mDampeningCycles > 0) {
            if (mDampeningCycleCount < dampeningCyclesTimes2) {
                double const relativeDeltaTime{ (mDampeningCycleCount + mTrajectoryDeltaTime) / dampeningCyclesTimes2 };
                mCurrentPlaybackDuration
                    = mPlaybackDuration - (std::pow(relativeDeltaTime, 2.0) * mPlaybackDuration * 0.25);
                currentScaleMin = relativeDeltaTime * mTrajectoryPoints.size() * 0.5;
                currentScaleMax = mTrajectoryPoints.size() - currentScaleMin;
                double const currentScale{ (currentScaleMax - currentScaleMin) / mTrajectoryPoints.size() };
                delta = delta * currentScale + currentScaleMin;
                mDampeningLastDelta = delta;
            } else {
                delta = mDampeningLastDelta;
            }
        } else {
            mDampeningLastDelta = delta;
        }

        double const deltaRatio{ static_cast<double>(mTrajectoryPoints.size() - 1) / mTrajectoryPoints.size() };
        delta *= deltaRatio;
        auto index = static_cast<int>(delta);
        if (index + 1 < mTrajectoryPoints.size()) {
            auto const frac{ static_cast<float>(delta) - static_cast<float>(index) };
            auto const & p1{ mTrajectoryPoints.getReference(index) };
            auto const & p2{ mTrajectoryPoints.getReference(index + 1) };
            mCurrentTrajectoryPoint = p1 + (p2 - p1) * frac;
        } else {
            mCurrentTrajectoryPoint = mTrajectoryPoints.getLast();
        }
    }

    if (mDegreeOfDeviationPerCycle != 0) {
        bool deviationFlag{ true };
        if (mIsBackAndForth && mDampeningCycles > 0) {
            if (approximatelyEqual(currentScaleMin, currentScaleMax)) {
                deviationFlag = false;
            }
        }
        if (deviationFlag) {
            mCurrentDegreeOfDeviation
                = static_cast<float>(mDeviationCycleCount + mTrajectoryDeltaTime) * mDegreeOfDeviationPerCycle;
            if (mCurrentDegreeOfDeviation >= 360.0f) {
                mCurrentDegreeOfDeviation -= 360.0f;
            }
        }
        AffineTransform const t{ AffineTransform::rotation(mCurrentDegreeOfDeviation / 360.0f
                                                               * MathConstants<float>::twoPi,
                                                           (mFieldWidth / 2.0f),
                                                           (mFieldWidth / 2.0f)) };
        mCurrentTrajectoryPoint.applyTransform(t);
    }

    if (mActivateState) {
        ;
        setSourcePosition(
            Point<float>{ mCurrentTrajectoryPoint.x / mFieldWidth, 1.0f - mCurrentTrajectoryPoint.y / mFieldWidth });
        sendTrajectoryPositionChangedEvent();
    }
}

Point<float> AutomationManager::getCurrentTrajectoryPoint() const
{
    if (mActivateState) {
        return mCurrentTrajectoryPoint;
    } else {
        return Point<float>{ getSourcePosition().x * mFieldWidth, (1.0f - getSourcePosition().y) * mFieldWidth };
    }
}

void AutomationManager::setSourceAndPlaybackPosition(Point<float> const pos)
{
    setSourcePosition(pos);
    setPlaybackPosition(pos);
}

void PositionAutomationManager::sendTrajectoryPositionChangedEvent()
{
    if (mActivateState || mTrajectoryType == PositionTrajectoryType::realtime) {
        mListeners.call([&](Listener & l) { l.trajectoryPositionChanged(this, mSource.getPos()); });
    }
}

void ElevationAutomationManager::sendTrajectoryPositionChangedEvent()
{
    if (mActivateState || mTrajectoryType == ElevationTrajectoryType::realtime) {
        mListeners.call([&](Listener & l) { l.trajectoryPositionChanged(this, mSource.getPos()); });
    }
}

void AutomationManager::fixSourcePosition()
{
    bool const shouldBeFixed{ mSourceLink != PositionSourceLink::independent };
    mSource.fixSourcePosition(shouldBeFixed);
}

void PositionAutomationManager::setTrajectoryType(PositionTrajectoryType const type, Point<float> const & startPos)
{
    mTrajectoryType = type;

    mTrajectoryPoints.clear();

    auto const offset{ mFieldWidth / 2.0f };
    auto constexpr minLim{ 8.0f };
    auto const maxLim{ mFieldWidth - minLim };

    Point<float> const translated{ startPos.translated(-0.5f, -0.5f) * 2.0f };
    float magnitude{ std::hypot(translated.x, translated.y)
                     * ((mFieldWidth - SOURCE_FIELD_COMPONENT_DIAMETER) / 2.0f) };
    float angle{ std::atan2(translated.y, translated.x) };

    auto const fSize{ static_cast<int>(mFieldWidth) };
    int const fSizeOver3{ fSize / 3 };
    int const fSizeOver4{ fSize / 4 };
    float x;
    float y;
    float step{};
    float transX{};
    float transY{};
    float adjustedMagnitude{};
    switch (mTrajectoryType) {
    case PositionTrajectoryType::realtime:
    case PositionTrajectoryType::drawing:
        mPlaybackPosition = Point<float>{ -1.0f, -1.0f };
        break;
    case PositionTrajectoryType::circleClockwise:
        for (int i{}; i < MAGIC_4; ++i) {
            x = sinf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_5 - angle) * magnitude + offset;
            y = -cosf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_5 - angle) * magnitude + offset;
            x = std::clamp(x, minLim, maxLim);
            y = std::clamp(y, minLim, maxLim);
            mTrajectoryPoints.add(Point<float>{ x, y });
        }
        break;
    case PositionTrajectoryType::circleCounterClockwise:
        for (int i{}; i < MAGIC_4; ++i) {
            x = -sinf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_5 - angle) * magnitude + offset;
            y = -cosf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_5 - angle) * magnitude + offset;
            x = std::clamp(x, minLim, maxLim);
            y = std::clamp(y, minLim, maxLim);
            mTrajectoryPoints.add(Point<float>{ x, y });
        }
        break;
    case PositionTrajectoryType::ellipseClockwise:
        for (int i{}; i < MAGIC_4; ++i) {
            x = sinf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_5) * magnitude * 0.5f;
            y = -cosf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_5) * magnitude;
            float const mag{ std::hypotf(x, y) };
            float const ang{ atan2f(y, x) };
            x = mag * cosf(ang - angle) + offset;
            y = mag * sinf(ang - angle) + offset;
            x = std::clamp(x, minLim, maxLim);
            y = std::clamp(y, minLim, maxLim);
            mTrajectoryPoints.add(Point<float>{ x, y });
        }
        break;
    case PositionTrajectoryType::ellipseCounterClockwise:
        for (int i{}; i < MAGIC_4; ++i) {
            x = -sinf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_5) * magnitude * 0.5f;
            y = -cosf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_5) * magnitude;
            float const mag{ std::hypotf(x, y) };
            float const ang{ atan2f(y, x) };
            x = mag * cosf(ang - angle) + offset;
            y = mag * sinf(ang - angle) + offset;
            x = std::clamp(x, minLim, maxLim);
            y = std::clamp(y, minLim, maxLim);
            mTrajectoryPoints.add(Point<float>{ x, y });
        }
        break;
    case PositionTrajectoryType::spiralClockwiseOutIn:
        for (int i{}; i < MAGIC_1; ++i) {
            x = sinf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_3) * magnitude
                * (1.0f - static_cast<float>(i) / MAGIC_2);
            y = -cosf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_3) * magnitude
                * (1.0f - static_cast<float>(i) / MAGIC_2);
            float const mag{ std::hypotf(x, y) };
            float const ang{ atan2f(y, x) };
            x = mag * cosf(ang - angle) + offset;
            y = mag * sinf(ang - angle) + offset;
            x = std::clamp(x, minLim, maxLim);
            y = std::clamp(y, minLim, maxLim);
            mTrajectoryPoints.add(Point<float>{ x, y });
        }
        break;
    case PositionTrajectoryType::spiralCounterClockwiseOutIn:
        for (int i{}; i < MAGIC_1; ++i) {
            x = -sinf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_3) * magnitude
                * (1.0f - static_cast<float>(i) / MAGIC_2);
            y = -cosf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_3) * magnitude
                * (1.0f - static_cast<float>(i) / MAGIC_2);
            float const mag{ std::hypotf(x, y) };
            float const ang{ atan2f(y, x) };
            x = mag * cosf(ang - angle) + offset;
            y = mag * sinf(ang - angle) + offset;
            x = std::clamp(x, minLim, maxLim);
            y = std::clamp(y, minLim, maxLim);
            mTrajectoryPoints.add(Point<float>{ x, y });
        }
        break;
    case PositionTrajectoryType::spiralClockwiseInOut:
        for (int i{}; i < MAGIC_1; ++i) {
            x = sinf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_3) * magnitude
                * (static_cast<float>(i) / MAGIC_2);
            y = -cosf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_3) * magnitude
                * (static_cast<float>(i) / MAGIC_2);
            float const mag{ std::hypotf(x, y) };
            float const ang{ atan2f(y, x) };
            x = mag * cosf(ang - angle) + offset;
            y = mag * sinf(ang - angle) + offset;
            x = std::clamp(x, minLim, maxLim);
            y = std::clamp(y, minLim, maxLim);
            mTrajectoryPoints.add(Point<float>{ x, y });
        }
        break;
    case PositionTrajectoryType::spiralCounterClockwiseInOut:
        for (int i{}; i < MAGIC_1; ++i) {
            x = -sinf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_3) * magnitude
                * (static_cast<float>(i) / MAGIC_2);
            y = -cosf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_3) * magnitude
                * (static_cast<float>(i) / MAGIC_2);
            float const mag{ std::hypotf(x, y) };
            float const ang{ atan2f(y, x) };
            x = mag * cosf(ang - angle) + offset;
            y = mag * sinf(ang - angle) + offset;
            x = std::clamp(x, minLim, maxLim);
            y = std::clamp(y, minLim, maxLim);
            mTrajectoryPoints.add(Point<float>{ x, y });
        }
        break;
    case PositionTrajectoryType::squareClockwise:
    case PositionTrajectoryType::squareCounterClockwise:
        step = 1.0f / (mFieldWidth / 4);
        transX = translated.x * ((mFieldWidth / 2 - SOURCE_FIELD_COMPONENT_RADIUS));
        transY = translated.y * ((mFieldWidth / 2 - SOURCE_FIELD_COMPONENT_RADIUS));
        magnitude = std::hypotf(transX, transY);
        adjustedMagnitude = magnitude * MathConstants<float>::halfPi;
        float tmp1;
        float tmp2;
        for (int i{}; i < fSize; ++i) {
            if (i < fSizeOver4) {
                tmp1 = static_cast<float>(i) * step;
                tmp2 = 0.0f;
            } else if (i < (fSizeOver4 * 2)) {
                tmp1 = 1.0f;
                tmp2 = static_cast<float>(i - fSizeOver4) * step;
            } else if (i < (fSizeOver4 * 3)) {
                tmp1 = 1.0f - (static_cast<float>(i - fSizeOver4 * 2) * step);
                tmp2 = 1.0f;
            } else {
                tmp1 = 0.0f;
                tmp2 = 1.0f - (static_cast<float>(i - fSizeOver4 * 3) * step);
            }
            if (mTrajectoryType == PositionTrajectoryType::squareClockwise) {
                x = tmp1;
                y = tmp2;
            } else {
                x = tmp2;
                y = tmp1;
            }
            x *= adjustedMagnitude;
            y *= adjustedMagnitude;
            x = x + (mFieldWidth / 2.0f) - (adjustedMagnitude / 2);
            y = y + (mFieldWidth / 2.0f) - (adjustedMagnitude / 2);
            Point<float> p{ x, y };
            AffineTransform const t{ AffineTransform::rotation(-angle + MathConstants<float>::pi / 4.0f,
                                                               (mFieldWidth / 2.0f),
                                                               (mFieldWidth / 2.0f)) };
            p.applyTransform(t);
            p.x = std::clamp(p.x, minLim, maxLim);
            p.y = std::clamp(p.y, minLim, maxLim);
            mTrajectoryPoints.add(p);
        }
        break;
    case PositionTrajectoryType::triangleClockwise:
    case PositionTrajectoryType::triangleCounterClockwise: {
        Point<float> const p1{ 0.0f, -1.0f };
        Point<float> p2{};
        Point<float> p3{};
        if (mTrajectoryType == PositionTrajectoryType::triangleClockwise) {
            p2 = Point<float>{ 1.0f, 1.0f };
            p3 = Point<float>{ -1.0f, 1.0f };
        } else {
            p2 = Point<float>{ -1.0f, 1.0f };
            p3 = Point<float>{ 1.0f, 1.0f };
        }
        step = 1.0f / (mFieldWidth / 3.0f);
        transX = translated.x * ((mFieldWidth / 2.0f - SOURCE_FIELD_COMPONENT_RADIUS));
        transY = translated.y * ((mFieldWidth / 2.0f - SOURCE_FIELD_COMPONENT_RADIUS));
        magnitude = std::hypotf(transX, transY);
        for (int i{}; i < fSize; ++i) {
            if (i < (fSizeOver3)) {
                x = (p2.x - p1.x) * static_cast<float>(i) * step + p1.x;
                y = (p2.y - p1.y) * static_cast<float>(i) * step + p1.y;
            } else if (i < (fSizeOver3 * 2)) {
                x = (p3.x - p2.x) * static_cast<float>(i - fSizeOver3) * step + p2.x;
                y = (p3.y - p2.y) * static_cast<float>(i - fSizeOver3) * step + p2.y;
            } else {
                x = (p1.x - p3.x) * static_cast<float>(i - fSizeOver3 * 2) * step + p3.x;
                y = (p1.y - p3.y) * static_cast<float>(i - fSizeOver3 * 2) * step + p3.y;
            }
            Point<float> p(x * magnitude + (mFieldWidth / 2.0f), y * magnitude + (mFieldWidth / 2.0f));
            p.applyTransform(AffineTransform::rotation(-angle, (mFieldWidth / 2.0f), (mFieldWidth / 2.0f)));
            p.x = std::clamp(p.x, minLim, maxLim);
            p.y = std::clamp(p.y, minLim, maxLim);
            mTrajectoryPoints.add(p);
        }
    } break;
    case PositionTrajectoryType::undefined:
        jassertfalse;
    }

    if (mTrajectoryType > PositionTrajectoryType::drawing) {
        setSourcePosition(
            Point<float>{ mTrajectoryPoints[0].x / mFieldWidth, 1.0f - mTrajectoryPoints[0].y / mFieldWidth });
    } else {
        setSourcePosition(Point<float>{ 0.5f, 0.5f });
    }
}

void ElevationAutomationManager::setTrajectoryType(ElevationTrajectoryType const type)
{
    mTrajectoryType = type;

    mTrajectoryPoints.clear();

    auto constexpr offset{ 10.0f + SOURCE_FIELD_COMPONENT_RADIUS };
    auto const width{ mFieldWidth - offset };
    auto constexpr minPos{ 15.0f };
    auto const maxPos{ mFieldWidth - 20.0f };

    switch (type) {
    case ElevationTrajectoryType::realtime:
    case ElevationTrajectoryType::drawing:
        mPlaybackPosition = Point<float>{ -1.0f, -1.0f };
        break;
    case ElevationTrajectoryType::downUp:
        for (int i{}; i < MAGIC_4; ++i) {
            float const x = (static_cast<float>(i) / MAGIC_6) * width + offset;
            float const y = (static_cast<float>(i) / MAGIC_6) * (maxPos - minPos) + minPos;
            mTrajectoryPoints.add(Point<float>{ x, y });
        }
        break;
    case ElevationTrajectoryType::upDown:
        for (int i{}; i < MAGIC_4; ++i) {
            float const x = (static_cast<float>(i) / MAGIC_6) * width + offset;
            float const y = (1.0f - static_cast<float>(i) / MAGIC_6) * (maxPos - minPos) + minPos;
            mTrajectoryPoints.add(Point<float>{ x, y });
        }
        break;
    case ElevationTrajectoryType::undefined:
        jassertfalse;
    }

    if (type > ElevationTrajectoryType::drawing) {
        setSourcePosition(
            Point<float>{ mTrajectoryPoints[0].x / mFieldWidth, 1.0f - mTrajectoryPoints[0].y / mFieldWidth });
    } else {
        setSourcePosition(Point<float>{ 0.5f, 0.5f });
    }
}
