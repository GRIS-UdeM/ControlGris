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
#include "ElevationSourceComponent.h"

FieldComponent::FieldComponent() noexcept
{
    mTrajectoryHandleComponent = std::make_unique<TrajectoryHandleComponent>(*this);
    mTrajectoryHandleComponent->setCentrePosition(getWidth() / 2, getHeight() / 2);
    addAndMakeVisible(mTrajectoryHandleComponent.get());
    setSize(MIN_FIELD_WIDTH, MIN_FIELD_WIDTH);
}

void FieldComponent::setSelectedSource(std::optional<SourceIndex> const selectedSource)
{
    mOldSelectedSource = mSelectedSource;
    mSelectedSource = selectedSource;
    applySourceSelectionToComponents();
}

void FieldComponent::setSources(Sources & sources)
{
    mSources = &sources;
    mSelectedSource.reset();
    mOldSelectedSource.reset();

    rebuildSourceComponents(sources.size());
}

void FieldComponent::drawBackgroundGrid(Graphics & g) const
{
    auto const fieldComponentSize{ getWidth() };
    auto const fieldComponentSize_f{ static_cast<float>(fieldComponentSize) };

    auto * lookAndFeel{ dynamic_cast<GrisLookAndFeel *>(&getLookAndFeel()) };
    jassert(lookAndFeel != nullptr);

    // Draw the background.
    g.setColour(lookAndFeel->getFieldColour());
    g.fillRect(0, 0, fieldComponentSize, fieldComponentSize);
    g.setColour(Colours::black);
    g.drawRect(0, 0, fieldComponentSize, fieldComponentSize);

    // Draw the grid.
    g.setColour(Colour::fromRGB(55, 56, 57));
    constexpr int gridCount = 8;
    for (int i{ 1 }; i < gridCount; ++i) {
        g.drawLine(fieldComponentSize_f * i / gridCount, 0, fieldComponentSize_f * i / gridCount, fieldComponentSize);
        g.drawLine(0, fieldComponentSize_f * i / gridCount, fieldComponentSize, fieldComponentSize_f * i / gridCount);
    }
    g.drawLine(0, 0, fieldComponentSize, fieldComponentSize);
    g.drawLine(0, fieldComponentSize, fieldComponentSize, 0);
}

void PositionFieldComponent::applySourceSelectionToComponents()
{
    if (mSelectedSource.has_value()) {
        for (auto component : mSourceComponents) {
            bool const selected{ mSelectedSource.value() == component->getSourceIndex() };
            component->setSelected(selected);
        }
    } else {
        for (auto component : mSourceComponents) {
            component->setSelected(false);
        }
    }
}

void ElevationFieldComponent::applySourceSelectionToComponents()
{
    // TODO: this is a dupe of PositionFieldComponent::applySourceSelectionToComponents()
    if (mSelectedSource.has_value()) {
        for (auto component : mSourceComponents) {
            bool const selected{ mSelectedSource.value() == component->getSourceIndex() };
            component->setSelected(selected);
        }
    } else {
        for (auto component : mSourceComponents) {
            component->setSelected(false);
        }
    }
}

void PositionFieldComponent::drawBackground(Graphics & g) const
{
    auto const fieldComponentSize{ getWidth() };
    auto const fieldComponentSize_f{ static_cast<float>(fieldComponentSize) };
    auto * lookAndFeel{ dynamic_cast<GrisLookAndFeel *>(&getLookAndFeel()) };
    jassert(lookAndFeel != nullptr);
    auto const fieldCenter{ fieldComponentSize / 2.0f };

    drawBackgroundGrid(g);

    g.setColour(lookAndFeel->getLightColour());
    if (mSpatMode == SpatMode::dome) {
        // Draw big background circles.
        for (int i{ 1 }; i < 3; ++i) {
            float const w{ i / 2.0f * (fieldComponentSize - SOURCE_FIELD_COMPONENT_DIAMETER) };
            float const x{ (fieldComponentSize - w) / 2.0f };
            g.drawEllipse(x, x, w, w, 1);
        }

        // Draw center dot.
        float const w{ 0.0125f * (fieldComponentSize - SOURCE_FIELD_COMPONENT_DIAMETER) };
        float const x{ (fieldComponentSize - w) / 2.0f };
        g.drawEllipse(x, x, w, w, 1);
    } else {
        // Draw big background squares.
        float const offset{ fieldComponentSize * 0.2f };
        float const size{ fieldComponentSize * 0.6f };
        g.drawRect(offset, offset, size, size);
        g.drawRect(10, 10, fieldComponentSize - 20, fieldComponentSize - 20);
    }

    // Draw cross.
    g.drawLine(fieldCenter,
               SOURCE_FIELD_COMPONENT_RADIUS,
               fieldCenter,
               fieldComponentSize - SOURCE_FIELD_COMPONENT_RADIUS);
    g.drawLine(SOURCE_FIELD_COMPONENT_RADIUS,
               fieldComponentSize_f / 2.0f,
               fieldComponentSize - SOURCE_FIELD_COMPONENT_RADIUS,
               fieldComponentSize_f / 2.0f);
}

