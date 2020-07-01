/*
  ==============================================================================

    ElevationDrawingHandle.h
    Created: 30 Jun 2020 10:07:46am
    Author:  samuel

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "SourceComponent.h"

class ElevationFieldComponent;

class ElevationDrawingHandle final : public SourceComponent
{
private:
    ElevationFieldComponent & mFieldComponent;

public:
    ElevationDrawingHandle(ElevationFieldComponent & fieldComponent) noexcept
        : SourceComponent(Colour::fromRGB(120u, 120u, 120u), "X")
        , mFieldComponent(fieldComponent)
    {
    }
    ~ElevationDrawingHandle() noexcept = default;

    void mouseDown(MouseEvent const & event) final;
    void mouseDrag(MouseEvent const & event) final;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ElevationDrawingHandle);
};