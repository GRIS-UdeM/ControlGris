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
    void computeParameters(Sources const & finalStates, SourcesSnapshots const & initialStates)
    {
        computeParameters_implementation(finalStates, initialStates);
        mInitialized = true;
    }
    //==============================================================================
    void enforce(std::array<Source, MAX_NUMBER_OF_SOURCES - 1> & finalSecondaryStates,
                 Array<SourceSnapshot> const & initialSecondaryStates) const
    {
        for (size_t i{}; i < initialSecondaryStates.size();
             ++i) { // TODO: this is applied to more sources than it should
            auto & finalState{ finalSecondaryStates[i] };
            auto const & initialState{ initialSecondaryStates.getReference(i) };
            enforce(finalState, initialState);
        }
    }
    //==============================================================================
    void enforce(Source & finalState, SourceSnapshot const & initialState) const
    {
        jassert(mInitialized);
        enforce_implementation(finalState, initialState);
    }
    //==============================================================================
    [[nodiscard]] SourceSnapshot computeInitialStateFromFinalState(Source const & finalState,
                                                                   SourceSnapshot const & initialState) const
    {
        jassert(mInitialized);
        return computeInitialStateFromFinalState_implementation(finalState, initialState);
    }

private:
    //==============================================================================
    virtual void computeParameters_implementation(Sources const & finalStates, SourcesSnapshots const & initialStates)
        = 0;
    virtual void enforce_implementation(Source & finalState, SourceSnapshot const & initialState) const = 0;
    [[nodiscard]] virtual SourceSnapshot
        computeInitialStateFromFinalState_implementation(Source const & finalState,
                                                         SourceSnapshot const & initialState) const = 0;
    //==============================================================================
    JUCE_LEAK_DETECTOR(LinkStrategy)

}; // class LinkStrategy

//==============================================================================
// only use full to recall saved positions
class IndependentStrategy final : public LinkStrategy
{
    void computeParameters_implementation(Sources const &, SourcesSnapshots const &) override {}
    //==============================================================================
    void enforce_implementation(Source & finalState, SourceSnapshot const & initialState) const override
    {
        finalState.setPos(initialState.position, SourceLinkNotification::silent);
    }
    //==============================================================================
    [[nodiscard]] SourceSnapshot computeInitialStateFromFinalState_implementation(
        Source const & finalState,
        [[maybe_unused]] SourceSnapshot const & initialState) const override
    {
        SourceSnapshot const result{ finalState };
        return result;
    }

private:
    JUCE_LEAK_DETECTOR(IndependentStrategy)
};

//==============================================================================
class CircularStrategy final : public LinkStrategy
{
    Radians mRotation{};
    float mRadiusRatio{};
    //==============================================================================
    void computeParameters_implementation(Sources const & finalState, SourcesSnapshots const & initialStates) override
    {
        auto const & primarySourceFinalState{ finalState.getPrimarySource() };
        auto const & primarySourceInitialState{ initialStates.primary };

        auto constexpr notQuiteZero{ 0.01f };
        auto const primarySourceInitialAngle{ Radians::fromPoint(primarySourceInitialState.position) };
        auto const primarySourceFinalAngle{ Radians::fromPoint(primarySourceFinalState.getPos()) };
        mRotation = primarySourceFinalAngle - primarySourceInitialAngle;
        auto const primarySourceInitialRadius{ primarySourceInitialState.position.getDistanceFromOrigin() };
        auto const primarySourceFinalRadius{ primarySourceFinalState.getPos().getDistanceFromOrigin() };
        auto const radiusRatio{ primarySourceInitialRadius == 0.0f
                                    ? notQuiteZero
                                    : primarySourceFinalRadius / primarySourceInitialRadius };
        mRadiusRatio = radiusRatio == 0.0f ? notQuiteZero : radiusRatio;
    }
    //==============================================================================
    void enforce_implementation(Source & finalState, SourceSnapshot const & initialState) const override
    {
        auto const finalPosition{ initialState.position.rotatedAboutOrigin(mRotation.getAsRadians()) * mRadiusRatio };
        finalState.setPos(finalPosition, SourceLinkNotification::silent);
    }
    //==============================================================================
    [[nodiscard]] SourceSnapshot
        computeInitialStateFromFinalState_implementation(Source const & finalState,
                                                         SourceSnapshot const & initialState) const override
    {
        SourceSnapshot newInitialState{ initialState };
        auto const newInitialPosition{
            (finalState.getPos() / mRadiusRatio).rotatedAboutOrigin(-mRotation.getAsRadians())
        };
        newInitialState.position = newInitialPosition;
        return newInitialState;
    }
};

