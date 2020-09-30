#include "LinkStrategies.h"

#include "Source.h"

//==============================================================================
void LinkStrategy::computeParameters(Sources const & finalStates, SourcesSnapshots const & initialStates)
{
    computeParameters_implementation(finalStates, initialStates);
    mInitialized = true;
}

//==============================================================================
void LinkStrategy::enforce(Sources & finalStates,
                           SourcesSnapshots const & initialState,
                           SourceIndex const sourceIndex) const
{
    jassert(mInitialized);
    enforce_implementation(finalStates, initialState, sourceIndex);
}

//==============================================================================
void LinkStrategy::enforce(Sources & finalStates, SourcesSnapshots const & initialState) const
{
    for (auto & source : finalStates) {
        enforce(finalStates, initialState, source.getIndex());
    }
}

//==============================================================================
SourceSnapshot LinkStrategy::computeInitialStateFromFinalState(Sources const & finalStates,
                                                               SourcesSnapshots const & initialStates,
                                                               SourceIndex const sourceIndex) const
{
    jassert(mInitialized);
    return computeInitialStateFromFinalState_implementation(finalStates, initialStates, sourceIndex);
}

//==============================================================================
std::unique_ptr<LinkStrategy> LinkStrategy::make(PositionSourceLink const sourceLink)
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
std::unique_ptr<LinkStrategy> LinkStrategy::make(ElevationSourceLink const sourceLink)
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
void IndependentStrategy::enforce_implementation(Sources & finalStates,
                                                 SourcesSnapshots const & initialStates,
                                                 SourceIndex const sourceIndex) const
{
    finalStates[sourceIndex].setPosition(initialStates[sourceIndex].position, SourceLinkBehavior::doNothing);
}

//==============================================================================
SourceSnapshot
    IndependentStrategy::computeInitialStateFromFinalState_implementation(Sources const & finalStates,
                                                                          SourcesSnapshots const & initialStates,
                                                                          SourceIndex const sourceIndex) const
{
    return SourceSnapshot{ finalStates[sourceIndex] };
}

//==============================================================================
void CircularStrategy::computeParameters_implementation(Sources const & finalState,
                                                        SourcesSnapshots const & initialStates)
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
void CircularStrategy::enforce_implementation(Sources & finalStates,
                                              SourcesSnapshots const & initialStates,
                                              SourceIndex const sourceIndex) const
{
    auto const finalPosition{ initialStates[sourceIndex].position.rotatedAboutOrigin(mRotation.getAsRadians())
                              * mRadiusRatio };
    finalStates[sourceIndex].setPosition(finalPosition, SourceLinkBehavior::doNothing);
}

//==============================================================================
SourceSnapshot
    CircularStrategy::computeInitialStateFromFinalState_implementation(Sources const & finalStates,
                                                                       SourcesSnapshots const & initialStates,
                                                                       SourceIndex const sourceIndex) const
{
    SourceSnapshot newInitialState{ initialStates[sourceIndex] };
    auto const newInitialPosition{
        (finalStates[sourceIndex].getPos() / mRadiusRatio).rotatedAboutOrigin(-mRotation.getAsRadians())
    };
    newInitialState.position = newInitialPosition;
    return newInitialState;
}

//==============================================================================
void CircularFixedRadiusStrategy::computeParameters_implementation(Sources const & finalStates,
                                                                   SourcesSnapshots const & initialStates)
{
    auto const & primarySourceFinalState{ finalStates.getPrimarySource() };

    auto const primarySourceInitialAngle{ Radians::fromPoint(initialStates.primary.position) };
    auto const primarySourceFinalAngle{ Radians::fromPoint(primarySourceFinalState.getPos()) };
    mRotation = primarySourceFinalAngle - primarySourceInitialAngle;
    mRadius = primarySourceFinalState.getPos().getDistanceFromOrigin();
}

//==============================================================================
void CircularFixedRadiusStrategy::enforce_implementation(Sources & finalStates,
                                                         SourcesSnapshots const & initialStates,
                                                         SourceIndex const sourceIndex) const
{
    auto const initialAngle{ Radians::fromPoint(initialStates[sourceIndex].position) };
    auto const finalAngle{ (mRotation + initialAngle).getAsRadians() };
    Point<float> const finalPosition{ std::cos(finalAngle) * mRadius, std::sin(finalAngle) * mRadius };

    finalStates[sourceIndex].setPosition(finalPosition, SourceLinkBehavior::doNothing);
}