void ElevationFieldComponent::drawBackground(Graphics & g) const
{
    auto const fieldComponentSize{ getWidth() };
    auto const fieldComponentSize_f{ static_cast<float>(fieldComponentSize) };
    auto * lookAndFeel{ dynamic_cast<GrisLookAndFeel *>(&getLookAndFeel()) };
    jassert(lookAndFeel != nullptr);

    g.setColour(lookAndFeel->getLightColour());
    g.drawVerticalLine(5, 5, fieldComponentSize_f - 5);
    g.drawHorizontalLine(fieldComponentSize - 5, 5, fieldComponentSize_f - 5);
}

//==============================================================================
Point<float> PositionFieldComponent::sourcePositionToComponentPosition(Point<float> const & sourcePosition) const
{
    jassert(getWidth() == getHeight());
    auto const effectiveWidth{ static_cast<float>(getWidth()) - SOURCE_FIELD_COMPONENT_DIAMETER };
    constexpr Point<float> sourceOffsetInComponent{ SOURCE_FIELD_COMPONENT_RADIUS, SOURCE_FIELD_COMPONENT_RADIUS };

    auto const result{ (sourcePosition + Point<float>{ 1.0f, 1.0f }) / 2.0f * effectiveWidth
                       + sourceOffsetInComponent };
    return result;
}

Point<float> PositionFieldComponent::componentPositionToSourcePosition(Point<float> const & componentPosition) const
{
    jassert(getWidth() == getHeight());
    auto const effectiveWidth{ static_cast<float>(getWidth()) - SOURCE_FIELD_COMPONENT_DIAMETER };
    constexpr Point<float> sourceOffsetInComponent{ SOURCE_FIELD_COMPONENT_RADIUS, SOURCE_FIELD_COMPONENT_RADIUS };

    auto const result{ (componentPosition - sourceOffsetInComponent) / effectiveWidth * 2.0f
                       - Point<float>{ 1.0f, 1.0f } };
    return result;
}

void PositionFieldComponent::notifySourcePositionChanged(SourceIndex const sourceIndex)
{
    mListeners.call([&](Listener & l) { l.fieldSourcePositionChanged(sourceIndex, 0); });
}

void PositionFieldComponent::rebuildSourceComponents(int numberOfSources)
{
    mSourceComponents.clearQuick(true);
    for (auto & source : *mSources) {
        mSourceComponents.add(new PositionSourceComponent{ *this, source });
        addAndMakeVisible(mSourceComponents.getLast());
    }
}

PositionFieldComponent::PositionFieldComponent(PositionAutomationManager & positionAutomationManager) noexcept
    : mAutomationManager(positionAutomationManager)
{
}

void PositionFieldComponent::setSpatMode(SpatMode const spatMode)
{
    mSpatMode = spatMode;
    repaint();
}