//==============================================================================
class CircularFixedRadiusStrategy final : public LinkStrategy
{
private:
    Radians mRotation{};
    float mRadius{};
    //==============================================================================
    void computeParameters_implementation(Sources const & finalStates, SourcesSnapshots const & initialStates) override
    {
        auto const & primarySourceFinalState{ finalStates.getPrimarySource() };

        auto const primarySourceInitialAngle{ Radians::fromPoint(initialStates.primary.position) };
        auto const primarySourceFinalAngle{ Radians::fromPoint(primarySourceFinalState.getPos()) };
        mRotation = primarySourceFinalAngle - primarySourceInitialAngle;
        mRadius = primarySourceFinalState.getPos().getDistanceFromOrigin();
    }
    //==============================================================================
    void enforce_implementation(Source & finalState, SourceSnapshot const & initialState) const override
    {
        auto const initialAngle{ Radians::fromPoint(initialState.position) };
        auto const finalAngle{ (mRotation + initialAngle).getAsRadians() };
        Point<float> const finalPosition{ std::cos(finalAngle) * mRadius, std::sin(finalAngle) * mRadius };

        finalState.setPos(finalPosition, SourceLinkNotification::silent);
    }
    //==============================================================================
    [[nodiscard]] SourceSnapshot
        computeInitialStateFromFinalState_implementation(Source const & finalState,
                                                         SourceSnapshot const & intialState) const override
    {
        auto const finalPosition{ finalState.getPos() };
        SourceSnapshot newInitialState{ intialState };

        auto const finalAngle{ Radians::fromPoint(finalPosition) };
        auto const inversedFinalAngle{ (finalAngle - mRotation).getAsRadians() };
        Point<float> const newInitialPosition{ std::cos(inversedFinalAngle) * mRadius,
                                               std::sin(inversedFinalAngle) * mRadius };

        newInitialState.position = newInitialPosition;
        return newInitialState;
    }
};

