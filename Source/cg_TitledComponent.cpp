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

#include "cg_TitledComponent.hpp"

#include "cg_ControlGrisLookAndFeel.hpp"
namespace gris
{
static constexpr auto RIGHT_PADDING = 5;

//==============================================================================
TitledComponent::TitledComponent(juce::String title,
                                 MinSizedComponent * contentComponent,
                                 GrisLookAndFeel & lookAndFeel)
    : mTitle(std::move(title))
    , mContentComponent(contentComponent)
    , mLookAndFeel(lookAndFeel)
{
    addAndMakeVisible(mContentComponent);
}

//==============================================================================
void TitledComponent::resized()
{
    auto const availableHeight{ std::max(getHeight() - TITLE_HEIGHT, 0) };
    juce::Rectangle<int> const contentBounds{ 0, TITLE_HEIGHT, getWidth(), availableHeight };
    mContentComponent->setBounds(contentBounds);
}

//==============================================================================
void TitledComponent::paint(juce::Graphics & g)
{
    g.setColour(juce::Colours::black);
    g.fillRect(getLocalBounds());
    if (mTitle != "") {
        g.setColour(juce::Colours::white);
        g.fillRect(0, 0, getWidth(), TITLE_HEIGHT);
        g.drawText(mTitle, 0, 0, getWidth(), TITLE_HEIGHT + 2, juce::Justification::left);
    }
}
} // namespace gris