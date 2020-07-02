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
class LinkStrategy
{
private:
    bool mInitialized{ false };

public:
    //==============================================================================
    LinkStrategy() noexcept = default;
    virtual ~LinkStrategy() noexcept = default;

    LinkStrategy(LinkStrategy const &) = default;
    LinkStrategy(LinkStrategy &&) noexcept = default;

    LinkStrategy & operator=(LinkStrategy const &) = default;
    LinkStrategy & operator=(LinkStrategy &&) = default;
    //==============================================================================
    void calculateParams(Source const & primarySource,
                         SourceSnapshot const & primarySourceSnapshot,
                         int const numberOfSources)
    {
        calculateParams_impl(primarySource, primarySourceSnapshot, numberOfSources);
        mInitialized = true;
    }
    //==============================================================================
    void apply(std::array<Source, MAX_NUMBER_OF_SOURCES - 1> & sources, Array<SourceSnapshot> const & snapshots) const
    {
        for (int i{}; i < snapshots.size(); ++i) { // TODO: this is applied to more sources than it should
            auto & source{ sources[i] };
            auto const & snapshot{ snapshots.getReference(i) };
            apply(source, snapshot);
        }
    }
    //==============================================================================
    void apply(Source & source, SourceSnapshot const & snapshot) const
    {
        jassert(mInitialized);
        apply_impl(source, snapshot);
    }
    //==============================================================================
    [[nodiscard]] SourceSnapshot getInversedSnapshot(Source const & source, SourceSnapshot const & snapshot) const
    {
        jassert(mInitialized);
        return getInversedSnapshot_impl(source, snapshot);
    }

private:
    //==============================================================================
    virtual void calculateParams_impl(Source const & primarySource,
                                      SourceSnapshot const & primarySourceSnapshot,
                                      int numberOfSources)
        = 0;
    virtual void apply_impl(Source & source, SourceSnapshot const & snapshot) const = 0;
    [[nodiscard]] virtual SourceSnapshot getInversedSnapshot_impl(Source const & source,
                                                                  SourceSnapshot const & snapshot) const = 0;
};

//==============================================================================
// only use full to recall saved positions
class IndependentStrategy : public LinkStrategy
{
private:
    //==============================================================================
    void calculateParams_impl([[maybe_unused]] Source const & primarySource,
                              [[maybe_unused]] SourceSnapshot const & primarySourceSnapshot,
                              [[maybe_unused]] int const numberOfSources) final
    {
    }
    //==============================================================================
    void apply_impl(Source & source, SourceSnapshot const & snapshot) const final
    {
        source.setPos(snapshot.position, SourceLinkNotification::silent);
    }
    //==============================================================================
    [[nodiscard]] SourceSnapshot getInversedSnapshot_impl(Source const & source,
                                                          SourceSnapshot const & snapshot) const final
    {
        SourceSnapshot const result{ source };
        return result;
    }
};

//==============================================================================
class CircularStrategy final : public LinkStrategy
{
private:
    Radians mRotation{};
    float mRadiusRatio{};
    //==============================================================================
    void calculateParams_impl(Source const & primarySource,
                              SourceSnapshot const & primarySourceSnapshot,
                              [[maybe_unused]] int const numberOfSources) final
    {
        auto constexpr notQuiteZero{ 0.01f };
        auto const initialAngle{ Radians::fromPoint(primarySourceSnapshot.position) };
        auto const terminalAngle{ Radians::fromPoint(primarySource.getPos()) };
        mRotation = terminalAngle - initialAngle;
        auto const primarySourceInitialRadius{ primarySourceSnapshot.position.getDistanceFromOrigin() };
        auto const radius{ primarySourceInitialRadius == 0.0f
                               ? notQuiteZero
                               : primarySource.getPos().getDistanceFromOrigin() / primarySourceInitialRadius };
        mRadiusRatio = radius == 0.0f ? notQuiteZero : radius;
    }
    //==============================================================================
    void apply_impl(Source & source, SourceSnapshot const & snapshot) const final
    {
        auto const newPosition{ snapshot.position.rotatedAboutOrigin(mRotation.getAsRadians()) * mRadiusRatio };
        source.setPos(newPosition, SourceLinkNotification::silent);
    }
    //==============================================================================
    [[nodiscard]] SourceSnapshot getInversedSnapshot_impl(Source const & source,
                                                          SourceSnapshot const & snapshot) const final
    {
        SourceSnapshot result{ snapshot };
        auto const newPosition{ (source.getPos() / mRadiusRatio).rotatedAboutOrigin(-mRotation.getAsRadians()) };
        result.position = newPosition;
        return result;
    }
};

