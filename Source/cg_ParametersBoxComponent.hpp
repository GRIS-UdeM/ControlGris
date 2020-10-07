/**************************************************************************
 * Copyright 2018 UdeM - GRIS - Olivier Belanger                          *
 *                                                                        *
 * This file is part of ControlGris, a multi-source spatialization plugin *
 *                                                                        *
 * ControlGris is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU Lesser General Public License as         *
 * published by the Free Software Foundation, either version 3 of the     *
 * License, or (at your option) any later version.                        *
 *                                                                        *
 * ControlGris is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 * GNU Lesser General Public License for more details.                    *
 *                                                                        *
 * You should have received a copy of the GNU Lesser General Public       *
 * License along with ControlGris.  If not, see                           *
 * <http://www.gnu.org/licenses/>.                                        *
 *************************************************************************/

#pragma once

#include <JuceHeader.h>

#include "cg_ControlGrisLookAndFeel.hpp"
#include "cg_Source.hpp"

//==============================================================================
class ParametersBoxComponent final
    : public juce::Component
    , public juce::Slider::Listener
{
public:
    //==============================================================================
    struct Listener {
        virtual ~Listener() {}

        virtual void parametersBoxAzimuthSpanDragStarted() = 0;
        virtual void parametersBoxAzimuthSpanDragEnded() = 0;
        virtual void parametersBoxElevationSpanDragStarted() = 0;
        virtual void parametersBoxElevationSpanDragEnded() = 0;
        virtual void parametersBoxSelectedSourceClicked() = 0;
        virtual void parametersBoxParameterChanged(SourceParameter sourceId, double value) = 0;
    };

private:
    //==============================================================================
    GrisLookAndFeel & mGrisLookAndFeel;

    juce::ListenerList<Listener> mListeners;

    bool mDistanceEnabled{ false };
    bool mSpanLinked{ false };

    Source * mSelectedSource{};

    juce::Label mAzimuthLabel{};
    juce::Label mElevationLabel{};
    juce::Slider mAzimuthSpan{};
    juce::Slider mElevationSpan{};

public:
    //==============================================================================
    ParametersBoxComponent(GrisLookAndFeel & grisLookAndFeel);
    ~ParametersBoxComponent() final = default;
    //==============================================================================
    void mouseDown(juce::MouseEvent const & event) final;
    void sliderValueChanged(juce::Slider * slider) final;
    void paint(juce::Graphics &) final;
    void resized() final;

    void setSelectedSource(Source * source);
    void setDistanceEnabled(bool distanceEnabled);
    void setSpanLinkState(bool spanLinkState);
    bool getSpanLinkState() const { return mSpanLinked; }

    void addListener(Listener * l) { mListeners.add(l); }
    void removeListener(Listener * l) { mListeners.remove(l); }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParametersBoxComponent)
};
