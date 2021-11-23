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

#include "cg_SourceComponent.hpp"

namespace gris
{
class ElevationFieldComponent;

//==============================================================================
/** A draggable component used to create elevation trajectories. */
class ElevationDrawingHandle final : public SourceComponent
{
    ElevationFieldComponent & mElevationFieldComponent;
    Radians mCurrentElevation;

public:
    //==============================================================================
    explicit ElevationDrawingHandle(ElevationFieldComponent & fieldComponent) noexcept;
    ElevationDrawingHandle() = delete;
    ~ElevationDrawingHandle() noexcept override = default;
    //==============================================================================
    ElevationDrawingHandle(ElevationDrawingHandle const &) = delete;
    ElevationDrawingHandle(ElevationDrawingHandle &&) = delete;
    ElevationDrawingHandle & operator=(ElevationDrawingHandle const &) = delete;
    ElevationDrawingHandle & operator=(ElevationDrawingHandle &&) = delete;
    //==============================================================================
    void mouseDown(juce::MouseEvent const & event) override;
    void mouseDrag(juce::MouseEvent const & event) override;
    void updatePositionInParent() override;

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR(ElevationDrawingHandle)

}; // class ElevationDrawingHandle

} // namespace gris