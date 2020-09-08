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

#include "ControlGrisConstants.h"
#include "Source.h"

struct SourceSnapshot {
    SourceSnapshot() noexcept = default;
    SourceSnapshot(Source const & source) noexcept : position(source.getPos()), z(source.getElevation()) {}

    Point<float> position;
    Radians z; // height in CUBE mode, elevation in DOME mode
};

struct SourcesSnapshots {
    SourceSnapshot primary{};
    Array<SourceSnapshot> secondaries{};

    SourceSnapshot const & operator[](SourceIndex const index) const
    {
        jassert(index.toInt() >= 0 && index.toInt() < secondaries.size() + 1);
        if (index.toInt() == 0) {
            return primary;
        }
        return secondaries.getReference(index.toInt() - 1);
    }
    SourceSnapshot & operator[](SourceIndex const index)
    {
        jassert(index.toInt() >= 0 && index.toInt() < secondaries.size() + 1);
        if (index.toInt() == 0) {
            return primary;
        }
        return secondaries.getReference(index.toInt() - 1);
    }
    int size() const { return secondaries.size() + 1; }
};

class SourceLinkEnforcer : juce::ChangeListener
{
private:
    Sources & mSources;
    SourcesSnapshots mSnapshots{};
    PositionSourceLink mPositionSourceLink{ PositionSourceLink::undefined };
    ElevationSourceLink mElevationSourceLink{ ElevationSourceLink::undefined };

public:
    SourceLinkEnforcer(Sources & sources, PositionSourceLink sourceLink = PositionSourceLink::independent) noexcept;
    SourceLinkEnforcer(Sources & sources, ElevationSourceLink sourceLink) noexcept;
    ~SourceLinkEnforcer() noexcept;

    void setSourceLink(ElevationSourceLink sourceLink);
    void setSourceLink(PositionSourceLink sourceLink);
    void numberOfSourcesChanged();
    void enforceSourceLink();

    auto const & getSnapshots() const { return mSnapshots; }
    void loadSnapshots(SourcesSnapshots const & snapshots);

private:
    void primarySourceMoved();
    void secondarySourceMoved(SourceIndex sourceIndex);
    void saveCurrentPositionsToInitialStates();
    void reset();

    void changeListenerCallback(ChangeBroadcaster * source) final;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceLinkEnforcer);
};
