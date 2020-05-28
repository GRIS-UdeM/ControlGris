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
constexpr auto MAGIC_2 = 300.f;
constexpr auto MAGIC_3 = 99;
constexpr auto MAGIC_4 = 200;
constexpr auto MAGIC_5 = 199;
constexpr auto MAGIC_6 = 199.f;

AutomationManager::AutomationManager()
{
    mCurrentTrajectoryPoint = Point<float>{ mFieldWidth / 2.f, mFieldWidth / 2.f };
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

void AutomationManager::setActivateState(bool const newState)
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
    setSourcePosition(Point<float>{ currentPosition.x / mFieldWidth, 1.f - currentPosition.y / mFieldWidth });
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
    mTrajectoryDeltaTime = std::fmod(mTrajectoryDeltaTime, 1.f);
    computeCurrentTrajectoryPoint();
}

void AutomationManager::compressTrajectoryXValues(int maxValue) // TODO: make this function useless asap
{
    auto const offset{ static_cast<int>(10.f + kSourceRadius) };
    maxValue -= offset;
    int const size{ getRecordingTrajectorySize() };
    auto const delta{ static_cast<float>(maxValue) / (static_cast<float>(size) + 1.f) };
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
                                                           (mFieldWidth / 2.f),
                                                           (mFieldWidth / 2.f)) };
        mCurrentTrajectoryPoint.applyTransform(t);
    }

    if (mActivateState) {
        ;
        setSourcePosition(
            Point<float>{ mCurrentTrajectoryPoint.x / mFieldWidth, 1.f - mCurrentTrajectoryPoint.y / mFieldWidth });
        sendTrajectoryPositionChangedEvent();
    }
}

Point<float> AutomationManager::getCurrentTrajectoryPoint() const
{
    if (mActivateState) {
        return mCurrentTrajectoryPoint;
    } else {
        return Point<float>{ getSourcePosition().x * mFieldWidth, (1.f - getSourcePosition().y) * mFieldWidth };
    }
}

void AutomationManager::setSourceAndPlaybackPosition(Point<float> const pos)
{
    setSourcePosition(pos);
    setPlaybackPosition(pos);
}

void AutomationManager::sendTrajectoryPositionChangedEvent()
{
    if (mActivateState || mDrawingType == TrajectoryType::realtime
        || static_cast<ElevationTrajectoryType>(mDrawingType) == ElevationTrajectoryType::realtime) {
        mListeners.call([&](Listener & l) { l.trajectoryPositionChanged(this, mSource.getPos()); });
    }
}

void AutomationManager::fixSourcePosition()
{
    bool const shouldBeFixed{ mSourceLink != SourceLink::independent };
    mSource.fixSourcePosition(shouldBeFixed);
}

