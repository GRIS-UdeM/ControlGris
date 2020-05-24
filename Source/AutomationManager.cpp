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

AutomationManager::AutomationManager()
{
    sourceLink = SourceLink::independent;
    drawingType = TrajectoryType::drawing;
    fieldWidth = MIN_FIELD_WIDTH;
    activateState = false;
    isBackAndForth = false;
    backAndForthDirection = 0;
    dampeningCycles = 0;
    dampeningCycleCount = 0;
    dampeningLastDelta = 0.0;
    source.setX(0.0f);
    source.setY(0.0f);
    playbackDuration = currentPlaybackDuration = 5.0;
    currentTrajectoryPoint = Point<float>(fieldWidth / 2, fieldWidth / 2);
    playbackPosition = Point<float>(-1.0f, -1.0f);
    trajectoryDeltaTime = lastTrajectoryDeltaTime = 0.0;
    degreeOfDeviationPerCycle = 0.0;
}

AutomationManager::~AutomationManager()
{
}

void AutomationManager::setFieldWidth(double newFieldWidth)
{
    float factor = newFieldWidth / fieldWidth;
    fieldWidth = newFieldWidth;
    if (trajectoryPoints.size() > 0) {
        AffineTransform t = AffineTransform::scale(factor);
        for (auto & p : trajectoryPoints)
            p.applyTransform(t);
    }
}

void AutomationManager::setActivateState(bool state)
{
    activateState = state;
    if (!state) {
        playbackPosition = Point<float>(-1.0f, -1.0f);
    } else {
        trajectoryDeltaTime = lastTrajectoryDeltaTime = 0.0;
        backAndForthDirection = 0;
        dampeningCycleCount = 0;
        dampeningLastDelta = 0.0;
        currentPlaybackDuration = playbackDuration;
        currentDegreeOfDeviation = 0.0;
        deviationCycleCount = 0;
    }
}

void AutomationManager::setPlaybackDuration(double value)
{
    playbackDuration = value;
}

void AutomationManager::setPlaybackPositionX(float value)
{
    playbackPosition.x = value;
}

void AutomationManager::setPlaybackPositionY(float value)
{
    playbackPosition.y = value;
}

void AutomationManager::setBackAndForth(bool shouldBeOn)
{
    if (shouldBeOn != isBackAndForth) {
        isBackAndForth = shouldBeOn;
    }
}

void AutomationManager::resetRecordingTrajectory(Point<float> currentPosition)
{
    playbackPosition = Point<float>(-1.0f, -1.0f);
    trajectoryPoints.clear();
    trajectoryPoints.add(currentPosition);
    lastRecordingPoint = currentPosition;
    setSourcePosition(Point<float>(currentPosition.x / fieldWidth, 1.0 - currentPosition.y / fieldWidth));
}

Point<float> AutomationManager::smoothRecordingPosition(Point<float> pos)
{
    Point<float> smoothed;
    smoothed.x = lastRecordingPoint.x = pos.x + (lastRecordingPoint.x - pos.x) * 0.8f;
    smoothed.y = lastRecordingPoint.y = pos.y + (lastRecordingPoint.y - pos.y) * 0.8f;
    return smoothed;
}

void AutomationManager::addRecordingPoint(Point<float> pos)
{
    trajectoryPoints.add(smoothRecordingPosition(pos));
}

void AutomationManager::createRecordingPath(Path & path)
{
    path.startNewSubPath(trajectoryPoints[0]);
    for (int i = 1; i < trajectoryPoints.size(); i++) {
        path.lineTo(trajectoryPoints[i]);
    }
}

void AutomationManager::setTrajectoryDeltaTime(double relativeTimeFromPlay)
{
    trajectoryDeltaTime = relativeTimeFromPlay / currentPlaybackDuration;
    trajectoryDeltaTime = std::fmod(trajectoryDeltaTime, 1.0f);
    computeCurrentTrajectoryPoint();
}

void AutomationManager::compressTrajectoryXValues(int maxValue)
{
    int offset = 10.0 + kSourceRadius;
    maxValue -= offset;
    int size = getRecordingTrajectorySize();
    float delta = maxValue / (size + 1.0);
    for (int i = 0; i < size; i++) {
        trajectoryPoints.data()[i].setX(i * delta + offset);
    }
}

