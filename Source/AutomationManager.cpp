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

constexpr auto MAGIC_1 = 300;
constexpr auto MAGIC_2 = 300.f;
constexpr auto MAGIC_3 = 99;
constexpr auto MAGIC_4 = 200;
constexpr auto MAGIC_5 = 199;
constexpr auto MAGIC_6 = 199.0;

AutomationManager::AutomationManager()
{
    mCurrentTrajectoryPoint = Point<float>{ mFieldWidth / 2, mFieldWidth / 2 };
    mSource.setX(0.0f);
    mSource.setY(0.0f);
}

void AutomationManager::setFieldWidth(float const newFieldWidth)
{
    float const factor{ newFieldWidth / mFieldWidth };
    mFieldWidth = newFieldWidth;
    if (mTrajectoryPoints.size() > 0) {
        AffineTransform const t{ AffineTransform::scale(factor) };
        for (auto & p : mTrajectoryPoints) {
            p.applyTransform(t);
        }
    }
}

void AutomationManager::setActivateState(bool const state)
{
    mActivateState = state;
    if (!state) {
        mPlaybackPosition = Point<float>{ -1.f, -1.f };
    } else {
        mTrajectoryDeltaTime = 0.0;
        mLastTrajectoryDeltaTime = 0.0;
        mBackAndForthDirection = 0;
        mDampeningCycleCount = 0;
        mDampeningLastDelta = 0.0;
        mCurrentPlaybackDuration = mPlaybackDuration;
        mCurrentDegreeOfDeviation = 0.0;
        mDeviationCycleCount = 0;
    }
}

void AutomationManager::setBackAndForth(bool const shouldBeOn)
{
    if (shouldBeOn != mIsBackAndForth) {
        mIsBackAndForth = shouldBeOn;
    }
}

void AutomationManager::resetRecordingTrajectory(Point<float> const currentPosition)
{
    mPlaybackPosition = Point<float>{ -1.f, -1.f };
    mTrajectoryPoints.clear();
    mTrajectoryPoints.add(currentPosition);
    mLastRecordingPoint = currentPosition;
    setSourcePosition(Point<float>{ currentPosition.x / mFieldWidth, 1.f - currentPosition.y / mFieldWidth });
}

