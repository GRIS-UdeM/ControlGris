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

#include "cg_LinkStrategies.hpp"
#include "cg_Sources.hpp"

namespace gris
{
namespace source_link_strategies
{
//==============================================================================
void Base::init(Sources const & currentStates, SourcesSnapshots const & initialStates) noexcept(IS_RELEASE)
{
    initImpl(currentStates, initialStates);
    mInitialized = true;
}

//==============================================================================
void Base::apply(Sources & currentStates, SourcesSnapshots const & initialStates, SourceIndex const sourceIndex) const
    noexcept(IS_RELEASE)
{
    jassert(mInitialized);
    applyImpl(currentStates, initialStates, sourceIndex);
}

//==============================================================================
void Base::apply(Sources & currentStates, SourcesSnapshots const & initialStates) const noexcept(IS_RELEASE)
{
    for (auto & source : currentStates) {
        if (source.isPrimarySource()) {
            continue; // do not apply primary source
        }
        apply(currentStates, initialStates, source.getIndex());
    }
}

//==============================================================================
SourceSnapshot Base::deduceInitialState(Sources const & currentStates,
                                        SourcesSnapshots const & initialStates,
                                        SourceIndex const sourceIndex) const noexcept(IS_RELEASE)
{
    jassert(mInitialized);
    return deduceInitialStateImpl(currentStates, initialStates, sourceIndex);
}

//==============================================================================
std::unique_ptr<Base> Base::make(PositionSourceLink const sourceLink) noexcept(IS_RELEASE)
{
    switch (sourceLink) {
    case PositionSourceLink::independent:
        return std::make_unique<PositionIndependent>();
    case PositionSourceLink::circular:
        return std::make_unique<Circular>();
    case PositionSourceLink::circularFixedRadius:
        return std::make_unique<CircularFixedRadius>();
    case PositionSourceLink::circularFixedAngle:
        return std::make_unique<CircularFixedAngle>();
    case PositionSourceLink::circularFullyFixed:
        return std::make_unique<CircularFullyFixed>();
    case PositionSourceLink::symmetricX:
        return std::make_unique<SymmetricX>();
    case PositionSourceLink::symmetricY:
        return std::make_unique<SymmetricY>();
    case PositionSourceLink::deltaLock:
        return std::make_unique<PositionDeltaLock>();
    case PositionSourceLink::undefined:
        break;
    }
    jassertfalse;
    return nullptr;
}

//==============================================================================
std::unique_ptr<Base> Base::make(ElevationSourceLink const sourceLink) noexcept(IS_RELEASE)
{
    switch (sourceLink) {
    case ElevationSourceLink::independent:
        return std::make_unique<ElevationIndependent>();
    case ElevationSourceLink::fixedElevation:
        return std::make_unique<FixedElevation>();
    case ElevationSourceLink::linearMin:
        return std::make_unique<LinearMin>();
    case ElevationSourceLink::linearMax:
        return std::make_unique<LinearMax>();
    case ElevationSourceLink::deltaLock:
        return std::make_unique<ElevationDeltaLock>();
    case ElevationSourceLink::undefined:
        break;
    }
    jassertfalse;
    return nullptr;
}

//==============================================================================
void PositionIndependent::applyImpl(Sources & finalStates,
                                    SourcesSnapshots const & initialStates,
                                    SourceIndex const sourceIndex) const noexcept(IS_RELEASE)
{
    finalStates[sourceIndex].setPosition(initialStates[sourceIndex].position, Source::OriginOfChange::link);
}

//==============================================================================
SourceSnapshot PositionIndependent::deduceInitialStateImpl(Sources const & finalStates,
                                                           SourcesSnapshots const & /*initialStates*/,
                                                           SourceIndex const sourceIndex) const noexcept(IS_RELEASE)
{
    return SourceSnapshot{ finalStates[sourceIndex] };
}

//==============================================================================
void Circular::initImpl(Sources const & finalState, SourcesSnapshots const & initialStates) noexcept(IS_RELEASE)
{
    auto const & primarySourceFinalState{ finalState.getPrimarySource() };
    auto const & primarySourceInitialState{ initialStates.primary };

    auto constexpr notQuiteZero{ 0.01f };
    auto const primarySourceInitialAngle{ Radians::angleOf(primarySourceInitialState.position) };
    auto const primarySourceFinalAngle{ Radians::angleOf(primarySourceFinalState.getPos()) };
    mRotation = primarySourceFinalAngle - primarySourceInitialAngle;
    auto const primarySourceInitialRadius{ primarySourceInitialState.position.getDistanceFromOrigin() };
    auto const primarySourceFinalRadius{ primarySourceFinalState.getPos().getDistanceFromOrigin() };
    auto const radiusRatio{ primarySourceInitialRadius == 0.0f
                                ? notQuiteZero
                                : primarySourceFinalRadius / primarySourceInitialRadius };
    mRadiusRatio = radiusRatio == 0.0f ? notQuiteZero : radiusRatio;
}

//==============================================================================
void Circular::applyImpl(Sources & finalStates,
                         SourcesSnapshots const & initialStates,
                         SourceIndex const sourceIndex) const noexcept(IS_RELEASE)
{
    auto const finalPosition{ initialStates[sourceIndex].position.rotatedAboutOrigin(mRotation.getAsRadians())
                              * mRadiusRatio };
    finalStates[sourceIndex].setPosition(finalPosition, Source::OriginOfChange::link);
}

//==============================================================================
SourceSnapshot Circular::deduceInitialStateImpl(Sources const & finalStates,
                                                SourcesSnapshots const & initialStates,
                                                SourceIndex const sourceIndex) const noexcept(IS_RELEASE)
{
    SourceSnapshot newInitialState{ initialStates[sourceIndex] };
    auto const newInitialPosition{
        (finalStates[sourceIndex].getPos() / mRadiusRatio).rotatedAboutOrigin(-mRotation.getAsRadians())
    };
    newInitialState.position = newInitialPosition;
    return newInitialState;
}

//==============================================================================
void CircularFixedRadius::initImpl(Sources const & finalStates,
                                   SourcesSnapshots const & initialStates) noexcept(IS_RELEASE)
{
    auto const & primarySourceFinalState{ finalStates.getPrimarySource() };

    auto const primarySourceInitialAngle{ Radians::angleOf(initialStates.primary.position) };
    auto const primarySourceFinalAngle{ Radians::angleOf(primarySourceFinalState.getPos()) };
    mRotation = primarySourceFinalAngle - primarySourceInitialAngle;
    mRadius = primarySourceFinalState.getPos().getDistanceFromOrigin();
}

//==============================================================================
void CircularFixedRadius::applyImpl(Sources & finalStates,
                                    SourcesSnapshots const & initialStates,
                                    SourceIndex const sourceIndex) const noexcept(IS_RELEASE)
{
    auto const initialAngle{ Radians::angleOf(initialStates[sourceIndex].position) };
    auto const finalAngle{ (mRotation + initialAngle).getAsRadians() };
    juce::Point<float> const finalPosition{ std::cos(finalAngle) * mRadius, std::sin(finalAngle) * mRadius };

    finalStates[sourceIndex].setPosition(finalPosition, Source::OriginOfChange::link);
}

//==============================================================================
SourceSnapshot CircularFixedRadius::deduceInitialStateImpl(Sources const & finalStates,
                                                           SourcesSnapshots const & initialStates,
                                                           SourceIndex const sourceIndex) const noexcept(IS_RELEASE)
{
    auto const finalPosition{ finalStates[sourceIndex].getPos() };
    auto newInitialState{ initialStates[sourceIndex] };

    auto const finalAngle{ Radians::angleOf(finalPosition) };
    auto const inverseFinalAngle{ (finalAngle - mRotation).getAsRadians() };
    juce::Point<float> const newInitialPosition{ std::cos(inverseFinalAngle) * mRadius,
                                                 std::sin(inverseFinalAngle) * mRadius };

    newInitialState.position = newInitialPosition;
    return newInitialState;
}

//==============================================================================
void CircularFixedAngle::initImpl(Sources const & finalStates,
                                  SourcesSnapshots const & initialStates) noexcept(IS_RELEASE)
{
    auto const & primarySourceInitialState{ initialStates.primary };
    auto const & primarySourceFinalState{ finalStates.getPrimarySource() };

    auto const notQuiteZero{ std::nextafter(0.0f, 1.0f) }; // dont divide by zero!
    auto const primarySourceInitialRadius{ std::max(primarySourceInitialState.position.getDistanceFromOrigin(),
                                                    notQuiteZero) };
    mRadiusRatio = primarySourceFinalState.getPos().getDistanceFromOrigin() / primarySourceInitialRadius;
    if (std::isinf(mRadiusRatio)) {
        mRadiusRatio = std::numeric_limits<float>::max();
    }
    jassert(!std::isnan(mRadiusRatio));

    auto const primarySourceFinalPosition{ primarySourceFinalState.getPos() };
    mPrimarySourceFinalAngle = Radians::angleOf(primarySourceFinalPosition);
    mDeviationPerSource = Degrees{ 360.0f } / narrow<float>(finalStates.size());
    auto const primarySourceInitialAngle{ Radians::angleOf(primarySourceInitialState.position) };
    mRotation = mPrimarySourceFinalAngle - primarySourceInitialAngle;

    // copy initialAngles
    std::array<std::pair<Degrees, SourceIndex>, MAX_NUMBER_OF_SOURCES> initialAngles{};
    for (auto const & finalState : finalStates) {
        auto const sourceIndex{ finalState.getIndex() };

        auto const & initialState{ initialStates[sourceIndex] };
        auto const initialAngle{ Radians::angleOf(initialState.position) };
        jassert(!std::isnan(initialAngle.getAsRadians()));

        initialAngles[sourceIndex.get()] = std::make_pair(initialAngle, sourceIndex);
    }
    // make all initialAngles bigger than the primary source's
    auto const minAngle{ Radians::angleOf(initialStates.primary.position) };
    auto const maxAngle{ minAngle + TWO_PI };
    std::for_each(std::begin(initialAngles),
                  std::begin(initialAngles) + finalStates.size(),
                  [&](std::pair<Degrees, SourceIndex> & data) {
                      while (data.first < minAngle) {
                          data.first += TWO_PI;
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
        mOrdering[sourceIndex.get()] = i;
    }
    jassert(mOrdering[0ull] == 0);
}

//==============================================================================
void CircularFixedAngle::applyImpl(Sources & finalStates,
                                   SourcesSnapshots const & initialStates,
                                   SourceIndex const sourceIndex) const noexcept(IS_RELEASE)
{
    auto const ordering{ mOrdering[sourceIndex.get()] };

    auto const finalAngle{ mPrimarySourceFinalAngle + mDeviationPerSource * narrow<float>(ordering) };
    auto const initialRadius{ initialStates[sourceIndex].position.getDistanceFromOrigin() };
    auto const finalRadius{ initialRadius * mRadiusRatio };
    juce::Point<float> const finalPosition{ std::cos(finalAngle.getAsRadians()) * finalRadius,
                                            std::sin(finalAngle.getAsRadians()) * finalRadius };

    finalStates[sourceIndex].setPosition(finalPosition, Source::OriginOfChange::link);
}

//==============================================================================
SourceSnapshot CircularFixedAngle::deduceInitialStateImpl(Sources const & finalStates,
                                                          SourcesSnapshots const & initialStates,
                                                          SourceIndex const sourceIndex) const noexcept(IS_RELEASE)
{
    SourceSnapshot newInitialState{ initialStates[sourceIndex] };

    static const float notQuiteZero{ std::nextafter(0.0f, 1.0f) };
    auto const divisor{ std::max(notQuiteZero, mRadiusRatio) };
    auto const newInitialRadius{ finalStates[sourceIndex].getPos().getDistanceFromOrigin() / divisor };

    Radians const finalAngle{ std::atan2(finalStates[sourceIndex].getY(), finalStates[sourceIndex].getX()) };
    auto const ordering{ mOrdering[sourceIndex.get()] };

    auto const newInitialAngle{ finalAngle - mRotation };

    juce::Point<float> const newInitialPosition{ std::cos(newInitialAngle.getAsRadians()) * newInitialRadius,
                                                 std::sin(newInitialAngle.getAsRadians()) * newInitialRadius };

    newInitialState.position = newInitialPosition;

    return newInitialState;
}

//==============================================================================
void CircularFullyFixed::initImpl(Sources const & finalStates,
                                  SourcesSnapshots const & initialStates) noexcept(IS_RELEASE)
{
    auto const & primarySourceInitialState{ initialStates.primary };
    auto const & primarySourceFinalState{ finalStates.getPrimarySource() };

    auto const notQuiteZero{ std::nextafter(0.0f, 1.0f) }; // dont divide by zero!
    mRadius = primarySourceFinalState.getPos().getDistanceFromOrigin();

    auto const primarySourceFinalPosition{ primarySourceFinalState.getPos() };
    mPrimarySourceFinalAngle = Radians::angleOf(primarySourceFinalPosition);
    mDeviationPerSource = Degrees{ 360.0f } / narrow<float>(finalStates.size());
    auto const primarySourceInitialAngle{ Radians::angleOf(primarySourceInitialState.position) };
    mRotation = mPrimarySourceFinalAngle - primarySourceInitialAngle;

    // copy initialAngles
    std::array<std::pair<Degrees, SourceIndex>, MAX_NUMBER_OF_SOURCES> initialAngles{};
    for (auto const & finalState : finalStates) {
        auto const sourceIndex{ finalState.getIndex() };

        auto const & initialState{ initialStates[sourceIndex] };
        auto const initialAngle{ Radians::angleOf(initialState.position) };

        initialAngles[sourceIndex.get()] = std::make_pair(initialAngle, sourceIndex);
    }
    // make all initialAngles bigger than the primary source's
    auto const minAngle{ Radians::angleOf(initialStates.primary.position) };
    auto const maxAngle{ minAngle + TWO_PI };
    std::for_each(std::begin(initialAngles),
                  std::begin(initialAngles) + finalStates.size(),
                  [&](std::pair<Degrees, SourceIndex> & data) {
                      if (data.first < minAngle) {
                          data.first += TWO_PI;
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
        mOrdering[sourceIndex.get()] = i;
    }
    jassert(mOrdering[0ull] == 0);
}

//==============================================================================
void CircularFullyFixed::applyImpl(Sources & finalStates,
                                   SourcesSnapshots const & /*initialStates*/,
                                   SourceIndex const sourceIndex) const noexcept(IS_RELEASE)
{
    auto const ordering{ mOrdering[sourceIndex.get()] };

    auto const finalAngle{ mPrimarySourceFinalAngle + mDeviationPerSource * narrow<float>(ordering) };
    juce::Point<float> const finalPosition{ std::cos(finalAngle.getAsRadians()) * mRadius,
                                            std::sin(finalAngle.getAsRadians()) * mRadius };

    finalStates[sourceIndex].setPosition(finalPosition, Source::OriginOfChange::link);
}

//==============================================================================
SourceSnapshot CircularFullyFixed::deduceInitialStateImpl(Sources const & finalStates,
                                                          SourcesSnapshots const & initialStates,
                                                          SourceIndex const sourceIndex) const noexcept(IS_RELEASE)
{
    SourceSnapshot newInitialState{ initialStates[sourceIndex] };

    Radians const finalAngle{ std::atan2(finalStates[sourceIndex].getY(), finalStates[sourceIndex].getX()) };
    auto const newInitialAngle{ finalAngle - mRotation };

    juce::Point<float> const newInitialPosition{ std::cos(newInitialAngle.getAsRadians()) * mRadius,
                                                 std::sin(newInitialAngle.getAsRadians()) * mRadius };

    newInitialState.position = newInitialPosition;

    return newInitialState;
}

//==============================================================================
void SymmetricX::initImpl(Sources const & finalStates, SourcesSnapshots const & /*initialStates*/) noexcept(IS_RELEASE)
{
    mPrimarySourceFinalPosition = finalStates.getPrimarySource().getPos();
}

//==============================================================================
void SymmetricX::applyImpl(Sources & finalStates,
                           SourcesSnapshots const & /*initialStates*/,
                           SourceIndex const sourceIndex) const noexcept(IS_RELEASE)
{
    juce::Point<float> const finalPosition{ mPrimarySourceFinalPosition.getX(), -mPrimarySourceFinalPosition.getY() };
    finalStates[sourceIndex].setPosition(finalPosition, Source::OriginOfChange::link);
}

//==============================================================================
SourceSnapshot SymmetricX::deduceInitialStateImpl(Sources const & finalStates,
                                                  SourcesSnapshots const & /*initialStates*/,
                                                  SourceIndex const sourceIndex) const noexcept(IS_RELEASE)
{
    // nothing to do here!
    return SourceSnapshot{ finalStates[sourceIndex] };
}

//==============================================================================
void SymmetricY::initImpl(Sources const & finalStates, SourcesSnapshots const & /*initialStates*/) noexcept(IS_RELEASE)
{
    mPrimarySourceFinalPosition = finalStates.getPrimarySource().getPos();
}

//==============================================================================
void SymmetricY::applyImpl(Sources & finalStates,
                           SourcesSnapshots const & /*initialStates*/,
                           SourceIndex const sourceIndex) const noexcept(IS_RELEASE)
{
    juce::Point<float> const finalPosition{ -mPrimarySourceFinalPosition.getX(), mPrimarySourceFinalPosition.getY() };
    finalStates[sourceIndex].setPosition(finalPosition, Source::OriginOfChange::link);
}

//==============================================================================
SourceSnapshot SymmetricY::deduceInitialStateImpl(Sources const & finalStates,
                                                  SourcesSnapshots const & /*initialStates*/,
                                                  SourceIndex const sourceIndex) const noexcept(IS_RELEASE)
{
    // nothing to do here!
    return SourceSnapshot{ finalStates[sourceIndex] };
}

//==============================================================================
void PositionDeltaLock::initImpl(Sources const & finalStates,
                                 SourcesSnapshots const & initialStates) noexcept(IS_RELEASE)
{
    mDelta = finalStates.getPrimarySource().getPos() - initialStates.primary.position;
}

//==============================================================================
void PositionDeltaLock::applyImpl(Sources & finalStates,
                                  SourcesSnapshots const & initialStates,
                                  SourceIndex const sourceIndex) const noexcept(IS_RELEASE)
{
    auto const finalPosition{ initialStates[sourceIndex].position + mDelta };
    finalStates[sourceIndex].setPosition(finalPosition, Source::OriginOfChange::link);
}

//==============================================================================
SourceSnapshot PositionDeltaLock::deduceInitialStateImpl(Sources const & finalStates,
                                                         SourcesSnapshots const & initialStates,
                                                         SourceIndex const sourceIndex) const noexcept(IS_RELEASE)
{
    SourceSnapshot newInitialState{ initialStates[sourceIndex] };

    auto const newInitialPosition{ finalStates[sourceIndex].getPos() - mDelta };
    newInitialState.position = newInitialPosition;

    return newInitialState;
}

//==============================================================================
void ElevationIndependent::applyImpl(Sources & finalStates,
                                     SourcesSnapshots const & initialStates,
                                     SourceIndex const sourceIndex) const noexcept(IS_RELEASE)
{
    finalStates[sourceIndex].setElevation(initialStates[sourceIndex].z, Source::OriginOfChange::link);
}

//==============================================================================
SourceSnapshot ElevationIndependent::deduceInitialStateImpl(Sources const & finalStates,
                                                            SourcesSnapshots const & /*initialStates*/,
                                                            SourceIndex const sourceIndex) const noexcept(IS_RELEASE)
{
    return SourceSnapshot{ finalStates[sourceIndex] };
}

//==============================================================================
void FixedElevation::initImpl(Sources const & finalStates,
                              SourcesSnapshots const & /*initialStates*/) noexcept(IS_RELEASE)
{
    mElevation = finalStates.getPrimarySource().getElevation();
}

//==============================================================================
void FixedElevation::applyImpl(Sources & finalStates,
                               SourcesSnapshots const & /*initialStates*/,
                               SourceIndex const sourceIndex) const noexcept(IS_RELEASE)
{
    finalStates[sourceIndex].setElevation(mElevation, Source::OriginOfChange::link);
}

//==============================================================================
SourceSnapshot FixedElevation::deduceInitialStateImpl(Sources const & /*finalStates*/,
                                                      SourcesSnapshots const & initialStates,
                                                      SourceIndex const sourceIndex) const noexcept(IS_RELEASE)
{
    return initialStates[sourceIndex];
}

//==============================================================================
void LinearMin::initImpl(Sources const & sources, SourcesSnapshots const & /*snapshots*/) noexcept(IS_RELEASE)
{
    mBaseElevation = sources.getPrimarySource().getElevation();
    mElevationPerSource = ELEVATION_DIFF / narrow<float>(sources.size() - 1);
}

//==============================================================================
void LinearMin::applyImpl(Sources & finalStates,
                          SourcesSnapshots const & /*initialStates*/,
                          SourceIndex const sourceIndex) const noexcept(IS_RELEASE)
{
    auto const newElevation{ mBaseElevation + mElevationPerSource * narrow<float>(sourceIndex.get()) };
    finalStates[sourceIndex].setElevation(newElevation, Source::OriginOfChange::link);
}

//==============================================================================
SourceSnapshot LinearMin::deduceInitialStateImpl([[maybe_unused]] Sources const & finalStates,
                                                 SourcesSnapshots const & /*initialStates*/,
                                                 SourceIndex const sourceIndex) const noexcept(IS_RELEASE)
{
    SourceSnapshot result{};
    result.z = mBaseElevation - mElevationPerSource * narrow<float>(sourceIndex.get());
    return result;
}

//==============================================================================
void LinearMax::initImpl(Sources const & sources, SourcesSnapshots const & /*snapshots*/) noexcept(IS_RELEASE)
{
    mBaseElevation = sources.getPrimarySource().getElevation();
    mElevationPerSource = ELEVATION_DIFF / narrow<float>(sources.size() - 1);
}

//==============================================================================
void LinearMax::applyImpl(Sources & finalStates,
                          SourcesSnapshots const & /*initialStates*/,
                          SourceIndex const sourceIndex) const noexcept(IS_RELEASE)
{
    auto const newElevation{ mBaseElevation + mElevationPerSource * narrow<float>(sourceIndex.get()) };
    finalStates[sourceIndex].setElevation(newElevation, Source::OriginOfChange::link);
}

//==============================================================================
SourceSnapshot LinearMax::deduceInitialStateImpl(Sources const & /*finalStates*/,
                                                 SourcesSnapshots const & initialStates,
                                                 SourceIndex const sourceIndex) const noexcept(IS_RELEASE)
{
    return initialStates[sourceIndex];
}

//==============================================================================
void ElevationDeltaLock::initImpl(Sources const & sources, SourcesSnapshots const & snapshots) noexcept(IS_RELEASE)
{
    mDelta = sources.getPrimarySource().getElevation() - snapshots.primary.z;
}

//==============================================================================
void ElevationDeltaLock::applyImpl(Sources & finalStates,
                                   SourcesSnapshots const & initialStates,
                                   SourceIndex const sourceIndex) const noexcept(IS_RELEASE)
{
    auto const newElevation{ initialStates[sourceIndex].z + mDelta };
    finalStates[sourceIndex].setElevation(newElevation, Source::OriginOfChange::link);
}

//==============================================================================
SourceSnapshot ElevationDeltaLock::deduceInitialStateImpl(Sources const & finalStates,
                                                          SourcesSnapshots const & initialStates,
                                                          SourceIndex const sourceIndex) const noexcept(IS_RELEASE)
{
    SourceSnapshot result{ initialStates[sourceIndex] };

    auto const initialElevation{ finalStates[sourceIndex].getElevation() - mDelta };
    result.z = initialElevation;

    return result;
}

} // namespace source_link_strategies

} // namespace gris