void AutomationManager::computeCurrentTrajectoryPoint()
{
    int dampeningCyclesTimes2 = dampeningCycles * 2;
    double currentScaleMin = 0.0, currentScaleMax = 0.0;

    if (trajectoryPoints.size() > 0) {
        if (trajectoryDeltaTime < lastTrajectoryDeltaTime) {
            if (isBackAndForth) {
                backAndForthDirection = 1 - backAndForthDirection;
                dampeningCycleCount++;
                if (dampeningCycleCount >= dampeningCyclesTimes2)
                    dampeningCycleCount = dampeningCyclesTimes2;
            }
            deviationCycleCount++;
        }
        lastTrajectoryDeltaTime = trajectoryDeltaTime;

        double trajectoryPhase;
        if (isBackAndForth && dampeningCycles > 0) {
            if (trajectoryDeltaTime <= 0.5) {
                trajectoryPhase = pow(trajectoryDeltaTime * 2.0, 2.0) * 0.5;
            } else {
                trajectoryPhase = 1.0 - pow(1.0 - ((trajectoryDeltaTime - 0.5) * 2.0), 2.0) * 0.5;
            }
        } else {
            trajectoryPhase = trajectoryDeltaTime;
        }

        double delta = trajectoryPhase * trajectoryPoints.size();

        if (backAndForthDirection == 1)
            delta = trajectoryPoints.size() - delta;

        if (delta + 1 >= trajectoryPoints.size()) {
            delta = trajectoryPoints.size();
        } else if (delta < 0) {
            delta = 0;
        }

        if (isBackAndForth && dampeningCycles > 0) {
            if (dampeningCycleCount < dampeningCyclesTimes2) {
                double relativeDeltaTime = (dampeningCycleCount + trajectoryDeltaTime) / dampeningCyclesTimes2;
                currentPlaybackDuration = playbackDuration - (pow(relativeDeltaTime, 2.0) * playbackDuration * 0.25);
                currentScaleMin = relativeDeltaTime * trajectoryPoints.size() * 0.5;
                currentScaleMax = trajectoryPoints.size() - currentScaleMin;
                double currentScale = (currentScaleMax - currentScaleMin) / trajectoryPoints.size();
                dampeningLastDelta = delta = delta * currentScale + currentScaleMin;
            } else {
                delta = dampeningLastDelta;
            }
        } else {
            dampeningLastDelta = delta;
        }

        double deltaRatio = static_cast<double>(trajectoryPoints.size() - 1) / trajectoryPoints.size();
        delta *= deltaRatio;
        int index = (int)delta;
        if (index + 1 < trajectoryPoints.size()) {
            double frac = delta - index;
            Point<float> p1 = trajectoryPoints[index];
            Point<float> p2 = trajectoryPoints[index + 1];
            currentTrajectoryPoint = Point<float>((p1.x + (p2.x - p1.x) * frac), (p1.y + (p2.y - p1.y) * frac));
        } else {
            currentTrajectoryPoint = Point<float>(trajectoryPoints.getLast().x, trajectoryPoints.getLast().y);
        }
    }

    if (degreeOfDeviationPerCycle != 0) {
        bool deviationFlag = true;
        if (isBackAndForth && dampeningCycles > 0) {
            if (approximatelyEqual(currentScaleMin, currentScaleMax)) {
                deviationFlag = false;
            }
        }
        if (deviationFlag) {
            currentDegreeOfDeviation = (deviationCycleCount + trajectoryDeltaTime) * degreeOfDeviationPerCycle;
            if (currentDegreeOfDeviation >= 360.0f) {
                currentDegreeOfDeviation -= 360.0f;
            }
        }
        AffineTransform t = AffineTransform::rotation(currentDegreeOfDeviation / 360.0f * 2.f * M_PI,
                                                      (fieldWidth / 2.f),
                                                      (fieldWidth / 2.f));
        currentTrajectoryPoint.applyTransform(t);
    }

    if (activateState) {
        setSourcePosition(
            Point<float>(currentTrajectoryPoint.x / fieldWidth, 1.0 - currentTrajectoryPoint.y / fieldWidth));
        sendTrajectoryPositionChangedEvent();
    }
}

