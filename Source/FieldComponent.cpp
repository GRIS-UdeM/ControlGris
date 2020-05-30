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
#include "FieldComponent.h"

#include "ControlGrisConstants.h"
#include "ControlGrisUtilities.h"

void FieldComponent::setSelectedSource(int const selectedId)
{
    mSelectedSourceId = selectedId;
    mOldSelectedSourceId = selectedId;
    repaint();
}

void FieldComponent::setSources(Source * sources, int const numberOfSources)
{
    mSources = sources;
    mNumberOfSources = numberOfSources;
    mSelectedSourceId = 0;
    mOldSelectedSourceId = 0;
    for (int i{}; i < mNumberOfSources; ++i) {
        auto hue{ static_cast<float>(i) / mNumberOfSources
                  + 0.577251f }; // TODO: why is the Euler–Mascheroni constant appearing here???
        if (hue > 1.0f) {
            hue -= 1.0f;
        }
        mSources[i].setColour(Colour::fromHSV(hue, 1.0f, 1.0f, 0.85f));
    }
    repaint();
}

void FieldComponent::drawFieldBackground(Graphics & g, bool const isMainField, SpatMode const spatMode) const
{
    int const width{ getWidth() };
    int const height{ getHeight() };
    float const fieldCenter{ width / 2.0f };

    auto * lookAndFeel{ static_cast<GrisLookAndFeel *>(&getLookAndFeel()) };

    // Draw the background.
    g.setColour(lookAndFeel->getFieldColour());
    g.fillRect(0, 0, width, height);
    g.setColour(Colours::black);
    g.drawRect(0, 0, width, height);

    // Draw the grid.
    g.setColour(Colour::fromRGB(55, 56, 57));
    constexpr int gridCount = 8;
    for (int i{ 1 }; i < gridCount; ++i) {
        g.drawLine(width * i / gridCount, 0, height * i / gridCount, height);
        g.drawLine(0, height * i / gridCount, width, height * i / gridCount);
    }
    g.drawLine(0, 0, height, height);
    g.drawLine(0, height, height, 0);

    if (isMainField) {
        g.setColour(lookAndFeel->getLightColour());
        if (spatMode == SpatMode::VBAP) {
            // Draw big background circles.
            for (int i{ 1 }; i < 3; ++i) {
                float const w{ i / 2.0f * (width - SOURCE_FIELD_COMPONENT_DIAMETER) };
                float const x{ (width - w) / 2.0f };
                g.drawEllipse(x, x, w, w, 1);
            }

            // Draw center dot.
            float const w{ 0.0125f * (width - SOURCE_FIELD_COMPONENT_DIAMETER) };
            float const x{ (width - w) / 2.0f };
            g.drawEllipse(x, x, w, w, 1);
        } else {
            // Draw big background squares.
            float const offset{ width * 0.2f };
            float const size{ width * 0.6f };
            g.drawRect(offset, offset, size, size);
            g.drawRect(10, 10, width - 20, width - 20);
        }

        // Draw cross.
        g.drawLine(fieldCenter, SOURCE_FIELD_COMPONENT_RADIUS, fieldCenter, height - SOURCE_FIELD_COMPONENT_RADIUS);
        g.drawLine(SOURCE_FIELD_COMPONENT_RADIUS, height / 2, width - SOURCE_FIELD_COMPONENT_RADIUS, height / 2);
    } else {
        // Draw guide lines
        g.setColour(lookAndFeel->getLightColour());
        g.drawVerticalLine(5, 5, height - 5);
        g.drawHorizontalLine(height - 5, 5, width - 5);
    }
}

Point<float> FieldComponent::posToXy(Point<float> const & p, int const p_iwidth) const
{
    float const effectiveWidth{ p_iwidth - SOURCE_FIELD_COMPONENT_DIAMETER };
    float const x{ p.getX() * effectiveWidth };
    float const y{ p.getY() * effectiveWidth };
    return Point<float>{ x, effectiveWidth - y };
}

Point<float> FieldComponent::xyToPos(Point<float> const & p, int const p_iwidth) const
{
    float const k2{ SOURCE_FIELD_COMPONENT_DIAMETER / 2.0f };
    float const half{ (p_iwidth - SOURCE_FIELD_COMPONENT_DIAMETER) / 2.0f };

    // Limits for the LBAP algorithm
    float const px{ std::clamp(p.getX(), SOURCE_FIELD_COMPONENT_RADIUS, p_iwidth - SOURCE_FIELD_COMPONENT_RADIUS) };
    float const py{ std::clamp(p.getY(), SOURCE_FIELD_COMPONENT_RADIUS, p_iwidth - SOURCE_FIELD_COMPONENT_RADIUS) };

    float const x{ (px - k2 - half) / half * 0.5f + 0.5f };
    float const y{ (py - k2 - half) / half * 0.5f + 0.5f };

    return Point<float>(x, y);
}

