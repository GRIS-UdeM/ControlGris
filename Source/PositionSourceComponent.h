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

#include "SourceComponent.h"
#include "StrongTypes.h"

class PositionFieldComponent;
class PositionAutomationManager;
class Source;

//==============================================================================
class PositionSourceComponent final
    : public SourceComponent
    , public juce::ChangeListener
{
    PositionFieldComponent & mFieldComponent;
    PositionAutomationManager & mAutomationManager;
    Source & mSource;
    DisplacementMode mDisplacementMode{};

public:
    //==============================================================================
    PositionSourceComponent() = delete;
    ~PositionSourceComponent() override;

    PositionSourceComponent(PositionSourceComponent const &) = delete;
    PositionSourceComponent(PositionSourceComponent &&) = delete;

    PositionSourceComponent & operator=(PositionSourceComponent const &) = delete;
    PositionSourceComponent & operator=(PositionSourceComponent &&) = delete;
    //==============================================================================
    PositionSourceComponent(PositionFieldComponent & fieldComponent, Source & source);
    //==============================================================================
    SourceIndex getSourceIndex() const;

    void mouseDown(MouseEvent const & event) override;
    void mouseDrag(MouseEvent const & event) override;
    void mouseUp(MouseEvent const & event) override;
    void updatePositionInParent() override;

private:
    //==============================================================================
    void setSourcePosition(MouseEvent const & event) const;
    void setDisplacementMode(MouseEvent const & event);

    void changeListenerCallback(ChangeBroadcaster * source) override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(PositionSourceComponent)
}; // class PositionSourceComponent