//==============================================================================
class CircularFixedAngleStrategy final : public LinkStrategy
{
private:
    Radians mDeviationPerSource{};
    Radians mPrimarySourceFinalAngle{};
    Radians mRotation{};
    float mRadiusRatio{};
    std::array<int, MAX_NUMBER_OF_SOURCES> mOrdering{};
    //==============================================================================
    void computeParameters_implementation(Sources const & finalStates, SourcesSnapshots const & initialStates) override
    {
        auto const & primarySourceInitialState{ initialStates.primary };
        auto const & primarySourceFinalState{ finalStates.getPrimarySource() };

        auto const notQuiteZero{ std::nextafter(0.0f, 1.0f) }; // dont divide by zero!
        auto const primarySourceInitialRadius{ std::max(primarySourceInitialState.position.getDistanceFromOrigin(),
                                                        notQuiteZero) };
        mRadiusRatio = std::max(primarySourceFinalState.getPos().getDistanceFromOrigin() / primarySourceInitialRadius,
                                notQuiteZero);

        auto const primarySourceFinalPosition{ primarySourceFinalState.getPos() };
        mPrimarySourceFinalAngle = Radians::fromPoint(primarySourceFinalPosition);
        mDeviationPerSource = Degrees{ 360.0f } / finalStates.size();
        auto const primarySourceInitialAngle{ Radians::fromPoint(primarySourceInitialState.position) };
        mRotation = mPrimarySourceFinalAngle - primarySourceInitialAngle;

        // copy initialAngles
        std::array<std::pair<Degrees, SourceIndex>, MAX_NUMBER_OF_SOURCES> initialAngles{};
        for (auto const & finalState : finalStates) {
            auto const sourceIndex{ finalState.getIndex() };

            auto const & initialState{ initialStates[sourceIndex] };
            auto const initialAngle{ Radians::fromPoint(initialState.position) };

            initialAngles[sourceIndex.toInt()] = std::make_pair(initialAngle, sourceIndex);
        }
        // make all initialAngles bigger than the primary source's
        auto const minAngle{ Radians::fromPoint(initialStates.primary.position) };
        auto const maxAngle{ minAngle + twoPi };
        std::for_each(std::begin(initialAngles),
                      std::begin(initialAngles) + finalStates.size(),
                      [&](std::pair<Degrees, SourceIndex> & data) {
                          if (data.first < minAngle) {
                              data.first += twoPi;
                          }
                          jassert(data.first >= minAngle);
                          jassert(data.first < maxAngle);
                      });
        // sort
        std::stable_sort(std::begin(initialAngles),
                         std::begin(initialAngles) + finalStates.size(),
                         [](auto const & a, auto const & b) -> bool { return a.first < b.first; });
        // store ordering
        for (int i{}; i < finalStates.size(); ++i) {
            auto const sourceIndex{ initialAngles[i].second };
            mOrdering[sourceIndex.toInt()] = i;
        }
        jassert(mOrdering[0ull] == 0);
    }
    //==============================================================================
    void enforce_implementation(Source & finalState, SourceSnapshot const & initialState) const override
    {
        auto const sourceIndex{ finalState.getIndex() };
        auto const ordering{ mOrdering[sourceIndex.toInt()] };

        auto const finalAngle{ mPrimarySourceFinalAngle + mDeviationPerSource * ordering };
        auto const initialRadius{ initialState.position.getDistanceFromOrigin() };
        auto const finalRadius{ initialRadius * mRadiusRatio };
        Point<float> const finalPosition{ std::cos(finalAngle.getAsRadians()) * finalRadius,
                                          std::sin(finalAngle.getAsRadians()) * finalRadius };

        finalState.setPos(finalPosition, SourceLinkNotification::silent);
    }
    //==============================================================================
    [[nodiscard]] SourceSnapshot
        computeInitialStateFromFinalState_implementation(Source const & finalState,
                                                         SourceSnapshot const & initialState) const override
    {
        SourceSnapshot newInitialState{ initialState };

        auto const newInitialRadius{ finalState.getPos().getDistanceFromOrigin() / mRadiusRatio };

        Radians const finalAngle{ std::atan2(finalState.getY(), finalState.getX()) };
        auto const newInitialAngle{ finalAngle - mRotation };

        Point<float> const newInitialPosition{ std::cos(newInitialAngle.getAsRadians()) * newInitialRadius,
                                               std::sin(newInitialAngle.getAsRadians()) * newInitialRadius };

        newInitialState.position = newInitialPosition;

        return newInitialState;
    }
};

