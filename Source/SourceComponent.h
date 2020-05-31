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

class Source;
class FieldComponent;

class SourceComponent final
    : public juce::Component
    , public juce::ChangeListener
{
private:
    static constexpr int MAX_LINE_THICKNESS = 3;

    FieldComponent & mFieldComponent;
    Source & mSource;

public:
    SourceComponent(FieldComponent & fieldComponent, Source & source);
    ~SourceComponent() final;

    void paint(Graphics & g) final;

    void mouseDown(MouseEvent const & event) final;
    void mouseDrag(MouseEvent const & event) final;
    void mouseUp(MouseEvent const & event) final;

private:
    void updatePositionInParent();
    void setSourcePosition(MouseEvent const & event);

    void changeListenerCallback(ChangeBroadcaster * source) final;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceComponent);
};