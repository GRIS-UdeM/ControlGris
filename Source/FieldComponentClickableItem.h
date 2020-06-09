/*
  ==============================================================================

    FieldComponentClickableItem.h
    Created: 8 Jun 2020 10:11:39pm
    Author:  samuel

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "ControlGrisConstants.h"

class Source;
class FieldComponent;

class FieldComponentClickableItem final
    : public juce::Component
    , public juce::ChangeListener
{
private:
    static constexpr int MAX_LINE_THICKNESS = 3;

    FieldComponent & mFieldComponent;
    Source & mSource;

public:
    FieldComponentClickableItem(FieldComponent & fieldComponent);
    ~FieldComponentClickableItem() final;

    void paint(Graphics & g) final;

    void mouseDown(MouseEvent const & event) final;
    void mouseDrag(MouseEvent const & event) final;
    void mouseUp(MouseEvent const & event) final;

private:
    void updatePositionInParent();
    void setSourcePosition(MouseEvent const & event);

    void changeListenerCallback(ChangeBroadcaster * source) final;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FieldComponentClickableItem);
};