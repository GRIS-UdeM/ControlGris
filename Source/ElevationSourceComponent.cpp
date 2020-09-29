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

#include "ElevationSourceComponent.h"

#include "FieldComponent.h"
#include "PluginProcessor.h"
#include "Source.h"

//==============================================================================
ElevationSourceComponent::ElevationSourceComponent(ElevationFieldComponent & fieldComponent, Source & source) noexcept
    : SourceComponent(source.getColour(), source.getId().toString())
    , mFieldComponent(fieldComponent)
    , mAutomationManager(fieldComponent.getAutomationManager())
    , mSource(source)
{
    source.addGuiChangeListener(this);
    updatePositionInParent();
}

//==============================================================================
ElevationSourceComponent::~ElevationSourceComponent() noexcept
{
    mSource.removeGuiChangeListener(this);
}

//==============================================================================
void ElevationSourceComponent::updatePositionInParent()
{
    auto const newCenter{
        mFieldComponent.sourceElevationToComponentPosition(mSource.getElevation(), mSource.getIndex()).roundToInt()
    };
    setCentrePosition(newCenter.getX(), newCenter.getY());
}

//==============================================================================
void ElevationSourceComponent::sourceMoved([[maybe_unused]] Source & source,
                                           [[maybe_unused]] SourceLinkBehavior const sourceLinkBehavior)
{
    jassert(&source == &mSource);
    jassert(sourceLinkBehavior == SourceLinkBehavior::doNothing);
    updatePositionInParent();
}

//==============================================================================
void ElevationSourceComponent::mouseDown(MouseEvent const & event)
{
    if (mSource.isPrimarySource()) {
        mAutomationManager.getProcessor().getChangeGestureManager().beginGesture(Automation::Ids::Z);
    }
    setSourcePosition(event);
    mFieldComponent.setSelectedSource(mSource.getIndex());
}

//==============================================================================
void ElevationSourceComponent::setSourcePosition(MouseEvent const & event)
{
    auto const eventRelativeToFieldComponent{ event.getEventRelativeTo(&mFieldComponent) };
    auto const newElevation{ mFieldComponent.componentPositionToSourceElevation(
        eventRelativeToFieldComponent.getPosition().toFloat()) };
    mSource.setElevation(newElevation, SourceLinkBehavior::moveSourceAnchor);

    if (mSource.isPrimarySource()) {
        mAutomationManager.sendTrajectoryPositionChangedEvent();
    }
    mFieldComponent.notifySourcePositionChanged(mSource.getIndex());
}

//==============================================================================
void ElevationSourceComponent::mouseDrag(MouseEvent const & event)
{
    if (mFieldComponent.getSelectedSourceIndex() == mSource.getIndex()) {
        jassert(mFieldComponent.getWidth() == mFieldComponent.getHeight());
        this->setSourcePosition(event);
    }
}

//==============================================================================
void ElevationSourceComponent::mouseUp(MouseEvent const & event)
{
    mouseDrag(event);
    if (mSource.isPrimarySource()) {
        mAutomationManager.getProcessor().getChangeGestureManager().endGesture(Automation::Ids::Z);
    }
}

//==============================================================================
SourceIndex ElevationSourceComponent::getSourceIndex() const
{
    return mSource.getIndex();
}