void PositionFieldComponent::drawDomeSpans(Graphics & g) const
{
    auto const width{ getWidth() };
    auto const halfWidth{ static_cast<float>(width) / 2.0f };
    Point<float> const fieldCenter{ halfWidth, halfWidth };
    auto const magnitude{ (width - SOURCE_FIELD_COMPONENT_DIAMETER) / 2.0f };

    for (auto const & source : *mSources) {
        auto const azimuth{ source.getAzimuth() };
        float const elevation{ source.getNormalizedElevation() };
        auto const azimuthSpan{ (Degrees{ 180.0f } * source.getAzimuthSpan()).getAsRadians() };
        float const elevationSpan{ source.getElevationSpan() };

        // Calculate min and max elevation in degrees.
        Range<float> const elevationLimits{ 0.0f, 1.0f };
        Range<float> const elevationRange{ elevation - elevationSpan, elevation + elevationSpan };
        auto const clippedElevationRange{ elevationRange.getIntersectionWith(elevationLimits) };

        Point<float> const lower_corner_a{ std::cos(azimuthSpan) * clippedElevationRange.getStart(),
                                           std::sin(azimuthSpan) * clippedElevationRange.getStart() };
        Point<float> const upper_corner_b{ std::cos(-azimuthSpan) * clippedElevationRange.getEnd(),
                                           std::sin(-azimuthSpan) * clippedElevationRange.getEnd() };

        // Draw the path
        Path path{};
        path.startNewSubPath(lower_corner_a);
        path.addCentredArc(
            0.0f,
            0.0f,
            clippedElevationRange.getStart(),
            clippedElevationRange.getStart(),
            0.0f,
            azimuthSpan
                + MathConstants<float>::halfPi, // addCentredArc counts radians from the top-center of the ellipse
            -azimuthSpan + MathConstants<float>::halfPi);
        path.lineTo(upper_corner_b); // lower right corner
        path.addCentredArc(0.0f,
                           0.0f,
                           clippedElevationRange.getEnd(),
                           clippedElevationRange.getEnd(),
                           0.0f,
                           -azimuthSpan + MathConstants<float>::halfPi,
                           azimuthSpan + MathConstants<float>::halfPi); // upper right corner
        path.closeSubPath();

        // rotate, scale and translate path
        auto const rotation{ azimuth - Degrees{ 90.0f } }; // correction for the way addCentredArc counts angles
        auto const transform{
            AffineTransform::rotation(rotation.getAsRadians()).scaled(magnitude).translated(fieldCenter)
        };
        path.applyTransform(transform);

        // draw
        g.setColour(source.getColour().withAlpha(0.1f));
        g.fillPath(path);
        g.setColour(source.getColour().withAlpha(0.5f));
        PathStrokeType strokeType = PathStrokeType(1.5);
        g.strokePath(path, strokeType);
    }
}

void PositionFieldComponent::drawCubeSpans(Graphics & g) const
{
    auto const width{ getWidth() };

    for (auto const & source : *mSources) {
        float const azimuthSpan{ width * source.getAzimuthSpan() };
        float const halfAzimuthSpan{ azimuthSpan / 2.0f - SOURCE_FIELD_COMPONENT_RADIUS };
        float const saturation{ (source.getIndex() == mSelectedSource) ? 1.0f : 0.5f };
        Point<float> const position{ sourcePositionToComponentPosition(source.getPos()) };

        g.setColour(source.getColour().withSaturation(saturation).withAlpha(0.5f));
        g.drawEllipse(position.x - halfAzimuthSpan, position.y - halfAzimuthSpan, azimuthSpan, azimuthSpan, 1.5f);
        g.setColour(source.getColour().withSaturation(saturation).withAlpha(0.1f));
        g.fillEllipse(position.x - halfAzimuthSpan, position.y - halfAzimuthSpan, azimuthSpan, azimuthSpan);
    }
}

void PositionFieldComponent::setCircularSourceSelectionWarning(bool const showCircularSourceSelectionWarning)
{
    mShowCircularSourceSelectionWarning = showCircularSourceSelectionWarning;
    repaint();
}

