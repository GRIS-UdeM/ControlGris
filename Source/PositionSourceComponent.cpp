/*
  ==============================================================================

    SourceComponent.cpp
    Created: 30 May 2020 11:52:24pm
    Author:  samuel

  ==============================================================================
*/

#include "PositionSourceComponent.h"

#include "FieldComponent.h"
#include "Source.h"

PositionSourceComponent::PositionSourceComponent(PositionFieldComponent & fieldComponent, Source & source)
    : FieldComponentClickableItem(source.getColour(), String{ source.getId() + 1 })
    , mFieldComponent(fieldComponent)
    , mAutomationManager(fieldComponent.getAutomationManager())
    , mSource(source)
{
    source.addChangeListener(this);
    this->updatePositionInParent();
}

PositionSourceComponent::~PositionSourceComponent()
{
    mSource.removeChangeListener(this);
}

void PositionSourceComponent::updatePositionInParent()
{
    auto const newCenter{ mFieldComponent.sourcePositionToComponentPosition(mSource.getPos()) };
    this->setCentrePosition(newCenter.getX(), newCenter.getY());
}

void PositionSourceComponent::mouseDown(MouseEvent const & event)
{
    auto const sourceLink{ mAutomationManager.getSourceLink() };

    if (mSource.getId() > 0 && sourceLink != PositionSourceLink::independent
        && sourceLink != PositionSourceLink::deltaLock) {
        mFieldComponent.setCircularSourceSelectionWarning(true);
    } else {
        this->setSourcePosition(event);
        mFieldComponent.setSelectedSource(mSource.getId());
    }
}

void PositionSourceComponent::setSourcePosition(MouseEvent const & event)
{
    jassert(mFieldComponent.getWidth() == mFieldComponent.getHeight());

    auto const eventRelativeToFieldComponent{ event.getEventRelativeTo(&mFieldComponent) };
    auto const newPosition{ mFieldComponent.componentPositionToSourcePosition(
        eventRelativeToFieldComponent.getPosition().toFloat()) };
    mSource.setPos(newPosition);

    mFieldComponent.notifySourcePositionChanged(mSource.getId());
}

void PositionSourceComponent::mouseDrag(MouseEvent const & event)
{
    if (mFieldComponent.getSelectedSourceId() == mSource.getId()) {
        jassert(mFieldComponent.getWidth() == mFieldComponent.getHeight());

        this->setSourcePosition(event);

        repaint();
    }
}

void PositionSourceComponent::mouseUp(MouseEvent const & event)
{
    mFieldComponent.setCircularSourceSelectionWarning(false);
}

void PositionSourceComponent::changeListenerCallback(ChangeBroadcaster * source)
{
    this->updatePositionInParent();
}
