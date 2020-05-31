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

//==============================================================================
class AutomationManager
{
public:
    struct Listener {
        virtual ~Listener() {}

        virtual void trajectoryPositionChanged(AutomationManager * manager, Point<float> position) = 0;
    };
    //==============================================================================
    enum class Direction { forward, backward };

protected:
    //==============================================================================
    static constexpr Point<float> INVALID_POSITION{ -1.0f, -1.0f };
    //==============================================================================
    ListenerList<Listener> mListeners;

    float mFieldWidth{ MIN_FIELD_WIDTH };

    PositionSourceLink mSourceLink{ PositionSourceLink::independent };

    bool mIsBackAndForth{ false };
    Direction mBackAndForthDirection{ Direction::forward };

    int mDampeningCycles{};
    int mDampeningCycleCount{};
    double mDampeningLastDelta{};

    bool mActivateState{ false };
    double mPlaybackDuration{ 5.0 };
    double mCurrentPlaybackDuration{ 5.0 };
    Point<float> mPlaybackPosition{ INVALID_POSITION };

    Source mSource{};

    double mTrajectoryDeltaTime{};
    double mLastTrajectoryDeltaTime{};
    Array<Point<float>> mTrajectoryPoints{};
    Point<float> mCurrentTrajectoryPoint;
    Point<float> mLastRecordingPoint{};

    float mDegreeOfDeviationPerCycle{};
    float mCurrentDegreeOfDeviation{};
    int mDeviationCycleCount{};

public:
    //==============================================================================
    AutomationManager();
    virtual ~AutomationManager() = default;
    //==============================================================================
    float getFieldWidth() const { return mFieldWidth; }
    void setFieldWidth(float newFieldWidth);

    void setPositionActivateState(bool state);
    bool getPositionActivateState() const { return mActivateState; }

    void setPlaybackDuration(double value) { mPlaybackDuration = value; }
    double getPlaybackDuration() const { return mPlaybackDuration; }

    void setPlaybackPosition(Point<float> const & value) { mPlaybackPosition = value; }
    void setPlaybackPositionX(float const value) { mPlaybackPosition.x = value; }
    void setPlaybackPositionY(float const value) { mPlaybackPosition.y = value; }
    bool hasValidPlaybackPosition() const { return mPlaybackPosition != INVALID_POSITION; }
    Point<float> getPlaybackPosition() const { return mPlaybackPosition; }

    void resetRecordingTrajectory(Point<float> currentPosition);
    void addRecordingPoint(Point<float> const & pos) { mTrajectoryPoints.add(smoothRecordingPosition(pos)); }
    int getRecordingTrajectorySize() const { return mTrajectoryPoints.size(); }
    Point<float> getFirstRecordingPoint() const { return mTrajectoryPoints.getFirst(); }
    Point<float> getLastRecordingPoint() const { return mTrajectoryPoints.getLast(); }
    Point<float> getCurrentTrajectoryPoint() const;
    void createRecordingPath(Path & path);
    void setTrajectoryDeltaTime(double relativeTimeFromPlay);
    void compressTrajectoryXValues(int maxValue);

    void setPostionSourceLink(PositionSourceLink value) { this->mSourceLink = value; }
    PositionSourceLink getSourceLink() const { return mSourceLink; }

    void setPositionBackAndForth(bool const newState) { mIsBackAndForth = newState; }
    void setPositionDampeningCycles(int value) { this->mDampeningCycles = value; }

    void setDeviationPerCycle(float value) { this->mDegreeOfDeviationPerCycle = value; }

    Source & getSource() { return mSource; }
    Source const & getSource() const { return mSource; }
    Point<float> getSourcePosition() const { return mSource.getPos(); }
    void setSourcePosition(Point<float> const & pos) { mSource.setPos(pos); }
    void setSourcePositionX(float const x) { mSource.setX(x); }
    void setSourcePositionY(float const y) { mSource.setY(y); }

    void fixSourcePosition();

    void setSourceAndPlaybackPosition(Point<float> pos);

    void addListener(Listener * l) { mListeners.add(l); }
    void removeListener(Listener * l) { mListeners.remove(l); }

    virtual void sendTrajectoryPositionChangedEvent() = 0;

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

public:
    //==============================================================================
    PositionAutomationManager() = default;
    //==============================================================================
    void setTrajectoryType(PositionTrajectoryType type, Point<float> const & startpos);
    PositionTrajectoryType getTrajectoryType() const { return mTrajectoryType; }

    void sendTrajectoryPositionChangedEvent() final;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PositionAutomationManager);
};

//==============================================================================
class ElevationAutomationManager final : public AutomationManager
{
    ElevationTrajectoryType mTrajectoryType{ ElevationTrajectoryType::drawing };

public:
    //==============================================================================
    ElevationAutomationManager() = default;
    //==============================================================================
    void setTrajectoryType(ElevationTrajectoryType type);
    ElevationTrajectoryType getTrajectoryType() const { return mTrajectoryType; }

    void sendTrajectoryPositionChangedEvent() final;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ElevationAutomationManager);
};