//==============================================================================
// TODO : copy-pasted code from fixedAngle
class CircularFullyFixedStrategy final : public LinkStrategy
{
private:
    Radians mDeviationPerSource{};
    Radians mPrimarySourceFinalAngle{};
    Radians mRotation{};
    float mRadius{};
    std::array<int, MAX_NUMBER_OF_SOURCES> mOrdering{};
    //==============================================================================
    void computeParameters_implementation(Sources const & finalStates, SourcesSnapshots const & initialStates) override
    {
        auto const & primarySourceInitialState{ initialStates.primary };
        auto const & primarySourceFinalState{ finalStates.getPrimarySource() };

        auto const notQuiteZero{ std::nextafter(0.0f, 1.0f) }; // dont divide by zero!
        auto const primarySourceInitialRadius{ std::max(primarySourceInitialState.position.getDistanceFromOrigin(),
                                                        notQuiteZero) };
        mRadius = primarySourceFinalState.getPos().getDistanceFromOrigin();

        auto const primarySourceFinalPosition{ primarySourceFinalState.getPos() };
        mPrimarySourceFinalAngle = Radians::fromPoint(primarySourceFinalPosition);
        mDeviationPerSource = Degrees{ 360.0f } / finalStates.size();
        auto const primarySourceInitialAngle{ Radians::fromPoint(primarySourceInitialState.position) };
        mRotation = mPrimarySourceFinalAngle - primarySourceInitialAngle;

        // copy initialAngles
        std::array<std::pair<Degrees, SourceIndex>, MAX_NUMBER_OF_SOURCES> initialAngles{};
        for (auto const & finalState : finalStates) {
            auto const sourceIndex{ finalState.getIndex() };

            auto const & initialState{ initialStates[sourceIndex] };
            auto const initialAngle{ Radians::fromPoint(initialState.position) };

            initialAngles[sourceIndex.toInt()] = std::make_pair(initialAngle, sourceIndex);
        }
        // make all initialAngles bigger than the primary source's
        auto const minAngle{ Radians::fromPoint(initialStates.primary.position) };
        auto const maxAngle{ minAngle + twoPi };
        std::for_each(std::begin(initialAngles),
                      std::begin(initialAngles) + finalStates.size(),
                      [&](std::pair<Degrees, SourceIndex> & data) {
                          if (data.first < minAngle) {
                              data.first += twoPi;
                          }
                          jassert(data.first >= minAngle);
                          jassert(data.first < maxAngle);
                      });
        // sort
        std::stable_sort(std::begin(initialAngles),
                         std::begin(initialAngles) + finalStates.size(),
                         [](auto const & a, auto const & b) -> bool { return a.first < b.first; });
        // store ordering
        for (int i{}; i < finalStates.size(); ++i) {
            auto const sourceIndex{ initialAngles[i].second };
            mOrdering[sourceIndex.toInt()] = i;
        }
        jassert(mOrdering[0ull] == 0);
    }
    //==============================================================================
    void enforce_implementation(Source & finalState, SourceSnapshot const & initialState) const override
    {
        auto const sourceIndex{ finalState.getIndex() };
        auto const ordering{ mOrdering[sourceIndex.toInt()] };

        auto const finalAngle{ mPrimarySourceFinalAngle + mDeviationPerSource * ordering };
        Point<float> const finalPosition{ std::cos(finalAngle.getAsRadians()) * mRadius,
                                          std::sin(finalAngle.getAsRadians()) * mRadius };

        finalState.setPos(finalPosition, SourceLinkNotification::silent);
    }
    //==============================================================================
    [[nodiscard]] SourceSnapshot
        computeInitialStateFromFinalState_implementation(Source const & finalState,
                                                         SourceSnapshot const & initialState) const override
    {
        SourceSnapshot newInitialState{ initialState };

        Radians const finalAngle{ std::atan2(finalState.getY(), finalState.getX()) };
        auto const newInitialAngle{ finalAngle - mRotation };

        Point<float> const newInitialPosition{ std::cos(newInitialAngle.getAsRadians()) * mRadius,
                                               std::sin(newInitialAngle.getAsRadians()) * mRadius };

        newInitialState.position = newInitialPosition;

        return newInitialState;
    }
};

//==============================================================================
class LinkSymmetricXStrategy final : public LinkStrategy
{
    Point<float> mPrimarySourceFinalPosition;
    //==============================================================================
    void computeParameters_implementation(Sources const & finalStates,
                                          [[maybe_unused]] SourcesSnapshots const & initialStates) override
    {
        mPrimarySourceFinalPosition = finalStates.getPrimarySource().getPos();
    }
    //==============================================================================
    void enforce_implementation(Source & finalState,
                                [[maybe_unused]] SourceSnapshot const & initialState) const override
    {
        Point<float> const finalPosition{ mPrimarySourceFinalPosition.getX(), -mPrimarySourceFinalPosition.getY() };
        finalState.setPos(finalPosition, SourceLinkNotification::silent);
    }
    //==============================================================================
    [[nodiscard]] SourceSnapshot
        computeInitialStateFromFinalState_implementation([[maybe_unused]] Source const & finalState,
                                                         SourceSnapshot const & initialState) const override
    {
        // nothing to do here!
        return SourceSnapshot{ finalState };
    }
};