void MainFieldComponent::drawSources(Graphics & g) const
{
    for (int sourceId{}; sourceId < mNumberOfSources; ++sourceId) {
        int const lineThickness{ (sourceId == mSelectedSourceId) ? 3 : 1 };
        float const saturation{ (sourceId == mSelectedSourceId) ? 1.0f : 0.75f };
        Point<float> pos;
        if (mSpatMode == SpatMode::VBAP) {
            pos = degreeToXy(AngleVector<float>{ mSources[sourceId].getAzimuth(), mSources[sourceId].getElevation() },
                             this->getWidth());
        } else {
            pos = posToXy(mSources[sourceId].getPos(), this->getWidth());
        }
        Rectangle<float> area{ pos.x, pos.y, SOURCE_FIELD_COMPONENT_DIAMETER, SOURCE_FIELD_COMPONENT_DIAMETER };
        area.expand(lineThickness, lineThickness);
        g.setColour(Colour(.2f, .2f, .2f, 1.0f));
        g.drawEllipse(area.translated(.5f, .5f), 1.0f);
        g.setGradientFill(ColourGradient(mSources[sourceId].getColour().withSaturation(saturation).darker(1.0f),
                                         pos.x + SOURCE_FIELD_COMPONENT_RADIUS,
                                         pos.y + SOURCE_FIELD_COMPONENT_RADIUS,
                                         mSources[sourceId].getColour().withSaturation(saturation),
                                         pos.x,
                                         pos.y,
                                         true));
        g.fillEllipse(area);
        g.setColour(Colours::white);
        g.drawFittedText(String(mSources[sourceId].getId() + 1),
                         area.getSmallestIntegerContainer(),
                         Justification(Justification::centred),
                         1);
    }
}

//==============================================================================
MainFieldComponent::MainFieldComponent(PositionAutomationManager & automan) : mAutomationManager(automan)
{
    mSpatMode = SpatMode::VBAP;
    mLineDrawingAnchor1 = INVALID_POINT;
    mLineDrawingAnchor2 = INVALID_POINT;
}

Point<float> MainFieldComponent::degreeToXy(AngleVector<float> const & p, int const p_iwidth) const
{
    float const effectiveWidth{ p_iwidth - SOURCE_FIELD_COMPONENT_DIAMETER };
    float const radius{ effectiveWidth / 2.0f };
    float const distance{ (90.0f - p.distance) / 90.0f };
    float const x{ radius * distance * sinf(degreeToRadian(p.angle)) + radius };
    float const y{ radius * distance * cosf(degreeToRadian(p.angle)) + radius };
    return Point<float>(effectiveWidth - x, effectiveWidth - y);
}

AngleVector<float> MainFieldComponent::xyToDegree(Point<float> const & p, int p_iwidth) const
{
    float const k2{ SOURCE_FIELD_COMPONENT_DIAMETER / 2.0f };
    float const half{ (p_iwidth - SOURCE_FIELD_COMPONENT_DIAMETER) / 2.0f };
    float const x{ (p.getX() - k2 - half) / half };
    float const y{ (p.getY() - k2 - half) / half };
    float ang{ atan2f(x, y) / MathConstants<float>::pi * 180.0f };
    if (ang <= -180.0f) {
        ang += 360.0f;
    }
    float rad = sqrtf(x * x + y * y);
    rad = 90.0f - rad * 90.0f;
    return AngleVector<float>{ -ang, rad };
}

void MainFieldComponent::setSpatMode(SpatMode const spatMode)
{
    mSpatMode = spatMode;
    repaint();
}