Point<float> AutomationManager::getCurrentTrajectoryPoint() const
{
    if (activateState) {
        return currentTrajectoryPoint;
    } else {
        return Point<float>(getSourcePosition().x * fieldWidth, (1.0 - getSourcePosition().y) * fieldWidth);
    }
}

void AutomationManager::setSourcePosition(Point<float> pos)
{
    source.setPos(pos);
}

void AutomationManager::setSourcePositionX(float x)
{
    source.setX(x);
}

void AutomationManager::setSourcePositionY(float y)
{
    source.setY(y);
}

void AutomationManager::setSourceAndPlaybackPosition(Point<float> pos)
{
    setSourcePositionX(pos.x);
    setSourcePositionY(pos.y);
    setPlaybackPositionX(pos.x);
    setPlaybackPositionY(pos.y);
}

void AutomationManager::sendTrajectoryPositionChangedEvent()
{
    if (activateState || drawingType == TrajectoryType::realtime
        || static_cast<TrajectoryTypeAlt>(drawingType) == TrajectoryTypeAlt::realtime) {
        listeners.call([&](Listener & l) { l.trajectoryPositionChanged(this, source.getPos()); });
    }
}

void AutomationManager::fixSourcePosition()
{
    bool shouldBeFixed = sourceLink != SourceLink::independent;
    source.fixSourcePosition(shouldBeFixed);
}

