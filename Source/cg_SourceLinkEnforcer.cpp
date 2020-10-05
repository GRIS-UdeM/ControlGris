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

#include "cg_SourceLinkEnforcer.hpp"

#include "cg_constants.hpp"

//==============================================================================
SourceLinkEnforcer::SourceLinkEnforcer(Sources & sources, PositionSourceLink const sourceLink) : mSources(sources)
{
    setSourceLink(sourceLink);
}

//==============================================================================
SourceLinkEnforcer::SourceLinkEnforcer(Sources & sources, ElevationSourceLink const sourceLink) : mSources(sources)
{
    setSourceLink(sourceLink);
}

//==============================================================================
void SourceLinkEnforcer::setSourceLink(PositionSourceLink const sourceLink)
{
    jassert(sourceLink != PositionSourceLink::undefined);
    if (sourceLink != mPositionSourceLink) {
        mPositionSourceLink = sourceLink;
        mElevationSourceLink = ElevationSourceLink::undefined;
        mLinkStrategy = LinkStrategy::make(sourceLink);
        saveCurrentPositionsToInitialStates();
        enforceSourceLink();
    }
}

//==============================================================================
void SourceLinkEnforcer::setSourceLink(ElevationSourceLink const sourceLink)
{
    jassert(sourceLink != ElevationSourceLink::undefined);
    if (sourceLink != mElevationSourceLink) {
        mElevationSourceLink = sourceLink;
        mPositionSourceLink = PositionSourceLink::undefined;
        mLinkStrategy = LinkStrategy::make(sourceLink);
        saveCurrentPositionsToInitialStates();
        enforceSourceLink();
    }
}

//==============================================================================
void SourceLinkEnforcer::enforceSourceLink()
{
    mLinkStrategy->computeParameters(mSources, mSnapshots);
    mLinkStrategy->enforce(mSources, mSnapshots);
    if (mPositionSourceLink == PositionSourceLink::circularFixedAngle
        || mPositionSourceLink == PositionSourceLink::circularFullyFixed) {
        // circularFixedAngle & circularFullyFixed links require the snapshots to be up-to-date or else moving the
        // relative ordering with the mouse won't make any sense.
        saveCurrentPositionsToInitialStates();
    }
}

//==============================================================================
void SourceLinkEnforcer::sourceMoved(Source & source)
{
    if (source.isPrimarySource()) {
        primarySourceMoved();
    } else {
        secondarySourceMoved(source.getIndex());
    }
}

//==============================================================================
void SourceLinkEnforcer::anchorMoved(Source & source)
{
    if (source.isPrimarySource()) {
        primaryAnchorMoved();
    } else {
        secondaryAnchorMoved(source.getIndex());
    }
}

//==============================================================================
void SourceLinkEnforcer::numberOfSourcesChanged()
{
    saveCurrentPositionsToInitialStates();
}

//==============================================================================
void SourceLinkEnforcer::primarySourceMoved()
{
    enforceSourceLink();
}