//==============================================================================
class CircularFixedRadiusStrategy : public LinkStrategy
{
private:
    Radians mRotation{};
    float mRadius{};
    //==============================================================================
    void calculateParams_impl(Source const & primarySource,
                              SourceSnapshot const & primarySourceSnapshot,
                              [[maybe_unused]] int const numberOfSources) final
    {
        auto const initialAngle{ Radians::fromPoint(primarySource.getPos()) };
        auto const terminalAngle{ Radians::fromPoint(primarySourceSnapshot.position) };
        mRotation = initialAngle - terminalAngle;
        mRadius = primarySource.getPos().getDistanceFromOrigin();
    }
    //==============================================================================
    void apply_impl(Source & source, SourceSnapshot const & snapshot) const final
    {
        Radians const oldAngle{ std::atan2(snapshot.position.getY(), snapshot.position.getX()) };
        auto const newAngle{ (mRotation + oldAngle).getAsRadians() };
        Point<float> const newPosition{ std::cos(newAngle) * mRadius, std::sin(newAngle) * mRadius };

        source.setPos(newPosition, SourceLinkNotification::silent);
    }
    //==============================================================================
    [[nodiscard]] SourceSnapshot getInversedSnapshot_impl(Source const & source,
                                                          SourceSnapshot const & snapshot) const final
    {
        auto const sourcePosition{ source.getPos() };
        SourceSnapshot result{ snapshot };

        Radians const sourceAngle{ std::atan2(sourcePosition.getY(), sourcePosition.getX()) };
        auto const inversedAngle{ (sourceAngle - mRotation).getAsRadians() };
        Point<float> const newPosition{ std::cos(inversedAngle) * mRadius, std::sin(inversedAngle) * mRadius };

        result.position = newPosition;
        return result;
    }
};

//==============================================================================
class CircularFixedAngleStrategy : public LinkStrategy
{
private:
    Radians mDeviationPerSource{};
    Radians mPrimaySourceAngle{};
    float mRadiusRatio{};
    //==============================================================================
    void calculateParams_impl(Source const & primarySource,
                              SourceSnapshot const & primarySourceSnapshot,
                              int const numberOfSources) final
    {
        auto const notQuiteZero{ std::nextafter(0.0f, 1.0f) };
        auto const initialRadius{ std::max(primarySourceSnapshot.position.getDistanceFromOrigin(), notQuiteZero) };
        mRadiusRatio = std::max(primarySource.getPos().getDistanceFromOrigin() / initialRadius, notQuiteZero);

        auto const sourcePosition{ primarySource.getPos() };
        mPrimaySourceAngle = Radians{ std::atan2(sourcePosition.getY(), sourcePosition.getX()) };
        mDeviationPerSource = Degrees{ 360 } / numberOfSources;
    }
    //==============================================================================
    void apply_impl(Source & source, SourceSnapshot const & snapshot) const final
    {
        auto const sourceIndex{ source.getIndex() };
        auto const newAngle{ mPrimaySourceAngle + mDeviationPerSource * sourceIndex.toInt() };
        auto const initialRadius{ snapshot.position.getDistanceFromOrigin() };
        auto const newRadius{ initialRadius * mRadiusRatio };
        Point<float> const newPosition{ std::cos(newAngle.getAsRadians()) * newRadius,
                                        std::sin(newAngle.getAsRadians()) * newRadius };

        source.setPos(newPosition, SourceLinkNotification::silent);
    }
    //==============================================================================
    [[nodiscard]] SourceSnapshot getInversedSnapshot_impl(Source const & source,
                                                          SourceSnapshot const & snapshot) const final
    {
        SourceSnapshot result{ snapshot };

        auto const newRadius{ source.getPos().getDistanceFromOrigin() / mRadiusRatio };
        Point<float> const newPosition{ newRadius, 0.0f }; // we only care about changing the radius

        result.position = newPosition;

        return result;
    }
};