void MainFieldComponent::drawDomeSpans(Graphics & g) const
{
    int const width{ getWidth() };
    float const fieldCenter{ width / 2.0f };

    for (int sourceId{}; sourceId < mNumberOfSources; ++sourceId) {
        float const azimuth{ mSources[sourceId].getAzimuth() };
        float const elevation{ mSources[sourceId].getElevation() };
        float const azimuthSpan{ 180.0f * mSources[sourceId].getAzimuthSpan() };
        float const elevationSpan{ 45.0f * mSources[sourceId].getElevationSpan() };

        // Calculate min and max elevation in degrees.
        Point<float> minElev{ azimuth, elevation - elevationSpan };
        Point<float> maxElev{ azimuth, elevation + elevationSpan };

        if (minElev.getY() < 0.0f) {
            maxElev.setY(maxElev.getY() - minElev.getY());
            minElev.setY(0);
        }
        if (maxElev.getY() > 89.99f) {
            minElev.setY(minElev.getY() + maxElev.getY() - 89.99f);
            maxElev.setY(89.99f);
        }

        // Convert min and max elevation to xy position.
        float const halfWidth{ (width - SOURCE_FIELD_COMPONENT_DIAMETER) / 2.0f };
        Point<float> const minElevPos{
            -halfWidth * sinf(degreeToRadian(minElev.getX())) * (90.0f - minElev.getY()) / 90.0f,
            -halfWidth * cosf(degreeToRadian(minElev.getX())) * (90.0f - minElev.getY()) / 90.0f
        };
        Point<float> const maxElevPos{
            -halfWidth * sinf(degreeToRadian(maxElev.getX())) * (90.0f - maxElev.getY()) / 90.0f,
            -halfWidth * cosf(degreeToRadian(maxElev.getX())) * (90.0f - maxElev.getY()) / 90.0f
        };

        // Calculate min and max radius.
        float const minRadius{ sqrtf(minElevPos.getX() * minElevPos.getX() + minElevPos.getY() * minElevPos.getY()) };
        float const maxRadius{ sqrtf(maxElevPos.getX() * maxElevPos.getX() + maxElevPos.getY() * maxElevPos.getY()) };

        // Draw the path for spanning.
        Path myPath{};
        myPath.startNewSubPath(fieldCenter + minElevPos.getX(), fieldCenter + minElevPos.getY());
        myPath.addCentredArc(fieldCenter,
                             fieldCenter,
                             minRadius,
                             minRadius,
                             0.0,
                             degreeToRadian(-azimuth),
                             degreeToRadian(-azimuth + azimuthSpan));
        myPath.addCentredArc(fieldCenter,
                             fieldCenter,
                             maxRadius,
                             maxRadius,
                             0.0,
                             degreeToRadian(-azimuth + azimuthSpan),
                             degreeToRadian(-azimuth - azimuthSpan));
        myPath.addCentredArc(fieldCenter,
                             fieldCenter,
                             minRadius,
                             minRadius,
                             0.0,
                             degreeToRadian(-azimuth - azimuthSpan),
                             degreeToRadian(-azimuth));
        myPath.closeSubPath();

        g.setColour(mSources[sourceId].getColour().withAlpha(0.1f));
        g.fillPath(myPath);
        g.setColour(mSources[sourceId].getColour().withAlpha(0.5f));
        PathStrokeType strokeType = PathStrokeType(1.5);
        g.strokePath(myPath, strokeType);
    }
}

void MainFieldComponent::drawCubeSpans(Graphics & g) const
{
    int const width{ getWidth() };

    for (int sourceId{}; sourceId < mNumberOfSources; ++sourceId) {
        float const azimuthSpan{ width * mSources[sourceId].getAzimuthSpan() };
        float const halfAzimuthSpan{ azimuthSpan / 2.0f - SOURCE_FIELD_COMPONENT_RADIUS };
        float const saturation{ (sourceId == mSelectedSourceId) ? 1.0f : 0.5f };
        Point<float> const pos{ posToXy(mSources[sourceId].getPos(), width) };

        g.setColour(mSources[sourceId].getColour().withSaturation(saturation).withAlpha(0.5f));
        g.drawEllipse(pos.x - halfAzimuthSpan, pos.y - halfAzimuthSpan, azimuthSpan, azimuthSpan, 1.5f);
        g.setColour(mSources[sourceId].getColour().withSaturation(saturation).withAlpha(0.1f));
        g.fillEllipse(pos.x - halfAzimuthSpan, pos.y - halfAzimuthSpan, azimuthSpan, azimuthSpan);
    }
}

void MainFieldComponent::drawTrajectoryHandle(Graphics & g) const
{
    int const width{ getWidth() };
    bool shouldDrawTrajectoryHandle{ false };
    if (mNumberOfSources == 1) {
        if (mAutomationManager.getTrajectoryType() == PositionTrajectoryType::drawing && !mIsPlaying) {
            shouldDrawTrajectoryHandle = true;
        }
    } else {
        if (mAutomationManager.getTrajectoryType() == PositionTrajectoryType::drawing && !mIsPlaying) {
            shouldDrawTrajectoryHandle = true;
        } else if (mAutomationManager.getTrajectoryType() == PositionTrajectoryType::realtime
                   && mAutomationManager.getSourceLink() == PositionSourceLink::circularDeltaLock) {
            shouldDrawTrajectoryHandle = true;
        }
    }

    if (shouldDrawTrajectoryHandle) {
        Point<float> rpos;
        if (mSpatMode == SpatMode::VBAP) {
            rpos = degreeToXy(AngleVector<float>{ mAutomationManager.getSource().getAzimuth(),
                                                  mAutomationManager.getSource().getElevation() },
                              width);
        } else {
            rpos = posToXy(mAutomationManager.getSourcePosition(), width);
        }
        Rectangle<float> const rarea{ rpos.x,
                                      rpos.y,
                                      SOURCE_FIELD_COMPONENT_DIAMETER,
                                      SOURCE_FIELD_COMPONENT_DIAMETER };
        g.setColour(Colour::fromRGB(176, 176, 228));
        g.fillEllipse(rarea);
        g.setColour(Colour::fromRGB(64, 64, 128));
        g.drawEllipse(rarea, 1);
        g.setColour(Colours::white);
        g.drawFittedText(String("X"), rarea.getSmallestIntegerContainer(), Justification(Justification::centred), 1);
    }
}

