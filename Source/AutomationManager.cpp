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
    activateState = false;
    source.setX(0.5f);
    source.setY(0.5f);
    playbackDuration = 1.0;
    playbackPosition = Point<float> (-1.0f, -1.0f);

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

Point<float> AutomationManager::getRecordingPointFromDeltaTime(double delta) {
    delta *= trajectoryPoints.size();
    int index = (int)delta;
    if (index + 1 < trajectoryPoints.size()) {
        double frac = delta - index;
        Point<float> p1 = trajectoryPoints[index];
        Point<float> p2 = trajectoryPoints[index+1];
        return Point<float> ((p1.x + (p2.x - p1.x) * frac), (p1.y + (p2.y - p1.y) * frac));
    } else {
        return Point<float> (trajectoryPoints.getLast().x, trajectoryPoints.getLast().y);
    }
}

void AutomationManager::setSourcePosition(Point<float> pos) {
    source.setPos(pos);
    listeners.call([&] (Listener& l) { l.trajectoryPositionChanged(pos); }); // AutomationManager::Listener
}

Source& AutomationManager::getSource() {
    return source;
}

Point<float> AutomationManager::getSourcePosition() {
    return source.getPos();
}

