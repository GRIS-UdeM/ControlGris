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

#include "../JuceLibraryCode/JuceHeader.h"

#include "ControlGrisConstants.h"
#include "SourceComponent.h"
#include "StrongTypes.h"

class ElevationFieldComponent;
class ElevationAutomationManager;
class Source;

class ElevationSourceComponent final
    : public SourceComponent
    , public juce::ChangeListener
{
private:
    ElevationFieldComponent & mFieldComponent;
    ElevationAutomationManager & mAutomationManager;
    Source & mSource;

public:
    ElevationSourceComponent(ElevationFieldComponent & fieldComponent, Source & source) noexcept;
    ~ElevationSourceComponent() noexcept final;

    void mouseDown(MouseEvent const & event) final;
    void mouseDrag(MouseEvent const & event) final;
    void mouseUp(MouseEvent const & event) final;

    SourceIndex getSourceIndex() const;
    void updatePositionInParent() final;

private:
    void setSourcePosition(MouseEvent const & event);

    void changeListenerCallback(ChangeBroadcaster * source) final;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ElevationSourceComponent);
};