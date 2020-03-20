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
#include "Source.h"

class AutomationManager
{
public:
    AutomationManager();
    ~AutomationManager();

    void setFieldWidth(double newFieldWidth);

    void setActivateState(bool state);
    bool getActivateState();

    void setPlaybackDuration(double value);
    double getPlaybackDuration();

    void setPlaybackPositionX(float value);
    void setPlaybackPositionY(float value);
    bool hasValidPlaybackPosition();
    Point<float> getPlaybackPosition();

    void resetRecordingTrajectory(Point<float> currentPosition);
    void addRecordingPoint(Point<float> pos);
    int getRecordingTrajectorySize();
    Point<float> getFirstRecordingPoint();
    Point<float> getLastRecordingPoint();
    Point<float> getCurrentTrajectoryPoint();
    void createRecordingPath(Path& path);
    void setTrajectoryDeltaTime(double relativeTimeFromPlay);
    void compressTrajectoryXValues(int maxValue);

    void setSourceLink(int value);
    int getSourceLink();
    void setDrawingType(int type, Point<float> startpos);
    int getDrawingType();
    void setDrawingTypeAlt(int type);

    void setBackAndForth(bool shouldBeOn);
    void setDampeningCycles(int value);

    Source& getSource();
    void setSourcePosition(Point<float> pos);
    void setSourcePositionX(float x);
    void setSourcePositionY(float y);
    Point<float> getSourcePosition();
    void fixSourcePosition();

    void setSourceAndPlaybackPosition(Point<float> pos);

    void sendTrajectoryPositionChangedEvent();

    struct Listener
    {
        virtual ~Listener() {}

        virtual void trajectoryPositionChanged(AutomationManager *manager, Point<float> position) = 0;
    };

    void addListener(Listener* l) { listeners.add (l); }
    void removeListener(Listener* l) { listeners.remove (l); }

private:
    ListenerList<Listener> listeners;

    double          fieldWidth;

    int             sourceLink;
    int             drawingType;

    bool            isBackAndForth;
    int             backAndForthDirection;

    int             dampeningCycles;
    int             dampeningCycleCount;
    double          dampeningLastDelta;

    bool            activateState;
    double          playbackDuration;
    double          currentPlaybackDuration;
    Point<float>    playbackPosition;

    Source source;

    double              trajectoryDeltaTime;
    double              lastTrajectoryDeltaTime;
    Array<Point<float>> trajectoryPoints;
    Point<float>        currentTrajectoryPoint;
    Point<float>        lastRecordingPoint;

    void computeCurrentTrajectoryPoint();
    Point <float> smoothRecordingPosition(Point<float> pos);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutomationManager)
};
