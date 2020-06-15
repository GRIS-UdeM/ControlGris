/*
  ==============================================================================

    TrajectoryHandleComponent.cpp
    Created: 9 Jun 2020 2:40:16pm
    Author:  samuel

  ==============================================================================
*/

#include "TrajectoryHandleComponent.h"

#include "FieldComponent.h"

TrajectoryHandleComponent::TrajectoryHandleComponent(FieldComponent & fieldComponent) noexcept
    : FieldComponentClickableItem(Colour::fromRGB(176, 176, 228), "X")
    , mFieldComponent(fieldComponent)
{
    setInterceptsMouseClicks(false, false);
}
