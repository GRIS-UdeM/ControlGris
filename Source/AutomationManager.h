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

class AutomationManager
{
public:
    AutomationManager();
    ~AutomationManager() = default;

    double getFieldWidth() const { return mFieldWidth; }
    void setFieldWidth(float newFieldWidth);

    void setActivateState(bool state);
    bool getActivateState() const { return mActivateState; }

    void setPlaybackDuration(double value) { mPlaybackDuration = value; }
    double getPlaybackDuration() const { return mPlaybackDuration; }

    void setPlaybackPositionX(float value) { mPlaybackPosition.x = value; }
    void setPlaybackPositionY(float value) { mPlaybackPosition.y = value; }
    bool hasValidPlaybackPosition() const { return mPlaybackPosition != Point<float>(-1.0f, -1.0f); }
    Point<float> getPlaybackPosition() const { return mPlaybackPosition; }

    void resetRecordingTrajectory(Point<float> currentPosition);
    void addRecordingPoint(Point<float> pos) { mTrajectoryPoints.add(smoothRecordingPosition(pos)); }
    int getRecordingTrajectorySize() const { return mTrajectoryPoints.size(); }
    Point<float> getFirstRecordingPoint() const { return mTrajectoryPoints.getFirst(); }
    Point<float> getLastRecordingPoint() const { return mTrajectoryPoints.getLast(); }
    Point<float> getCurrentTrajectoryPoint() const;
    void createRecordingPath(Path & path);
    void setTrajectoryDeltaTime(double relativeTimeFromPlay);
    void compressTrajectoryXValues(int maxValue);

    void setSourceLink(SourceLink value) { this->mSourceLink = value; }
    SourceLink getSourceLink() const { return mSourceLink; }
    void setDrawingType(TrajectoryType type, Point<float> startpos);
    TrajectoryType getDrawingType() const { return mDrawingType; }
    void setDrawingTypeAlt(ElevationTrajectoryType type);

    void setBackAndForth(bool shouldBeOn);
    void setDampeningCycles(int value) { this->mDampeningCycles = value; }

    void setDeviationPerCycle(float value) { this->mDegreeOfDeviationPerCycle = value; }

    Source & getSource() { return mSource; }
    Source const & getSource() const { return mSource; }
    void setSourcePosition(Point<float> const & pos) { mSource.setPos(pos); }
    void setSourcePositionX(float const x) { mSource.setX(x); }
    void setSourcePositionY(float const y) { mSource.setY(y); }
    Point<float> getSourcePosition() const { return mSource.getPos(); }
    void fixSourcePosition();

    void setSourceAndPlaybackPosition(Point<float> pos);

    void sendTrajectoryPositionChangedEvent();

    struct Listener {
        virtual ~Listener() {}

        virtual void trajectoryPositionChanged(AutomationManager * manager, Point<float> position) = 0;
    };

    void addListener(Listener * l) { mListeners.add(l); }
    void removeListener(Listener * l) { mListeners.remove(l); }

private:
    ListenerList<Listener> mListeners;

    float mFieldWidth{ MIN_FIELD_WIDTH };

    SourceLink mSourceLink{ SourceLink::independent };
    TrajectoryType mDrawingType{ TrajectoryType::drawing };

    bool mIsBackAndForth{ false };
    int mBackAndForthDirection{};

    int mDampeningCycles{};
    int mDampeningCycleCount{};
    double mDampeningLastDelta{};

    bool mActivateState{ false };
    double mPlaybackDuration{ 5.0 };
    double mCurrentPlaybackDuration{ 5.0 };
    Point<float> mPlaybackPosition{ -1.f, -1.f };

    Source mSource{};

    double mTrajectoryDeltaTime{};
    double mLastTrajectoryDeltaTime{};
    Array<Point<float>> mTrajectoryPoints{};
    Point<float> mCurrentTrajectoryPoint;
    Point<float> mLastRecordingPoint{};

    float mDegreeOfDeviationPerCycle{};
    float mCurrentDegreeOfDeviation{};
    int mDeviationCycleCount{};

    void computeCurrentTrajectoryPoint();
    Point<float> smoothRecordingPosition(Point<float> pos);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AutomationManager)
};
