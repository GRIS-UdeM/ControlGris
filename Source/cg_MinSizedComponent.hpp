/**************************************************************************
 * Copyright 2021 UdeM - GRIS - Samuel Béland & Olivier Belanger          *
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
namespace gris
{
//==============================================================================
class MinSizedComponent
    : public juce::Component
    , public juce::SettableTooltipClient
{
public:
    MinSizedComponent() = default;
    virtual ~MinSizedComponent() = default;
    //==============================================================================
    MinSizedComponent(MinSizedComponent const &) = delete;
    MinSizedComponent(MinSizedComponent &&) = delete;
    MinSizedComponent & operator=(MinSizedComponent const &) = delete;
    MinSizedComponent & operator=(MinSizedComponent &&) = delete;
    //==============================================================================
    [[nodiscard]] virtual int getMinWidth() const noexcept = 0;
    [[nodiscard]] virtual int getMinHeight() const noexcept = 0;

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR(MinSizedComponent)
};
} // namespace gris