//==============================================================================
class LinkSymmetricYStrategy final : public LinkStrategy
{
    Point<float> mPrimarySourceFinalPosition;
    //==============================================================================
    void computeParameters_implementation(Sources const & finalStates,
                                          [[maybe_unused]] SourcesSnapshots const & initialStates) override
    {
        mPrimarySourceFinalPosition = finalStates.getPrimarySource().getPos();
    }
    //==============================================================================
    void enforce_implementation(Source & finalState,
                                [[maybe_unused]] SourceSnapshot const & initialState) const override
    {
        Point<float> const finalPosition{ -mPrimarySourceFinalPosition.getX(), mPrimarySourceFinalPosition.getY() };
        finalState.setPos(finalPosition, SourceLinkNotification::silent);
    }
    //==============================================================================
    [[nodiscard]] SourceSnapshot
        computeInitialStateFromFinalState_implementation([[maybe_unused]] Source const & finalState,
                                                         SourceSnapshot const & intialState) const override
    {
        // nothing to do here!
        return SourceSnapshot{ finalState };
    }
};

//==============================================================================
class DeltaLockStrategy final : public LinkStrategy
{
    Point<float> mDelta;
    //==============================================================================
    void computeParameters_implementation(Sources const & finalStates, SourcesSnapshots const & initialStates) override
    {
        mDelta = finalStates.getPrimarySource().getPos() - initialStates.primary.position;
    }
    //==============================================================================
    void enforce_implementation(Source & finalState, SourceSnapshot const & initialState) const override
    {
        auto const finalPosition{ initialState.position + mDelta };
        finalState.setPos(finalPosition, SourceLinkNotification::silent);
    }
    //==============================================================================
    [[nodiscard]] SourceSnapshot
        computeInitialStateFromFinalState_implementation(Source const & finalState,
                                                         SourceSnapshot const & intialState) const override
    {
        SourceSnapshot newInitialState{ intialState };

        auto const newInitialPosition{ finalState.getPos() - mDelta };
        newInitialState.position = newInitialPosition;

        return newInitialState;
    }
};

//==============================================================================
// only usefuLl to recall saved positions
class IndependentElevationStrategy final : public LinkStrategy
{
    void computeParameters_implementation(Sources const &, SourcesSnapshots const &) override {}
    //==============================================================================
    void enforce_implementation(Source & finalState, SourceSnapshot const & initialState) const override
    {
        finalState.setElevation(initialState.z, SourceLinkNotification::silent);
    }
    //==============================================================================
    [[nodiscard]] SourceSnapshot
        computeInitialStateFromFinalState_implementation([[maybe_unused]] Source const & finalState,
                                                         SourceSnapshot const & initialState) const override
    {
        return SourceSnapshot{ finalState };
    }
};

//==============================================================================
class FixedElevationStrategy final : public LinkStrategy
{
    Radians mElevation{};
    //==============================================================================
    void computeParameters_implementation(Sources const & finalStates,
                                          [[maybe_unused]] SourcesSnapshots const & initialStates) override
    {
        mElevation = finalStates.getPrimarySource().getElevation();
    }
    //==============================================================================
    void enforce_implementation(Source & finalState,
                                [[maybe_unused]] SourceSnapshot const & initialState) const override
    {
        finalState.setElevation(mElevation, SourceLinkNotification::silent);
    }
    //==============================================================================
    [[nodiscard]] SourceSnapshot
        computeInitialStateFromFinalState_implementation(Source const & finalState,
                                                         SourceSnapshot const & initialState) const override
    {
        return initialState;
    }
};

//==============================================================================
class LinearMinElevationStrategy final : public LinkStrategy
{
    static constexpr Radians ELEVATION_DIFF{ -MAX_ELEVATION / 3.0f * 2.0f };
    Radians mBaseElevation{};
    Radians mElevationPerSource{};
    //==============================================================================
    void computeParameters_implementation(Sources const & sources, SourcesSnapshots const & snapshots) override
    {
        mBaseElevation = sources.getPrimarySource().getElevation();
        mElevationPerSource = ELEVATION_DIFF / (sources.size() - 1);
    }
    //==============================================================================
    void enforce_implementation(Source & source, [[maybe_unused]] SourceSnapshot const & snapshot) const override
    {
        auto const sourceIndex{ source.getIndex().toInt() };
        auto const newElevation{ mBaseElevation + mElevationPerSource * sourceIndex };
        source.setElevation(newElevation, SourceLinkNotification::silent);
    }
    //==============================================================================
    [[nodiscard]] SourceSnapshot
        computeInitialStateFromFinalState_implementation([[maybe_unused]] Source const & source,
                                                         SourceSnapshot const & snapshot) const override
    {
        return snapshot;
    }
};