void AutomationManager::setDrawingType(TrajectoryType type, Point<float> startpos)
{
    drawingType = type;

    trajectoryPoints.clear();

    int offset = fieldWidth / 2;
    int minlim = 8;
    int maxlim = fieldWidth - 8;

    Point<float> translated = startpos.translated(-0.5f, -0.5f) * 2.0f;
    float magnitude
        = sqrtf(translated.x * translated.x + translated.y * translated.y) * ((fieldWidth - kSourceDiameter) / 2);
    float angle = atan2f(translated.y, translated.x) - M_PI / 2.0f;

    int fsize = static_cast<int>(fieldWidth);
    int fsizeOver3 = fsize / 3;
    int fsizeOver4 = fsize / 4;
    float x, y;
    float step = 0, transx = 0, transy = 0, adjustedMagnitude = 0;
    switch (drawingType) {
    case TrajectoryType::realtime:
        playbackPosition = Point<float>(-1.0f, -1.0f);
        break;
    case TrajectoryType::drawing:
        playbackPosition = Point<float>(-1.0f, -1.0f);
        break;
    case TrajectoryType::circleClockwise:
        for (int i = 0; i < 200; i++) {
            x = sinf(2.0 * M_PI * i / 199 - angle) * magnitude + offset;
            x = x < minlim ? minlim : x > maxlim ? maxlim : x;
            y = -cosf(2.0 * M_PI * i / 199 - angle) * magnitude + offset;
            y = y < minlim ? minlim : y > maxlim ? maxlim : y;
            trajectoryPoints.add(Point<float>(x, y));
        }
        break;
    case TrajectoryType::circleCounterClockwise:
        for (int i = 0; i < 200; i++) {
            x = -sinf(2.0 * M_PI * i / 199 - angle) * magnitude + offset;
            x = x < minlim ? minlim : x > maxlim ? maxlim : x;
            y = -cosf(2.0 * M_PI * i / 199 - angle) * magnitude + offset;
            y = y < minlim ? minlim : y > maxlim ? maxlim : y;
            trajectoryPoints.add(Point<float>(x, y));
        }
        break;
    case TrajectoryType::ellipseClockwise:
        for (int i = 0; i < 200; i++) {
            x = sinf(2.0 * M_PI * i / 199) * magnitude * 0.5;
            y = -cosf(2.0 * M_PI * i / 199) * magnitude;
            float mag = sqrtf(x * x + y * y);
            float ang = atan2f(y, x);
            x = mag * cosf(ang - angle) + offset;
            y = mag * sinf(ang - angle) + offset;
            x = x < minlim ? minlim : x > maxlim ? maxlim : x;
            y = y < minlim ? minlim : y > maxlim ? maxlim : y;
            trajectoryPoints.add(Point<float>(x, y));
        }
        break;
    case TrajectoryType::ellipseCounterClockwise:
        for (int i = 0; i < 200; i++) {
            x = -sinf(2.0 * M_PI * i / 199) * magnitude * 0.5;
            y = -cosf(2.0 * M_PI * i / 199) * magnitude;
            float mag = sqrtf(x * x + y * y);
            float ang = atan2f(y, x);
            x = mag * cosf(ang - angle) + offset;
            y = mag * sinf(ang - angle) + offset;
            x = x < minlim ? minlim : x > maxlim ? maxlim : x;
            y = y < minlim ? minlim : y > maxlim ? maxlim : y;
            trajectoryPoints.add(Point<float>(x, y));
        }
        break;
    case TrajectoryType::spiralClockwiseOutIn:
        for (int i = 0; i < 300; i++) {
            x = sinf(2.0 * M_PI * i / 99) * magnitude * (1.0 - i / 300.0);
            y = -cosf(2.0 * M_PI * i / 99) * magnitude * (1.0 - i / 300.0);
            float mag = sqrtf(x * x + y * y);
            float ang = atan2f(y, x);
            x = mag * cosf(ang - angle) + offset;
            y = mag * sinf(ang - angle) + offset;
            x = x < minlim ? minlim : x > maxlim ? maxlim : x;
            y = y < minlim ? minlim : y > maxlim ? maxlim : y;
            trajectoryPoints.add(Point<float>(x, y));
        }
        break;
    case TrajectoryType::spiralCounterClockwiseOutIn:
        for (int i = 0; i < 300; i++) {
            x = -sinf(2.0 * M_PI * i / 99) * magnitude * (1.0 - i / 300.0);
            y = -cosf(2.0 * M_PI * i / 99) * magnitude * (1.0 - i / 300.0);
            float mag = sqrtf(x * x + y * y);
            float ang = atan2f(y, x);
            x = mag * cosf(ang - angle) + offset;
            y = mag * sinf(ang - angle) + offset;
            x = x < minlim ? minlim : x > maxlim ? maxlim : x;
            y = y < minlim ? minlim : y > maxlim ? maxlim : y;
            trajectoryPoints.add(Point<float>(x, y));
        }
        break;
    case TrajectoryType::spiralClockwiseInOut:
        for (int i = 0; i < 300; i++) {
            x = sinf(2.0 * M_PI * i / 99) * magnitude * (i / 300.0);
            y = -cosf(2.0 * M_PI * i / 99) * magnitude * (i / 300.0);
            float mag = sqrtf(x * x + y * y);
            float ang = atan2f(y, x);
            x = mag * cosf(ang - angle) + offset;
            y = mag * sinf(ang - angle) + offset;
            x = x < minlim ? minlim : x > maxlim ? maxlim : x;
            y = y < minlim ? minlim : y > maxlim ? maxlim : y;
            trajectoryPoints.add(Point<float>(x, y));
        }
        break;
    case TrajectoryType::spiralCounterClockwiseInOut:
        for (int i = 0; i < 300; i++) {
            x = -sinf(2.0 * M_PI * i / 99) * magnitude * (i / 300.0);
            y = -cosf(2.0 * M_PI * i / 99) * magnitude * (i / 300.0);
            float mag = sqrtf(x * x + y * y);
            float ang = atan2f(y, x);
            x = mag * cosf(ang - angle) + offset;
            y = mag * sinf(ang - angle) + offset;
            x = x < minlim ? minlim : x > maxlim ? maxlim : x;
            y = y < minlim ? minlim : y > maxlim ? maxlim : y;
            trajectoryPoints.add(Point<float>(x, y));
        }
        break;
    case TrajectoryType::squareClockwise:
    case TrajectoryType::squareCounterClockwise:
        step = 1.f / (fieldWidth / 4);
        transx = translated.x * ((fieldWidth / 2 - kSourceRadius));
        transy = translated.y * ((fieldWidth / 2 - kSourceRadius));
        magnitude = sqrtf(transx * transx + transy * transy);
        adjustedMagnitude = magnitude * M_PI / 2.f;
        float tmp1, tmp2;
        for (int i = 0; i < fsize; i++) {
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
            if (drawingType == TrajectoryType::squareClockwise) {
                x = tmp1;
                y = tmp2;
            } else {
                x = tmp2;
                y = tmp1;
            }
            x *= adjustedMagnitude;
            y *= adjustedMagnitude;
            x = x + (fieldWidth / 2.f) - (adjustedMagnitude / 2);
            y = y + (fieldWidth / 2.f) - (adjustedMagnitude / 2);
            Point<float> p(x, y);
            AffineTransform t = AffineTransform::rotation(-angle + M_PI / 4.f, (fieldWidth / 2.f), (fieldWidth / 2.f));
            p.applyTransform(t);
            p.x = p.x < minlim ? minlim : p.x > maxlim ? maxlim : p.x;
            p.y = p.y < minlim ? minlim : p.y > maxlim ? maxlim : p.y;
            trajectoryPoints.add(p);
        }
        break;
    case TrajectoryType::triangleClockwise:
    case TrajectoryType::triangleCounterClockwise:
        Point<float> p1(0.f, -1.f);
        Point<float> p2, p3;
        if (drawingType == TrajectoryType::triangleClockwise) {
            p2 = Point<float>(1.f, 1.f);
            p3 = Point<float>(-1.f, 1.f);
        } else {
            p2 = Point<float>(-1.f, 1.f);
            p3 = Point<float>(1.f, 1.f);
        }
        step = 1.f / (fieldWidth / 3);
        transx = translated.x * ((fieldWidth / 2 - kSourceRadius));
        transy = translated.y * ((fieldWidth / 2 - kSourceRadius));
        magnitude = sqrtf(transx * transx + transy * transy);
        for (int i = 0; i < fsize; i++) {
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
            Point<float> p(x * magnitude + (fieldWidth / 2.f), y * magnitude + (fieldWidth / 2.f));
            p.applyTransform(AffineTransform::rotation(-angle, (fieldWidth / 2.f), (fieldWidth / 2.f)));
            p.x = p.x < minlim ? minlim : p.x > maxlim ? maxlim : p.x;
            p.y = p.y < minlim ? minlim : p.y > maxlim ? maxlim : p.y;
            trajectoryPoints.add(p);
        }
        break;
    }

    if (drawingType > TrajectoryType::drawing) {
        setSourcePosition(Point<float>(trajectoryPoints[0].x / fieldWidth, 1.0 - trajectoryPoints[0].y / fieldWidth));
    } else {
        setSourcePosition(Point<float>(0.5f, 0.5f));
    }
}

