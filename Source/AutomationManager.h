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

    void setActivateState(bool state);
    bool getActivateState();

    void setPlaybackDuration(double value);
    double getPlaybackDuration();

    void setNumberOfCycles(int value);

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

    void setSourceLink(int value);
    int getSourceLink();
    void setDrawingType(int type);
    int getDrawingType();
    void setDrawingTypeAlt(int type);

    Source& getSource();
    void setSourcePosition(Point<float> pos);
    Point<float> getSourcePosition();
    void fixSourcePosition();

    struct Listener
    {
        virtual ~Listener() {}

        virtual void trajectoryPositionChanged(AutomationManager *manager, Point<float> position) = 0;
    };

    void addListener(Listener* l) { listeners.add (l); }
    void removeListener(Listener* l) { listeners.remove (l); }

private:
    ListenerList<Listener> listeners;

    int             sourceLink;
    int             drawingType;

    int             numOfCycles;
    int             currentCycle;

    bool            activateState;
    double          playbackDuration;
    Point<float>    playbackPosition;

    Source source;

    double              trajectoryDeltaTime;
    Array<Point<float>> trajectoryPoints;
    Point<float>        currentTrajectoryPoint;
    Point<float>        lastRecordingPoint;

    void computeCurrentTrajectoryPoint();
    Point <float> smoothRecordingPosition(Point<float> pos);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutomationManager)
};
