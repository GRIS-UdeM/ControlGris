/*
  ==============================================================================

    SourceLinkEnforcer.cpp
    Created: 16 Jun 2020 2:21:04pm
    Author:  samuel

  ==============================================================================
*/

#include "SourceLinkEnforcer.h"

class LinkStrategy
{
    bool mInitialized{ false };

public:
    LinkStrategy() noexcept = default;
    virtual ~LinkStrategy() noexcept = default;

    LinkStrategy(LinkStrategy const &) = default;
    LinkStrategy(LinkStrategy &&) noexcept = default;

    LinkStrategy & operator=(LinkStrategy const &) = default;
    LinkStrategy & operator=(LinkStrategy &&) = default;

    void calculateParams(SourceSnapshot const & primarySourceSnapshot)
    {
        calculateParams_impl(primarySourceSnapshot);
        mInitialized = true;
    }

    void apply(Array<SourceSnapshot> & secondarySnapshots) const
    {
        for (auto & snapshot : secondarySnapshots) {
            apply(snapshot);
        }
    }

    void apply(SourceSnapshot & snapshot) const
    {
        jassert(mInitialized);
        apply_impl(snapshot);
    }

    SourceSnapshot getInversedSnapshot(SourceSnapshot const & snapshot) const
    {
        jassert(mInitialized);
        return getInversedSnapshot_impl(snapshot);
    }

private:
    virtual void calculateParams_impl(SourceSnapshot const & primarySourceSnapshot) = 0;
    virtual void apply_impl(SourceSnapshot & snapshot) const = 0;
    virtual SourceSnapshot getInversedSnapshot_impl(SourceSnapshot const & snapshot) const = 0;
};

class CircularStrategy final : public LinkStrategy
{
private:
    Radians mRotation;
    float mRadiusRatio;

    void calculateParams_impl(SourceSnapshot const & primarySourceSnapshot) final
    {
        auto const notQuiteZero{ std::nextafter(0.0f, 1.0f) };
        mRotation = primarySourceSnapshot.source->getAzimuth() - primarySourceSnapshot.azimuth;
        auto const primarySourceInitialRadius{ primarySourceSnapshot.position.getDistanceFromOrigin() };
        auto const radius{ primarySourceInitialRadius == 0.0f
                               ? notQuiteZero
                               : primarySourceSnapshot.source->getPos().getDistanceFromOrigin()
                                     / primarySourceInitialRadius };
        mRadiusRatio = radius == 0.0f ? notQuiteZero : radius;
    }

    void apply_impl(SourceSnapshot & snapshot) const final
    {
        auto const newPosition{ snapshot.position.rotatedAboutOrigin(mRotation.getAsRadians()) * mRadiusRatio };
        snapshot.source->setPos(newPosition, SourceLinkNotification::silent);
    }

    SourceSnapshot getInversedSnapshot_impl(SourceSnapshot const & snapshot) const final
    {
        SourceSnapshot result{ snapshot };
        auto const newPosition{
            (snapshot.source->getPos() / mRadiusRatio).rotatedAboutOrigin(-mRotation.getAsRadians())
        };
        result.position = newPosition;
        return result;
    }
};

class CircularFixedRadiusStrategy : public LinkStrategy
{
private:
    Radians mRotation;
    float mRadius;

    void calculateParams_impl(SourceSnapshot const & primarySourceSnapshot) final
    {
        mRotation = primarySourceSnapshot.source->getAzimuth() - primarySourceSnapshot.azimuth;
        mRadius = primarySourceSnapshot.source->getPos().getDistanceFromOrigin();
    }

    void apply_impl(SourceSnapshot & snapshot) const final
    {
        Radians const oldAngle{ std::atan2(snapshot.position.getY(), snapshot.position.getX()) };
        auto const newAngle{ (mRotation + oldAngle).getAsRadians() };
        Point<float> const newPosition{ std::cos(newAngle) * mRadius, std::sin(newAngle) * mRadius };

        snapshot.source->setPos(newPosition, SourceLinkNotification::silent);
    }

    SourceSnapshot getInversedSnapshot_impl(SourceSnapshot const & snapshot) const final
    {
        auto const sourcePosition{ snapshot.source->getPos() };
        SourceSnapshot result{ snapshot };

        Radians const sourceAngle{ std::atan2(sourcePosition.getY(), sourcePosition.getX()) };
        auto const inversedAngle{ (sourceAngle - mRotation).getAsRadians() };
        Point<float> const newPosition{ std::cos(inversedAngle) * mRadius, std::sin(inversedAngle) * mRadius };
        result.position = newPosition;

        return result;
    }
};

SourceLinkEnforcer::SourceLinkEnforcer(Sources & sources, AnySourceLink const sourceLink) noexcept
    : mSources(sources)
    , mSourceLink(sourceLink)
{
    reset();
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

void SourceLinkEnforcer::numberOfSourcesChanged()
{
    reset();
}

void SourceLinkEnforcer::primarySourceMoved()
{
    std::unique_ptr<LinkStrategy> strategy{};

    if (std::holds_alternative<PositionSourceLink>(mSourceLink)) {
        switch (std::get<PositionSourceLink>(mSourceLink)) {
        case PositionSourceLink::independent:
            break;
        case PositionSourceLink::circular: {
            strategy.reset(new CircularStrategy{});
            break;
        }
        case PositionSourceLink::circularFixedRadius: {
            strategy.reset(new CircularFixedRadiusStrategy{});
            break;
        }
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

    if (strategy != nullptr) {
        strategy->calculateParams(mPrimarySourceSnapshot);
        strategy->apply(mSecondarySourcesSnapshots);
    }
}

void SourceLinkEnforcer::secondarySourceMoved(SourceIndex const sourceIndex)
{
    jassert(sourceIndex.toInt() > 0 && sourceIndex.toInt() < MAX_NUMBER_OF_SOURCES);
    auto const secondaryIndex{ sourceIndex.toInt() - 1 };
    auto & snapshot{ mSecondarySourcesSnapshots.getReference(secondaryIndex) };

    std::unique_ptr<LinkStrategy> strategy{};

    if (std::holds_alternative<PositionSourceLink>(mSourceLink)) {
        switch (std::get<PositionSourceLink>(mSourceLink)) {
        case PositionSourceLink::independent:
            break;
        case PositionSourceLink::circular: {
            strategy.reset(new CircularStrategy{});
            break;
        }
        case PositionSourceLink::circularFixedRadius: {
            strategy.reset(new CircularFixedRadiusStrategy{});
            break;
        }
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

    if (strategy != nullptr) {
        strategy->calculateParams(mPrimarySourceSnapshot);
        snapshot = strategy->getInversedSnapshot(snapshot);
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

void SourceLinkEnforcer::reset()
{
    for (auto & source : mSources) {
        source.removeSourceLinkListener(this);
    }
    snapAll();
    for (auto & source : mSources) {
        source.addSourceLinkListener(this);
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
