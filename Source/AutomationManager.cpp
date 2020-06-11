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

AutomationManager::AutomationManager(Source & principalSource) : mPrincipalSource(principalSource)
{
}

void AutomationManager::setFieldWidth(float const newFieldWidth)
{
    mFieldWidth = newFieldWidth;
}

void AutomationManager::setPositionActivateState(bool const newState)
{
    mActivateState = newState;
    if (newState) {
        mTrajectoryDeltaTime = 0.0;
        mLastTrajectoryDeltaTime = 0.0;
        mBackAndForthDirection = Direction::forward;
        mDampeningCycleCount = 0;
        mDampeningLastDelta = 0.0;
        mCurrentPlaybackDuration = mPlaybackDuration;
        mCurrentDegreeOfDeviation = Degrees{ 0.0f };
        mDeviationCycleCount = 0;
    } else {
        mPlaybackPosition = Point<float>{ INVALID_POSITION };
    }
}

void AutomationManager::setPlaybackPositionX(float const value)
{
    if (mPlaybackPosition.has_value()) {
        mPlaybackPosition->setX(value);
    } else {
        mPlaybackPosition = Point<float>{ value, 0.0f };
    }
}
void AutomationManager::setPlaybackPositionY(float const value)
{
    if (mPlaybackPosition.has_value()) {
        mPlaybackPosition->setY(value);
    } else {
        mPlaybackPosition = Point<float>{ 0.0f, value };
    }
}

void AutomationManager::resetRecordingTrajectory(Point<float> const currentPosition)
{
    jassert(currentPosition.getX() >= -1.0f && currentPosition.getX() <= 1.0f && currentPosition.getY() >= -1.0f
            && currentPosition.getY() <= 1.0f);
    jassert(mTrajectory.has_value());

    mPlaybackPosition.reset();
    mTrajectory->clear();
    mTrajectory->add(currentPosition);
    mLastRecordingPoint = currentPosition;
    mTrajectoryHandlePosition = currentPosition;
    // mPrincipalSource.setPos(currentPosition); ???
}

Point<float> AutomationManager::smoothRecordingPosition(Point<float> const & pos)
{
    constexpr auto smoothingFactor = 0.8f;

    mLastRecordingPoint = (mLastRecordingPoint - pos) * smoothingFactor + pos;
    return mLastRecordingPoint;
}

void AutomationManager::setTrajectoryDeltaTime(double const relativeTimeFromPlay)
{
    mTrajectoryDeltaTime = relativeTimeFromPlay / mCurrentPlaybackDuration;
    mTrajectoryDeltaTime = std::fmod(mTrajectoryDeltaTime, 1.0f);
    computeCurrentTrajectoryPoint();
}

void AutomationManager::compressTrajectoryXValues(int maxValue)
{
    // TODO: what does this do ???
    //    auto const offset{ static_cast<int>(10.0f + SOURCE_FIELD_COMPONENT_RADIUS) };
    //    maxValue -= offset;
    //    int const size{ getRecordingTrajectorySize() };
    //    auto const delta{ static_cast<float>(maxValue) / (static_cast<float>(size) + 1.0f) };
    //    for (int i{}; i < size; ++i) {
    //        mTrajectoryPoints.data()[i].setX(static_cast<float>(i) * delta + offset);
    //    }
    jassertfalse;
}

