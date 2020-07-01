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

class SourceComponent : public juce::Component
{
private:
    Colour mColour;
    String mIcon;
    bool mSelected{ false };

protected:
    static constexpr int MAX_LINE_THICKNESS = 3;

public:
    SourceComponent(Colour colour, String const & icon);
    virtual ~SourceComponent() = default;

    void setSelected(bool selected);
    void paint(Graphics & g) final;

    Colour getColour() const { return mColour; }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceComponent);
};