void MainFieldComponent::paint(Graphics & g)
{
    int const width{ getWidth() };

    drawFieldBackground(g, true, mSpatMode);

    // Draw recording trajectory handle before sources (if source link *is not* Delta Lock).
    if (mAutomationManager.getSourceLink() != PositionSourceLink::circularDeltaLock) {
        drawTrajectoryHandle(g);
    }

    // Draw recording trajectory path and current position dot.
    g.setColour(Colour::fromRGB(176, 176, 228));
    if (hasValidLineDrawingAnchor1() && hasValidLineDrawingAnchor2()) {
        Path lineDrawingPath;
        lineDrawingPath.startNewSubPath(mLineDrawingAnchor1);
        lineDrawingPath.lineTo(mLineDrawingAnchor2);
        lineDrawingPath.closeSubPath();
        g.strokePath(lineDrawingPath, PathStrokeType(.75f));
    }
    if (mAutomationManager.getRecordingTrajectorySize() > 1) {
        Path trajectoryPath;
        mAutomationManager.createRecordingPath(trajectoryPath);
        g.strokePath(trajectoryPath, PathStrokeType(.75f));
    }
    if (mIsPlaying && !isMouseButtonDown() && mAutomationManager.getTrajectoryType() != PositionTrajectoryType::realtime
        && mAutomationManager.getPositionActivateState()) {
        Point<float> const dpos{ mAutomationManager.getCurrentTrajectoryPoint() };
        g.fillEllipse(dpos.x - 4, dpos.y - 4, 8, 8);
    }

    // Draw sources.
    this->drawSources(g);

    // Draw spanning.
    this->drawSpans(g);

    // Draw recording trajectory handle after sources (if source link *is* Delta Lock).
    if (mAutomationManager.getSourceLink() == PositionSourceLink::circularDeltaLock) {
        drawTrajectoryHandle(g);
    }

    if (mShowCircularSourceSelectionWarning) {
        g.setColour(Colours::white);
        g.drawFittedText(WARNING_CIRCULAR_SOURCE_SELECTION,
                         juce::Rectangle<int>(0, 0, width, 50),
                         Justification(Justification::centred),
                         1);
    }
}

bool MainFieldComponent::isTrajectoryHandleClicked(MouseEvent const & event)
{
    int const width{ getWidth() };
    if (mAutomationManager.getTrajectoryType() == PositionTrajectoryType::drawing
        || mAutomationManager.getTrajectoryType() == PositionTrajectoryType::realtime) {
        Point<float> pos;
        if (mSpatMode == SpatMode::VBAP) {
            pos = degreeToXy(AngleVector<float>{ mAutomationManager.getSource().getAzimuth(),
                                                 mAutomationManager.getSource().getElevation() },
                             width);
        } else {
            pos = posToXy(mAutomationManager.getSource().getPos(), width);
        }
        Rectangle<float> const area{ pos.x, pos.y, SOURCE_FIELD_COMPONENT_DIAMETER, SOURCE_FIELD_COMPONENT_DIAMETER };
        if (area.contains(event.getMouseDownPosition().toFloat())) {
            mOldSelectedSourceId = mSelectedSourceId;
            mSelectedSourceId = TRAJECTORY_HANDLE_SOURCE_ID;
            if (mAutomationManager.getTrajectoryType() == PositionTrajectoryType::drawing) {
                mAutomationManager.resetRecordingTrajectory(event.getMouseDownPosition().toFloat());
                if (event.mods.isShiftDown())
                    mLineDrawingAnchor1 = event.getMouseDownPosition().toFloat();
            } else {
                listeners.call([&](Listener & l) { l.fieldTrajectoryHandleClicked(0); });
            }
            repaint();
            return true;
        } else {
            return false;
        }
    }
    return false;
}

void MainFieldComponent::drawSpans(Graphics & g) const
{
    if (mSpatMode == SpatMode::VBAP) {
        drawDomeSpans(g);
    } else {
        drawCubeSpans(g);
    }
}

