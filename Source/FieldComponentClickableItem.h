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

class FieldComponentClickableItem : public juce::Component
{
private:
    Colour mColour;
    String mIcon;
    bool mSelected{ false };

protected:
    static constexpr int MAX_LINE_THICKNESS = 3;

public:
    FieldComponentClickableItem(Colour colour, String const & icon);
    virtual ~FieldComponentClickableItem() = default;

    void setSelected(bool selected);

    void paint(Graphics & g) final;

    void mouseDown(MouseEvent const & event) override = 0;
    void mouseDrag(MouseEvent const & event) override = 0;
    void mouseUp(MouseEvent const & event) override = 0;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FieldComponentClickableItem);
};