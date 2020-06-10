/*
  ==============================================================================

    ElevationSourceComponent.h
    Created: 9 Jun 2020 2:40:05pm
    Author:  samuel

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "ControlGrisConstants.h"
#include "FieldComponentClickableItem.h"

class ElevationFieldComponent;
class ElevationAutomationManager;
class Source;

class ElevationSourceComponent final
    : public FieldComponentClickableItem
    , public juce::ChangeListener
{
private:
    ElevationFieldComponent & mFieldComponent;
    ElevationAutomationManager & mAutomationManager;
    Source & mSource;

public:
    ElevationSourceComponent(ElevationFieldComponent & fieldComponent, Source & source);
    ~ElevationSourceComponent() final;

    void mouseDown(MouseEvent const & event) final;
    void mouseDrag(MouseEvent const & event) final;
    void mouseUp(MouseEvent const & event) final;

private:
    void updatePositionInParent();
    void setSourcePosition(MouseEvent const & event);

    void changeListenerCallback(ChangeBroadcaster * source) final;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ElevationSourceComponent);
};