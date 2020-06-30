/*
  ==============================================================================

    ElevationSourceComponent.cpp
    Created: 9 Jun 2020 2:40:05pm
    Author:  samuel

  ==============================================================================
*/

#include "ElevationSourceComponent.h"

#include "FieldComponent.h"
#include "PluginProcessor.h"
#include "Source.h"

ElevationSourceComponent::ElevationSourceComponent(ElevationFieldComponent & fieldComponent, Source & source) noexcept
    : SourceComponent(source.getColour(), source.getId().toString())
    , mAutomationManager(fieldComponent.getAutomationManager())
    , mFieldComponent(fieldComponent)
    , mSource(source)
{
    source.addGuiChangeListener(this);
    this->updatePositionInParent();
}

ElevationSourceComponent::~ElevationSourceComponent() noexcept
{
    mSource.removeGuiChangeListener(this);
}

void ElevationSourceComponent::updatePositionInParent()
{
    auto const newCenter{ mFieldComponent.sourceElevationToComponentPosition(mSource.getElevation(),
                                                                             mSource.getIndex()) };
    this->setCentrePosition(newCenter.getX(), newCenter.getY());
}

void ElevationSourceComponent::mouseDown(MouseEvent const & event)
{
    if (mSource.isPrimarySource()) {
        mAutomationManager.getProcessor().beginSourceElevationChangeGesture();
    }
    this->setSourcePosition(event);
    mFieldComponent.setSelectedSource(mSource.getIndex());
}

void ElevationSourceComponent::setSourcePosition(MouseEvent const & event)
{
    auto const eventRelativeToFieldComponent{ event.getEventRelativeTo(&mFieldComponent) };
    auto const newElevation{ mFieldComponent.componentPositionToSourceElevation(
        eventRelativeToFieldComponent.getPosition().toFloat()) };
    mSource.setElevation(newElevation, SourceLinkNotification::notify);

    if (mSource.isPrimarySource()) {
        mAutomationManager.sendTrajectoryPositionChangedEvent();
    }
    mFieldComponent.notifySourcePositionChanged(mSource.getIndex());
}

void ElevationSourceComponent::mouseDrag(MouseEvent const & event)
{
    if (mFieldComponent.getSelectedSourceIndex() == mSource.getIndex()) {
        jassert(mFieldComponent.getWidth() == mFieldComponent.getHeight());
        this->setSourcePosition(event);
    }
}

void ElevationSourceComponent::mouseUp(MouseEvent const & event)
{
    if (mSource.isPrimarySource()) {
        mAutomationManager.getProcessor().endSourceElevationChangeGesture();
    }
}

SourceIndex ElevationSourceComponent::getSourceIndex() const
{
    return mSource.getIndex();
}

void ElevationSourceComponent::changeListenerCallback(ChangeBroadcaster * source)
{
    this->updatePositionInParent();
}