void MainFieldComponent::mouseDown(MouseEvent const & event)
{
    int const width{ getWidth() };
    int const height{ getHeight() };

    if (hasValidLineDrawingAnchor1()) {
        Point<float> const anchor1{ mLineDrawingAnchor1 };
        Point<float> const anchor2{ clipRecordingPosition(event.getPosition()).toFloat() };
        auto const numSteps{ static_cast<int>(jmax(std::abs(anchor2.x - anchor1.x), std::abs(anchor2.y - anchor1.y))) };
        auto const xInc{ (anchor2.x - anchor1.x) / numSteps };
        auto const yInc{ (anchor2.y - anchor1.y) / numSteps };
        for (int i{ 1 }; i <= numSteps; ++i) {
            mAutomationManager.addRecordingPoint(Point<float>{ anchor1.x + xInc * i, anchor1.y + yInc * i });
        }
        if (event.mods.isShiftDown()) {
            mLineDrawingAnchor1 = anchor2;
            mLineDrawingAnchor2 = INVALID_POINT;
        } else {
            mLineDrawingAnchor1 = INVALID_POINT;
            mLineDrawingAnchor2 = INVALID_POINT;
        }
        repaint();
        return;
    }

    if (!event.mods.isShiftDown()) {
        mLineDrawingAnchor1 = INVALID_POINT;
        mLineDrawingAnchor2 = INVALID_POINT;
    }

    Point<int> const mouseLocation{ event.x, height - event.y };

    mSelectedSourceId = NO_SELECTION_SOURCE_ID;

    // Check if we click on the trajectory handle.
    if (mAutomationManager.getSourceLink() == PositionSourceLink::circularDeltaLock) {
        if (isTrajectoryHandleClicked(event)) {
            return;
        }
    }

    // Check if we click on a new source.
    bool clickOnSource = false;
    for (int i{}; i < mNumberOfSources; ++i) {
        Point<float> pos;
        if (mSpatMode == SpatMode::VBAP) {
            pos = degreeToXy(AngleVector<float>{ mSources[i].getAzimuth(), mSources[i].getElevation() }, width);
        } else {
            pos = posToXy(mSources[i].getPos(), width);
        }
        Rectangle<float> area
            = Rectangle<float>(pos.x, pos.y, SOURCE_FIELD_COMPONENT_DIAMETER, SOURCE_FIELD_COMPONENT_DIAMETER);
        if (area.contains(event.getMouseDownPosition().toFloat())) {
            if (i > 0 && mAutomationManager.getSourceLink() != PositionSourceLink::independent
                && mAutomationManager.getSourceLink() != PositionSourceLink::circularDeltaLock) {
                mShowCircularSourceSelectionWarning = true;
            } else {
                mSelectedSourceId = i;
                listeners.call([&](Listener & l) { l.fieldSourcePositionChanged(mSelectedSourceId, 0); });
                clickOnSource = true;
            }
            break;
        }
    }

    if (clickOnSource) {
        repaint();
        return;
    }

    // If clicked in an empty space while in mode DRAWING, start a new drawing.
    if (mAutomationManager.getTrajectoryType() == PositionTrajectoryType::drawing) {
        mOldSelectedSourceId = mSelectedSourceId;
        mSelectedSourceId = TRAJECTORY_HANDLE_SOURCE_ID;
        mAutomationManager.resetRecordingTrajectory(event.getMouseDownPosition().toFloat());
        if (event.mods.isShiftDown()) {
            mLineDrawingAnchor1 = event.getMouseDownPosition().toFloat();
        }
        repaint();
    }
}

void MainFieldComponent::mouseDrag(const MouseEvent & event)
{
    // No selection.
    if (mSelectedSourceId == NO_SELECTION_SOURCE_ID) {
        return;
    }

    auto const width{ getWidth() };
    auto const height{ getHeight() };

    Point<int> const mousePosition{ event.x, height - event.y };

    auto * selectedSource{ mSelectedSourceId == TRAJECTORY_HANDLE_SOURCE_ID ? &mAutomationManager.getSource()
                                                                            : &mSources[mSelectedSourceId] };

    if (mSpatMode == SpatMode::VBAP) {
        auto const pos{ xyToDegree(mousePosition.toFloat(), width) };
        selectedSource->setAzimuth(pos.angle);
        selectedSource->setElevationNoClip(pos.distance);
    } else {
        Point<float> const pos{ xyToPos(mousePosition.toFloat(), width) };
        selectedSource->setX(pos.x);
        selectedSource->setY(pos.y);
    }

    if (mSelectedSourceId == TRAJECTORY_HANDLE_SOURCE_ID) {
        if (mAutomationManager.getTrajectoryType() == PositionTrajectoryType::drawing) {
            if (hasValidLineDrawingAnchor1()) {
                mLineDrawingAnchor2 = clipRecordingPosition(event.getPosition()).toFloat();
            } else {
                mAutomationManager.addRecordingPoint(clipRecordingPosition(event.getPosition()).toFloat());
            }
        } else if (mAutomationManager.getTrajectoryType() == PositionTrajectoryType::realtime) {
            mAutomationManager.sendTrajectoryPositionChangedEvent();
        }
    } else {
        listeners.call([&](Listener & l) { l.fieldSourcePositionChanged(mSelectedSourceId, 0); });
    }

    bool needToAdjustAutomationManager{ false };
    if (mSelectedSourceId == 0 && mAutomationManager.getTrajectoryType() == PositionTrajectoryType::realtime
        && (mAutomationManager.getSourceLink() == PositionSourceLink::independent
            || mAutomationManager.getSourceLink() == PositionSourceLink::linkSymmetricX
            || mAutomationManager.getSourceLink() == PositionSourceLink::linkSymmetricY)) {
        needToAdjustAutomationManager = true;
    } else if (mAutomationManager.getSourceLink() >= PositionSourceLink::circular
               && mAutomationManager.getSourceLink() < PositionSourceLink::circularDeltaLock
               && mAutomationManager.getTrajectoryType() == PositionTrajectoryType::realtime) {
        needToAdjustAutomationManager = true;
    }

    if (needToAdjustAutomationManager) {
        if (mSpatMode == SpatMode::VBAP) {
            mAutomationManager.getSource().setAzimuth(mSources[0].getAzimuth());
            mAutomationManager.getSource().setElevation(mSources[0].getElevation());
        } else {
            mAutomationManager.getSource().setX(mSources[0].getX());
            mAutomationManager.getSource().setY(mSources[0].getY());
        }
        mAutomationManager.sendTrajectoryPositionChangedEvent();
    }

    repaint();
}

