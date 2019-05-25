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
#include "AutomationManager.h"

AutomationManager::AutomationManager() {
    sourceLink = 1;
    drawingType = 1;
    numOfCycles = 1;
    currentCycle = 0;
    activateState = false;
    source.setX(0.5f);
    source.setY(0.5f);
    playbackDuration = 5.0;
    currentTrajectoryPoint = Point<float> (140, 140);
    playbackPosition = Point<float> (-1.0f, -1.0f);
    trajectoryDeltaTime = 0.0;
}

AutomationManager::~AutomationManager() {}

void AutomationManager::setActivateState(bool state) {
    activateState = state;
}

bool AutomationManager::getActivateState() {
    return activateState;
}

void AutomationManager::setPlaybackDuration(double value) {
    playbackDuration = value;
}

double AutomationManager::getPlaybackDuration() {
    return playbackDuration;
}

void AutomationManager::setNumberOfCycles(int value) {
    numOfCycles = value;
}

void AutomationManager::setPlaybackPositionX(float value) {
    playbackPosition.x = value;
}

void AutomationManager::setPlaybackPositionY(float value) {
    playbackPosition.y = value;
}

bool AutomationManager::hasValidPlaybackPosition() {
    return playbackPosition != Point<float> (-1.0f, -1.0f);
}

Point<float> AutomationManager::getPlaybackPosition() {
    return playbackPosition;
}

void AutomationManager::resetRecordingTrajectory(Point<float> currentPosition) {
    trajectoryPoints.clear();
    lastRecordingPoint = currentPosition;
}

Point<float> AutomationManager::smoothRecordingPosition(Point<float> pos) {
    Point<float> smoothed;
    smoothed.x = lastRecordingPoint.x = pos.x + (lastRecordingPoint.x - pos.x) * 0.8f;
    smoothed.y = lastRecordingPoint.y = pos.y + (lastRecordingPoint.y - pos.y) * 0.8f;
    return smoothed;
}

void AutomationManager::addRecordingPoint(Point<float> pos) {
    trajectoryPoints.add(smoothRecordingPosition(pos));
}

int AutomationManager::getRecordingTrajectorySize() {
    return trajectoryPoints.size();
}

Point<float> AutomationManager::getLastRecordingPoint() {
    return trajectoryPoints.getLast();
}

void AutomationManager::createRecordingPath(Path& path) {
    path.startNewSubPath(trajectoryPoints[0]);
    for (int i = 1; i < trajectoryPoints.size(); i++) {
        path.lineTo(trajectoryPoints[i]);
    }
}

void AutomationManager::setTrajectoryDeltaTime(double relativeTimeFromPlay) {
    trajectoryDeltaTime = relativeTimeFromPlay / (playbackDuration * numOfCycles);
    computeCurrentTrajectoryPoint();
}

void AutomationManager::computeCurrentTrajectoryPoint() {
    if (trajectoryDeltaTime < 1.0) {
        double delta = trajectoryDeltaTime * numOfCycles * trajectoryPoints.size();
        if (delta + 1 >= trajectoryPoints.size()) {
            delta = std::fmod(delta, trajectoryPoints.size());
        }
        int index = (int)delta;
        if (index + 1 < trajectoryPoints.size()) {
            double frac = delta - index;
            Point<float> p1 = trajectoryPoints[index];
            Point<float> p2 = trajectoryPoints[index+1];
            currentTrajectoryPoint = Point<float> ((p1.x + (p2.x - p1.x) * frac), (p1.y + (p2.y - p1.y) * frac));
        } else {
            currentTrajectoryPoint = Point<float> (trajectoryPoints.getLast().x, trajectoryPoints.getLast().y);
        }
    } else {
        currentTrajectoryPoint = Point<float> (trajectoryPoints.getLast().x, trajectoryPoints.getLast().y);
    }

    if (activateState) {
        setSourcePosition(Point<float> (currentTrajectoryPoint.x / 300, 1.0 - currentTrajectoryPoint.y / 300));
    }
}

Point<float> AutomationManager::getCurrentTrajectoryPoint() {
    if (activateState) {
        return currentTrajectoryPoint;
    } else {
        return getSourcePosition() * 300;
    }
}

void AutomationManager::setSourcePosition(Point<float> pos) {
    source.setPos(pos);
    if (activateState)
        listeners.call([&] (Listener& l) { l.trajectoryPositionChanged(pos); });
}

Source& AutomationManager::getSource() {
    return source;
}

Point<float> AutomationManager::getSourcePosition() {
    return source.getPos();
}

void AutomationManager::setDrawingType(int type) {
    drawingType = type;
    trajectoryPoints.clear();
    switch (drawingType) {
        case 1:
            break;
        case 2:
            for (int i = 0; i < 200; i++) {
                float x = sinf(2.0 * M_PI * i / 199) * 140 + 150;
                float y = -cosf(2.0 * M_PI * i / 199) * 140 + 150;
                trajectoryPoints.add(Point<float> (x, y));
            }
            break;
        case 3:
            for (int i = 0; i < 200; i++) {
                float x = -sinf(2.0 * M_PI * i / 199) * 140 + 150;
                float y = -cosf(2.0 * M_PI * i / 199) * 140 + 150;
                trajectoryPoints.add(Point<float> (x, y));
            }
            break;
    }
}

int AutomationManager::getDrawingType() {
    return drawingType;
}

void AutomationManager::setSourceLink(int value) {
    sourceLink = value;
}

int AutomationManager::getSourceLink() {
    return sourceLink;
}
