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
    constexpr float DUMMY{ 0.0f };

    auto const relativePosition{ event.getEventRelativeTo(getParentComponent()).getPosition().toFloat() };
    auto const elevation{ mFieldComponent.componentPositionToSourceElevation(relativePosition) };
    Point<float> const position{ DUMMY, elevation / MAX_ELEVATION * 2.0f - 1.0f };

    auto & automationManager{ mFieldComponent.getAutomationManager() };
    automationManager.resetRecordingTrajectory(position);
    automationManager.addRecordingPoint(position); // two points minimum are needed for elevation drawing

    setCentrePosition(mFieldComponent.sourceElevationToComponentPosition(elevation, SourceIndex{ -1 }).toInt());
    mFieldComponent.repaint();
}

void ElevationDrawingHandle::mouseDrag(MouseEvent const & event)
{
    auto & automationManager{ mFieldComponent.getAutomationManager() };
    auto const relativePosition{ event.getEventRelativeTo(getParentComponent()).getPosition().toFloat() };
    auto const elevation{ mFieldComponent.componentPositionToSourceElevation(relativePosition) };
    constexpr auto DUMMY{ 0.0f };
    Point<float> const position{ DUMMY, elevation / MAX_ELEVATION * 2.0f - 1.0f };

    automationManager.addRecordingPoint(position);

    setCentrePosition(mFieldComponent.sourceElevationToComponentPosition(elevation, SourceIndex{ -1 }).toInt());
    mFieldComponent.repaint();
}
