#pragma once

#include <JuceHeader.h>

#include "ControlGrisConstants.h"
#include "Source.h"
#include "SourceSnapshot.h"

class Source;
class Sources;
class SourcesSnapshots;

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
    void computeParameters(Sources const & finalStates, SourcesSnapshots const & initialStates);
    void enforce(std::array<Source, MAX_NUMBER_OF_SOURCES - 1> & finalSecondaryStates,
                 Array<SourceSnapshot> const & initialSecondaryStates) const;
    void enforce(Source & finalState, SourceSnapshot const & initialState) const;
    [[nodiscard]] SourceSnapshot computeInitialStateFromFinalState(Source const & finalState,
                                                                   SourceSnapshot const & initialState) const;
    //==============================================================================
    static std::unique_ptr<LinkStrategy> make(PositionSourceLink sourceLink);
    static std::unique_ptr<LinkStrategy> make(ElevationSourceLink sourceLink);

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
    void enforce_implementation(Source & finalState, SourceSnapshot const & initialState) const override;
    [[nodiscard]] SourceSnapshot computeInitialStateFromFinalState_implementation(
        Source const & finalState,
        [[maybe_unused]] SourceSnapshot const & initialState) const override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(IndependentStrategy)
};

//==============================================================================
class CircularStrategy final : public LinkStrategy
{
    Radians mRotation{};
    float mRadiusRatio{};
    //==============================================================================
    void computeParameters_implementation(Sources const & finalState, SourcesSnapshots const & initialStates) override;
    void enforce_implementation(Source & finalState, SourceSnapshot const & initialState) const override;
    [[nodiscard]] SourceSnapshot
        computeInitialStateFromFinalState_implementation(Source const & finalState,
                                                         SourceSnapshot const & initialState) const override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(CircularStrategy)
};

//==============================================================================
class CircularFixedRadiusStrategy final : public LinkStrategy
{
    Radians mRotation{};
    float mRadius{};
    //==============================================================================
    void computeParameters_implementation(Sources const & finalStates, SourcesSnapshots const & initialStates) override;
    void enforce_implementation(Source & finalState, SourceSnapshot const & initialState) const override;
    [[nodiscard]] SourceSnapshot
        computeInitialStateFromFinalState_implementation(Source const & finalState,
                                                         SourceSnapshot const & intialState) const override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(CircularFixedRadiusStrategy)
};

//==============================================================================
class CircularFixedAngleStrategy final : public LinkStrategy
{
    Radians mDeviationPerSource{};
    Radians mPrimarySourceFinalAngle{};
    Radians mRotation{};
    float mRadiusRatio{};
    std::array<int, MAX_NUMBER_OF_SOURCES> mOrdering{};
    //==============================================================================
    void computeParameters_implementation(Sources const & finalStates, SourcesSnapshots const & initialStates) override;
    void enforce_implementation(Source & finalState, SourceSnapshot const & initialState) const override;
    [[nodiscard]] SourceSnapshot
        computeInitialStateFromFinalState_implementation(Source const & finalState,
                                                         SourceSnapshot const & initialState) const override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(CircularFixedAngleStrategy)
};

//==============================================================================
// TODO : copy-pasted code from fixedAngle
class CircularFullyFixedStrategy final : public LinkStrategy
{
    Radians mDeviationPerSource{};
    Radians mPrimarySourceFinalAngle{};
    Radians mRotation{};
    float mRadius{};
    std::array<int, MAX_NUMBER_OF_SOURCES> mOrdering{};
    //==============================================================================
    void computeParameters_implementation(Sources const & finalStates, SourcesSnapshots const & initialStates) override;
    void enforce_implementation(Source & finalState, SourceSnapshot const & initialState) const override;
    [[nodiscard]] SourceSnapshot
        computeInitialStateFromFinalState_implementation(Source const & finalState,
                                                         SourceSnapshot const & initialState) const override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(CircularFullyFixedStrategy)
};

//==============================================================================
class LinkSymmetricXStrategy final : public LinkStrategy
{
    Point<float> mPrimarySourceFinalPosition;
    //==============================================================================
    void computeParameters_implementation(Sources const & finalStates,
                                          [[maybe_unused]] SourcesSnapshots const & initialStates) override;
    void enforce_implementation(Source & finalState,
                                [[maybe_unused]] SourceSnapshot const & initialState) const override;
    [[nodiscard]] SourceSnapshot
        computeInitialStateFromFinalState_implementation([[maybe_unused]] Source const & finalState,
                                                         SourceSnapshot const & initialState) const override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(LinkSymmetricXStrategy)
};

