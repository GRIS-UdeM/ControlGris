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

#include "cg_ElevationSourceComponent.hpp"

#include "cg_ControlGrisAudioProcessor.hpp"
#include "cg_FieldComponent.hpp"
#include "cg_Source.hpp"

namespace gris
{
//==============================================================================
ElevationSourceComponent::ElevationSourceComponent(ElevationFieldComponent & fieldComponent, Source & source) noexcept
    : SourceComponent(source.getColour(), source.getId().toString())
    , mFieldComponent(fieldComponent)
    , mTrajectoryManager(fieldComponent.getAutomationManager())
    , mSource(source)
{
    source.addGuiListener(this);
    updatePositionInParent();
}

//==============================================================================
ElevationSourceComponent::~ElevationSourceComponent() noexcept
{
    mSource.removeGuiListener(this);
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
void ElevationSourceComponent::sourceMovedCallback()
{
    updatePositionInParent();
}

//==============================================================================
void ElevationSourceComponent::mouseDown(juce::MouseEvent const & event)
{
    mDisplacementMode = getDisplacementMode(event);
    mCanDrag = isMoveAllowed(mDisplacementMode, mSource.isPrimarySource(), mTrajectoryManager.getSourceLink());
    if (mCanDrag) {
        if (mSource.isPrimarySource() || mDisplacementMode == DisplacementMode::all) {
            mTrajectoryManager.getProcessor().getChangeGestureManager().beginGesture(Automation::Ids::Z);
        }
        setSourcePosition(event);
    } else {
        mFieldComponent.displayInvalidSourceMoveWarning(true);
    }
}

//==============================================================================
void ElevationSourceComponent::setSourcePosition(juce::MouseEvent const & event) const
{
    auto const eventRelativeToFieldComponent{ event.getEventRelativeTo(&mFieldComponent) };
    auto const newElevation{ mFieldComponent.componentPositionToSourceElevation(
        eventRelativeToFieldComponent.getPosition().toFloat()) };
    auto const sourceLinkBehavior{ mDisplacementMode == DisplacementMode::selectedSourceOnly
                                       ? Source::OriginOfChange::userAnchorMove
                                       : Source::OriginOfChange::userMove };
    mSource.setElevation(newElevation, sourceLinkBehavior);
}

//==============================================================================
void ElevationSourceComponent::mouseDrag(juce::MouseEvent const & event)
{
    if (mCanDrag && mFieldComponent.getSelectedSourceIndex() == mSource.getIndex()) {
        jassert(mFieldComponent.getWidth() == mFieldComponent.getHeight());
        setSourcePosition(event);
    }
}

//==============================================================================
void ElevationSourceComponent::mouseUp(juce::MouseEvent const & event)
{
    if (mCanDrag) {
        mouseDrag(event);
        if (mSource.isPrimarySource() || mDisplacementMode == DisplacementMode::all) {
            mTrajectoryManager.getProcessor().getChangeGestureManager().endGesture(Automation::Ids::Z);
        }
    } else {
        mFieldComponent.displayInvalidSourceMoveWarning(false);
    }
}

//==============================================================================
SourceIndex ElevationSourceComponent::getSourceIndex() const
{
    return mSource.getIndex();
}

} // namespace gris