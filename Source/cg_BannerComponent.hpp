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

namespace gris
{
//==============================================================================
/**
 * A Label-derived class used to identify the different UI sections of the plugin.
 */
class BannerComponent final : public juce::Label
{
    static juce::Colour const BACKGROUND_COLOUR;
    static juce::Colour const OUTLINE_COLOUR;
    static juce::Colour const TEXT_COLOUR;

public:
    //==============================================================================
    BannerComponent() noexcept;
    ~BannerComponent() override = default;

    BannerComponent(BannerComponent const &) = delete;
    BannerComponent(BannerComponent &&) = delete;

    BannerComponent & operator=(BannerComponent const &) = delete;
    BannerComponent & operator=(BannerComponent &&) = delete;

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR(BannerComponent)
}; // class BannerComponent

} // namespace gris