void MainFieldComponent::mouseMove(const MouseEvent & event)
{
    if (mSelectedSourceId == TRAJECTORY_HANDLE_SOURCE_ID
        && mAutomationManager.getTrajectoryType() == PositionTrajectoryType::drawing && hasValidLineDrawingAnchor1()) {
        mLineDrawingAnchor2 = clipRecordingPosition(event.getPosition()).toFloat();
        repaint();
    }
}

void MainFieldComponent::mouseUp(const MouseEvent & event)
{
    if (mSelectedSourceId == TRAJECTORY_HANDLE_SOURCE_ID) {
        if (mAutomationManager.getTrajectoryType() == PositionTrajectoryType::drawing && !event.mods.isShiftDown()) {
            mAutomationManager.addRecordingPoint(mAutomationManager.getLastRecordingPoint());
            mSelectedSourceId = mOldSelectedSourceId;
        }
        repaint();
    }
    mShowCircularSourceSelectionWarning = false;
}

Point<int> MainFieldComponent::clipRecordingPosition(Point<int> const & pos)
{
    constexpr int MAGIC{ 10 }; // TODO

    int const max{ getWidth() - MAGIC };
    Point<int> const clipped{ std::clamp(pos.x, MAGIC, max), std::clamp(pos.y, MAGIC, max) };

    return clipped;
}

//==============================================================================
void ElevationFieldComponent::drawSources(Graphics & g) const
{
    jassert(getWidth() == getHeight());
    auto const componentSize{ this->getWidth() };

    for (int sourceId{}; sourceId < mNumberOfSources; ++sourceId) {
        auto const lineThickness{ (sourceId == mSelectedSourceId) ? 3 : 1 };
        float const saturation{ (sourceId == mSelectedSourceId) ? 1.0f : 0.75f };
        float const x{ static_cast<float>(sourceId) / mNumberOfSources * (componentSize - 50.0f) + 50.0f };
        float const y{ (90.0f - mSources[sourceId].getElevation()) / 90.0f * (componentSize - 35.0f) + 5.0f };
        Point<float> const pos{ x, y };
        Rectangle<float> area(pos.x, pos.y, SOURCE_FIELD_COMPONENT_DIAMETER, SOURCE_FIELD_COMPONENT_DIAMETER);

        area.expand(lineThickness, lineThickness);
        g.setColour(Colour(.2f, .2f, .2f, 1.0f));
        g.drawEllipse(area.translated(.5f, .5f), 1.0f);
        g.setGradientFill(ColourGradient(mSources[sourceId].getColour().withSaturation(saturation).darker(1.0f),
                                         pos.x + SOURCE_FIELD_COMPONENT_RADIUS,
                                         pos.y + SOURCE_FIELD_COMPONENT_RADIUS,
                                         mSources[sourceId].getColour().withSaturation(saturation),
                                         pos.x,
                                         pos.y,
                                         true));
        g.fillEllipse(area);
        g.drawLine(pos.x + SOURCE_FIELD_COMPONENT_RADIUS,
                   pos.y + SOURCE_FIELD_COMPONENT_DIAMETER + lineThickness / 2,
                   pos.x + SOURCE_FIELD_COMPONENT_RADIUS,
                   componentSize - 5,
                   lineThickness);
        g.setColour(Colours::white);
        g.drawFittedText(String(mSources[sourceId].getId() + 1),
                         area.getSmallestIntegerContainer(),
                         Justification(Justification::centred),
                         1);

        // draw Spans
        float const elevationSpan{ 50.0f * mSources[sourceId].getElevationSpan() };
        g.setColour(mSources[sourceId].getColour().withSaturation(saturation).withAlpha(0.5f));
        g.drawRect(pos.x + SOURCE_FIELD_COMPONENT_RADIUS - elevationSpan / 2,
                   pos.y + SOURCE_FIELD_COMPONENT_DIAMETER + lineThickness / 2,
                   elevationSpan,
                   componentSize - 5.0f,
                   1.5);
        g.setColour(mSources[sourceId].getColour().withSaturation(saturation).withAlpha(0.1f));
        g.fillRect(pos.x + SOURCE_FIELD_COMPONENT_RADIUS - elevationSpan / 2,
                   pos.y + SOURCE_FIELD_COMPONENT_DIAMETER + lineThickness / 2,
                   elevationSpan,
                   componentSize - 5.0f);
    }
}