//==============================================================================
class CircularFullyFixedStrategy : public LinkStrategy
{
    Radians mDeviationPerSource{};
    Radians mPrimaySourceAngle{};
    float mRadius{};
    //==============================================================================
    void calculateParams_impl(Source const & primarySource,
                              SourceSnapshot const & primarySourceSnapshot,
                              int const numberOfSources) final
    {
        mDeviationPerSource = Degrees{ 360.0f } / numberOfSources;
        auto const primarySourcePosition{ primarySource.getPos() };
        mPrimaySourceAngle = Radians{ std::atan2(primarySourcePosition.getY(), primarySourcePosition.getX()) };
        mRadius = primarySourcePosition.getDistanceFromOrigin();
    }
    //==============================================================================
    void apply_impl(Source & source, SourceSnapshot const & snapshot) const final
    {
        auto const secondaryIndex{ source.getIndex() };
        auto const angle{ mPrimaySourceAngle + mDeviationPerSource * secondaryIndex.toInt() };
        Point<float> newPosition{ std::cos(angle.getAsRadians()) * mRadius, std::sin(angle.getAsRadians()) * mRadius };

        source.setPos(newPosition, SourceLinkNotification::silent);
    }
    //==============================================================================
    [[nodiscard]] SourceSnapshot getInversedSnapshot_impl([[maybe_unused]] Source const & source,
                                                          [[maybe_unused]] SourceSnapshot const & snapshot) const final
    {
        // nothing to do here!
        return snapshot;
    }
};

//==============================================================================
class LinkSymmetricXStrategy : public LinkStrategy
{
    Point<float> mPrimaryPosition;
    //==============================================================================
    void calculateParams_impl(Source const & primarySource,
                              SourceSnapshot const & primarySourceSnapshot,
                              [[maybe_unused]] int const numberOfSources) final
    {
        mPrimaryPosition = primarySource.getPos();
    }
    //==============================================================================
    void apply_impl(Source & source, [[maybe_unused]] SourceSnapshot const & snapshot) const final
    {
        Point<float> const newPosition{ -mPrimaryPosition.getX(), mPrimaryPosition.getY() };
        source.setPos(newPosition, SourceLinkNotification::silent);
    }
    //==============================================================================
    [[nodiscard]] SourceSnapshot getInversedSnapshot_impl([[maybe_unused]] Source const & source,
                                                          [[maybe_unused]] SourceSnapshot const & snapshot) const final
    {
        // nothing to do here!
        return snapshot;
    }
};

//==============================================================================
class LinkSymmetricYStrategy : public LinkStrategy
{
    Point<float> mPrimaryPosition;
    //==============================================================================
    void calculateParams_impl(Source const & primarySource,
                              SourceSnapshot const & primarySourceSnapshot,
                              [[maybe_unused]] int const numberOfSources) final
    {
        mPrimaryPosition = primarySource.getPos();
    }
    //==============================================================================
    void apply_impl(Source & source, [[maybe_unused]] SourceSnapshot const & snapshot) const final
    {
        Point<float> const newPosition{ mPrimaryPosition.getX(), -mPrimaryPosition.getY() };
        source.setPos(newPosition, SourceLinkNotification::silent);
    }
    //==============================================================================
    [[nodiscard]] SourceSnapshot getInversedSnapshot_impl([[maybe_unused]] Source const & source,
                                                          [[maybe_unused]] SourceSnapshot const & snapshot) const final
    {
        // nothing to do here!
        return snapshot;
    }
};

//==============================================================================
class DeltaLockStrategy : public LinkStrategy
{
    Point<float> mDelta;
    //==============================================================================
    void calculateParams_impl(Source const & primarySource,
                              SourceSnapshot const & primarySourceSnapshot,
                              [[maybe_unused]] int const numberOfSources) final
    {
        mDelta = primarySource.getPos() - primarySourceSnapshot.position;
    }
    //==============================================================================
    void apply_impl(Source & source, SourceSnapshot const & snapshot) const final
    {
        auto const newPosition{ snapshot.position + mDelta };
        source.setPos(newPosition, SourceLinkNotification::silent);
    }
    //==============================================================================
    [[nodiscard]] SourceSnapshot getInversedSnapshot_impl(Source const & source,
                                                          SourceSnapshot const & snapshot) const final
    {
        SourceSnapshot result{ snapshot };

        auto const initialPos{ source.getPos() - mDelta };
        result.position = initialPos;

        return result;
    }
};

