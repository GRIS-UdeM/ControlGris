/*
  ==============================================================================

    ElevationDrawingHandle.cpp
    Created: 30 Jun 2020 10:07:46am
    Author:  samuel

  ==============================================================================
*/

#include "ElevationDrawingHandle.h"

#include "FieldComponent.h"

void ElevationDrawingHandle::mouseDown(MouseEvent const & event)
{
    auto & trajectory{ mFieldComponent.getAutomationManager().getDrawingTrajectory() };
    trajectory.clear();

    auto const relativePosition{ event.getEventRelativeTo(getParentComponent()).getPosition().toFloat() };
    auto const elevation{ mFieldComponent.componentPositionToSourceElevation(relativePosition) };

    trajectory.addValue(elevation);
    trajectory.addValue(elevation); // two points minimum are needed

    setCentrePosition(mFieldComponent.sourceElevationToComponentPosition(elevation, SourceIndex{ -1 }).toInt());
    mFieldComponent.repaint();
}

void ElevationDrawingHandle::mouseDrag(MouseEvent const & event)
{
    auto & trajectory{ mFieldComponent.getAutomationManager().getDrawingTrajectory() };
    auto const relativePosition{ event.getEventRelativeTo(getParentComponent()).getPosition().toFloat() };
    auto const elevation{ mFieldComponent.componentPositionToSourceElevation(relativePosition) };
    trajectory.addValue(elevation);

    setCentrePosition(mFieldComponent.sourceElevationToComponentPosition(elevation, SourceIndex{ -1 }).toInt());
    mFieldComponent.repaint();
}