//==============================================================================
SourceSnapshot CircularFixedRadiusStrategy::computeInitialStateFromFinalState_implementation(
    Sources const & finalStates,
    SourcesSnapshots const & initialStates,
    SourceIndex const sourceIndex) const
{
    auto const finalPosition{ finalStates[sourceIndex].getPos() };
    auto newInitialState{ initialStates[sourceIndex] };

    auto const finalAngle{ Radians::fromPoint(finalPosition) };
    auto const inverseFinalAngle{ (finalAngle - mRotation).getAsRadians() };
    Point<float> const newInitialPosition{ std::cos(inverseFinalAngle) * mRadius,
                                           std::sin(inverseFinalAngle) * mRadius };

    newInitialState.position = newInitialPosition;
    return newInitialState;
}

//==============================================================================
void CircularFixedAngleStrategy::computeParameters_implementation(Sources const & finalStates,
                                                                  SourcesSnapshots const & initialStates)
{
    auto const & primarySourceInitialState{ initialStates.primary };
    auto const & primarySourceFinalState{ finalStates.getPrimarySource() };

    auto const notQuiteZero{ std::nextafter(0.0f, 1.0f) }; // dont divide by zero!
    auto const primarySourceInitialRadius{ std::max(primarySourceInitialState.position.getDistanceFromOrigin(),
                                                    notQuiteZero) };
    mRadiusRatio
        = std::max(primarySourceFinalState.getPos().getDistanceFromOrigin() / primarySourceInitialRadius, notQuiteZero);

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
void CircularFixedAngleStrategy::enforce_implementation(Sources & finalStates,
                                                        SourcesSnapshots const & initialStates,
                                                        SourceIndex const sourceIndex) const
{
    auto const ordering{ mOrdering[sourceIndex.toInt()] };

    auto const finalAngle{ mPrimarySourceFinalAngle + mDeviationPerSource * ordering };
    auto const initialRadius{ initialStates[sourceIndex].position.getDistanceFromOrigin() };
    auto const finalRadius{ initialRadius * mRadiusRatio };
    Point<float> const finalPosition{ std::cos(finalAngle.getAsRadians()) * finalRadius,
                                      std::sin(finalAngle.getAsRadians()) * finalRadius };

    finalStates[sourceIndex].setPosition(finalPosition, SourceLinkBehavior::doNothing);
}

SourceSnapshot
    CircularFixedAngleStrategy::computeInitialStateFromFinalState_implementation(Sources const & finalStates,
                                                                                 SourcesSnapshots const & initialStates,
                                                                                 SourceIndex const sourceIndex) const
{
    SourceSnapshot newInitialState{ initialStates[sourceIndex] };

    auto const newInitialRadius{ finalStates[sourceIndex].getPos().getDistanceFromOrigin() / mRadiusRatio };

    Radians const finalAngle{ std::atan2(finalStates[sourceIndex].getY(), finalStates[sourceIndex].getX()) };
    auto const newInitialAngle{ finalAngle - mRotation };

    Point<float> const newInitialPosition{ std::cos(newInitialAngle.getAsRadians()) * newInitialRadius,
                                           std::sin(newInitialAngle.getAsRadians()) * newInitialRadius };

    newInitialState.position = newInitialPosition;

    return newInitialState;
}

//==============================================================================
void CircularFullyFixedStrategy::computeParameters_implementation(Sources const & finalStates,
                                                                  SourcesSnapshots const & initialStates)
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
void CircularFullyFixedStrategy::enforce_implementation(Sources & finalStates,
                                                        SourcesSnapshots const & initialStates,
                                                        SourceIndex const sourceIndex) const
{
    auto const ordering{ mOrdering[sourceIndex.toInt()] };

    auto const finalAngle{ mPrimarySourceFinalAngle + mDeviationPerSource * ordering };
    Point<float> const finalPosition{ std::cos(finalAngle.getAsRadians()) * mRadius,
                                      std::sin(finalAngle.getAsRadians()) * mRadius };

    finalStates[sourceIndex].setPosition(finalPosition, SourceLinkBehavior::doNothing);
}

//==============================================================================
SourceSnapshot
    CircularFullyFixedStrategy::computeInitialStateFromFinalState_implementation(Sources const & finalStates,
                                                                                 SourcesSnapshots const & initialStates,
                                                                                 SourceIndex const sourceIndex) const
{
    SourceSnapshot newInitialState{ initialStates[sourceIndex] };

    Radians const finalAngle{ std::atan2(finalStates[sourceIndex].getY(), finalStates[sourceIndex].getX()) };
    auto const newInitialAngle{ finalAngle - mRotation };

    Point<float> const newInitialPosition{ std::cos(newInitialAngle.getAsRadians()) * mRadius,
                                           std::sin(newInitialAngle.getAsRadians()) * mRadius };

    newInitialState.position = newInitialPosition;

    return newInitialState;
}

//==============================================================================
void LinkSymmetricXStrategy::computeParameters_implementation(Sources const & finalStates,
                                                              SourcesSnapshots const & initialStates)
{
    mPrimarySourceFinalPosition = finalStates.getPrimarySource().getPos();
}

//==============================================================================
void LinkSymmetricXStrategy::enforce_implementation(Sources & finalStates,
                                                    SourcesSnapshots const & initialStates,
                                                    SourceIndex const sourceIndex) const
{
    Point<float> const finalPosition{ mPrimarySourceFinalPosition.getX(), -mPrimarySourceFinalPosition.getY() };
    finalStates[sourceIndex].setPosition(finalPosition, SourceLinkBehavior::doNothing);
}

//==============================================================================
SourceSnapshot
    LinkSymmetricXStrategy::computeInitialStateFromFinalState_implementation(Sources const & finalStates,
                                                                             SourcesSnapshots const & initialStates,
                                                                             SourceIndex const sourceIndex) const
{
    // nothing to do here!
    return SourceSnapshot{ finalStates[sourceIndex] };
}

//==============================================================================
void LinkSymmetricYStrategy::computeParameters_implementation(Sources const & finalStates,
                                                              SourcesSnapshots const & initialStates)
{
    mPrimarySourceFinalPosition = finalStates.getPrimarySource().getPos();
}

//==============================================================================
void LinkSymmetricYStrategy::enforce_implementation(Sources & finalStates,
                                                    SourcesSnapshots const & initialStates,
                                                    SourceIndex const sourceIndex) const
{
    Point<float> const finalPosition{ -mPrimarySourceFinalPosition.getX(), mPrimarySourceFinalPosition.getY() };
    finalStates[sourceIndex].setPosition(finalPosition, SourceLinkBehavior::doNothing);
}

//==============================================================================
SourceSnapshot
    LinkSymmetricYStrategy::computeInitialStateFromFinalState_implementation(Sources const & finalStates,
                                                                             SourcesSnapshots const & initialStates,
                                                                             SourceIndex const sourceIndex) const
{
    // nothing to do here!
    return SourceSnapshot{ finalStates[sourceIndex] };
}

//==============================================================================
void DeltaLockStrategy::computeParameters_implementation(Sources const & finalStates,
                                                         SourcesSnapshots const & initialStates)
{
    mDelta = finalStates.getPrimarySource().getPos() - initialStates.primary.position;
}

//==============================================================================
void DeltaLockStrategy::enforce_implementation(Sources & finalStates,
                                               SourcesSnapshots const & initialStates,
                                               SourceIndex const sourceIndex) const
{
    auto const finalPosition{ initialStates[sourceIndex].position + mDelta };
    finalStates[sourceIndex].setPosition(finalPosition, SourceLinkBehavior::doNothing);
}

//==============================================================================
SourceSnapshot
    DeltaLockStrategy::computeInitialStateFromFinalState_implementation(Sources const & finalStates,
                                                                        SourcesSnapshots const & initialStates,
                                                                        SourceIndex const sourceIndex) const
{
    SourceSnapshot newInitialState{ initialStates[sourceIndex] };

    auto const newInitialPosition{ finalStates[sourceIndex].getPos() - mDelta };
    newInitialState.position = newInitialPosition;

    return newInitialState;
}

//==============================================================================
void IndependentElevationStrategy::enforce_implementation(Sources & finalStates,
                                                          SourcesSnapshots const & initialStates,
                                                          SourceIndex const sourceIndex) const
{
    finalStates[sourceIndex].setElevation(initialStates[sourceIndex].z, SourceLinkBehavior::doNothing);
}

//==============================================================================
SourceSnapshot IndependentElevationStrategy::computeInitialStateFromFinalState_implementation(
    Sources const & finalStates,
    SourcesSnapshots const & initialStates,
    SourceIndex const sourceIndex) const
{
    return SourceSnapshot{ finalStates[sourceIndex] };
}

//==============================================================================
void FixedElevationStrategy::computeParameters_implementation(Sources const & finalStates,
                                                              SourcesSnapshots const & initialStates)
{
    mElevation = finalStates.getPrimarySource().getElevation();
}

//==============================================================================
void FixedElevationStrategy::enforce_implementation(Sources & finalStates,
                                                    SourcesSnapshots const & initialStates,
                                                    SourceIndex const sourceIndex) const
{
    finalStates[sourceIndex].setElevation(mElevation, SourceLinkBehavior::doNothing);
}

//==============================================================================
SourceSnapshot
    FixedElevationStrategy::computeInitialStateFromFinalState_implementation(Sources const & finalStates,
                                                                             SourcesSnapshots const & initialStates,
                                                                             SourceIndex const sourceIndex) const
{
    return initialStates[sourceIndex];
}

//==============================================================================
void LinearMinElevationStrategy::computeParameters_implementation(Sources const & sources,
                                                                  SourcesSnapshots const & snapshots)
{
    mBaseElevation = sources.getPrimarySource().getElevation();
    mElevationPerSource = ELEVATION_DIFF / (sources.size() - 1);
}

//==============================================================================
void LinearMinElevationStrategy::enforce_implementation(Sources & finalStates,
                                                        SourcesSnapshots const & initialStates,
                                                        SourceIndex const sourceIndex) const
{
    auto const newElevation{ mBaseElevation + mElevationPerSource * sourceIndex.toInt() };
    finalStates[sourceIndex].setElevation(newElevation, SourceLinkBehavior::doNothing);
}

//==============================================================================
SourceSnapshot LinearMinElevationStrategy::computeInitialStateFromFinalState_implementation(
    [[maybe_unused]] Sources const & finalStates,
    SourcesSnapshots const & initialStates,
    SourceIndex const sourceIndex) const
{
    SourceSnapshot result{};
    result.z = mBaseElevation - mElevationPerSource * sourceIndex.toInt();
    return result;
}

//==============================================================================
void LinearMaxElevationStrategy::computeParameters_implementation(Sources const & sources,
                                                                  SourcesSnapshots const & snapshots)
{
    mBaseElevation = sources.getPrimarySource().getElevation();
    mElevationPerSource = ELEVATION_DIFF / (sources.size() - 1);
}

//==============================================================================
void LinearMaxElevationStrategy::enforce_implementation(Sources & finalStates,
                                                        SourcesSnapshots const & initialStates,
                                                        SourceIndex const sourceIndex) const
{
    auto const newElevation{ mBaseElevation + mElevationPerSource * sourceIndex.toInt() };
    finalStates[sourceIndex].setElevation(newElevation, SourceLinkBehavior::doNothing);
}

//==============================================================================
SourceSnapshot
    LinearMaxElevationStrategy::computeInitialStateFromFinalState_implementation(Sources const & finalStates,
                                                                                 SourcesSnapshots const & initialStates,
                                                                                 SourceIndex const sourceIndex) const
{
    return initialStates[sourceIndex];
}

//==============================================================================
void DeltaLockElevationStrategy::computeParameters_implementation(Sources const & sources,
                                                                  SourcesSnapshots const & snapshots)
{
    mDelta = sources.getPrimarySource().getElevation() - snapshots.primary.z;
}

//==============================================================================
void DeltaLockElevationStrategy::enforce_implementation(Sources & finalStates,
                                                        SourcesSnapshots const & initialStates,
                                                        SourceIndex const sourceIndex) const
{
    auto const newElevation{ initialStates[sourceIndex].z + mDelta };
    finalStates[sourceIndex].setElevation(newElevation, SourceLinkBehavior::doNothing);
}

//==============================================================================
SourceSnapshot
    DeltaLockElevationStrategy::computeInitialStateFromFinalState_implementation(Sources const & finalStates,
                                                                                 SourcesSnapshots const & initialStates,
                                                                                 SourceIndex const sourceIndex) const
{
    SourceSnapshot result{ initialStates[sourceIndex] };

    auto const initialElevation{ finalStates[sourceIndex].getElevation() - mDelta };
    result.z = initialElevation;

    return result;
}