//==============================================================================
// only usefuLl to recall saved positions
class IndependentElevationStrategy : public LinkStrategy
{
private:
    //==============================================================================
    void calculateParams_impl([[maybe_unused]] Source const & primarySource,
                              [[maybe_unused]] SourceSnapshot const & primarySourceSnapshot,
                              [[maybe_unused]] int const numberOfSources) final
    {
    }
    //==============================================================================
    void apply_impl(Source & source, SourceSnapshot const & snapshot) const final
    {
        source.setElevation(snapshot.z, SourceLinkNotification::silent);
    }
    //==============================================================================
    [[nodiscard]] SourceSnapshot getInversedSnapshot_impl(Source const & source,
                                                          [[maybe_unused]] SourceSnapshot const & snapshot) const final
    {
        SourceSnapshot const result{ source };
        return result;
    }
};

//==============================================================================
class FixedElevationStrategy : public LinkStrategy
{
    Radians mElevation{};
    //==============================================================================
    void calculateParams_impl(Source const & primarySource,
                              SourceSnapshot const & primarySourceSnapshot,
                              [[maybe_unused]] int const numberOfSources) final
    {
        mElevation = primarySource.getElevation();
    }
    //==============================================================================
    void apply_impl(Source & source, SourceSnapshot const & snapshot) const final
    {
        source.setElevation(mElevation, SourceLinkNotification::silent);
    }
    //==============================================================================
    [[nodiscard]] SourceSnapshot getInversedSnapshot_impl(Source const & source,
                                                          SourceSnapshot const & snapshot) const final
    {
        return snapshot;
    }
};

//==============================================================================
class LinearMinElevationStrategy : public LinkStrategy
{
    static constexpr Radians ELEVATION_DIFF{ -MAX_ELEVATION / 3.0f * 2.0f };
    Radians mBaseElevation{};
    Radians mElevationPerSource{};
    //==============================================================================
    void calculateParams_impl(Source const & primarySource,
                              SourceSnapshot const & primarySourceSnapshot,
                              [[maybe_unused]] int const numberOfSources) final
    {
        mBaseElevation = primarySource.getElevation();
        mElevationPerSource = ELEVATION_DIFF / (numberOfSources - 1);
    }
    //==============================================================================
    void apply_impl(Source & source, SourceSnapshot const & snapshot) const final
    {
        auto const sourceIndex{ source.getIndex().toInt() };
        auto const newElevation{ mBaseElevation + mElevationPerSource * sourceIndex };
        source.setElevation(newElevation, SourceLinkNotification::silent);
    }
    //==============================================================================
    [[nodiscard]] SourceSnapshot getInversedSnapshot_impl(Source const & source,
                                                          SourceSnapshot const & snapshot) const final
    {
        return snapshot;
    }
};

//==============================================================================
class LinearMaxElevationStrategy : public LinkStrategy
{
    static constexpr Radians ELEVATION_DIFF{ MAX_ELEVATION / 3.0f * 2.0f };
    Radians mBaseElevation{};
    Radians mElevationPerSource{};
    //==============================================================================
    void calculateParams_impl(Source const & primarySource,
                              SourceSnapshot const & primarySourceSnapshot,
                              [[maybe_unused]] int const numberOfSources) final
    {
        mBaseElevation = primarySource.getElevation();
        mElevationPerSource = ELEVATION_DIFF / (numberOfSources - 1);
    }
    //==============================================================================
    void apply_impl(Source & source, SourceSnapshot const & snapshot) const final
    {
        auto const sourceIndex{ source.getIndex().toInt() };
        auto const newElevation{ mBaseElevation + mElevationPerSource * sourceIndex };
        source.setElevation(newElevation, SourceLinkNotification::silent);
    }
    //==============================================================================
    [[nodiscard]] SourceSnapshot getInversedSnapshot_impl(Source const & source,
                                                          SourceSnapshot const & snapshot) const final
    {
        return snapshot;
    }
};

//==============================================================================
class DeltaLockElevationStrategy : public LinkStrategy
{
    Radians mDelta;
    //==============================================================================
    void calculateParams_impl(Source const & primarySource,
                              SourceSnapshot const & primarySourceSnapshot,
                              [[maybe_unused]] int const numberOfSources) final
    {
        mDelta = primarySource.getElevation() - primarySourceSnapshot.z;
    }
    //==============================================================================
    void apply_impl(Source & source, SourceSnapshot const & snapshot) const final
    {
        auto const newElevation{ snapshot.z + mDelta };
        source.setElevation(newElevation, SourceLinkNotification::silent);
    }
    //==============================================================================
    [[nodiscard]] SourceSnapshot getInversedSnapshot_impl(Source const & source,
                                                          SourceSnapshot const & snapshot) const final
    {
        SourceSnapshot result{ snapshot };

        auto const initialElevation{ source.getElevation() - mDelta };
        result.z = initialElevation;

        return result;
    }
};

