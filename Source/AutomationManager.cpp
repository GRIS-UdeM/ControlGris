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

AutomationManager::AutomationManager(ControlGrisAudioProcessor & processor, Source & principalSource) noexcept
    : mProcessor(processor)
    , mPrincipalSource(principalSource)
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
        mPlaybackPosition.reset();
    }
}

void AutomationManager::setPlaybackPositionX(float const value)
{
    mPlaybackPosition.x = value;
}
void AutomationManager::setPlaybackPositionY(float const value)
{
    mPlaybackPosition.y = value;
}

void AutomationManager::resetRecordingTrajectory(Point<float> const currentPosition)
{
    jassert(currentPosition.getX() >= -1.0f && currentPosition.getX() <= 1.0f && currentPosition.getY() >= -1.0f
            && currentPosition.getY() <= 1.0f);
    jassert(mTrajectory.has_value());

    mPlaybackPosition.reset();
    mTrajectory->clear();
    mTrajectory->addPoint(currentPosition);
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
    applyCurrentTrajectoryPointToPrimarySource();
}

void AutomationManager::computeCurrentTrajectoryPoint()
{
    if (!mTrajectory.has_value()) {
        mCurrentTrajectoryPoint = mPrincipalSource.getPos();
        return;
    }

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
        auto index{ static_cast<int>(delta) };
        if (index + 1 < mTrajectory->size()) {
            Normalized const progression{ static_cast<float>(delta / mTrajectory->size()) };
            mCurrentTrajectoryPoint = mTrajectory->getPosition(progression);
        } else {
            mCurrentTrajectoryPoint = mTrajectory->getEndPosition();
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
}

int AutomationManager::getRecordingTrajectorySize() const
{
    jassert(mTrajectory.has_value());
    return mTrajectory->size();
}

Point<float> AutomationManager::getFirstRecordingPoint() const
{
    jassert(mTrajectory.has_value());
    return mTrajectory->getStartPosition();
}

Point<float> AutomationManager::getLastRecordingPoint() const
{
    jassert(mTrajectory.has_value());
    return mTrajectory->getEndPosition();
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
    mPrincipalSource.setPos(pos, SourceLinkNotification::notify);
    setPlaybackPosition(pos);
}

void PositionAutomationManager::sendTrajectoryPositionChangedEvent()
{
    mListeners.call([&](Listener & l) { l.trajectoryPositionChanged(this, mPrincipalSource.getPos()); });
}

void ElevationAutomationManager::sendTrajectoryPositionChangedEvent()
{
    mListeners.call([&](Listener & l) { l.trajectoryPositionChanged(this, mPrincipalSource.getPos()); });
}

void PositionAutomationManager::setTrajectoryType(PositionTrajectoryType const type, Point<float> const & startPosition)
{
    mTrajectoryType = type;
    if (type != PositionTrajectoryType::realtime) {
        mTrajectory = Trajectory{ type, startPosition };
    } else {
        mTrajectory.reset();
    }
    mPrincipalSource.setPos(startPosition, SourceLinkNotification::notify);
}

void AutomationManager::addRecordingPoint(Point<float> const & pos)
{
    jassert(mTrajectory.has_value());
    mTrajectory->addPoint(smoothRecordingPosition(pos));
}

void ElevationAutomationManager::setTrajectoryType(ElevationTrajectoryType const type)
{
    mTrajectoryType = type;

    if (type != ElevationTrajectoryType::realtime) {
        mTrajectory = Trajectory{ type };
    } else {
        mTrajectory.reset();
    }

    // TODO
    //    if (type > ElevationTrajectoryType::drawing) {
    //        setSourcePosition(
    //            Point<float>{ mTrajectoryPoints[0].x / mFieldWidth, 1.0f - mTrajectoryPoints[0].y / mFieldWidth });
    //    } else {
    //        setSourcePosition(Point<float>{ 0.5f, 0.5f });
    //    }
}

void PositionAutomationManager::applyCurrentTrajectoryPointToPrimarySource()
{
    if (mActivateState) {
        mPrincipalSource.setPos(mCurrentTrajectoryPoint, SourceLinkNotification::notify);
        sendTrajectoryPositionChangedEvent();
    }
}

void ElevationAutomationManager::applyCurrentTrajectoryPointToPrimarySource()
{
    if (mActivateState) {
        auto const currentElevation{ MAX_ELEVATION * (mCurrentTrajectoryPoint.getY() + 1.0f) / 2.0f };
        mPrincipalSource.setElevation(currentElevation, SourceLinkNotification::notify);
        sendTrajectoryPositionChangedEvent();
    }
}