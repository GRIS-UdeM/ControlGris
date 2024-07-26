/**************************************************************************
 * Copyright 2024 UdeM - GRIS - Gaël LANE LÉPINE                          *
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
 * <https://www.gnu.org/licenses/>.                                       *
 *************************************************************************/

#include "cg_SectionSoundReactiveSpatialization.h"

//==============================================================================
gris::SectionSoundReactiveSpatialization::SectionSoundReactiveSpatialization(GrisLookAndFeel & grisLookAndFeel)
    : mGrisLookAndFeel(grisLookAndFeel)
{
    setName("Sound Reactive Spatialization");
}

//==============================================================================
void gris::SectionSoundReactiveSpatialization::paint(juce::Graphics & g)
{
    g.fillAll(mGrisLookAndFeel.findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(mGrisLookAndFeel.getDarkColor());
    g.drawLine(juce::Line<float>(0.0f, 20.0f, (float)getWidth(), 20.0f));
}

//==============================================================================
void gris::SectionSoundReactiveSpatialization::resized()
{
}
