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
        case PositionSourceLink::circular:
            applyCircular();
            break;
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
    mSecondarySourcesSnapshots.getReference(secondaryIndex).snapShot();
    primarySourceMoved(); // some positions are invalid - fix them right away
}

void SourceLinkEnforcer::applyCircular()
{
    auto const notQuiteZero{ std::nextafter(0.0f, 1.0f) };
    auto const rotation{ mSources.getPrimarySource().getAzimuth() - mPrimarySourceSnapshot.azimuth };
    auto const primarySourceInitialRadius{ mPrimarySourceSnapshot.position.getDistanceFromOrigin() };
    auto const radiusRatio{ primarySourceInitialRadius == 0.0f
                                ? notQuiteZero
                                : mSources.getPrimarySource().getPos().getDistanceFromOrigin()
                                      / primarySourceInitialRadius };
    for (auto & snapShot : mSecondarySourcesSnapshots) {
        auto const newPosition{ snapShot.position.rotatedAboutOrigin(rotation.getAsRadians()) * radiusRatio };
        snapShot.source->setPos(newPosition, SourceLinkNotification::silent);
    }
}

void SourceLinkEnforcer::snapAll()
{
    mPrimarySourceSnapshot.source = &mSources.getPrimarySource();
    mPrimarySourceSnapshot.snapShot();
    for (auto & secondarySource : mSources.getSecondarySources()) {
        SourceSnapshot newItem;
        newItem.source = &secondarySource;
        newItem.snapShot();
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