//==============================================================================
std::unique_ptr<LinkStrategy> getLinkStrategy(AnySourceLink const sourceLink)
{
    if (std::holds_alternative<PositionSourceLink>(sourceLink)) {
        switch (std::get<PositionSourceLink>(sourceLink)) {
        case PositionSourceLink::independent:
            return std::make_unique<IndependentStrategy>();
        case PositionSourceLink::circular:
            return std::make_unique<CircularStrategy>();
        case PositionSourceLink::circularFixedRadius:
            return std::make_unique<CircularFixedRadiusStrategy>();
        case PositionSourceLink::circularFixedAngle:
            return std::make_unique<CircularFixedAngleStrategy>();
        case PositionSourceLink::circularFullyFixed:
            return std::make_unique<CircularFullyFixedStrategy>();
        case PositionSourceLink::linkSymmetricX:
            return std::make_unique<LinkSymmetricXStrategy>();
        case PositionSourceLink::linkSymmetricY:
            return std::make_unique<LinkSymmetricYStrategy>();
        case PositionSourceLink::deltaLock:
            return std::make_unique<DeltaLockStrategy>();
        case PositionSourceLink::undefined:
        default:
            jassertfalse;
        }
    } else {
        jassert(std::holds_alternative<ElevationSourceLink>(sourceLink));
        switch (std::get<ElevationSourceLink>(sourceLink)) {
        case ElevationSourceLink::independent:
            return std::make_unique<IndependentElevationStrategy>();
            ;
        case ElevationSourceLink::fixedElevation:
            return std::make_unique<FixedElevationStrategy>();
        case ElevationSourceLink::linearMin:
            return std::make_unique<LinearMinElevationStrategy>();
        case ElevationSourceLink::linearMax:
            return std::make_unique<LinearMaxElevationStrategy>();
        case ElevationSourceLink::deltaLock:
            return std::make_unique<DeltaLockElevationStrategy>();
        case ElevationSourceLink::undefined:
        default:
            jassertfalse;
        }
    }
    jassertfalse;
    return nullptr;
}

//==============================================================================
SourceLinkEnforcer::SourceLinkEnforcer(Sources & sources, AnySourceLink const sourceLink) noexcept
    : mSources(sources)
    , mSourceLink(sourceLink)
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
void SourceLinkEnforcer::setSourceLink(AnySourceLink const sourceLink)
{
    if (sourceLink != mSourceLink) {
        mSourceLink = sourceLink;
        snapAll();
    }
}

//==============================================================================
void SourceLinkEnforcer::enforceSourceLink()
{
    auto strategy{ getLinkStrategy(mSourceLink) };

    if (strategy != nullptr) {
        strategy->calculateParams(mSources.getPrimarySource(), mSnapshots.primary, mSources.size());
        strategy->apply(mSources.getSecondarySources(), mSnapshots.secondaries);
    }
}

//==============================================================================
void SourceLinkEnforcer::loadSnapshots(SourcesSnapshots const & snapshots)
{
    mSnapshots = snapshots;
    enforceSourceLink();
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
}

//==============================================================================
void SourceLinkEnforcer::secondarySourceMoved(SourceIndex const sourceIndex)
{
    jassert(sourceIndex.toInt() > 0 && sourceIndex.toInt() < MAX_NUMBER_OF_SOURCES);
    auto & source{ mSources[sourceIndex] };
    auto const secondaryIndex{ sourceIndex.toInt() - 1 };
    auto & snapshot{ mSnapshots.secondaries.getReference(secondaryIndex) };
    auto strategy{ getLinkStrategy(mSourceLink) };

    if (strategy != nullptr) {
        strategy->calculateParams(mSources.getPrimarySource(), mSnapshots.primary, mSources.size());
        snapshot = strategy->getInversedSnapshot(source, snapshot);
        primarySourceMoved(); // some positions are invalid - fix them right away
    }
}

//==============================================================================
void SourceLinkEnforcer::snapAll()
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
    snapAll();
    for (auto & source : mSources) {
        source.addSourceLinkListener(this);
    }
}

//==============================================================================
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