void PositionFieldComponent::paint(Graphics & g)
{
    int const width{ getWidth() };

    drawBackground(g);

    bool shouldDrawTrajectoryHandle{ false };
    if (mAutomationManager.getTrajectoryType() == PositionTrajectoryType::drawing && !mIsPlaying) {
        shouldDrawTrajectoryHandle = true;
    } else if (mAutomationManager.getTrajectoryType() == PositionTrajectoryType::realtime
               && mAutomationManager.getSourceLink() == PositionSourceLink::deltaLock) {
        shouldDrawTrajectoryHandle = true;
    }
    mTrajectoryHandleComponent->setVisible(shouldDrawTrajectoryHandle);

    // Draw recording trajectory path and current position dot.
    g.setColour(Colour::fromRGB(176, 176, 228));
    if (mLineDrawingAnchor1.has_value() && mLineDrawingAnchor2.has_value()) {
        Path lineDrawingPath;
        lineDrawingPath.startNewSubPath(*mLineDrawingAnchor1);
        lineDrawingPath.lineTo(*mLineDrawingAnchor2);
        lineDrawingPath.closeSubPath();
        g.strokePath(lineDrawingPath, PathStrokeType(.75f));
    }
    if (mAutomationManager.getTrajectory().has_value()) {
        auto const trajectoryPath{ mAutomationManager.getTrajectory()->createDrawablePath(getWidth()) };
        g.strokePath(trajectoryPath, PathStrokeType(.75f));
    }
    // position dot
    if (mIsPlaying && !isMouseButtonDown() && mAutomationManager.getTrajectoryType() != PositionTrajectoryType::realtime
        && mAutomationManager.getPositionActivateState()) {
        constexpr float radius = 4.0f;
        constexpr float diameter = radius * 2.0f;
        Point<float> const dotCenter{ sourcePositionToComponentPosition(
            mAutomationManager.getCurrentTrajectoryPoint()) };
        g.fillEllipse(dotCenter.getX() - radius, dotCenter.getY() - radius, diameter, diameter);
    }

    this->drawSpans(g);

    if (mShowCircularSourceSelectionWarning) {
        g.setColour(Colours::white);
        g.drawFittedText(WARNING_CIRCULAR_SOURCE_SELECTION,
                         juce::Rectangle<int>(0, 0, width, 50),
                         Justification(Justification::centred),
                         1);
    }
}

void PositionFieldComponent::drawSpans(Graphics & g) const
{
    if (mSpatMode == SpatMode::dome) {
        drawDomeSpans(g);
    } else {
        drawCubeSpans(g);
    }
}

void PositionFieldComponent::mouseDown(MouseEvent const & event)
{
    setSelectedSource(std::nullopt);

    if (mAutomationManager.getTrajectoryType() == PositionTrajectoryType::drawing) {
        auto const mousePosition{ event.getPosition() };
        auto const clippedPosition{ clipRecordingPosition(mousePosition).toFloat() };
        auto const position{ componentPositionToSourcePosition(clippedPosition) };
        auto const isShiftDown{ event.mods.isShiftDown() };

        mOldSelectedSource.reset();
        mAutomationManager.resetRecordingTrajectory(position);
        mTrajectoryHandleComponent->setCentrePosition(sourcePositionToComponentPosition(position).toInt());

        if (mLineDrawingAnchor1.has_value()) {
            auto const anchor1{ mLineDrawingAnchor1.value() };
            auto const anchor2{ position };
            auto const numSteps{ static_cast<int>(
                jmax(std::abs(anchor2.x - anchor1.x), std::abs(anchor2.y - anchor1.y))) };
            auto const xInc{ (anchor2.x - anchor1.x) / numSteps };
            auto const yInc{ (anchor2.y - anchor1.y) / numSteps };
            for (int i{ 1 }; i <= numSteps; ++i) {
                mAutomationManager.addRecordingPoint(Point<float>{ anchor1.x + xInc * i, anchor1.y + yInc * i });
            }
            if (isShiftDown) {
                mLineDrawingAnchor1 = anchor2;
                mLineDrawingAnchor2.reset();
            } else {
                mLineDrawingAnchor1.reset();
                mLineDrawingAnchor2.reset();
            }
        } else {
            if (isShiftDown) {
                mLineDrawingAnchor1 = position;
            }
        }
    }

    repaint();
}

void PositionFieldComponent::mouseDrag(const MouseEvent & event)
{
    if (mAutomationManager.getTrajectoryType() == PositionTrajectoryType::drawing) {
        auto const mousePosition{ event.getPosition() };
        mTrajectoryHandleComponent->setCentrePosition(mousePosition.getX(), mousePosition.getY());
        auto const clippedPosition{ clipRecordingPosition(mousePosition) };
        auto const relativePosition{ componentPositionToSourcePosition(clippedPosition.toFloat()) };

        if (mLineDrawingAnchor1.has_value()) {
            mLineDrawingAnchor2 = relativePosition;
        } else {
            mAutomationManager.addRecordingPoint(relativePosition);
        }
        repaint();
    }
}

void PositionFieldComponent::mouseUp(const MouseEvent & event)
{
    if (mAutomationManager.getTrajectoryType() == PositionTrajectoryType::drawing) {
        if (!event.mods.isShiftDown()) {
            mAutomationManager.addRecordingPoint(mAutomationManager.getLastRecordingPoint());
            repaint();
        }
    }
}