//==============================================================================
void SourceLinkEnforcer::secondarySourceMoved(SourceIndex const sourceIndex)
{
    jassert(sourceIndex.toInt() > 0 && sourceIndex.toInt() < MAX_NUMBER_OF_SOURCES);

    auto const spatMode{ mSources.getPrimarySource().getSpatMode() };
    auto const isElevationSourceLink{ mElevationSourceLink != ElevationSourceLink::undefined };
    if (spatMode == SpatMode::dome && isElevationSourceLink) {
        return;
    }

    if (mElevationSourceLink == ElevationSourceLink::independent
        || mPositionSourceLink == PositionSourceLink::independent) {
        secondaryAnchorMoved(sourceIndex);
        return;
    }

    if (isElevationSourceLink) {
        // get expected elevation
        auto const currentElevation{ mSources[sourceIndex].getElevation() };
        mLinkStrategy->computeParameters(mSources, mSnapshots);
        mLinkStrategy->enforce_implementation(mSources, mSnapshots, sourceIndex);
        auto const expectedElevation{ mSources[sourceIndex].getElevation() };

        // get motion
        auto const motion{ currentElevation - expectedElevation };

        // apply motion to primary source
        mSources.getPrimarySource().setElevation(mSources.getPrimarySource().getElevation() + motion,
                                                 Source::OriginOfChange::link);
    } else {
        auto const primaryStart{ mSnapshots.primary };
        SourceSnapshot const primaryEnd{ mSources.getPrimarySource() };
        auto const secondaryStart{ mSnapshots[sourceIndex] };
        SourceSnapshot const secondaryEnd{ mSources[sourceIndex] };

        if (mPositionSourceLink == PositionSourceLink::circularFixedAngle
            || mPositionSourceLink == PositionSourceLink::circularFullyFixed) {
            mLinkStrategy = LinkStrategy::make(PositionSourceLink::circular);
        }
        mLinkStrategy->computeParameters(mSources, mSnapshots);

        // get motion start and end
        mLinkStrategy->enforce_implementation(mSources, mSnapshots, sourceIndex);
        SourceSnapshot const motionStart{ mSources[sourceIndex] };
        auto const motionEnd{ secondaryEnd };

        // train motion
        mSnapshots.primary = motionStart;
        mSources.getPrimarySource().setPosition(motionEnd.position, Source::OriginOfChange::none);
        mLinkStrategy->computeParameters(mSources, mSnapshots);

        // apply motion to secondary source (temp)
        mSnapshots.primary = primaryEnd;
        mSnapshots[sourceIndex] = primaryEnd;
        mLinkStrategy->enforce_implementation(mSources, mSnapshots, sourceIndex);
        SourceSnapshot const target{ mSources[sourceIndex] };

        // rebuild source snapshot
        mSnapshots[sourceIndex] = secondaryStart;

        // enforce link
        if (mPositionSourceLink == PositionSourceLink::circularFixedAngle
            || mPositionSourceLink == PositionSourceLink::circularFullyFixed) {
            if (mPositionSourceLink == PositionSourceLink::undefined) {
                mLinkStrategy = LinkStrategy::make(mElevationSourceLink);
            } else {
                mLinkStrategy = LinkStrategy::make(mPositionSourceLink);
            }
        }
        mSnapshots.primary = primaryStart;
        mSources.getPrimarySource().setPosition(target.position, Source::OriginOfChange::link);
    }
}

//==============================================================================
void SourceLinkEnforcer::primaryAnchorMoved()
{
    // mLinkStrategy->computeParameters(mSources, mSnapshots);
    auto const newPrimarySnapshot{
        mLinkStrategy->computeInitialStateFromFinalState(mSources, mSnapshots, mSources.getPrimarySource().getIndex())
    };
    mSnapshots.primary = newPrimarySnapshot;
    enforceSourceLink();
}

//==============================================================================
void SourceLinkEnforcer::secondaryAnchorMoved(SourceIndex const sourceIndex)
{
    jassert(sourceIndex.toInt() > 0 && sourceIndex.toInt() < MAX_NUMBER_OF_SOURCES);

    auto const secondaryIndex{ sourceIndex.toInt() - 1 };
    auto & snapshot{ mSnapshots.secondaries.getReference(secondaryIndex) };
    mLinkStrategy->computeParameters(mSources, mSnapshots);
    auto const newSecondaryAnchor{
        mLinkStrategy->computeInitialStateFromFinalState(mSources, mSnapshots, sourceIndex)
    };
    snapshot = newSecondaryAnchor;
    primarySourceMoved(); // some positions are invalid - fix them right away
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

// std::unique_ptr<LinkStrategy> SourceLinkEnforcer::getStrategy() const
//{
//    if (mPositionSourceLink != PositionSourceLink::undefined) {
//        jassert(mElevationSourceLink == ElevationSourceLink::undefined);
//        return LinkStrategy::make(mPositionSourceLink);
//    }
//    jassert(mElevationSourceLink != ElevationSourceLink::undefined);
//    return LinkStrategy::make(mElevationSourceLink);
//}