//==============================================================================
class LinearMaxElevationStrategy final : public LinkStrategy
{
    static constexpr Radians ELEVATION_DIFF{ MAX_ELEVATION / 3.0f * 2.0f };
    Radians mBaseElevation{};
    Radians mElevationPerSource{};
    //==============================================================================
    void computeParameters_implementation(Sources const & sources, SourcesSnapshots const & snapshots) override
    {
        mBaseElevation = sources.getPrimarySource().getElevation();
        mElevationPerSource = ELEVATION_DIFF / (sources.size() - 1);
    }
    //==============================================================================
    void enforce_implementation(Source & source, [[maybe_unused]] SourceSnapshot const & snapshot) const override
    {
        auto const sourceIndex{ source.getIndex().toInt() };
        auto const newElevation{ mBaseElevation + mElevationPerSource * sourceIndex };
        source.setElevation(newElevation, SourceLinkNotification::silent);
    }
    //==============================================================================
    [[nodiscard]] SourceSnapshot
        computeInitialStateFromFinalState_implementation([[maybe_unused]] Source const & source,
                                                         SourceSnapshot const & snapshot) const override
    {
        return snapshot;
    }
};

//==============================================================================
class DeltaLockElevationStrategy final : public LinkStrategy
{
    Radians mDelta;
    //==============================================================================
    void computeParameters_implementation(Sources const & sources, SourcesSnapshots const & snapshots) override
    {
        mDelta = sources.getPrimarySource().getElevation() - snapshots.primary.z;
    }
    //==============================================================================
    void enforce_implementation(Source & source, SourceSnapshot const & snapshot) const override
    {
        auto const newElevation{ snapshot.z + mDelta };
        source.setElevation(newElevation, SourceLinkNotification::silent);
    }
    //==============================================================================
    [[nodiscard]] SourceSnapshot
        computeInitialStateFromFinalState_implementation(Source const & source,
                                                         SourceSnapshot const & snapshot) const override
    {
        SourceSnapshot result{ snapshot };

        auto const initialElevation{ source.getElevation() - mDelta };
        result.z = initialElevation;

        return result;
    }
};

//==============================================================================
std::unique_ptr<LinkStrategy> getLinkStrategy(PositionSourceLink const sourceLink)
{
    switch (sourceLink) {
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
        break;
    }
    jassertfalse;
    return nullptr;
}

//==============================================================================
std::unique_ptr<LinkStrategy> getLinkStrategy(ElevationSourceLink const sourceLink)
{
    switch (sourceLink) {
    case ElevationSourceLink::independent:
        return std::make_unique<IndependentElevationStrategy>();
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
    jassertfalse;
    return nullptr;
}

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
        strategy = getLinkStrategy(mPositionSourceLink);
    } else {
        jassert(mElevationSourceLink != ElevationSourceLink::undefined);
        strategy = getLinkStrategy(mElevationSourceLink);
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
    jassert(sourceIndex.toInt() > 0 && sourceIndex.toInt() < MAX_NUMBER_OF_SOURCES);
    auto & source{ mSources[sourceIndex] };
    auto const secondaryIndex{ sourceIndex.toInt() - 1 };
    auto & snapshot{ mSnapshots.secondaries.getReference(secondaryIndex) };
    std::unique_ptr<LinkStrategy> strategy{};
    if (mElevationSourceLink != ElevationSourceLink::undefined) {
        jassert(mPositionSourceLink == PositionSourceLink::undefined);
        strategy = getLinkStrategy(mElevationSourceLink);
    } else {
        jassert(mPositionSourceLink != PositionSourceLink::undefined);
        strategy = getLinkStrategy(mPositionSourceLink);
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
