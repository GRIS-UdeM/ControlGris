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

#include "SourceLinkEnforcer.h"

#include "ControlGrisConstants.h"

//==============================================================================
SourceLinkEnforcer::SourceLinkEnforcer(Sources & sources, PositionSourceLink const sourceLink) noexcept
    : mSources(sources)
    , mPositionSourceLink(sourceLink)
{
    reset();
}

//==============================================================================
SourceLinkEnforcer::SourceLinkEnforcer(Sources & sources, ElevationSourceLink const sourceLink) noexcept
    : mSources(sources)
    , mElevationSourceLink(sourceLink)
{
    reset();
}

//==============================================================================
SourceLinkEnforcer::~SourceLinkEnforcer() noexcept
{
    for (auto & source : mSources) {
        source.removeSourceLinkListener(this);
    }
}

//==============================================================================
void SourceLinkEnforcer::setSourceLink(PositionSourceLink const sourceLink)
{
    if (sourceLink != mPositionSourceLink) {
        //        saveCurrentPositionsToInitialStates();
        mPositionSourceLink = sourceLink;
        mElevationSourceLink = ElevationSourceLink::undefined;
        saveCurrentPositionsToInitialStates();
    }
}

//==============================================================================
void SourceLinkEnforcer::setSourceLink(ElevationSourceLink const sourceLink)
{
    if (sourceLink != mElevationSourceLink) {
        //        saveCurrentPositionsToInitialStates();
        mElevationSourceLink = sourceLink;
        mPositionSourceLink = PositionSourceLink::undefined;
        saveCurrentPositionsToInitialStates();
    }
}

//==============================================================================
void SourceLinkEnforcer::enforceSourceLink()
{
    std::unique_ptr<LinkStrategy> strategy{};

    if (mPositionSourceLink != PositionSourceLink::undefined) {
        jassert(mElevationSourceLink == ElevationSourceLink::undefined);
        strategy = LinkStrategy::make(mPositionSourceLink);
    } else {
        jassert(mElevationSourceLink != ElevationSourceLink::undefined);
        strategy = LinkStrategy::make(mElevationSourceLink);
    }

    if (strategy != nullptr) {
        strategy->computeParameters(mSources, mSnapshots);
        strategy->enforce(mSources.getSecondarySources(), mSnapshots.secondaries);
        if (mPositionSourceLink == PositionSourceLink::circularFixedAngle
            || mPositionSourceLink == PositionSourceLink::circularFullyFixed) {
            // circularFixedAngle & circularFullyFixed links require the snapshots to be up-to-date or else moving the
            // relative ordering with the mouse won't make any sense.
            saveCurrentPositionsToInitialStates();
        }
    }
}

//==============================================================================
void SourceLinkEnforcer::loadSnapshots(SourcesSnapshots const & snapshots)
{
    mSnapshots = snapshots;
    enforceSourceLink();
}

//==============================================================================
void SourceLinkEnforcer::sourceMoved(Source & source, SourceLinkBehavior const sourceLinkBehavior)
{
    switch (sourceLinkBehavior) {
    case SourceLinkBehavior::doNothing:
        jassertfalse;
        break;
    case SourceLinkBehavior::moveSourceAnchor:
        if (source.isPrimarySource()) {
            primarySourceAnchorMoved();
        } else {
            secondarySourceAnchorMoved(source.getIndex());
        }
        break;
    case SourceLinkBehavior::moveAllSources:
        if (source.isPrimarySource()) {
            primarySourceMoved();
        } else {
            secondarySourceMoved(source.getIndex());
        }
        break;
    }
}

//==============================================================================
void SourceLinkEnforcer::numberOfSourcesChanged()
{
    reset();
}

//==============================================================================
void SourceLinkEnforcer::primarySourceMoved()
{
    enforceSourceLink();

    // We need to force an update in independent mode.
    bool isIndependent{ mElevationSourceLink == ElevationSourceLink::independent
                        || mPositionSourceLink == PositionSourceLink::independent };
    if (isIndependent) {
        mSnapshots.primary = SourceSnapshot{ mSources.getPrimarySource() };
    }
}

//==============================================================================
void SourceLinkEnforcer::secondarySourceMoved(SourceIndex const sourceIndex)
{
}

//==============================================================================
void SourceLinkEnforcer::primarySourceAnchorMoved()
{
    auto & snapshot{ mSnapshots.primary };
    std::unique_ptr<LinkStrategy> strategy{};
    if (mElevationSourceLink != ElevationSourceLink::undefined) {
        jassert(mPositionSourceLink == PositionSourceLink::undefined);
        strategy = LinkStrategy::make(mElevationSourceLink);
    } else {
    }
}

//==============================================================================
void SourceLinkEnforcer::secondarySourceAnchorMoved(SourceIndex const sourceIndex)
{
    jassert(sourceIndex.toInt() > 0 && sourceIndex.toInt() < MAX_NUMBER_OF_SOURCES);
    auto & source{ mSources[sourceIndex] };
    auto const secondaryIndex{ sourceIndex.toInt() - 1 };
    auto & snapshot{ mSnapshots.secondaries.getReference(secondaryIndex) };
    std::unique_ptr<LinkStrategy> strategy{};
    if (mElevationSourceLink != ElevationSourceLink::undefined) {
        jassert(mPositionSourceLink == PositionSourceLink::undefined);
        strategy = LinkStrategy::make(mElevationSourceLink);
    } else {
        jassert(mPositionSourceLink != PositionSourceLink::undefined);
        strategy = LinkStrategy::make(mPositionSourceLink);
    }

    if (strategy != nullptr) {
        strategy->computeParameters(mSources, mSnapshots);
        snapshot = strategy->computeInitialStateFromFinalState(source, snapshot);
        primarySourceMoved(); // some positions are invalid - fix them right away
    }
}

//==============================================================================
void SourceLinkEnforcer::saveCurrentPositionsToInitialStates()
{
    mSnapshots.primary = SourceSnapshot{ mSources.getPrimarySource() };
    mSnapshots.secondaries.clearQuick();
    for (int i{}; i < mSources.getSecondarySources().size(); ++i) {
        mSnapshots.secondaries.add(SourceSnapshot{ mSources.getSecondarySources()[i] });
    }
}

//==============================================================================
void SourceLinkEnforcer::reset()
{
    for (auto & source : mSources) {
        source.removeSourceLinkListener(this);
    }
    saveCurrentPositionsToInitialStates();
    for (auto & source : mSources) {
        source.addSourceLinkListener(this);
    }
}