Point<float> AutomationManager::smoothRecordingPosition(Point<float> const pos)
{
    mLastRecordingPoint.x = pos.x + (mLastRecordingPoint.x - pos.x) * 0.8f;
    mLastRecordingPoint.y = pos.y + (mLastRecordingPoint.y - pos.y) * 0.8f;
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

void AutomationManager::compressTrajectoryXValues(int maxValue)
{
    auto const offset{ static_cast<int>(10.0 + kSourceRadius) };
    maxValue -= offset;
    int const size{ getRecordingTrajectorySize() };
    float const delta{ maxValue / (size + 1.f) };
    for (int i{}; i < size; ++i) {
        mTrajectoryPoints.data()[i].setX(i * delta + offset);
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
                mBackAndForthDirection = 1 - mBackAndForthDirection;
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
                trajectoryPhase = std::pow(mTrajectoryDeltaTime * 2.0, 2.0) * 0.5f;
            } else {
                trajectoryPhase = 1.0 - pow(1.0 - ((mTrajectoryDeltaTime - 0.5) * 2.0), 2.0) * 0.5f;
            }
        } else {
            trajectoryPhase = mTrajectoryDeltaTime;
        }

        double delta{ trajectoryPhase * mTrajectoryPoints.size() };

        if (mBackAndForthDirection == 1) {
            delta = mTrajectoryPoints.size() - delta;
        }

        if (delta + 1 >= mTrajectoryPoints.size()) {
            delta = mTrajectoryPoints.size();
        } else if (delta < 0) {
            delta = 0;
        }

        if (mIsBackAndForth && mDampeningCycles > 0) {
            if (mDampeningCycleCount < dampeningCyclesTimes2) {
                double const relativeDeltaTime{ (mDampeningCycleCount + mTrajectoryDeltaTime) / dampeningCyclesTimes2 };
                mCurrentPlaybackDuration = mPlaybackDuration - (pow(relativeDeltaTime, 2.0) * mPlaybackDuration * 0.25);
                currentScaleMin = relativeDeltaTime * mTrajectoryPoints.size() * 0.5f;
                currentScaleMax = mTrajectoryPoints.size() - currentScaleMin;
                double const currentScale{ (currentScaleMax - currentScaleMin) / mTrajectoryPoints.size() };
                mDampeningLastDelta = delta = delta * currentScale + currentScaleMin;
            } else {
                delta = mDampeningLastDelta;
            }
        } else {
            mDampeningLastDelta = delta;
        }

        double const deltaRatio{ static_cast<double>(mTrajectoryPoints.size() - 1) / mTrajectoryPoints.size() };
        delta *= deltaRatio;
        int index = (int)delta;
        if (index + 1 < mTrajectoryPoints.size()) {
            auto const frac{ static_cast<float>(delta) - index };
            Point<float> const p1{ mTrajectoryPoints[index] };
            Point<float> const p2{ mTrajectoryPoints[index + 1] };
            mCurrentTrajectoryPoint = Point<float>{ p1.x + (p2.x - p1.x) * frac, p1.y + (p2.y - p1.y) * frac };
        } else {
            mCurrentTrajectoryPoint = Point<float>{ mTrajectoryPoints.getLast().x, mTrajectoryPoints.getLast().y };
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
            mCurrentDegreeOfDeviation = (mDeviationCycleCount + mTrajectoryDeltaTime) * mDegreeOfDeviationPerCycle;
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
    setSourcePositionX(pos.x);
    setSourcePositionY(pos.y);
    setPlaybackPositionX(pos.x);
    setPlaybackPositionY(pos.y);
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

void AutomationManager::setDrawingType(TrajectoryType const type, Point<float> const startpos)
{
    mDrawingType = type;

    mTrajectoryPoints.clear();

    auto const offset{ static_cast<int>(mFieldWidth / 2) };
    auto const minlim{ 8 };
    auto const maxlim{ static_cast<int>(mFieldWidth - 8) };

    Point<float> const translated{ startpos.translated(-0.5f, -0.5f) * 2.0f };
    float magnitude{ sqrtf(translated.x * translated.x + translated.y * translated.y)
                     * ((mFieldWidth - kSourceDiameter) / 2) };
    float angle{ atan2f(translated.y, translated.x) - MathConstants<float>::halfPi };

    int const fsize{ static_cast<int>(mFieldWidth) };
    int const fsizeOver3{ fsize / 3 };
    int const fsizeOver4{ fsize / 4 };
    float x;
    float y;
    float step{};
    float transx{};
    float transy{};
    float adjustedMagnitude{};
    switch (mDrawingType) {
    case TrajectoryType::realtime:
        mPlaybackPosition = Point<float>{ -1.0f, -1.0f };
        break;
    case TrajectoryType::drawing:
        mPlaybackPosition = Point<float>{ -1.0f, -1.0f };
        break;
    case TrajectoryType::circleClockwise:
        for (int i{}; i < MAGIC_4; ++i) {
            x = sinf(MathConstants<float>::twoPi * i / MAGIC_5 - angle) * magnitude + offset;
            x = x < minlim ? minlim : x > maxlim ? maxlim : x;
            y = -cosf(MathConstants<float>::twoPi * i / MAGIC_5 - angle) * magnitude + offset;
            y = y < minlim ? minlim : y > maxlim ? maxlim : y;
            mTrajectoryPoints.add(Point<float>{ x, y });
        }
        break;
    case TrajectoryType::circleCounterClockwise:
        for (int i{}; i < MAGIC_4; ++i) {
            x = -sinf(MathConstants<float>::twoPi * i / MAGIC_5 - angle) * magnitude + offset;
            x = x < minlim ? minlim : x > maxlim ? maxlim : x;
            y = -cosf(MathConstants<float>::twoPi * i / MAGIC_5 - angle) * magnitude + offset;
            y = y < minlim ? minlim : y > maxlim ? maxlim : y;
            mTrajectoryPoints.add(Point<float>{ x, y });
        }
        break;
    case TrajectoryType::ellipseClockwise:
        for (int i{}; i < MAGIC_4; ++i) {
            x = sinf(MathConstants<float>::twoPi * i / MAGIC_5) * magnitude * 0.5f;
            y = -cosf(MathConstants<float>::twoPi * i / MAGIC_5) * magnitude;
            float const mag{ sqrtf(x * x + y * y) };
            float const ang{ atan2f(y, x) };
            x = mag * cosf(ang - angle) + offset;
            y = mag * sinf(ang - angle) + offset;
            x = x < minlim ? minlim : x > maxlim ? maxlim : x;
            y = y < minlim ? minlim : y > maxlim ? maxlim : y;
            mTrajectoryPoints.add(Point<float>{ x, y });
        }
        break;
    case TrajectoryType::ellipseCounterClockwise:
        for (int i{}; i < MAGIC_4; ++i) {
            x = -sinf(MathConstants<float>::twoPi * i / MAGIC_5) * magnitude * 0.5f;
            y = -cosf(MathConstants<float>::twoPi * i / MAGIC_5) * magnitude;
            float const mag{ sqrtf(x * x + y * y) };
            float const ang{ atan2f(y, x) };
            x = mag * cosf(ang - angle) + offset;
            y = mag * sinf(ang - angle) + offset;
            x = x < minlim ? minlim : x > maxlim ? maxlim : x;
            y = y < minlim ? minlim : y > maxlim ? maxlim : y;
            mTrajectoryPoints.add(Point<float>{ x, y });
        }
        break;
    case TrajectoryType::spiralClockwiseOutIn:
        for (int i{}; i < MAGIC_1; ++i) {
            x = sinf(MathConstants<float>::twoPi * i / MAGIC_3) * magnitude * (1.f - i / MAGIC_2);
            y = -cosf(MathConstants<float>::twoPi * i / MAGIC_3) * magnitude * (1.f - i / MAGIC_2);
            float const mag{ sqrtf(x * x + y * y) };
            float const ang{ atan2f(y, x) };
            x = mag * cosf(ang - angle) + offset;
            y = mag * sinf(ang - angle) + offset;
            x = x < minlim ? minlim : x > maxlim ? maxlim : x;
            y = y < minlim ? minlim : y > maxlim ? maxlim : y;
            mTrajectoryPoints.add(Point<float>{ x, y });
        }
        break;
    case TrajectoryType::spiralCounterClockwiseOutIn:
        for (int i{}; i < MAGIC_1; ++i) {
            x = -sinf(MathConstants<float>::twoPi * i / MAGIC_3) * magnitude * (1.f - i / MAGIC_2);
            y = -cosf(MathConstants<float>::twoPi * i / MAGIC_3) * magnitude * (1.f - i / MAGIC_2);
            float const mag{ sqrtf(x * x + y * y) };
            float const ang{ atan2f(y, x) };
            x = mag * cosf(ang - angle) + offset;
            y = mag * sinf(ang - angle) + offset;
            x = x < minlim ? minlim : x > maxlim ? maxlim : x;
            y = y < minlim ? minlim : y > maxlim ? maxlim : y;
            mTrajectoryPoints.add(Point<float>{ x, y });
        }
        break;
    case TrajectoryType::spiralClockwiseInOut:
        for (int i{}; i < MAGIC_1; ++i) {
            x = sinf(MathConstants<float>::twoPi * i / MAGIC_3) * magnitude * (i / MAGIC_2);
            y = -cosf(MathConstants<float>::twoPi * i / MAGIC_3) * magnitude * (i / MAGIC_2);
            float const mag{ sqrtf(x * x + y * y) };
            float const ang{ atan2f(y, x) };
            x = mag * cosf(ang - angle) + offset;
            y = mag * sinf(ang - angle) + offset;
            x = x < minlim ? minlim : x > maxlim ? maxlim : x;
            y = y < minlim ? minlim : y > maxlim ? maxlim : y;
            mTrajectoryPoints.add(Point<float>{ x, y });
        }
        break;
    case TrajectoryType::spiralCounterClockwiseInOut:
        for (int i{}; i < MAGIC_1; ++i) {
            x = -sinf(MathConstants<float>::twoPi * i / MAGIC_3) * magnitude * (i / MAGIC_2);
            y = -cosf(MathConstants<float>::twoPi * i / MAGIC_3) * magnitude * (i / MAGIC_2);
            float const mag{ sqrtf(x * x + y * y) };
            float const ang{ atan2f(y, x) };
            x = mag * cosf(ang - angle) + offset;
            y = mag * sinf(ang - angle) + offset;
            x = x < minlim ? minlim : x > maxlim ? maxlim : x;
            y = y < minlim ? minlim : y > maxlim ? maxlim : y;
            mTrajectoryPoints.add(Point<float>{ x, y });
        }
        break;
    case TrajectoryType::squareClockwise:
    case TrajectoryType::squareCounterClockwise:
        step = 1.f / (mFieldWidth / 4);
        transx = translated.x * ((mFieldWidth / 2 - kSourceRadius));
        transy = translated.y * ((mFieldWidth / 2 - kSourceRadius));
        magnitude = sqrtf(transx * transx + transy * transy);
        adjustedMagnitude = magnitude * MathConstants<float>::halfPi;
        float tmp1;
        float tmp2;
        for (int i{}; i < fsize; ++i) {
            if (i < fsizeOver4) {
                tmp1 = i * step;
                tmp2 = 0.f;
            } else if (i < (fsizeOver4 * 2)) {
                tmp1 = 1.f;
                tmp2 = (i - fsizeOver4) * step;
            } else if (i < (fsizeOver4 * 3)) {
                tmp1 = 1.f - ((i - fsizeOver4 * 2) * step);
                tmp2 = 1.f;
            } else {
                tmp1 = 0.f;
                tmp2 = 1.f - ((i - fsizeOver4 * 3) * step);
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
            p.x = p.x < minlim ? minlim : p.x > maxlim ? maxlim : p.x;
            p.y = p.y < minlim ? minlim : p.y > maxlim ? maxlim : p.y;
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
        step = 1.f / (mFieldWidth / 3.0);
        transx = translated.x * ((mFieldWidth / 2.0 - kSourceRadius));
        transy = translated.y * ((mFieldWidth / 2.0 - kSourceRadius));
        magnitude = sqrtf(transx * transx + transy * transy);
        for (int i{}; i < fsize; ++i) {
            if (i < (fsizeOver3)) {
                x = (p2.x - p1.x) * i * step + p1.x;
                y = (p2.y - p1.y) * i * step + p1.y;
            } else if (i < (fsizeOver3 * 2)) {
                x = (p3.x - p2.x) * (i - fsizeOver3) * step + p2.x;
                y = (p3.y - p2.y) * (i - fsizeOver3) * step + p2.y;
            } else {
                x = (p1.x - p3.x) * (i - fsizeOver3 * 2) * step + p3.x;
                y = (p1.y - p3.y) * (i - fsizeOver3 * 2) * step + p3.y;
            }
            Point<float> p(x * magnitude + (mFieldWidth / 2.f), y * magnitude + (mFieldWidth / 2.f));
            p.applyTransform(AffineTransform::rotation(-angle, (mFieldWidth / 2.f), (mFieldWidth / 2.f)));
            p.x = p.x < minlim ? minlim : p.x > maxlim ? maxlim : p.x;
            p.y = p.y < minlim ? minlim : p.y > maxlim ? maxlim : p.y;
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

    float const offset{ 10.f + kSourceRadius };
    float const width{ mFieldWidth - offset };
    float const minPos{ 15.f };
    float const maxPos{ mFieldWidth - 20.f };

    switch (type) {
    case ElevationTrajectoryType::realtime:
        mPlaybackPosition = Point<float>{ -1.0f, -1.0f };
        break;
    case ElevationTrajectoryType::drawing:
        mPlaybackPosition = Point<float>{ -1.0f, -1.0f };
        break;
    case ElevationTrajectoryType::downUp:
        for (int i{}; i < MAGIC_4; ++i) {
            float x = (i / MAGIC_6) * width + offset;
            float y = (i / MAGIC_6) * (maxPos - minPos) + minPos;
            mTrajectoryPoints.add(Point<float>{ x, y });
        }
        break;
    case ElevationTrajectoryType::upDown:
        for (int i{}; i < MAGIC_4; ++i) {
            float x = (i / MAGIC_6) * width + offset;
            float y = (1.0 - i / MAGIC_6) * (maxPos - minPos) + minPos;
            mTrajectoryPoints.add(Point<float>{ x, y });
        }
        break;
    case ElevationTrajectoryType::undefined:
        jassertfalse;
    }

    if (type > ElevationTrajectoryType::drawing) {
        setSourcePosition(
            Point<float>(mTrajectoryPoints[0].x / mFieldWidth, 1.0 - mTrajectoryPoints[0].y / mFieldWidth));
    } else {
        setSourcePosition(Point<float>(0.5f, 0.5f));
    }
}