void AutomationManager::setDrawingTypeAlt(TrajectoryTypeAlt type)
{
    drawingType = static_cast<TrajectoryType>(type);

    trajectoryPoints.clear();

    float offset = 10.0 + kSourceRadius;
    float width = fieldWidth - offset;
    float minPos = 15.0, maxPos = fieldWidth - 20.0;

    switch (type) {
    case TrajectoryTypeAlt::realtime:
        playbackPosition = Point<float>(-1.0f, -1.0f);
        break;
    case TrajectoryTypeAlt::drawing:
        playbackPosition = Point<float>(-1.0f, -1.0f);
        break;
    case TrajectoryTypeAlt::downUp:
        for (int i = 0; i < 200; i++) {
            float x = (i / 199.0) * width + offset;
            float y = (i / 199.0) * (maxPos - minPos) + minPos;
            trajectoryPoints.add(Point<float>(x, y));
        }
        break;
    case TrajectoryTypeAlt::upDown:
        for (int i = 0; i < 200; i++) {
            float x = (i / 199.0) * width + offset;
            float y = (1.0 - i / 199.0) * (maxPos - minPos) + minPos;
            trajectoryPoints.add(Point<float>(x, y));
        }
        break;
    }

    if (type > TrajectoryTypeAlt::drawing) {
        setSourcePosition(Point<float>(trajectoryPoints[0].x / fieldWidth, 1.0 - trajectoryPoints[0].y / fieldWidth));
    } else {
        setSourcePosition(Point<float>(0.5f, 0.5f));
    }
}
