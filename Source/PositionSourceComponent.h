/*
  ==============================================================================

    SourceComponent.h
    Created: 30 May 2020 11:52:24pm
    Author:  samuel

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "ControlGrisConstants.h"
#include "FieldComponentClickableItem.h"
#include "StrongTypes.h"

class PositionFieldComponent;
class PositionAutomationManager;
class Source;

class PositionSourceComponent final
    : public FieldComponentClickableItem
    , public juce::ChangeListener
{
private:
    PositionFieldComponent & mFieldComponent;
    PositionAutomationManager & mAutomationManager;
    Source & mSource;

public:
    PositionSourceComponent(PositionFieldComponent & fieldComponent, Source & source);
    ~PositionSourceComponent() final;

    void mouseDown(MouseEvent const & event) final;
    void mouseDrag(MouseEvent const & event) final;
    void mouseUp(MouseEvent const & event) final;

    SourceIndex getSourceIndex() const;

private:
    void updatePositionInParent();
    void setSourcePosition(MouseEvent const & event);

    void changeListenerCallback(ChangeBroadcaster * source) final;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PositionSourceComponent);
};