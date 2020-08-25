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

#include "PositionSourceComponent.h"

#include "FieldComponent.h"
#include "PluginProcessor.h"
#include "Source.h"

//==============================================================================
PositionSourceComponent::PositionSourceComponent(PositionFieldComponent & fieldComponent, Source & source)
    : SourceComponent(source.getColour(), source.getId().toString())
    , mFieldComponent(fieldComponent)
    , mAutomationManager(fieldComponent.getAutomationManager())
    , mSource(source)
{
    source.addGuiChangeListener(this);
    this->updatePositionInParent();
}

//==============================================================================
PositionSourceComponent::~PositionSourceComponent()
{
    mSource.removeGuiChangeListener(this);
}

//==============================================================================
void PositionSourceComponent::updatePositionInParent()
{
    auto const newCenter{ mFieldComponent.sourcePositionToComponentPosition(mSource.getPos()).roundToInt() };
    this->setCentrePosition(newCenter.getX(), newCenter.getY());
}

//==============================================================================
void PositionSourceComponent::mouseDown(MouseEvent const & event)
{
    if (mSource.isPrimarySource()) {
        mAutomationManager.getProcessor().getChangeGestureManager().beginGesture(Automation::Ids::x);
        mAutomationManager.getProcessor().getChangeGestureManager().beginGesture(Automation::Ids::y);
    }
    this->setSourcePosition(event);
    mFieldComponent.setSelectedSource(mSource.getIndex());
}

//==============================================================================
void PositionSourceComponent::setSourcePosition(MouseEvent const & event)
{
    jassert(mFieldComponent.getWidth() == mFieldComponent.getHeight());

    auto const eventRelativeToFieldComponent{ event.getEventRelativeTo(&mFieldComponent) };
    auto const newPosition{ mFieldComponent.componentPositionToSourcePosition(
        eventRelativeToFieldComponent.getPosition().toFloat()) };
    mSource.setPos(newPosition, SourceLinkNotification::notify);

    if (mSource.isPrimarySource()) {
        mAutomationManager.sendTrajectoryPositionChangedEvent();
    }
    mFieldComponent.notifySourcePositionChanged(mSource.getIndex());
}

//==============================================================================
void PositionSourceComponent::mouseDrag(MouseEvent const & event)
{
    if (mFieldComponent.getSelectedSourceIndex() == mSource.getIndex()) {
        jassert(mFieldComponent.getWidth() == mFieldComponent.getHeight());
        this->setSourcePosition(event);
    }
}

//==============================================================================
void PositionSourceComponent::mouseUp(MouseEvent const & event)
{
    mouseDrag(event);
    if (mSource.isPrimarySource()) {
        mAutomationManager.getProcessor().getChangeGestureManager().endGesture(Automation::Ids::x);
        mAutomationManager.getProcessor().getChangeGestureManager().endGesture(Automation::Ids::y);
    }
}

//==============================================================================
SourceIndex PositionSourceComponent::getSourceIndex() const
{
    return mSource.getIndex();
}

//==============================================================================
void PositionSourceComponent::changeListenerCallback([[maybe_unused]] ChangeBroadcaster * source)
{
    this->updatePositionInParent();
}
