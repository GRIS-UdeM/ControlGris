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

FieldComponent::FieldComponent(Sources & sources) noexcept : mSources(sources)
{
    setSize(MIN_FIELD_WIDTH, MIN_FIELD_WIDTH);
}

void FieldComponent::setSelectedSource(std::optional<SourceIndex> const selectedSource)
{
    mOldSelectedSource = mSelectedSource;
    mSelectedSource = selectedSource;
    applySourceSelectionToComponents();
}

void FieldComponent::refreshSources()
{
    mSelectedSource.reset();
    mOldSelectedSource.reset();

    rebuildSourceComponents(mSources.size());
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

    drawBackgroundGrid(g);

    g.setColour(lookAndFeel->getLightColour());
    g.drawVerticalLine(5, 5, fieldComponentSize_f - 5);
    g.drawHorizontalLine(fieldComponentSize - 5, 5, fieldComponentSize_f - 5);
}

//==============================================================================
Point<float> PositionFieldComponent::sourcePositionToComponentPosition(Point<float> const & sourcePosition) const
{
    auto const effectiveArea{ getEffectiveArea() };
    auto const normalizedPosition{ (sourcePosition + Point<float>{ 1.0f, 1.0f }) / 2.0f };
    auto const result{ normalizedPosition * effectiveArea.getWidth() + effectiveArea.getPosition() };
    return result;
}

Point<float> PositionFieldComponent::componentPositionToSourcePosition(Point<float> const & componentPosition) const
{
    auto const effectiveArea{ getEffectiveArea() };
    auto const normalizedPosition{ (componentPosition - effectiveArea.getPosition()) / effectiveArea.getWidth() };
    auto const result{ normalizedPosition * 2.0f - Point<float>{ 1.0f, 1.0f } };
    return result;
}

Rectangle<float> PositionFieldComponent::getEffectiveArea() const
{
    jassert(getWidth() == getHeight());

    auto const componentSize{ static_cast<float>(getWidth()) };
    auto const effectiveSize{ componentSize - SOURCE_FIELD_COMPONENT_RADIUS * 2.0f };

    Rectangle<float> const result{ SOURCE_FIELD_COMPONENT_RADIUS,
                                   SOURCE_FIELD_COMPONENT_RADIUS,
                                   effectiveSize,
                                   effectiveSize };
    return result;
}

void PositionFieldComponent::notifySourcePositionChanged(SourceIndex const sourceIndex)
{
    mListeners.call([&](Listener & l) { l.fieldSourcePositionChanged(sourceIndex, 0); });
}

void PositionFieldComponent::rebuildSourceComponents(int numberOfSources)
{
    mSourceComponents.clearQuick(true);
    for (auto & source : mSources) {
        mSourceComponents.add(new PositionSourceComponent{ *this, source });
        addAndMakeVisible(mSourceComponents.getLast());
    }
}