void AutomationManager::computeCurrentTrajectoryPoint()
{
    jassert(mTrajectory.has_value());

    int const dampeningCyclesTimes2{ mDampeningCycles * 2 };
    double currentScaleMin{ 0.0 };
    double currentScaleMax{ 0.0 };

    if (mTrajectory->size() > 0) {
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

        double delta{ trajectoryPhase * mTrajectory->size() };

        if (mBackAndForthDirection == Direction::backward) {
            delta = mTrajectory->size() - delta;
        }

        delta = std::clamp(delta, 0.0, static_cast<double>(mTrajectory->size()));

        if (mIsBackAndForth && mDampeningCycles > 0) {
            if (mDampeningCycleCount < dampeningCyclesTimes2) {
                double const relativeDeltaTime{ (mDampeningCycleCount + mTrajectoryDeltaTime) / dampeningCyclesTimes2 };
                mCurrentPlaybackDuration
                    = mPlaybackDuration - (std::pow(relativeDeltaTime, 2.0) * mPlaybackDuration * 0.25);
                currentScaleMin = relativeDeltaTime * mTrajectory->size() * 0.5;
                currentScaleMax = mTrajectory->size() - currentScaleMin;
                double const currentScale{ (currentScaleMax - currentScaleMin) / mTrajectory->size() };
                delta = delta * currentScale + currentScaleMin;
                mDampeningLastDelta = delta;
            } else {
                delta = mDampeningLastDelta;
            }
        } else {
            mDampeningLastDelta = delta;
        }

        double const deltaRatio{ static_cast<double>(mTrajectory->size() - 1) / mTrajectory->size() };
        delta *= deltaRatio;
        auto index = static_cast<int>(delta);
        if (index + 1 < mTrajectory->size()) {
            auto const frac{ static_cast<float>(delta) - static_cast<float>(index) };
            auto const p1{ mTrajectory.value()[index] };
            auto const p2{ mTrajectory.value()[index + 1] };
            mCurrentTrajectoryPoint = p1 + (p2 - p1) * frac;
        } else {
            mCurrentTrajectoryPoint = mTrajectory->getLast();
        }
    }

    if (mDegreeOfDeviationPerCycle != Degrees{ 0.0f }) {
        bool deviationFlag{ true };
        if (mIsBackAndForth && mDampeningCycles > 0) {
            if (approximatelyEqual(currentScaleMin, currentScaleMax)) {
                deviationFlag = false;
            }
        }
        if (deviationFlag) {
            mCurrentDegreeOfDeviation
                = mDegreeOfDeviationPerCycle * static_cast<float>(mDeviationCycleCount + mTrajectoryDeltaTime);
            if (mCurrentDegreeOfDeviation >= Degrees{ 360.0f }) {
                mCurrentDegreeOfDeviation -= Degrees{ 360.0f };
            }
        }

        mCurrentTrajectoryPoint = mCurrentTrajectoryPoint.rotatedAboutOrigin(mCurrentDegreeOfDeviation.getAsRadians());
    }

    if (mActivateState) {
        mPrincipalSource.setPos(mCurrentTrajectoryPoint);
        sendTrajectoryPositionChangedEvent();
    }
}

int AutomationManager::getRecordingTrajectorySize() const
{
    jassert(mTrajectory.has_value());
    return mTrajectory->size();
}

Point<float> AutomationManager::getFirstRecordingPoint() const
{
    jassert(mTrajectory.has_value());
    return mTrajectory->getFirst();
}

Point<float> AutomationManager::getLastRecordingPoint() const
{
    jassert(mTrajectory.has_value());
    return mTrajectory->getLast();
}

Point<float> AutomationManager::getCurrentTrajectoryPoint() const
{
    if (mActivateState) {
        return mCurrentTrajectoryPoint;
    } else {
        return mPrincipalSource.getPos();
    }
}

void AutomationManager::setPrincipalSourceAndPlaybackPosition(Point<float> const & pos)
{
    mPrincipalSource.setPos(pos);
    setPlaybackPosition(pos);
}

void PositionAutomationManager::sendTrajectoryPositionChangedEvent()
{
    if (mActivateState || mTrajectoryType == PositionTrajectoryType::realtime) {
        mListeners.call([&](Listener & l) { l.trajectoryPositionChanged(this, mPrincipalSource.getPos()); });
    }
}

void ElevationAutomationManager::sendTrajectoryPositionChangedEvent()
{
    if (mActivateState || mTrajectoryType == ElevationTrajectoryType::realtime) {
        mListeners.call([&](Listener & l) { l.trajectoryPositionChanged(this, mPrincipalSource.getPos()); });
    }
}

