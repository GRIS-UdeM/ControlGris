/*
  ==============================================================================

    SourceComponent.cpp
    Created: 30 May 2020 11:52:24pm
    Author:  samuel

  ==============================================================================
*/

#include "PositionSourceComponent.h"

#include "FieldComponent.h"
#include "PluginProcessor.h"
#include "Source.h"

PositionSourceComponent::PositionSourceComponent(PositionFieldComponent & fieldComponent, Source & source)
    : SourceComponent(source.getColour(), source.getId().toString())
    , mFieldComponent(fieldComponent)
    , mAutomationManager(fieldComponent.getAutomationManager())
    , mSource(source)
{
    source.addGuiChangeListener(this);
    this->updatePositionInParent();
}

PositionSourceComponent::~PositionSourceComponent()
{
    mSource.removeGuiChangeListener(this);
}

void PositionSourceComponent::updatePositionInParent()
{
    auto const newCenter{ mFieldComponent.sourcePositionToComponentPosition(mSource.getPos()) };
    this->setCentrePosition(newCenter.getX(), newCenter.getY());
}

void PositionSourceComponent::mouseDown(MouseEvent const & event)
{
    if (mSource.isPrimarySource()) {
        mAutomationManager.getProcessor().beginChangeGesture(ParameterNames::x);
        mAutomationManager.getProcessor().beginChangeGesture(ParameterNames::y);
    }
    this->setSourcePosition(event);
    mFieldComponent.setSelectedSource(mSource.getIndex());
}

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

void PositionSourceComponent::mouseDrag(MouseEvent const & event)
{
    if (mFieldComponent.getSelectedSourceIndex() == mSource.getIndex()) {
        jassert(mFieldComponent.getWidth() == mFieldComponent.getHeight());
        this->setSourcePosition(event);
    }
}

void PositionSourceComponent::mouseUp(MouseEvent const & event)
{
    mouseDrag(event);
    if (mSource.isPrimarySource()) {
        mAutomationManager.getProcessor().endChangeGesture(ParameterNames::x);
        mAutomationManager.getProcessor().endChangeGesture(ParameterNames::y);
    }
}

SourceIndex PositionSourceComponent::getSourceIndex() const
{
    return mSource.getIndex();
}

void PositionSourceComponent::changeListenerCallback(ChangeBroadcaster * source)
{
    this->updatePositionInParent();
}
