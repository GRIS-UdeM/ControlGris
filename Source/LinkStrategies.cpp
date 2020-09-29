#include "LinkStrategies.h"

#include "Source.h"

//==============================================================================
void LinkStrategy::computeParameters(Sources const & finalStates, SourcesSnapshots const & initialStates)
{
    computeParameters_implementation(finalStates, initialStates);
    mInitialized = true;
}

//==============================================================================
void LinkStrategy::enforce(std::array<Source, MAX_NUMBER_OF_SOURCES - 1> & finalSecondaryStates,
                           Array<SourceSnapshot> const & initialSecondaryStates) const
{
    for (size_t i{}; i < initialSecondaryStates.size(); ++i) {
        // TODO: this is applied to more sources than it should
        auto & finalState{ finalSecondaryStates[i] };
        auto const & initialState{ initialSecondaryStates.getReference(i) };
        enforce(finalState, initialState);
    }
}

//==============================================================================
void LinkStrategy::enforce(Source & finalState, SourceSnapshot const & initialState) const
{
    jassert(mInitialized);
    enforce_implementation(finalState, initialState);
}

//==============================================================================
SourceSnapshot LinkStrategy::computeInitialStateFromFinalState(Source const & finalState,
                                                               SourceSnapshot const & initialState) const
{
    jassert(mInitialized);
    return computeInitialStateFromFinalState_implementation(finalState, initialState);
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
void IndependentStrategy::enforce_implementation(Source & finalState, SourceSnapshot const & initialState) const
{
    finalState.setPosition(initialState.position, SourceLinkBehavior::doNothing);
}

//==============================================================================
SourceSnapshot
    IndependentStrategy::computeInitialStateFromFinalState_implementation(Source const & finalState,
                                                                          SourceSnapshot const & initialState) const
{
    SourceSnapshot const result{ finalState };
    return result;
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
void CircularStrategy::enforce_implementation(Source & finalState, SourceSnapshot const & initialState) const
{
    auto const finalPosition{ initialState.position.rotatedAboutOrigin(mRotation.getAsRadians()) * mRadiusRatio };
    finalState.setPosition(finalPosition, SourceLinkBehavior::doNothing);
}

//==============================================================================
SourceSnapshot
    CircularStrategy::computeInitialStateFromFinalState_implementation(Source const & finalState,
                                                                       SourceSnapshot const & initialState) const
{
    SourceSnapshot newInitialState{ initialState };
    auto const newInitialPosition{ (finalState.getPos() / mRadiusRatio).rotatedAboutOrigin(-mRotation.getAsRadians()) };
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
void CircularFixedRadiusStrategy::enforce_implementation(Source & finalState, SourceSnapshot const & initialState) const
{
    auto const initialAngle{ Radians::fromPoint(initialState.position) };
    auto const finalAngle{ (mRotation + initialAngle).getAsRadians() };
    Point<float> const finalPosition{ std::cos(finalAngle) * mRadius, std::sin(finalAngle) * mRadius };

    finalState.setPosition(finalPosition, SourceLinkBehavior::doNothing);
}

//==============================================================================
SourceSnapshot CircularFixedRadiusStrategy::computeInitialStateFromFinalState_implementation(
    Source const & finalState,
    SourceSnapshot const & intialState) const
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
void CircularFixedAngleStrategy::enforce_implementation(Source & finalState, SourceSnapshot const & initialState) const
{
    auto const sourceIndex{ finalState.getIndex() };
    auto const ordering{ mOrdering[sourceIndex.toInt()] };

    auto const finalAngle{ mPrimarySourceFinalAngle + mDeviationPerSource * ordering };
    auto const initialRadius{ initialState.position.getDistanceFromOrigin() };
    auto const finalRadius{ initialRadius * mRadiusRatio };
    Point<float> const finalPosition{ std::cos(finalAngle.getAsRadians()) * finalRadius,
                                      std::sin(finalAngle.getAsRadians()) * finalRadius };

    finalState.setPosition(finalPosition, SourceLinkBehavior::doNothing);
}

SourceSnapshot CircularFixedAngleStrategy::computeInitialStateFromFinalState_implementation(
    Source const & finalState,
    SourceSnapshot const & initialState) const
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
void CircularFullyFixedStrategy::enforce_implementation(Source & finalState, SourceSnapshot const & initialState) const
{
    auto const sourceIndex{ finalState.getIndex() };
    auto const ordering{ mOrdering[sourceIndex.toInt()] };

    auto const finalAngle{ mPrimarySourceFinalAngle + mDeviationPerSource * ordering };
    Point<float> const finalPosition{ std::cos(finalAngle.getAsRadians()) * mRadius,
                                      std::sin(finalAngle.getAsRadians()) * mRadius };

    finalState.setPosition(finalPosition, SourceLinkBehavior::doNothing);
}

//==============================================================================
SourceSnapshot CircularFullyFixedStrategy::computeInitialStateFromFinalState_implementation(
    Source const & finalState,
    SourceSnapshot const & initialState) const
{
    SourceSnapshot newInitialState{ initialState };

    Radians const finalAngle{ std::atan2(finalState.getY(), finalState.getX()) };
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
void LinkSymmetricXStrategy::enforce_implementation(Source & finalState, SourceSnapshot const & initialState) const
{
    Point<float> const finalPosition{ mPrimarySourceFinalPosition.getX(), -mPrimarySourceFinalPosition.getY() };
    finalState.setPosition(finalPosition, SourceLinkBehavior::doNothing);
}

//==============================================================================
SourceSnapshot
    LinkSymmetricXStrategy::computeInitialStateFromFinalState_implementation(Source const & finalState,
                                                                             SourceSnapshot const & initialState) const
{
    // nothing to do here!
    return SourceSnapshot{ finalState };
}

//==============================================================================
void LinkSymmetricYStrategy::computeParameters_implementation(Sources const & finalStates,
                                                              SourcesSnapshots const & initialStates)
{
    mPrimarySourceFinalPosition = finalStates.getPrimarySource().getPos();
}

//==============================================================================
void LinkSymmetricYStrategy::enforce_implementation(Source & finalState, SourceSnapshot const & initialState) const
{
    Point<float> const finalPosition{ -mPrimarySourceFinalPosition.getX(), mPrimarySourceFinalPosition.getY() };
    finalState.setPosition(finalPosition, SourceLinkBehavior::doNothing);
}

//==============================================================================
SourceSnapshot
    LinkSymmetricYStrategy::computeInitialStateFromFinalState_implementation(Source const & finalState,
                                                                             SourceSnapshot const & intialState) const
{
    // nothing to do here!
    return SourceSnapshot{ finalState };
}

//==============================================================================
void DeltaLockStrategy::computeParameters_implementation(Sources const & finalStates,
                                                         SourcesSnapshots const & initialStates)
{
    mDelta = finalStates.getPrimarySource().getPos() - initialStates.primary.position;
}

//==============================================================================
void DeltaLockStrategy::enforce_implementation(Source & finalState, SourceSnapshot const & initialState) const
{
    auto const finalPosition{ initialState.position + mDelta };
    finalState.setPosition(finalPosition, SourceLinkBehavior::doNothing);
}

//==============================================================================
SourceSnapshot
    DeltaLockStrategy::computeInitialStateFromFinalState_implementation(Source const & finalState,
                                                                        SourceSnapshot const & intialState) const
{
    SourceSnapshot newInitialState{ intialState };

    auto const newInitialPosition{ finalState.getPos() - mDelta };
    newInitialState.position = newInitialPosition;

    return newInitialState;
}

//==============================================================================
void IndependentElevationStrategy::enforce_implementation(Source & finalState,
                                                          SourceSnapshot const & initialState) const
{
    finalState.setElevation(initialState.z, SourceLinkBehavior::doNothing);
}

//==============================================================================
SourceSnapshot IndependentElevationStrategy::computeInitialStateFromFinalState_implementation(
    Source const & finalState,
    SourceSnapshot const & initialState) const
{
    return SourceSnapshot{ finalState };
}

//==============================================================================
void FixedElevationStrategy::computeParameters_implementation(Sources const & finalStates,
                                                              SourcesSnapshots const & initialStates)
{
    mElevation = finalStates.getPrimarySource().getElevation();
}

//==============================================================================
void FixedElevationStrategy::enforce_implementation(Source & finalState, SourceSnapshot const & initialState) const
{
    finalState.setElevation(mElevation, SourceLinkBehavior::doNothing);
}

//==============================================================================
SourceSnapshot
    FixedElevationStrategy::computeInitialStateFromFinalState_implementation(Source const & finalState,
                                                                             SourceSnapshot const & initialState) const
{
    return initialState;
}

//==============================================================================
void LinearMinElevationStrategy::computeParameters_implementation(Sources const & sources,
                                                                  SourcesSnapshots const & snapshots)
{
    mBaseElevation = sources.getPrimarySource().getElevation();
    mElevationPerSource = ELEVATION_DIFF / (sources.size() - 1);
}

//==============================================================================
void LinearMinElevationStrategy::enforce_implementation(Source & source, SourceSnapshot const & snapshot) const
{
    auto const sourceIndex{ source.getIndex().toInt() };
    auto const newElevation{ mBaseElevation + mElevationPerSource * sourceIndex };
    source.setElevation(newElevation, SourceLinkBehavior::doNothing);
}

//==============================================================================
SourceSnapshot
    LinearMinElevationStrategy::computeInitialStateFromFinalState_implementation(Source const & source,
                                                                                 SourceSnapshot const & snapshot) const
{
    return snapshot;
}

//==============================================================================
void LinearMaxElevationStrategy::computeParameters_implementation(Sources const & sources,
                                                                  SourcesSnapshots const & snapshots)
{
    mBaseElevation = sources.getPrimarySource().getElevation();
    mElevationPerSource = ELEVATION_DIFF / (sources.size() - 1);
}

//==============================================================================
void LinearMaxElevationStrategy::enforce_implementation(Source & source, SourceSnapshot const & snapshot) const
{
    auto const sourceIndex{ source.getIndex().toInt() };
    auto const newElevation{ mBaseElevation + mElevationPerSource * sourceIndex };
    source.setElevation(newElevation, SourceLinkBehavior::doNothing);
}

//==============================================================================
SourceSnapshot
    LinearMaxElevationStrategy::computeInitialStateFromFinalState_implementation(Source const & source,
                                                                                 SourceSnapshot const & snapshot) const
{
    return snapshot;
}

//==============================================================================
void DeltaLockElevationStrategy::computeParameters_implementation(Sources const & sources,
                                                                  SourcesSnapshots const & snapshots)
{
    mDelta = sources.getPrimarySource().getElevation() - snapshots.primary.z;
}

//==============================================================================
void DeltaLockElevationStrategy::enforce_implementation(Source & source, SourceSnapshot const & snapshot) const
{
    auto const newElevation{ snapshot.z + mDelta };
    source.setElevation(newElevation, SourceLinkBehavior::doNothing);
}

//==============================================================================
SourceSnapshot
    DeltaLockElevationStrategy::computeInitialStateFromFinalState_implementation(Source const & source,
                                                                                 SourceSnapshot const & snapshot) const
{
    SourceSnapshot result{ snapshot };

    auto const initialElevation{ source.getElevation() - mDelta };
    result.z = initialElevation;

    return result;
}