void ElevationFieldComponent::paint(Graphics & g)
{
    int const width{ getWidth() };
    int const height{ getHeight() };

    drawFieldBackground(g, false);

    bool shouldDrawTrajectoryHandle{ false };
    if (mNumberOfSources == 1) {
        if (static_cast<ElevationTrajectoryType>(mAutomationManager.getTrajectoryType())
                == ElevationTrajectoryType::drawing
            && !mIsPlaying) {
            shouldDrawTrajectoryHandle = true;
        }
    } else {
        if (static_cast<ElevationTrajectoryType>(mAutomationManager.getTrajectoryType())
                == ElevationTrajectoryType::drawing
            && !mIsPlaying) {
            shouldDrawTrajectoryHandle = true;
        } else if (mAutomationManager.getTrajectoryType() == ElevationTrajectoryType::realtime
                   && static_cast<ElevationSourceLink>(mAutomationManager.getSourceLink())
                          == ElevationSourceLink::deltaLock) {
            shouldDrawTrajectoryHandle = true;
        }
    }

    // Draw recording trajectory handle.
    if (shouldDrawTrajectoryHandle) {
        auto const pos{ posToXy(mAutomationManager.getSourcePosition(), width) };
        auto const lineThickness{ mSelectedSourceId == TRAJECTORY_HANDLE_SOURCE_ID ? 3 : 1 };
        Rectangle<float> const rarea{ 10.0f, pos.y, SOURCE_FIELD_COMPONENT_DIAMETER, SOURCE_FIELD_COMPONENT_DIAMETER };

        g.setColour(Colour::fromRGB(176, 176, 228));
        g.drawLine(10 + SOURCE_FIELD_COMPONENT_RADIUS,
                   pos.y + SOURCE_FIELD_COMPONENT_DIAMETER + lineThickness / 2.0f,
                   10 + SOURCE_FIELD_COMPONENT_RADIUS,
                   height - 5,
                   lineThickness);
        g.fillEllipse(rarea);
        g.setColour(Colour::fromRGB(64, 64, 128));
        g.drawEllipse(rarea, 1);
        g.setColour(Colours::white);
        g.drawFittedText(String("X"), rarea.getSmallestIntegerContainer(), Justification(Justification::centred), 1);
    }

    // Draw recording trajectory path and current position dot.
    g.setColour(Colour::fromRGB(176, 176, 228));
    if (mAutomationManager.getRecordingTrajectorySize() > 1) {
        Path trajectoryPath{};
        mAutomationManager.createRecordingPath(trajectoryPath);
        g.strokePath(trajectoryPath, PathStrokeType(.75f));
    }
    if (mIsPlaying && !isMouseButtonDown()
        && static_cast<ElevationTrajectoryType>(mAutomationManager.getTrajectoryType())
               != ElevationTrajectoryType::realtime
        && mAutomationManager.getPositionActivateState()) {
        Point<float> const dpos{ mAutomationManager.getCurrentTrajectoryPoint() };
        g.fillEllipse(dpos.x - 4, dpos.y - 4, 8, 8);
    }

    // Draw sources.
    this->drawSources(g);
}

void ElevationFieldComponent::mouseDown(const MouseEvent & event)
{
    auto const width{ getWidth() };
    auto const height{ getHeight() };

    mSelectedSourceId = NO_SELECTION_SOURCE_ID;

    // Check if we click on a new source.
    bool clickOnSource{ false };
    for (int i{}; i < mNumberOfSources; ++i) {
        float const x{ static_cast<float>(i) / mNumberOfSources * (width - 50.0f) + 50.0f };
        float const y{ (90.0f - mSources[i].getElevation()) / 90.0f * (height - 35.0f) + 5.0f };
        Point<float> const pos{ x, y };
        Rectangle<float> const area{ pos.x, pos.y, SOURCE_FIELD_COMPONENT_DIAMETER, SOURCE_FIELD_COMPONENT_DIAMETER };
        if (area.contains(event.getMouseDownPosition().toFloat())) {
            mSelectedSourceId = i;
            listeners.call([&](Listener & l) { l.fieldSourcePositionChanged(mSelectedSourceId, 1); });
            clickOnSource = true;
            break;
        }
    }

    if (clickOnSource) {
        repaint();
        return;
    }

    // Check if we record a trajectory.
    if (static_cast<ElevationTrajectoryType>(mAutomationManager.getTrajectoryType()) == ElevationTrajectoryType::drawing
        || static_cast<ElevationTrajectoryType>(mAutomationManager.getTrajectoryType())
               == ElevationTrajectoryType::realtime) {
        Point<float> const pos{ posToXy(mAutomationManager.getSourcePosition(), width).withX(10.0f) };
        Rectangle<float> const area{ pos.x, pos.y, SOURCE_FIELD_COMPONENT_DIAMETER, SOURCE_FIELD_COMPONENT_DIAMETER };
        if (area.contains(event.getMouseDownPosition().toFloat())) {
            mOldSelectedSourceId = mSelectedSourceId;
            mSelectedSourceId = TRAJECTORY_HANDLE_SOURCE_ID;
            if (static_cast<ElevationTrajectoryType>(mAutomationManager.getTrajectoryType())
                == ElevationTrajectoryType::drawing) {
                mCurrentRecordingPositionX = 10 + SOURCE_FIELD_COMPONENT_RADIUS;
                mAutomationManager.resetRecordingTrajectory(event.getMouseDownPosition().toFloat());
            } else {
                listeners.call([&](Listener & l) { l.fieldTrajectoryHandleClicked(1); });
            }
            repaint();
            return;
        }
    }

    // If clicked in an empty space while in mode DRAWING, start a new drawing.
    if (static_cast<ElevationTrajectoryType>(mAutomationManager.getTrajectoryType())
        == ElevationTrajectoryType::drawing) {
        mOldSelectedSourceId = mSelectedSourceId;
        mSelectedSourceId = TRAJECTORY_HANDLE_SOURCE_ID;
        mCurrentRecordingPositionX = 10 + static_cast<int>(SOURCE_FIELD_COMPONENT_RADIUS);
        mAutomationManager.resetRecordingTrajectory(
            Point<float>{ static_cast<float>(mCurrentRecordingPositionX), event.getMouseDownPosition().toFloat().y });
        repaint();
    }
}

