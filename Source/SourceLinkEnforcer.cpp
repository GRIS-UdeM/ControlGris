/*
  ==============================================================================

    SourceLinkEnforcer.cpp
    Created: 16 Jun 2020 2:21:04pm
    Author:  samuel

  ==============================================================================
*/

#include "SourceLinkEnforcer.h"

SourceLinkEnforcer::SourceLinkEnforcer(Sources & sources, AnySourceLink const sourceLink) noexcept
    : mSources(sources)
    , mSourceLink(sourceLink)
{
    snapAll();
    for (auto & source : sources) {
        source.addSourceLinkListener(this);
    }
}

SourceLinkEnforcer::~SourceLinkEnforcer() noexcept
{
    for (auto & source : mSources) {
        source.removeSourceLinkListener(this);
    }
}

void SourceLinkEnforcer::setSourceLink(AnySourceLink sourceLink)
{
    if (sourceLink != mSourceLink) {
        mSourceLink = sourceLink;
        snapAll();
    }
}

void SourceLinkEnforcer::primarySourceMoved()
{
    if (std::holds_alternative<PositionSourceLink>(mSourceLink)) {
        switch (std::get<PositionSourceLink>(mSourceLink)) {
        case PositionSourceLink::independent:
            break;
        case PositionSourceLink::circular: {
            CircularLinkAlgo circularLinkAlgo{};
            circularLinkAlgo.calculateParams(mPrimarySourceSnapshot);
            circularLinkAlgo.apply(mSecondarySourcesSnapshots);
            break;
        }
        case PositionSourceLink::circularFixedRadius:
        case PositionSourceLink::circularFixedAngle:
        case PositionSourceLink::circularFullyFixed:
        case PositionSourceLink::linkSymmetricX:
        case PositionSourceLink::linkSymmetricY:
        case PositionSourceLink::deltaLock:
            jassertfalse;
            break;
        case PositionSourceLink::undefined:
        default:
            jassertfalse;
        }
    } else {
        jassert(std::holds_alternative<ElevationSourceLink>(mSourceLink));
    }
}

void SourceLinkEnforcer::secondarySourceMoved(SourceIndex const sourceIndex)
{
    jassert(sourceIndex.toInt() > 0 && sourceIndex.toInt() < MAX_NUMBER_OF_SOURCES);
    auto const secondaryIndex{ sourceIndex.toInt() - 1 };
    auto & snapshot{ mSecondarySourcesSnapshots.getReference(secondaryIndex) };

    if (std::holds_alternative<PositionSourceLink>(mSourceLink)) {
        switch (std::get<PositionSourceLink>(mSourceLink)) {
        case PositionSourceLink::independent:
            break;
        case PositionSourceLink::circular: {
            CircularLinkAlgo circularLinkAlgo{};
            circularLinkAlgo.calculateParams(mPrimarySourceSnapshot);
            snapshot = circularLinkAlgo.getInversedSnapshot(snapshot);
            break;
        }
        case PositionSourceLink::circularFixedRadius:
        case PositionSourceLink::circularFixedAngle:
        case PositionSourceLink::circularFullyFixed:
        case PositionSourceLink::linkSymmetricX:
        case PositionSourceLink::linkSymmetricY:
        case PositionSourceLink::deltaLock:
            jassertfalse;
            break;
        case PositionSourceLink::undefined:
        default:
            jassertfalse;
        }
    } else {
        jassert(std::holds_alternative<ElevationSourceLink>(mSourceLink));
    }

    primarySourceMoved(); // some positions are invalid - fix them right away
}

void SourceLinkEnforcer::snapAll()
{
    mPrimarySourceSnapshot.source = &mSources.getPrimarySource();
    mPrimarySourceSnapshot.takeSnapshot();
    mSecondarySourcesSnapshots.clear();
    for (auto & secondarySource : mSources.getSecondarySources()) {
        SourceSnapshot newItem;
        newItem.source = &secondarySource;
        newItem.takeSnapshot();
        mSecondarySourcesSnapshots.add(newItem);
    }
}

void SourceLinkEnforcer::changeListenerCallback(ChangeBroadcaster * broadcaster)
{
    auto sourceChangeBroadcaster{ dynamic_cast<Source::SourceChangeBroadcaster *>(broadcaster) };
    jassert(sourceChangeBroadcaster != nullptr);
    if (sourceChangeBroadcaster != nullptr) {
        auto & source{ sourceChangeBroadcaster->getSource() };
        if (source.isPrimarySource()) {
            primarySourceMoved();
        } else {
            secondarySourceMoved(source.getIndex());
        }
    }
}
