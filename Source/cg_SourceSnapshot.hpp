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

#include "cg_StrongTypes.hpp"

namespace gris
{
class Source;

//==============================================================================
/** A snapshot of a source's position, used mainly for source linking. */
struct SourceSnapshot {
    juce::Point<float> position;
    Radians z{}; // height in CUBE mode, elevation in DOME mode
    //==============================================================================
    explicit SourceSnapshot(Source const & source) noexcept;
    SourceSnapshot() noexcept = default;
    ~SourceSnapshot() = default;
    //==============================================================================
    SourceSnapshot(SourceSnapshot const &) = default;
    SourceSnapshot(SourceSnapshot &&) = default;
    SourceSnapshot & operator=(SourceSnapshot const &) = default;
    SourceSnapshot & operator=(SourceSnapshot &&) = default;

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR(SourceSnapshot)
}; // class SourceSnapshot

//==============================================================================
/** A collection of source snapshots. */
struct SourcesSnapshots {
    SourceSnapshot primary{};
    juce::Array<SourceSnapshot> secondaries{};
    //==============================================================================
    [[nodiscard]] SourceSnapshot const & operator[](SourceIndex index) const;
    [[nodiscard]] SourceSnapshot & operator[](SourceIndex index);
    [[nodiscard]] int size() const noexcept { return secondaries.size() + 1; }

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR(SourceSnapshot)
}; // class SourcesSnapshots

} // namespace gris