Point<int> PositionFieldComponent::clipRecordingPosition(Point<int> const & pos)
{
    // TODO: constrain with SOURCE_COMPONENT_RADIUS instead.

    constexpr int min{ TRAJECTORY_MARGINS };
    int const max{ getWidth() - min };
    Point<int> const clipped{ std::clamp(pos.x, min, max), std::clamp(pos.y, min, max) };

    return clipped;
}

//==============================================================================
void ElevationFieldComponent::drawSpans(Graphics & g) const
{
    jassert(getWidth() == getHeight());
    auto const componentSize{ this->getWidth() };

    float sourceIndex{};
    for (auto const & source : *mSources) {
        auto const lineThickness{ (source.getIndex() == mSelectedSource) ? 3 : 1 };
        float const saturation{ (source.getIndex() == mSelectedSource) ? 1.0f : 0.75f };
        float const x{ sourceIndex / static_cast<float>(mSources->size()) * (componentSize - 50.0f) + 50.0f };
        float const y{ (Degrees{ 90.0f } - source.getElevation()) / Degrees{ 90.0f } * (componentSize - 35.0f) + 5.0f };
        Point<float> const pos{ x, y };

        // draw Spans
        float const elevationSpan{ 50.0f * source.getElevationSpan() };
        g.setColour(source.getColour().withSaturation(saturation).withAlpha(0.5f));
        g.drawRect(pos.x + SOURCE_FIELD_COMPONENT_RADIUS - elevationSpan / 2,
                   pos.y + SOURCE_FIELD_COMPONENT_DIAMETER + lineThickness / 2,
                   elevationSpan,
                   componentSize - 5.0f,
                   1.5);
        g.setColour(source.getColour().withSaturation(saturation).withAlpha(0.1f));
        g.fillRect(pos.x + SOURCE_FIELD_COMPONENT_RADIUS - elevationSpan / 2,
                   pos.y + SOURCE_FIELD_COMPONENT_DIAMETER + lineThickness / 2,
                   elevationSpan,
                   componentSize - 5.0f);
        sourceIndex += 1.0f;
    }
}

