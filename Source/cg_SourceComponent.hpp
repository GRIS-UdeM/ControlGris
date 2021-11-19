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

#include "cg_constants.hpp"

namespace gris
{
class Source;
class FieldComponent;

//==============================================================================
/** A base class for a draggable Component in a field (position or elevation). */
class SourceComponent : public juce::Component
{
    juce::Colour mColour{};
    juce::String mIcon{};
    bool mSelected{ false };

protected:
    //==============================================================================
    enum class DisplacementMode { all, selectedSourceOnly };
    //==============================================================================
    static constexpr int MAX_LINE_THICKNESS = 3;
    static constexpr int DISPLACEMENT_MODIFIER{ juce::ModifierKeys::Flags::ctrlModifier };

public:
    //==============================================================================
    SourceComponent(juce::Colour colour, juce::String const & icon);
    SourceComponent() = delete;
    ~SourceComponent() override = default;
    //==============================================================================
    SourceComponent(SourceComponent const &) = delete;
    SourceComponent(SourceComponent &&) = delete;
    SourceComponent & operator=(SourceComponent const &) = delete;
    SourceComponent & operator=(SourceComponent &&) = delete;
    //==============================================================================
    void setSelected(bool selected);
    void paint(juce::Graphics & g) final;
    [[nodiscard]] juce::Colour getColour() const { return mColour; }
    //==============================================================================
    virtual void updatePositionInParent() {}

protected:
    //==============================================================================
    [[nodiscard]] static DisplacementMode getDisplacementMode(juce::MouseEvent const & event);
    [[nodiscard]] static bool
        isMoveAllowed(DisplacementMode displacementMode, bool isPrimarySource, PositionSourceLink sourceLink);
    [[nodiscard]] static bool
        isMoveAllowed(DisplacementMode displacementMode, bool isPrimarySource, ElevationSourceLink sourceLink);

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR(SourceComponent)
}; // class SourceComponent

} // namespace gris