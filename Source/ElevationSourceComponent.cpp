/*
  ==============================================================================

    ElevationSourceComponent.cpp
    Created: 9 Jun 2020 2:40:05pm
    Author:  samuel

  ==============================================================================
*/

#include "ElevationSourceComponent.h"

#include "FieldComponent.h"
#include "Source.h"

ElevationSourceComponent::ElevationSourceComponent(ElevationFieldComponent & fieldComponent, Source & source)
    : FieldComponentClickableItem(source.getColour(), String{ source.getId() + 1 })
    , mAutomationManager(fieldComponent.getAutomationManager())
    , mFieldComponent(fieldComponent)
    , mSource(source)
{
    source.addChangeListener(this);
    this->updatePositionInParent();
}

ElevationSourceComponent::~ElevationSourceComponent()
{
    mSource.removeChangeListener(this);
}

void ElevationSourceComponent::updatePositionInParent()
{
    auto const newCenter{ mFieldComponent.sourceElevationToComponentPosition(mSource.getElevation(), mSource.getId()) };
    this->setCentrePosition(newCenter.getX(), newCenter.getY());
}

void ElevationSourceComponent::mouseDown(MouseEvent const & event)
{
    mFieldComponent.setSelectedSource(mSource.getId());
    this->setSourcePosition(event);
}

void ElevationSourceComponent::setSourcePosition(MouseEvent const & event)
{
    auto const eventRelativeToFieldComponent{ event.getEventRelativeTo(&mFieldComponent) };
    auto const newElevation{ mFieldComponent.componentPositionToSourceElevation(
        eventRelativeToFieldComponent.getPosition().toFloat()) };

    mSource.setElevation(newElevation);

    mFieldComponent.notifySourcePositionChanged(mSource.getId());
}

void ElevationSourceComponent::mouseDrag(MouseEvent const & event)
{
    if (mFieldComponent.getSelectedSourceId() == mSource.getId()) {
        jassert(mFieldComponent.getWidth() == mFieldComponent.getHeight());

        this->setSourcePosition(event);

        repaint();

        // TODO: what if trajectory handle?
    }
}

void ElevationSourceComponent::mouseUp(MouseEvent const & event)
{
    // TODO: what if trajectory handle?
    // TODO: maybe turn off circularSourceSelectionWarning?
}

void ElevationSourceComponent::changeListenerCallback(ChangeBroadcaster * source)
{
    this->updatePositionInParent();
}