PositionFieldComponent::PositionFieldComponent(Sources & sources,
                                               PositionAutomationManager & positionAutomationManager) noexcept
    : FieldComponent(sources)
    , mAutomationManager(positionAutomationManager)
{
    mDrawingHandleComponent.setInterceptsMouseClicks(false, false);
    mDrawingHandleComponent.setCentrePosition(getWidth() / 2, getHeight() / 2);
    addAndMakeVisible(mDrawingHandleComponent);
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

    for (auto const & source : mSources) {
        auto const azimuth{ source.getAzimuth() };
        auto const elevation{ source.getNormalizedElevation() };
        auto const azimuthSpan{ (Degrees{ 180.0f } * source.getAzimuthSpan().toFloat()).getAsRadians() };
        auto const elevationSpan{ source.getElevationSpan() };

        // Calculate min and max elevation in degrees.
        Range<float> const elevationLimits{ 0.0f, 1.0f };
        Range<float> const elevationRange{ (elevation - elevationSpan).toFloat(),
                                           (elevation + elevationSpan).toFloat() };
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
    constexpr float MIN_SPAN_WIDTH = SOURCE_FIELD_COMPONENT_DIAMETER;
    constexpr float MAGIC_MAX_SPAN_RATIO
        = 0.8f; // TODO : there is probably some reasonning behind this value in SpatGRIS2 source.

    auto const effectiveWidth{ getEffectiveArea().getWidth() };

    for (auto const & source : mSources) {
        float const azimuthSpan{ effectiveWidth * source.getAzimuthSpan().toFloat() * MAGIC_MAX_SPAN_RATIO
                                 + MIN_SPAN_WIDTH };
        float const halfAzimuthSpan{ azimuthSpan / 2.0f };
        float const saturation{ (source.getIndex() == mSelectedSource) ? 1.0f : 0.5f };
        Point<float> const center{ sourcePositionToComponentPosition(source.getPos()) };
        Rectangle<float> const area{ center.getX() - halfAzimuthSpan,
                                     center.getY() - halfAzimuthSpan,
                                     azimuthSpan,
                                     azimuthSpan };

        g.setColour(source.getColour().withSaturation(saturation).withAlpha(0.5f));
        g.drawEllipse(area, 1.5f);
        g.setColour(source.getColour().withSaturation(saturation).withAlpha(0.1f));
        g.fillEllipse(area);
    }
}

void PositionFieldComponent::setCircularSourceSelectionWarning(bool const showCircularSourceSelectionWarning)
{
    mShowCircularSourceSelectionWarning = showCircularSourceSelectionWarning;
    repaint();
}

void PositionFieldComponent::paint(Graphics & g)
{
    int const componentSize{ getWidth() };

    drawBackground(g);

    mDrawingHandleComponent.setVisible(mAutomationManager.getTrajectoryType() == PositionTrajectoryType::drawing
                                       && !mIsPlaying);

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
        auto const trajectoryPath{ mAutomationManager.getTrajectory()->getDrawablePath(getEffectiveArea(), mSpatMode) };
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
                         juce::Rectangle<int>(0, 0, componentSize, 50),
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
        auto const mousePosition{ event.getPosition().toFloat() };
        auto const unclippedPosition{ componentPositionToSourcePosition(mousePosition) };
        auto const position{ Source::clipPosition(unclippedPosition, mSpatMode) };
        auto const isShiftDown{ event.mods.isShiftDown() };

        mOldSelectedSource.reset();
        mAutomationManager.resetRecordingTrajectory(position);
        mSources.getPrimarySource().setPos(position, SourceLinkNotification::notify);
        mDrawingHandleComponent.setCentrePosition(sourcePositionToComponentPosition(position).toInt());

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
        mDrawingHandleComponent.setCentrePosition(mousePosition.getX(), mousePosition.getY());
        auto const unclippedPosition{ componentPositionToSourcePosition(mousePosition.toFloat()) };
        auto const position{ Source::clipPosition(unclippedPosition, mSpatMode) };

        if (mLineDrawingAnchor1.has_value()) {
            mLineDrawingAnchor2 = position;
        } else {
            mAutomationManager.addRecordingPoint(position);
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

//==============================================================================
void ElevationFieldComponent::drawSpans(Graphics & g) const
{
    jassert(getWidth() == getHeight());
    auto const componentSize{ static_cast<float>(getWidth()) };
    auto const effectiveArea{ getEffectiveArea() };

    float sourceIndex{};
    for (auto const & source : mSources) {
        auto const lineThickness{ (source.getIndex() == mSelectedSource) ? 3 : 1 };
        auto const saturation{ (source.getIndex() == mSelectedSource) ? 1.0f : 0.75f };
        auto const position{ sourceElevationToComponentPosition(source.getElevation(), source.getIndex()) };
        constexpr auto anchorThickness = 5;
        auto const halfSpanHeight{ source.getElevationSpan().toFloat() * effectiveArea.getHeight() };
        auto const spanHeight{ halfSpanHeight * 2.0f };
        Line<float> anchor{ position, position.translated(0, componentSize) };
        Rectangle<float> unclippedSpanArea{ position.getX() - SOURCE_FIELD_COMPONENT_RADIUS,
                                            position.getY() - halfSpanHeight,
                                            SOURCE_FIELD_COMPONENT_DIAMETER,
                                            spanHeight };
        auto const spanArea{ unclippedSpanArea.getIntersection(effectiveArea) };

        // draw Spans
        g.setColour(source.getColour().withSaturation(saturation).withAlpha(0.5f));
        g.drawLine(anchor, anchorThickness);
        g.drawRect(spanArea, lineThickness);
        g.setColour(source.getColour().withSaturation(saturation).withAlpha(0.1f));
        g.fillRect(spanArea);
        sourceIndex += 1.0f;
    }
}

ElevationFieldComponent::ElevationFieldComponent(Sources & sources,
                                                 ElevationAutomationManager & automationManager) noexcept
    : FieldComponent(sources)
    , mAutomationManager(automationManager)
{
    mDrawingHandle.setCentrePosition(sourceElevationToComponentPosition(Radians{ 0.0f }, SourceIndex{ -1 }).toInt());
    addAndMakeVisible(mDrawingHandle);
}

void ElevationFieldComponent::paint(Graphics & g)
{
    drawBackground(g);
    drawSpans(g);

    auto const trajectoryType{ mAutomationManager.getTrajectoryType() };

    mDrawingHandle.setVisible(trajectoryType == ElevationTrajectoryType::drawing && !mIsPlaying);

    // Draw recording trajectory path and current position dot.
    g.setColour(Colour::fromRGB(176, 176, 228));
    if (trajectoryType == ElevationTrajectoryType::drawing) {
        auto const trajectoryPath{ mAutomationManager.getDrawingTrajectory().getDrawablePath(getEffectiveArea()) };
        g.strokePath(trajectoryPath, PathStrokeType{ 0.75f });
    } else if (mAutomationManager.getTrajectory().has_value()) {
        auto const trajectoryPath{ mAutomationManager.getTrajectory()->getDrawablePath(
            getEffectiveArea(),
            mSources.getPrimarySource().getSpatMode()) };
        g.strokePath(trajectoryPath, PathStrokeType{ .75f });
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
    mSelectedSource.reset();

    setSelectedSource(std::nullopt);

    if (mAutomationManager.getTrajectoryType() == ElevationTrajectoryType::drawing) {
        auto const mousePosition{ event.getPosition().toFloat() };
        auto const elevation{ componentPositionToSourceElevation(mousePosition) };

        mOldSelectedSource.reset();
        //        mAutomationManager.resetRecordingTrajectory(position);
        //        mTrajectoryHandleComponent->setCentrePosition(sourcePositionToComponentPosition(position).toInt());
        //
        //        if (mLineDrawingAnchor1.has_value()) {
        //            auto const anchor1{ mLineDrawingAnchor1.value() };
        //            auto const anchor2{ position };
        //            auto const numSteps{ static_cast<int>(
        //                jmax(std::abs(anchor2.x - anchor1.x), std::abs(anchor2.y - anchor1.y))) };
        //            auto const xInc{ (anchor2.x - anchor1.x) / numSteps };
        //            auto const yInc{ (anchor2.y - anchor1.y) / numSteps };
        //            for (int i{ 1 }; i <= numSteps; ++i) {
        //                mAutomationManager.addRecordingPoint(Point<float>{ anchor1.x + xInc * i, anchor1.y + yInc * i
        //                });
        //            }
        //            if (isShiftDown) {
        //                mLineDrawingAnchor1 = anchor2;
        //                mLineDrawingAnchor2.reset();
        //            } else {
        //                mLineDrawingAnchor1.reset();
        //                mLineDrawingAnchor2.reset();
        //            }
        //        } else {
        //            if (isShiftDown) {
        //                mLineDrawingAnchor1 = position;
        //            }
        //        }
    }

    repaint();
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
    for (auto & source : mSources) {
        mSourceComponents.add(new ElevationSourceComponent{ *this, source });
        addAndMakeVisible(mSourceComponents.getLast());
    }
}

Point<float> ElevationFieldComponent::sourceElevationToComponentPosition(Radians const sourceElevation,
                                                                         SourceIndex const index) const
{
    auto const availableWidth{ static_cast<float>(getWidth()) - LEFT_PADDING - RIGHT_PADDING };
    auto const availableHeight{ static_cast<float>(getHeight()) - TOP_PADDING - BOTTOM_PADDING };
    auto const widthBetweenEachSource{ availableWidth / static_cast<float>(mSources.size() + 1) };

    auto const x{
        LEFT_PADDING + widthBetweenEachSource * (static_cast<float>(index.toInt() + 1))
    }; // We add +1 to the index for the drawing handle.
    auto const clippedElevation{ sourceElevation.clamp(MIN_ELEVATION, MAX_ELEVATION) };
    auto const y{ sourceElevation / MAX_ELEVATION * availableHeight + TOP_PADDING };
    Point<float> const result{ x, y };

    return result;
}

Radians ElevationFieldComponent::componentPositionToSourceElevation(Point<float> const & componentPosition) const
{
    auto const effectiveHeight{ static_cast<float>(getHeight()) - TOP_PADDING - BOTTOM_PADDING };

    Radians const elevation{ MAX_ELEVATION * ((componentPosition.getY() - TOP_PADDING) / effectiveHeight) };
    auto const result{ elevation.clamp(MIN_ELEVATION, MAX_ELEVATION) };

    return result;
}

Rectangle<float> ElevationFieldComponent::getEffectiveArea() const
{
    jassert(getWidth() == getHeight());

    auto const componentSize{ static_cast<float>(getWidth()) };
    auto const effectiveWidth{ componentSize - LEFT_PADDING - RIGHT_PADDING };
    auto const effectiveHeight{ componentSize - TOP_PADDING - BOTTOM_PADDING };

    Rectangle<float> const result{ LEFT_PADDING, TOP_PADDING, effectiveWidth, effectiveHeight };
    return result;
}