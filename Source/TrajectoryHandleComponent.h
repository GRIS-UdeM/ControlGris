/*
  ==============================================================================

    TrajectoryHandleComponent.h
    Created: 9 Jun 2020 2:40:16pm
    Author:  samuel

  ==============================================================================
*/

#pragma once

#include "FieldComponentClickableItem.h"

class TrajectoryHandleComponent : public FieldComponentClickableItem
{
private:
    FieldComponent & mFieldComponent;

public:
    TrajectoryHandleComponent(FieldComponent & fieldComponent);

private:
    void mouseDown(MouseEvent const & event) final;
    void mouseDrag(MouseEvent const & event) final;
    void mouseUp(MouseEvent const & event) final;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrajectoryHandleComponent);
};