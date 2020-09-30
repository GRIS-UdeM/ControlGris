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
        mPositionSourceLink = sourceLink;
        mElevationSourceLink = ElevationSourceLink::undefined;
        mLinkStrategy = LinkStrategy::make(sourceLink);
        saveCurrentPositionsToInitialStates();
    }
}

//==============================================================================
void SourceLinkEnforcer::setSourceLink(ElevationSourceLink const sourceLink)
{
    if (sourceLink != mElevationSourceLink) {
        mElevationSourceLink = sourceLink;
        mPositionSourceLink = PositionSourceLink::undefined;
        mLinkStrategy = LinkStrategy::make(sourceLink);
        saveCurrentPositionsToInitialStates();
    }
}

//==============================================================================
void SourceLinkEnforcer::enforceSourceLink()
{
    if (!mLinkStrategy) {
        if (mElevationSourceLink != ElevationSourceLink::undefined) {
            jassert(mPositionSourceLink == PositionSourceLink::undefined);
            mLinkStrategy = LinkStrategy::make(mElevationSourceLink);
        } else {
            jassert(mPositionSourceLink != PositionSourceLink::undefined);
            mLinkStrategy = LinkStrategy::make(mPositionSourceLink);
        }
    }
    mLinkStrategy->computeParameters(mSources, mSnapshots);
    mLinkStrategy->enforce(mSources, mSnapshots);
    if (mPositionSourceLink == PositionSourceLink::circularFixedAngle
        || mPositionSourceLink == PositionSourceLink::circularFullyFixed) {
        // circularFixedAngle & circularFullyFixed links require the snapshots to be up-to-date or else moving the
        // relative ordering with the mouse won't make any sense.
        saveCurrentPositionsToInitialStates();
        mLinkStrategy->computeParameters(mSources, mSnapshots);
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
    if (!mLinkStrategy) {
        if (mPositionSourceLink != PositionSourceLink::undefined) {
            mLinkStrategy = LinkStrategy::make(mPositionSourceLink);
        } else {
            mLinkStrategy = LinkStrategy::make(mElevationSourceLink);
        }
    }
    if (!mLinkStrategy->isInitialized()) {
        mLinkStrategy->computeParameters(mSources, mSnapshots);
    }

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
    // We need to force an update in independent mode.
    auto const isIndependent{ mElevationSourceLink == ElevationSourceLink::independent
                              || mPositionSourceLink == PositionSourceLink::independent };
    if (isIndependent) {
        mSnapshots.primary = SourceSnapshot{ mSources.getPrimarySource() };
    } else {
        enforceSourceLink();
    }
}

//==============================================================================
void SourceLinkEnforcer::secondarySourceMoved(SourceIndex const sourceIndex)
{
    jassert(sourceIndex.toInt() > 0 && sourceIndex.toInt() < MAX_NUMBER_OF_SOURCES);

    if (!mLinkStrategy) {
        return;
    }

    auto const spatMode{ mSources.getPrimarySource().getSpatMode() };
    auto const isElevationSourceLink{ mElevationSourceLink != ElevationSourceLink::undefined };
    if (spatMode == SpatMode::dome && isElevationSourceLink) {
        return;
    }

    if (mElevationSourceLink == ElevationSourceLink::independent
        || mPositionSourceLink == PositionSourceLink::independent) {
        secondarySourceAnchorMoved(sourceIndex);
        return;
    }

    if (isElevationSourceLink) {
        // get expected elevation
        auto const currentElevation{ mSources[sourceIndex].getElevation() };
        mLinkStrategy->enforce_implementation(mSources, mSnapshots, sourceIndex);
        auto const expectedElevation{ mSources[sourceIndex].getElevation() };

        // get motion
        auto const motion{ currentElevation - expectedElevation };

        // apply motion to primary source
        mSources.getPrimarySource().setElevation(mSources.getPrimarySource().getElevation() + motion,
                                                 SourceLinkBehavior::doNothing);
    } else {
        auto const primaryStart{ mSnapshots.primary };
        SourceSnapshot const primaryEnd{ mSources.getPrimarySource() };
        auto const secondaryStart{ mSnapshots[sourceIndex] };
        SourceSnapshot const secondaryEnd{ mSources[sourceIndex] };

        if (mPositionSourceLink == PositionSourceLink::circularFixedAngle
            || mPositionSourceLink == PositionSourceLink::circularFullyFixed) {
            mLinkStrategy = LinkStrategy::make(PositionSourceLink::circular);
            mLinkStrategy->computeParameters(mSources, mSnapshots);
        }

        // get motion start and end
        mLinkStrategy->enforce_implementation(mSources, mSnapshots, sourceIndex);
        SourceSnapshot const motionStart{ mSources[sourceIndex] };
        auto const motionEnd{ secondaryEnd };

        // train motion
        mSnapshots.primary = motionStart;
        mSources.getPrimarySource().setPosition(motionEnd.position, SourceLinkBehavior::doNothing);
        mLinkStrategy->computeParameters(mSources, mSnapshots);

        // apply motion to primary position
        mSnapshots.primary = primaryEnd;
        mLinkStrategy->enforce_implementation(mSources, mSnapshots, mSources.getPrimarySource().getIndex());
        SourceSnapshot const target{ mSources.getPrimarySource() };

        // enforce link
        mSnapshots.primary = primaryStart;
        mSources.getPrimarySource().setPosition(target.position, SourceLinkBehavior::doNothing);

        if (mPositionSourceLink == PositionSourceLink::circularFixedAngle
            || mPositionSourceLink == PositionSourceLink::circularFullyFixed) {
            mLinkStrategy = LinkStrategy::make(mPositionSourceLink);
        }
    }
    enforceSourceLink();
}

//==============================================================================
void SourceLinkEnforcer::primarySourceAnchorMoved()
{
    if (mLinkStrategy) {
        auto const newInitialState{ mLinkStrategy->computeInitialStateFromFinalState(
            mSources,
            mSnapshots,
            mSources.getPrimarySource().getIndex()) };
        mSnapshots.primary = newInitialState;
        enforceSourceLink();
    }
}

//==============================================================================
void SourceLinkEnforcer::secondarySourceAnchorMoved(SourceIndex const sourceIndex)
{
    jassert(sourceIndex.toInt() > 0 && sourceIndex.toInt() < MAX_NUMBER_OF_SOURCES);

    if (mLinkStrategy) {
        auto const secondaryIndex{ sourceIndex.toInt() - 1 };
        auto & snapshot{ mSnapshots.secondaries.getReference(secondaryIndex) };
        mLinkStrategy->computeParameters(mSources, mSnapshots);
        snapshot = mLinkStrategy->computeInitialStateFromFinalState(mSources, mSnapshots, sourceIndex);
        primarySourceMoved(); // some positions are invalid - fix them right away
    }
}

//==============================================================================
void SourceLinkEnforcer::saveCurrentPositionsToInitialStates()
{
    mSnapshots.primary = SourceSnapshot{ mSources.getPrimarySource() };
    mSnapshots.secondaries.clearQuick();
    for (unsigned i{}; i < mSources.getSecondarySources().size(); ++i) {
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