//==============================================================================
class LinkSymmetricYStrategy final : public LinkStrategy
{
    Point<float> mPrimarySourceFinalPosition;
    //==============================================================================
    void computeParameters_implementation(Sources const & finalStates,
                                          [[maybe_unused]] SourcesSnapshots const & initialStates) override;
    void enforce_implementation(Source & finalState,
                                [[maybe_unused]] SourceSnapshot const & initialState) const override;
    [[nodiscard]] SourceSnapshot
        computeInitialStateFromFinalState_implementation([[maybe_unused]] Source const & finalState,
                                                         SourceSnapshot const & intialState) const override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(LinkSymmetricYStrategy)
};

//==============================================================================
class DeltaLockStrategy final : public LinkStrategy
{
    Point<float> mDelta;
    //==============================================================================
    void computeParameters_implementation(Sources const & finalStates, SourcesSnapshots const & initialStates) override;
    void enforce_implementation(Source & finalState, SourceSnapshot const & initialState) const override;
    [[nodiscard]] SourceSnapshot
        computeInitialStateFromFinalState_implementation(Source const & finalState,
                                                         SourceSnapshot const & intialState) const override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(DeltaLockStrategy)
};

//==============================================================================
// only usefuLl to recall saved positions
class IndependentElevationStrategy final : public LinkStrategy
{
    void computeParameters_implementation(Sources const &, SourcesSnapshots const &) override {}
    void enforce_implementation(Source & finalState, SourceSnapshot const & initialState) const override;
    [[nodiscard]] SourceSnapshot
        computeInitialStateFromFinalState_implementation([[maybe_unused]] Source const & finalState,
                                                         SourceSnapshot const & initialState) const override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(IndependentElevationStrategy)
};

//==============================================================================
class FixedElevationStrategy final : public LinkStrategy
{
    Radians mElevation{};
    //==============================================================================
    void computeParameters_implementation(Sources const & finalStates,
                                          [[maybe_unused]] SourcesSnapshots const & initialStates) override;
    void enforce_implementation(Source & finalState,
                                [[maybe_unused]] SourceSnapshot const & initialState) const override;
    [[nodiscard]] SourceSnapshot
        computeInitialStateFromFinalState_implementation(Source const & finalState,
                                                         SourceSnapshot const & initialState) const override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(FixedElevationStrategy)
};

//==============================================================================
class LinearMinElevationStrategy final : public LinkStrategy
{
    static constexpr Radians ELEVATION_DIFF{ -MAX_ELEVATION / 3.0f * 2.0f };
    Radians mBaseElevation{};
    Radians mElevationPerSource{};
    //==============================================================================
    void computeParameters_implementation(Sources const & sources, SourcesSnapshots const & snapshots) override;
    void enforce_implementation(Source & source, [[maybe_unused]] SourceSnapshot const & snapshot) const override;
    [[nodiscard]] SourceSnapshot
        computeInitialStateFromFinalState_implementation([[maybe_unused]] Source const & source,
                                                         SourceSnapshot const & snapshot) const override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(LinearMinElevationStrategy)
};

//==============================================================================
class LinearMaxElevationStrategy final : public LinkStrategy
{
    static constexpr Radians ELEVATION_DIFF{ MAX_ELEVATION / 3.0f * 2.0f };
    Radians mBaseElevation{};
    Radians mElevationPerSource{};
    //==============================================================================
    void computeParameters_implementation(Sources const & sources, SourcesSnapshots const & snapshots) override;
    void enforce_implementation(Source & source, [[maybe_unused]] SourceSnapshot const & snapshot) const override;
    [[nodiscard]] SourceSnapshot
        computeInitialStateFromFinalState_implementation([[maybe_unused]] Source const & source,
                                                         SourceSnapshot const & snapshot) const override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(LinearMaxElevationStrategy)
};

//==============================================================================
class DeltaLockElevationStrategy final : public LinkStrategy
{
    Radians mDelta;
    //==============================================================================
    void computeParameters_implementation(Sources const & sources, SourcesSnapshots const & snapshots) override;
    void enforce_implementation(Source & source, SourceSnapshot const & snapshot) const override;
    [[nodiscard]] SourceSnapshot
        computeInitialStateFromFinalState_implementation(Source const & source,
                                                         SourceSnapshot const & snapshot) const override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(DeltaLockElevationStrategy)
};