void AutomationManager::setDrawingType(TrajectoryType const type, Point<float> const & startPos)
{
    mDrawingType = type;

    mTrajectoryPoints.clear();

    auto const offset{ static_cast<int>(mFieldWidth / 2.f) };
    auto constexpr minLim{ 8 };
    auto const maxLim{ static_cast<int>(mFieldWidth - minLim) };

    Point<float> const translated{ startPos.translated(-0.5f, -0.5f) * 2.0f };
    float magnitude{ sqrtf(translated.x * translated.x + translated.y * translated.y)
                     * ((mFieldWidth - kSourceDiameter) / 2.f) };
    float angle{ atan2f(translated.y, translated.x) - MathConstants<float>::halfPi };

    auto const fSize{ static_cast<int>(mFieldWidth) };
    int const fSizeOver3{ fSize / 3 };
    int const fSizeOver4{ fSize / 4 };
    float x;
    float y;
    float step{};
    float transX{};
    float transY{};
    float adjustedMagnitude{};
    switch (mDrawingType) {
    case TrajectoryType::realtime:
    case TrajectoryType::drawing:
        mPlaybackPosition = Point<float>{ -1.0f, -1.0f };
        break;
    case TrajectoryType::circleClockwise:
        for (int i{}; i < MAGIC_4; ++i) {
            x = sinf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_5 - angle) * magnitude + offset;
            x = x < minLim ? minLim : x > maxLim ? maxLim : x;
            y = -cosf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_5 - angle) * magnitude + offset;
            y = y < minLim ? minLim : y > maxLim ? maxLim : y;
            mTrajectoryPoints.add(Point<float>{ x, y });
        }
        break;
    case TrajectoryType::circleCounterClockwise:
        for (int i{}; i < MAGIC_4; ++i) {
            x = -sinf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_5 - angle) * magnitude + offset;
            x = x < minLim ? minLim : x > maxLim ? maxLim : x;
            y = -cosf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_5 - angle) * magnitude + offset;
            y = y < minLim ? minLim : y > maxLim ? maxLim : y;
            mTrajectoryPoints.add(Point<float>{ x, y });
        }
        break;
    case TrajectoryType::ellipseClockwise:
        for (int i{}; i < MAGIC_4; ++i) {
            x = sinf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_5) * magnitude * 0.5f;
            y = -cosf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_5) * magnitude;
            float const mag{ sqrtf(x * x + y * y) };
            float const ang{ atan2f(y, x) };
            x = mag * cosf(ang - angle) + offset;
            y = mag * sinf(ang - angle) + offset;
            x = x < minLim ? minLim : x > maxLim ? maxLim : x;
            y = y < minLim ? minLim : y > maxLim ? maxLim : y;
            mTrajectoryPoints.add(Point<float>{ x, y });
        }
        break;
    case TrajectoryType::ellipseCounterClockwise:
        for (int i{}; i < MAGIC_4; ++i) {
            x = -sinf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_5) * magnitude * 0.5f;
            y = -cosf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_5) * magnitude;
            float const mag{ sqrtf(x * x + y * y) };
            float const ang{ atan2f(y, x) };
            x = mag * cosf(ang - angle) + offset;
            y = mag * sinf(ang - angle) + offset;
            x = x < minLim ? minLim : x > maxLim ? maxLim : x;
            y = y < minLim ? minLim : y > maxLim ? maxLim : y;
            mTrajectoryPoints.add(Point<float>{ x, y });
        }
        break;
    case TrajectoryType::spiralClockwiseOutIn:
        for (int i{}; i < MAGIC_1; ++i) {
            x = sinf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_3) * magnitude
                * (1.f - static_cast<float>(i) / MAGIC_2);
            y = -cosf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_3) * magnitude
                * (1.f - static_cast<float>(i) / MAGIC_2);
            float const mag{ sqrtf(x * x + y * y) };
            float const ang{ atan2f(y, x) };
            x = mag * cosf(ang - angle) + offset;
            y = mag * sinf(ang - angle) + offset;
            x = x < minLim ? minLim : x > maxLim ? maxLim : x;
            y = y < minLim ? minLim : y > maxLim ? maxLim : y;
            mTrajectoryPoints.add(Point<float>{ x, y });
        }
        break;
    case TrajectoryType::spiralCounterClockwiseOutIn:
        for (int i{}; i < MAGIC_1; ++i) {
            x = -sinf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_3) * magnitude
                * (1.f - static_cast<float>(i) / MAGIC_2);
            y = -cosf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_3) * magnitude
                * (1.f - static_cast<float>(i) / MAGIC_2);
            float const mag{ sqrtf(x * x + y * y) };
            float const ang{ atan2f(y, x) };
            x = mag * cosf(ang - angle) + offset;
            y = mag * sinf(ang - angle) + offset;
            x = x < minLim ? minLim : x > maxLim ? maxLim : x;
            y = y < minLim ? minLim : y > maxLim ? maxLim : y;
            mTrajectoryPoints.add(Point<float>{ x, y });
        }
        break;
    case TrajectoryType::spiralClockwiseInOut:
        for (int i{}; i < MAGIC_1; ++i) {
            x = sinf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_3) * magnitude
                * (static_cast<float>(i) / MAGIC_2);
            y = -cosf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_3) * magnitude
                * (static_cast<float>(i) / MAGIC_2);
            float const mag{ sqrtf(x * x + y * y) };
            float const ang{ atan2f(y, x) };
            x = mag * cosf(ang - angle) + offset;
            y = mag * sinf(ang - angle) + offset;
            x = x < minLim ? minLim : x > maxLim ? maxLim : x;
            y = y < minLim ? minLim : y > maxLim ? maxLim : y;
            mTrajectoryPoints.add(Point<float>{ x, y });
        }
        break;
    case TrajectoryType::spiralCounterClockwiseInOut:
        for (int i{}; i < MAGIC_1; ++i) {
            x = -sinf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_3) * magnitude
                * (static_cast<float>(i) / MAGIC_2);
            y = -cosf(MathConstants<float>::twoPi * static_cast<float>(i) / MAGIC_3) * magnitude
                * (static_cast<float>(i) / MAGIC_2);
            float const mag{ sqrtf(x * x + y * y) };
            float const ang{ atan2f(y, x) };
            x = mag * cosf(ang - angle) + offset;
            y = mag * sinf(ang - angle) + offset;
            x = x < minLim ? minLim : x > maxLim ? maxLim : x;
            y = y < minLim ? minLim : y > maxLim ? maxLim : y;
            mTrajectoryPoints.add(Point<float>{ x, y });
        }
        break;
    case TrajectoryType::squareClockwise:
    case TrajectoryType::squareCounterClockwise:
        step = 1.f / (mFieldWidth / 4);
        transX = translated.x * ((mFieldWidth / 2 - kSourceRadius));
        transY = translated.y * ((mFieldWidth / 2 - kSourceRadius));
        magnitude = sqrtf(transX * transX + transY * transY);
        adjustedMagnitude = magnitude * MathConstants<float>::halfPi;
        float tmp1;
        float tmp2;
        for (int i{}; i < fSize; ++i) {
            if (i < fSizeOver4) {
                tmp1 = static_cast<float>(i) * step;
                tmp2 = 0.f;
            } else if (i < (fSizeOver4 * 2)) {
                tmp1 = 1.f;
                tmp2 = static_cast<float>(i - fSizeOver4) * step;
            } else if (i < (fSizeOver4 * 3)) {
                tmp1 = 1.f - (static_cast<float>(i - fSizeOver4 * 2) * step);
                tmp2 = 1.f;
            } else {
                tmp1 = 0.f;
                tmp2 = 1.f - (static_cast<float>(i - fSizeOver4 * 3) * step);
            }
            if (mDrawingType == TrajectoryType::squareClockwise) {
                x = tmp1;
                y = tmp2;
            } else {
                x = tmp2;
                y = tmp1;
            }
            x *= adjustedMagnitude;
            y *= adjustedMagnitude;
            x = x + (mFieldWidth / 2.f) - (adjustedMagnitude / 2);
            y = y + (mFieldWidth / 2.f) - (adjustedMagnitude / 2);
            Point<float> p{ x, y };
            AffineTransform const t = AffineTransform::rotation(-angle + MathConstants<float>::pi / 4.f,
                                                                (mFieldWidth / 2.f),
                                                                (mFieldWidth / 2.f));
            p.applyTransform(t);
            p.x = p.x < minLim ? minLim : p.x > maxLim ? maxLim : p.x;
            p.y = p.y < minLim ? minLim : p.y > maxLim ? maxLim : p.y;
            mTrajectoryPoints.add(p);
        }
        break;
    case TrajectoryType::triangleClockwise:
    case TrajectoryType::triangleCounterClockwise: {
        Point<float> const p1{ 0.f, -1.f };
        Point<float> p2{};
        Point<float> p3{};
        if (mDrawingType == TrajectoryType::triangleClockwise) {
            p2 = Point<float>{ 1.f, 1.f };
            p3 = Point<float>{ -1.f, 1.f };
        } else {
            p2 = Point<float>{ -1.f, 1.f };
            p3 = Point<float>{ 1.f, 1.f };
        }
        step = 1.f / (mFieldWidth / 3.f);
        transX = translated.x * ((mFieldWidth / 2.f - kSourceRadius));
        transY = translated.y * ((mFieldWidth / 2.f - kSourceRadius));
        magnitude = sqrtf(transX * transX + transY * transY);
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
            Point<float> p(x * magnitude + (mFieldWidth / 2.f), y * magnitude + (mFieldWidth / 2.f));
            p.applyTransform(AffineTransform::rotation(-angle, (mFieldWidth / 2.f), (mFieldWidth / 2.f)));
            p.x = p.x < minLim ? minLim : p.x > maxLim ? maxLim : p.x;
            p.y = p.y < minLim ? minLim : p.y > maxLim ? maxLim : p.y;
            mTrajectoryPoints.add(p);
        }
    } break;
    case TrajectoryType::undefined:
        jassertfalse;
    }

    if (mDrawingType > TrajectoryType::drawing) {
        setSourcePosition(
            Point<float>{ mTrajectoryPoints[0].x / mFieldWidth, 1.f - mTrajectoryPoints[0].y / mFieldWidth });
    } else {
        setSourcePosition(Point<float>{ 0.5f, 0.5f });
    }
}

void AutomationManager::setDrawingTypeAlt(ElevationTrajectoryType const type)
{
    mDrawingType = static_cast<TrajectoryType>(type);

    mTrajectoryPoints.clear();

    auto constexpr offset{ 10.f + kSourceRadius };
    auto const width{ mFieldWidth - offset };
    auto constexpr minPos{ 15.f };
    auto const maxPos{ mFieldWidth - 20.f };

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
            float const y = (1.f - static_cast<float>(i) / MAGIC_6) * (maxPos - minPos) + minPos;
            mTrajectoryPoints.add(Point<float>{ x, y });
        }
        break;
    case ElevationTrajectoryType::undefined:
        jassertfalse;
    }

    if (type > ElevationTrajectoryType::drawing) {
        setSourcePosition(
            Point<float>{ mTrajectoryPoints[0].x / mFieldWidth, 1.f - mTrajectoryPoints[0].y / mFieldWidth });
    } else {
        setSourcePosition(Point<float>{ 0.5f, 0.5f });
    }
}