void ElevationFieldComponent::paint(Graphics & g)
{
    drawBackground(g);

    bool shouldDrawTrajectoryHandle{ false };
    if (mSources->size() == 1) {
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
    // TODO
    //    if (shouldDrawTrajectoryHandle) {
    //        auto const position{ sourceElevationToComponentPosition(mSources[0].getElevation()) };
    //        auto const lineThickness{ mSelectedSource == TRAJECTORY_HANDLE_SOURCE_ID ? 3 : 1 };
    //        Rectangle<float> const rarea{ 10.0f,
    //                                      position.y,
    //                                      SOURCE_FIELD_COMPONENT_DIAMETER,
    //                                      SOURCE_FIELD_COMPONENT_DIAMETER };
    //
    //        g.setColour(Colour::fromRGB(176, 176, 228));
    //        g.drawLine(10 + SOURCE_FIELD_COMPONENT_RADIUS,
    //                   position.y + SOURCE_FIELD_COMPONENT_DIAMETER + lineThickness / 2.0f,
    //                   10 + SOURCE_FIELD_COMPONENT_RADIUS,
    //                   height - 5,
    //                   lineThickness);
    //        g.fillEllipse(rarea);
    //        g.setColour(Colour::fromRGB(64, 64, 128));
    //        g.drawEllipse(rarea, 1);
    //        g.setColour(Colours::white);
    //        g.drawFittedText(String("X"), rarea.getSmallestIntegerContainer(), Justification(Justification::centred),
    //        1);
    //    }

    // Draw recording trajectory path and current position dot.
    g.setColour(Colour::fromRGB(176, 176, 228));
    if (mAutomationManager.getTrajectory().has_value()) {
        auto const trajectoryPath{ mAutomationManager.getTrajectory()->createDrawablePath(getWidth()) };
        g.strokePath(trajectoryPath, PathStrokeType(.75f));
    }
    if (mIsPlaying && !isMouseButtonDown()
        && static_cast<ElevationTrajectoryType>(mAutomationManager.getTrajectoryType())
               != ElevationTrajectoryType::realtime
        && mAutomationManager.getPositionActivateState()) {
        Point<float> const dpos{ mAutomationManager.getCurrentTrajectoryPoint() };
        g.fillEllipse(dpos.x - 4, dpos.y - 4, 8, 8);
    }
}

void ElevationFieldComponent::mouseDown(const MouseEvent & event)
{
    auto const width{ getWidth() };
    auto const height{ getHeight() };

    mSelectedSource.reset();

    // Check if we click on a new source.
    //    bool clickOnSource{ false };
    //    for (int sourceIndex{}; sourceIndex < mNumberOfSources; ++sourceIndex) {
    //        float const x{ static_cast<float>(sourceIndex) / mNumberOfSources * (width - 50.0f) + 50.0f };
    //        float const y{ (Degrees{ 90.0f } - mSources[sourceIndex].getElevation()) / Degrees{ 90.0f } * (height
    //        - 35.0f)
    //                       + 5.0f };
    //        Point<float> const pos{ x, y };
    //        Rectangle<float> const area{ pos.x, pos.y, SOURCE_FIELD_COMPONENT_DIAMETER,
    //        SOURCE_FIELD_COMPONENT_DIAMETER }; if (area.contains(event.getMouseDownPosition().toFloat())) {
    //            mSelectedSource = sourceIndex;
    //            mListeners.call([&](Listener & l) { l.fieldSourcePositionChanged(sourceIndex, 1); });
    //            clickOnSource = true;
    //            break;
    //        }
    //    }

    //    if (clickOnSource) {
    //        repaint();
    //        return;
    //    }

    // Check if we record a trajectory.
    // TODO

    //    if (static_cast<ElevationTrajectoryType>(mAutomationManager.getTrajectoryType()) ==
    //    ElevationTrajectoryType::drawing
    //        || static_cast<ElevationTrajectoryType>(mAutomationManager.getTrajectoryType())
    //               == ElevationTrajectoryType::realtime) {
    //        auto const position{
    //            sourcePositionToComponentPosition(mAutomationManager.getTrajectoryHandle().getPos()).withX(10.0f)
    //        };
    //        Rectangle<float> const area{ position.x,
    //                                     position.y,
    //                                     SOURCE_FIELD_COMPONENT_DIAMETER,
    //                                     SOURCE_FIELD_COMPONENT_DIAMETER };
    //        if (area.contains(event.getMouseDownPosition().toFloat())) {
    //            mOldSelectedSource = mSelectedSource;
    //            mSelectedSource = TRAJECTORY_HANDLE_SOURCE_ID;
    //            if (static_cast<ElevationTrajectoryType>(mAutomationManager.getTrajectoryType())
    //                == ElevationTrajectoryType::drawing) {
    //                mCurrentRecordingPositionX = 10 + SOURCE_FIELD_COMPONENT_RADIUS;
    //                mAutomationManager.resetRecordingTrajectory(event.getMouseDownPosition().toFloat());
    //            } else {
    //                mListeners.call([&](Listener & l) { l.fieldTrajectoryHandleClicked(1); });
    //            }
    //            repaint();
    //            return;
    //        }
    //    }
    //
    //    // If clicked in an empty space while in mode DRAWING, start a new drawing.
    //    if (static_cast<ElevationTrajectoryType>(mAutomationManager.getTrajectoryType())
    //        == ElevationTrajectoryType::drawing) {
    //        mOldSelectedSource = mSelectedSource;
    //        mSelectedSource = TRAJECTORY_HANDLE_SOURCE_ID;
    //        mCurrentRecordingPositionX = 10 + static_cast<int>(SOURCE_FIELD_COMPONENT_RADIUS);
    //        mAutomationManager.resetRecordingTrajectory(
    //            Point<float>{ static_cast<float>(mCurrentRecordingPositionX), event.getMouseDownPosition().toFloat().y
    //            });
    //        repaint();
    //    }
}

void ElevationFieldComponent::notifySourcePositionChanged(SourceIndex const sourceIndex)
{
    mListeners.call([&](Listener & l) { l.fieldSourcePositionChanged(sourceIndex, 1); });
}

void ElevationFieldComponent::mouseDrag(const MouseEvent & event)
{
    auto const height{ static_cast<float>(getHeight()) };

    // No selection.
    if (!mSelectedSource.has_value()) {
        return;
    }
    auto const selectedSourceId{ mSelectedSource.value() };

    //    if (mSelectedSource == TRAJECTORY_HANDLE_SOURCE_ID) {
    //        if (mAutomationManager.getTrajectoryType() == ElevationTrajectoryType::drawing) {
    //            mCurrentRecordingPositionX += 1;
    //            if (mCurrentRecordingPositionX >= height) {
    //                mCurrentRecordingPositionX = height;
    //                mAutomationManager.compressTrajectoryXValues(height);
    //            }
    //            float y{ event.getPosition().toFloat().y };
    //            y = std::clamp(y, 15.0f, height - 20.0f);
    //            mAutomationManager.addRecordingPoint(Point<float>{ static_cast<float>(mCurrentRecordingPositionX), y
    //            }); y = height - event.getPosition().toFloat().y; y = std::clamp(y, 15.0f, height - 20.0f);
    //            mAutomationManager.getTrajectoryHandle().setPos(
    //                componentPositionToSourcePosition(Point<float>{ 10.0f, y }));
    //        } else if (mAutomationManager.getTrajectoryType() == ElevationTrajectoryType::realtime) {
    //            float y{ height - event.y };
    //            y = std::clamp(y, 15.0f, height - 20.0f);
    //            mAutomationManager.getTrajectoryHandle().setPos(
    //                componentPositionToSourcePosition(Point<float>{ 10.0f, y }));
    //            mAutomationManager.sendTrajectoryPositionChangedEvent();
    //        }
    //    } else {
    //    Degrees const elevation{ (height - event.y - SOURCE_FIELD_COMPONENT_DIAMETER) / (height - 35.0f) * 90.0f };
    //    mSources[selectedSourceId].setElevation(elevation);
    //    mListeners.call([&](Listener & l) { l.fieldSourcePositionChanged(selectedSourceId, 1); });
    //    }

    //    bool needToAdjustAutomationManager{ false };
    //    if (static_cast<ElevationSourceLink>(mAutomationManager.getSourceLink()) == ElevationSourceLink::independent
    //        && mSelectedSource == 0
    //        && static_cast<ElevationTrajectoryType>(mAutomationManager.getTrajectoryType())
    //               == ElevationTrajectoryType::realtime) {
    //        needToAdjustAutomationManager = true;
    //    } else if (static_cast<ElevationSourceLink>(mAutomationManager.getSourceLink())
    //                   >= ElevationSourceLink::fixedElevation
    //               && static_cast<ElevationSourceLink>(mAutomationManager.getSourceLink()) <
    //               ElevationSourceLink::deltaLock
    //               && static_cast<ElevationTrajectoryType>(mAutomationManager.getTrajectoryType())
    //                      == ElevationTrajectoryType::realtime) {
    //        needToAdjustAutomationManager = true;
    //    }
    //
    //    if (needToAdjustAutomationManager) {
    //        float const y{ mSources[0].getElevation() / Degrees{ 90.0f } * (height - 15.0f) + 5.0f };
    //        mAutomationManager.getTrajectoryHandle().setPos(componentPositionToSourcePosition(Point<float>{ 10.0f, y
    //        })); mAutomationManager.sendTrajectoryPositionChangedEvent();
    //    }

    repaint();
}

void ElevationFieldComponent::mouseUp(const MouseEvent & event)
{
    if (mCurrentlyDrawing) {
        if (mAutomationManager.getTrajectoryType() == ElevationTrajectoryType::drawing) {
            mAutomationManager.addRecordingPoint(mAutomationManager.getLastRecordingPoint());
            mSelectedSource = mOldSelectedSource;
        }
        repaint();
    }
}

void ElevationFieldComponent::rebuildSourceComponents(int numberOfSources)
{
    mSourceComponents.clearQuick(true);
    for (auto & source : *mSources) {
        mSourceComponents.add(new ElevationSourceComponent{ *this, source });
        addAndMakeVisible(mSourceComponents.getLast());
    }
}

Point<float> ElevationFieldComponent::sourceElevationToComponentPosition(Radians const sourceElevation,
                                                                         SourceIndex const index) const
{
    auto const availableWidth{ getWidth() - (xPadding * 2) };
    auto const widthPerSource{ availableWidth / static_cast<float>(mSources->size()) };
    auto const height{ getHeight() - yTopPadding - yBottomPadding };

    Point<float> const result{ widthPerSource * index.toInt() + widthPerSource / 2.0f,
                               sourceElevation / Degrees{ 90.0f } * height };

    return result;
}

Radians ElevationFieldComponent::componentPositionToSourceElevation(Point<float> const & componentPosition) const
{
    auto const height{ getHeight() - yTopPadding - yBottomPadding };

    Radians const result{ Degrees{ 90.0f } * ((componentPosition.getY() - yTopPadding) / height) };
    return result;
}