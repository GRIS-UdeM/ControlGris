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

#include "cg_PositionSourceComponent.hpp"

#include "cg_ControlGrisAudioProcessor.hpp"
#include "cg_FieldComponent.hpp"
#include "cg_Source.hpp"

namespace gris
{
//==============================================================================
PositionSourceComponent::PositionSourceComponent(PositionFieldComponent & fieldComponent, Source & source)
    : SourceComponent(source.getColour(), source.getId().toString())
    , mFieldComponent(fieldComponent)
    , mTrajectoryManager(fieldComponent.getAutomationManager())
    , mSource(source)
{
    source.addGuiListener(this);
    updatePositionInParent();
}

//==============================================================================
PositionSourceComponent::~PositionSourceComponent()
{
    mSource.removeGuiListener(this);
}

//==============================================================================
void PositionSourceComponent::updatePositionInParent()
{
    auto const newCenter{ mFieldComponent.sourcePositionToComponentPosition(mSource.getPos()).roundToInt() };
    setCentrePosition(newCenter.getX(), newCenter.getY());
}

//==============================================================================
void PositionSourceComponent::sourceMovedCallback()
{
    updatePositionInParent();
}

//==============================================================================
void PositionSourceComponent::mouseDown(juce::MouseEvent const & event)
{
    mDisplacementMode = getDisplacementMode(event);
    mCanDrag = isMoveAllowed(mDisplacementMode, mSource.isPrimarySource(), mTrajectoryManager.getSourceLink());
    if (mCanDrag) {
        if (mSource.isPrimarySource() || mDisplacementMode == DisplacementMode::all) {
            mTrajectoryManager.getProcessor().getChangeGestureManager().beginGesture(Automation::Ids::X);
            mTrajectoryManager.getProcessor().getChangeGestureManager().beginGesture(Automation::Ids::Y);
        }
        setSourcePosition(event);
    } else {
        mFieldComponent.displayInvalidSourceMoveWarning(true);
    }
}

//==============================================================================
void PositionSourceComponent::setSourcePosition(juce::MouseEvent const & event) const
{
    jassert(mFieldComponent.getWidth() == mFieldComponent.getHeight());

    auto const origin{ mDisplacementMode == DisplacementMode::selectedSourceOnly
                           ? Source::OriginOfChange::userAnchorMove
                           : Source::OriginOfChange::userMove };
    auto const eventRelativeToFieldComponent{ event.getEventRelativeTo(&mFieldComponent) };
    auto const newPosition{ mFieldComponent.componentPositionToSourcePosition(
        eventRelativeToFieldComponent.getPosition().toFloat()) };
    mSource.setPosition(newPosition, origin);
}

//==============================================================================
void PositionSourceComponent::mouseDrag(juce::MouseEvent const & event)
{
    if (mCanDrag && mFieldComponent.getSelectedSourceIndex() == mSource.getIndex()) {
        jassert(mFieldComponent.getWidth() == mFieldComponent.getHeight());
        setSourcePosition(event);
    }
}

//==============================================================================
void PositionSourceComponent::mouseUp(juce::MouseEvent const & event)
{
    if (mCanDrag) {
        mouseDrag(event);
        if (mSource.isPrimarySource() || mDisplacementMode == DisplacementMode::all) {
            mTrajectoryManager.getProcessor().getChangeGestureManager().endGesture(Automation::Ids::X);
            mTrajectoryManager.getProcessor().getChangeGestureManager().endGesture(Automation::Ids::Y);
        }
    } else {
        mFieldComponent.displayInvalidSourceMoveWarning(false);
    }
}

//==============================================================================
SourceIndex PositionSourceComponent::getSourceIndex() const
{
    return mSource.getIndex();
}

} // namespace gris