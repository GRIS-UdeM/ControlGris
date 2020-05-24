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
#include "BannerComponent.h"

BannerComponent::BannerComponent()
{
    setEditable(false, false, false);
    setColour(Label::backgroundColourId, Colour::fromRGB(64, 64, 64));
    setColour(Label::outlineColourId, Colour::fromRGB(16, 16, 16));
    setColour(Label::textColourId, Colour::fromRGB(255, 255, 255));
}

BannerComponent::~BannerComponent()
{
    setLookAndFeel(nullptr);
}