void AutomationManager::fixPrincipalSourcePosition()
{
    bool shouldBeFixed{};
    auto positionAutomationManager{ dynamic_cast<PositionAutomationManager *>(this) };
    if (positionAutomationManager != nullptr) {
        shouldBeFixed = positionAutomationManager->getSourceLink() != PositionSourceLink::independent;
    } else {
        auto elevationAutomationManager{ dynamic_cast<ElevationAutomationManager *>(this) };
        jassert(elevationAutomationManager != nullptr);
        shouldBeFixed = elevationAutomationManager->getSourceLink() != ElevationSourceLink::independent;
    }
    mPrincipalSource.fixSourcePosition(shouldBeFixed);
}

void PositionAutomationManager::setTrajectoryType(PositionTrajectoryType const type, Point<float> const & startPosition)
{
    mTrajectoryType = type;
    if (type != PositionTrajectoryType::realtime) {
        mTrajectory = Trajectory{ type, startPosition };
    } else {
        mTrajectory.reset();
    }
    mPrincipalSource.setPos(startPosition);
}

void AutomationManager::addRecordingPoint(Point<float> const & pos)
{
    jassert(mTrajectory.has_value());
    mTrajectory->add(smoothRecordingPosition(pos));
}

void ElevationAutomationManager::setTrajectoryType(ElevationTrajectoryType const type)
{
    constexpr auto NB_POINTS = 400;
    static_assert(NB_POINTS % 2 == 0);
    constexpr auto NB_POINTS_PER_GESTURE = NB_POINTS / 2;

    mTrajectoryType = type;

    if (type != ElevationTrajectoryType::realtime) {
        mTrajectory = Trajectory{ type };
    } else {
        mTrajectory.reset();
    }

    // TODO
    //    auto constexpr offset{ 10.0f + SOURCE_FIELD_COMPONENT_RADIUS };
    //    auto const width{ mFieldWidth - offset };
    //    auto constexpr minPos{ 15.0f };
    //    auto const maxPos{ mFieldWidth - 20.0f };
    //
    //    switch (type) {
    //    case ElevationTrajectoryType::realtime:
    //    case ElevationTrajectoryType::drawing:
    //        mPlaybackPosition = Point<float>{ -1.0f, -1.0f };
    //        break;
    //    case ElevationTrajectoryType::downUp:
    //        for (int i{}; i < NB_POINTS_PER_GESTURE; ++i) {
    //            float const x = (static_cast<float>(i) / (NB_POINTS_PER_GESTURE - 1)) * width + offset;
    //            float const y = (static_cast<float>(i) / (NB_POINTS_PER_GESTURE - 1)) * (maxPos - minPos) + minPos;
    //            mTrajectoryPoints.add(Point<float>{ x, y });
    //        }
    //        break;
    //    case ElevationTrajectoryType::upDown:
    //        for (int i{}; i < NB_POINTS_PER_GESTURE; ++i) {
    //            float const x = (static_cast<float>(i) / (NB_POINTS_PER_GESTURE - 1)) * width + offset;
    //            float const y = (1.0f - static_cast<float>(i) / (NB_POINTS_PER_GESTURE - 1)) * (maxPos - minPos) +
    //            minPos; mTrajectoryPoints.add(Point<float>{ x, y });
    //        }
    //        break;
    //    case ElevationTrajectoryType::undefined:
    //        jassertfalse;
    //    }
    //
    //    if (type > ElevationTrajectoryType::drawing) {
    //        setSourcePosition(
    //            Point<float>{ mTrajectoryPoints[0].x / mFieldWidth, 1.0f - mTrajectoryPoints[0].y / mFieldWidth });
    //    } else {
    //        setSourcePosition(Point<float>{ 0.5f, 0.5f });
    //    }
}
