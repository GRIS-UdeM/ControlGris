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
    updatePositionInParent();
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
    setCentrePosition(newCenter.getX(), newCenter.getY());
}

//==============================================================================
void PositionSourceComponent::sourceMoved([[maybe_unused]] Source & source,
                                          [[maybe_unused]] SourceLinkBehavior const sourceLinkBehavior)
{
    jassert(&source == &mSource);
    jassert(sourceLinkBehavior == SourceLinkBehavior::doNothing);
    updatePositionInParent();
}

//==============================================================================
void PositionSourceComponent::mouseDown(MouseEvent const & event)
{
    setDisplacementMode(event);
    if (mSource.isPrimarySource()) {
        mAutomationManager.getProcessor().getChangeGestureManager().beginGesture(Automation::Ids::X);
        mAutomationManager.getProcessor().getChangeGestureManager().beginGesture(Automation::Ids::Y);
    }
    setSourcePosition(event);
    mFieldComponent.setSelectedSource(mSource.getIndex());
}

//==============================================================================
void PositionSourceComponent::setSourcePosition(MouseEvent const & event) const
{
    jassert(mFieldComponent.getWidth() == mFieldComponent.getHeight());

    auto const sourceLinkBehavior{ mDisplacementMode == DisplacementMode::selectedSourceOnly
                                       ? SourceLinkBehavior::moveSourceAnchor
                                       : SourceLinkBehavior::moveAllSources };
    auto const eventRelativeToFieldComponent{ event.getEventRelativeTo(&mFieldComponent) };
    auto const newPosition{ mFieldComponent.componentPositionToSourcePosition(
        eventRelativeToFieldComponent.getPosition().toFloat()) };
    mSource.setPosition(newPosition, sourceLinkBehavior);

    if (mSource.isPrimarySource()) {
        mAutomationManager.sendTrajectoryPositionChangedEvent();
    }
    mFieldComponent.notifySourcePositionChanged(mSource.getIndex());
}

//==============================================================================
void PositionSourceComponent::setDisplacementMode(MouseEvent const & event)
{
    auto const pressedModifierKey{ event.mods.getRawFlags() & DISPLACEMENT_MODIFIER };
    mDisplacementMode = pressedModifierKey ? DisplacementMode::selectedSourceOnly : DisplacementMode::all;
}

//==============================================================================
void PositionSourceComponent::mouseDrag(MouseEvent const & event)
{
    if (mFieldComponent.getSelectedSourceIndex() == mSource.getIndex()) {
        jassert(mFieldComponent.getWidth() == mFieldComponent.getHeight());
        setSourcePosition(event);
    }
}

//==============================================================================
void PositionSourceComponent::mouseUp(MouseEvent const & event)
{
    mouseDrag(event);
    if (mSource.isPrimarySource()) {
        mAutomationManager.getProcessor().getChangeGestureManager().endGesture(Automation::Ids::X);
        mAutomationManager.getProcessor().getChangeGestureManager().endGesture(Automation::Ids::Y);
    }
}

//==============================================================================
SourceIndex PositionSourceComponent::getSourceIndex() const
{
    return mSource.getIndex();
}
