/*
  ==============================================================================

    TrajectoryHandleComponent.cpp
    Created: 9 Jun 2020 2:40:16pm
    Author:  samuel

  ==============================================================================
*/

#include "TrajectoryHandleComponent.h"

TrajectoryHandleComponent::TrajectoryHandleComponent(FieldComponent & fieldComponent)
    : FieldComponentClickableItem(Colours::blue, "X")
    , mFieldComponent(fieldComponent)
{
}

void TrajectoryHandleComponent::mouseDown(MouseEvent const & event)
{
    // TODO
    jassertfalse;
}
void TrajectoryHandleComponent::mouseDrag(MouseEvent const & event)
{
    // TODO
    jassertfalse;
}
void TrajectoryHandleComponent::mouseUp(MouseEvent const & event)
{
    // TODO
    jassertfalse;
}