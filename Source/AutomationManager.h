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

#include "../JuceLibraryCode/JuceHeader.h"

#include "ControlGrisConstants.h"
#include "Source.h"
#include "Trajectory.h"

class ControlGrisAudioProcessor;

//==============================================================================
struct PlaybackPosition {
    std::optional<float> x{};
    std::optional<float> y{};

    void reset()
    {
        x.reset();
        y.reset();
    }

    bool operator!=(Point<float> const & point) const
    {
        if (!isValid()) {
            return true;
        }
        return x.value() != point.getX() || y.value() != point.getY();
    }

    bool isValid() const { return x.has_value() && y.has_value(); }

    PlaybackPosition & operator=(Point<float> const & point)
    {
        x = point.getX();
        y = point.getY();
        return *this;
    }

    Point<float> value() const { return Point<float>{ x.value(), y.value() }; }
};

class AutomationManager
{
public:
    class Listener
    {
    public:
        Listener() noexcept = default;
        virtual ~Listener() noexcept = default;

        virtual void trajectoryPositionChanged(AutomationManager * manager, Point<float> position) = 0;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Listener);
    };
    //==============================================================================
    enum class Direction { forward, backward };

protected:
    //==============================================================================
    ControlGrisAudioProcessor & mProcessor;

    ListenerList<Listener> mListeners;

    float mFieldWidth{ MIN_FIELD_WIDTH };

    bool mIsBackAndForth{ false };
    Direction mBackAndForthDirection{ Direction::forward };

    int mDampeningCycles{};
    int mDampeningCycleCount{};
    double mDampeningLastDelta{};

    bool mActivateState{ false };
    double mPlaybackDuration{ 5.0 };
    double mCurrentPlaybackDuration{ 5.0 };
    PlaybackPosition mPlaybackPosition{};

    Point<float> mTrajectoryHandlePosition{};

    double mTrajectoryDeltaTime{};
    double mLastTrajectoryDeltaTime{};
    std::optional<Trajectory> mTrajectory{};
    Point<float> mCurrentTrajectoryPoint{};
    Point<float> mLastRecordingPoint{};

    Degrees mDegreeOfDeviationPerCycle{};
    Degrees mCurrentDegreeOfDeviation{};
    int mDeviationCycleCount{};

    Source & mPrincipalSource;

public:
    //==============================================================================
    AutomationManager(ControlGrisAudioProcessor & processor, Source & principalSource) noexcept;
    virtual ~AutomationManager() noexcept = default;
    //==============================================================================
    ControlGrisAudioProcessor & getProcessor() { return mProcessor; }

    float getFieldWidth() const { return mFieldWidth; }
    void setFieldWidth(float newFieldWidth);

    void setPositionActivateState(bool state);
    bool getPositionActivateState() const { return mActivateState; }

    void setPlaybackDuration(double value) { mPlaybackDuration = value; }
    double getPlaybackDuration() const { return mPlaybackDuration; }

    void setPlaybackPosition(Point<float> const & value) { mPlaybackPosition = value; }
    void setPlaybackPositionX(float const value);
    void setPlaybackPositionY(float const value);
    PlaybackPosition const & getPlaybackPosition() const { return mPlaybackPosition; }
    PlaybackPosition & getPlaybackPosition() { return mPlaybackPosition; }
    void resetPlaybackPosition() { mPlaybackPosition.reset(); }

    void resetRecordingTrajectory(Point<float> currentPosition);
    void addRecordingPoint(Point<float> const & pos);
    int getRecordingTrajectorySize() const;
    Point<float> getFirstRecordingPoint() const;
    Point<float> getLastRecordingPoint() const;
    Point<float> getCurrentTrajectoryPoint() const;
    void setTrajectoryDeltaTime(double relativeTimeFromPlay);
    void compressTrajectoryXValues(int maxValue);

    Point<float> const & getTrajectoryHandlePosition() const { return mTrajectoryHandlePosition; }

    std::optional<Trajectory> const & getTrajectory() const { return mTrajectory; }

    void setPositionBackAndForth(bool const newState) { mIsBackAndForth = newState; }
    void setPositionDampeningCycles(int value) { this->mDampeningCycles = value; }

    void setDeviationPerCycle(Degrees const value) { this->mDegreeOfDeviationPerCycle = value; }

    void setPrincipalSourceAndPlaybackPosition(Point<float> const & pos);

    void addListener(Listener * l) { mListeners.add(l); }
    void removeListener(Listener * l) { mListeners.remove(l); }

    virtual void sendTrajectoryPositionChangedEvent() = 0;
    virtual void applyCurrentTrajectoryPointToPrimarySource() = 0;

private:
    //==============================================================================
    void invertBackAndForthDirection()
    {
        mBackAndForthDirection
            = mBackAndForthDirection == Direction::forward ? Direction::backward : Direction::forward;
    }
    void computeCurrentTrajectoryPoint();
    Point<float> smoothRecordingPosition(Point<float> const & pos);
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AutomationManager);
};

//==============================================================================
class PositionAutomationManager final : public AutomationManager
{
    PositionTrajectoryType mTrajectoryType{ PositionTrajectoryType::drawing };
    PositionSourceLink mSourceLink{ PositionSourceLink::independent };

public:
    //==============================================================================
    PositionAutomationManager(ControlGrisAudioProcessor & processor, Source & principalSource) noexcept
        : AutomationManager(processor, principalSource)
    {
    }
    ~PositionAutomationManager() noexcept final = default;
    //==============================================================================
    void setTrajectoryType(PositionTrajectoryType type, Point<float> const & startpos);
    PositionTrajectoryType getTrajectoryType() const { return mTrajectoryType; }

    void setSourceLink(PositionSourceLink const sourceLink) { mSourceLink = sourceLink; }
    PositionSourceLink getSourceLink() const { return mSourceLink; }

    void applyCurrentTrajectoryPointToPrimarySource() final;

    void sendTrajectoryPositionChangedEvent() final;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PositionAutomationManager);
};

//==============================================================================
class ElevationAutomationManager final : public AutomationManager
{
    ElevationTrajectoryType mTrajectoryType{ ElevationTrajectoryType::drawing };
    ElevationSourceLink mSourceLink{ ElevationSourceLink::independent };

public:
    //==============================================================================
    ElevationAutomationManager(ControlGrisAudioProcessor & processor, Source & principalSource) noexcept
        : AutomationManager(processor, principalSource)
    {
    }
    ~ElevationAutomationManager() noexcept final = default;
    //==============================================================================
    void setTrajectoryType(ElevationTrajectoryType type);
    ElevationTrajectoryType getTrajectoryType() const { return mTrajectoryType; }
    void setPrincipalSourceAndPlaybackElevation(Radians elevation);

    void setSourceLink(ElevationSourceLink sourceLink) { mSourceLink = sourceLink; }
    ElevationSourceLink getSourceLink() const { return mSourceLink; }

    void sendTrajectoryPositionChangedEvent() final;
    void applyCurrentTrajectoryPointToPrimarySource() final;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ElevationAutomationManager);
};