void ElevationFieldComponent::mouseDrag(const MouseEvent & event)
{
    auto const height{ static_cast<float>(getHeight()) };

    // No selection.
    if (mSelectedSourceId == NO_SELECTION_SOURCE_ID) {
        return;
    }

    if (mSelectedSourceId == TRAJECTORY_HANDLE_SOURCE_ID) {
        if (static_cast<ElevationTrajectoryType>(mAutomationManager.getTrajectoryType())
            == ElevationTrajectoryType::drawing) {
            mCurrentRecordingPositionX += 1;
            if (mCurrentRecordingPositionX >= height) {
                mCurrentRecordingPositionX = height;
                mAutomationManager.compressTrajectoryXValues(height);
            }
            float y{ event.getPosition().toFloat().y };
            y = std::clamp(y, 15.0f, height - 20.0f);
            mAutomationManager.addRecordingPoint(Point<float>{ static_cast<float>(mCurrentRecordingPositionX), y });
            y = height - event.getPosition().toFloat().y;
            y = std::clamp(y, 15.0f, height - 20.0f);
            mAutomationManager.setSourcePosition(xyToPos(Point<float>{ 10.0f, y }, height));
        } else if (static_cast<ElevationTrajectoryType>(mAutomationManager.getTrajectoryType())
                   == ElevationTrajectoryType::realtime) {
            float y{ height - event.y };
            y = std::clamp(y, 15.0f, height - 20.0f);
            mAutomationManager.setSourcePosition(xyToPos(Point<float>{ 10.0f, y }, height));
            mAutomationManager.sendTrajectoryPositionChangedEvent();
        }
    } else {
        float const elevation{ (height - event.y - SOURCE_FIELD_COMPONENT_DIAMETER) / (height - 35.0f) * 90.0f };
        mSources[mSelectedSourceId].setElevation(elevation);
        listeners.call([&](Listener & l) { l.fieldSourcePositionChanged(mSelectedSourceId, 1); });
    }

    bool needToAdjustAutomationManager{ false };
    if (static_cast<ElevationSourceLink>(mAutomationManager.getSourceLink()) == ElevationSourceLink::independent
        && mSelectedSourceId == 0
        && static_cast<ElevationTrajectoryType>(mAutomationManager.getTrajectoryType())
               == ElevationTrajectoryType::realtime) {
        needToAdjustAutomationManager = true;
    } else if (static_cast<ElevationSourceLink>(mAutomationManager.getSourceLink())
                   >= ElevationSourceLink::fixedElevation
               && static_cast<ElevationSourceLink>(mAutomationManager.getSourceLink()) < ElevationSourceLink::deltaLock
               && static_cast<ElevationTrajectoryType>(mAutomationManager.getTrajectoryType())
                      == ElevationTrajectoryType::realtime) {
        needToAdjustAutomationManager = true;
    }

    if (needToAdjustAutomationManager) {
        float const y{ mSources[0].getElevation() / 90.0f * (height - 15.0f) + 5.0f };
        mAutomationManager.setSourcePosition(xyToPos(Point<float>{ 10.0f, y }, height));
        mAutomationManager.sendTrajectoryPositionChangedEvent();
    }

    repaint();
}

void ElevationFieldComponent::mouseUp(const MouseEvent & event)
{
    if (mSelectedSourceId == TRAJECTORY_HANDLE_SOURCE_ID) {
        if (static_cast<ElevationTrajectoryType>(mAutomationManager.getTrajectoryType())
            == ElevationTrajectoryType::drawing) {
            mAutomationManager.addRecordingPoint(mAutomationManager.getLastRecordingPoint());
            mSelectedSourceId = mOldSelectedSourceId;
        }
        repaint();
    }
}
