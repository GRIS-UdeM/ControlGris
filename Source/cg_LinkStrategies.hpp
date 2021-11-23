/**************************************************************************
 * Copyright 2021 UdeM - GRIS - Samuel B�land & Olivier Belanger          *
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

#pragma once

#include "cg_ElevationSourceLink.hpp"
#include "cg_PositionSourceLink.hpp"
#include "cg_SourceSnapshot.hpp"
#include "cg_constants.hpp"

namespace gris
{
class Source;
class Sources;
struct SourcesSnapshots;
class SourceLinkEnforcer;

namespace source_link_strategies
{
//==============================================================================
/** Base class for a source link strategy.
 *
 * Link strategies define how the secondary sources move in reaction to the primary source.
 */
class Base
{
    friend SourceLinkEnforcer;
    bool mInitialized{ false };

public:
    //==============================================================================
    Base() noexcept = default;
    virtual ~Base() noexcept = default;
    //==============================================================================
    Base(Base const &) = default;
    Base(Base &&) noexcept = default;
    Base & operator=(Base const &) = default;
    Base & operator=(Base &&) = default;
    //==============================================================================
    void init(Sources const & currentStates, SourcesSnapshots const & initialStates) noexcept(IS_RELEASE);
    void apply(Sources & currentStates, SourcesSnapshots const & initialStates, SourceIndex sourceIndex) const
        noexcept(IS_RELEASE);
    void apply(Sources & currentStates, SourcesSnapshots const & initialStates) const noexcept(IS_RELEASE);
    [[nodiscard]] SourceSnapshot deduceInitialState(Sources const & currentStates,
                                                    SourcesSnapshots const & initialStates,
                                                    SourceIndex sourceIndex) const noexcept(IS_RELEASE);
    [[nodiscard]] bool isInitialized() const noexcept { return mInitialized; }
    //==============================================================================
    static std::unique_ptr<Base> make(PositionSourceLink sourceLink) noexcept(IS_RELEASE);
    static std::unique_ptr<Base> make(ElevationSourceLink sourceLink) noexcept(IS_RELEASE);

private:
    //==============================================================================
    virtual void initImpl(Sources const & currentStates, SourcesSnapshots const & initialStates) = 0;
    virtual void
        applyImpl(Sources & currentStates, SourcesSnapshots const & initialStates, SourceIndex sourceIndex) const = 0;
    [[nodiscard]] virtual SourceSnapshot deduceInitialStateImpl(Sources const & currentStates,
                                                                SourcesSnapshots const & initialStates,
                                                                SourceIndex sourceIndex) const = 0;
    //==============================================================================
    JUCE_LEAK_DETECTOR(Base)

}; // class Base

//==============================================================================
/** Only use full to recall saved positions */
class PositionIndependent final : public Base
{
    void initImpl(Sources const &, SourcesSnapshots const &) noexcept(IS_RELEASE) override {}
    void applyImpl(Sources & finalStates, SourcesSnapshots const & initialStates, SourceIndex sourceIndex) const
        noexcept(IS_RELEASE) override;
    [[nodiscard]] SourceSnapshot deduceInitialStateImpl(Sources const & finalStates,
                                                        SourcesSnapshots const & initialStates,
                                                        SourceIndex sourceIndex) const noexcept(IS_RELEASE) override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(PositionIndependent)
};

//==============================================================================
class Circular final : public Base
{
    Radians mRotation{};
    float mRadiusRatio{};
    //==============================================================================
    void initImpl(Sources const & finalState, SourcesSnapshots const & initialStates) noexcept(IS_RELEASE) override;
    void applyImpl(Sources & finalStates, SourcesSnapshots const & initialStates, SourceIndex sourceIndex) const
        noexcept(IS_RELEASE) override;
    [[nodiscard]] SourceSnapshot deduceInitialStateImpl(Sources const & finalStates,
                                                        SourcesSnapshots const & initialStates,
                                                        SourceIndex sourceIndex) const noexcept(IS_RELEASE) override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(Circular)
};

//==============================================================================
class CircularFixedRadius final : public Base
{
    Radians mRotation{};
    float mRadius{};
    //==============================================================================
    void initImpl(Sources const & finalStates, SourcesSnapshots const & initialStates) noexcept(IS_RELEASE) override;
    void applyImpl(Sources & finalStates, SourcesSnapshots const & initialStates, SourceIndex sourceIndex) const
        noexcept(IS_RELEASE) override;
    [[nodiscard]] SourceSnapshot deduceInitialStateImpl(Sources const & finalStates,
                                                        SourcesSnapshots const & initialStates,
                                                        SourceIndex sourceIndex) const noexcept(IS_RELEASE) override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(CircularFixedRadius)
};

//==============================================================================
class CircularFixedAngle final : public Base
{
    Radians mDeviationPerSource{};
    Radians mPrimarySourceFinalAngle{};
    Radians mRotation{};
    float mRadiusRatio{};
    std::array<int, MAX_NUMBER_OF_SOURCES> mOrdering{};
    //==============================================================================
    void initImpl(Sources const & finalStates, SourcesSnapshots const & initialStates) noexcept(IS_RELEASE) override;
    void applyImpl(Sources & finalStates, SourcesSnapshots const & initialStates, SourceIndex sourceIndex) const
        noexcept(IS_RELEASE) override;
    [[nodiscard]] SourceSnapshot deduceInitialStateImpl(Sources const & finalStates,
                                                        SourcesSnapshots const & initialStates,
                                                        SourceIndex sourceIndex) const noexcept(IS_RELEASE) override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(CircularFixedAngle)
};

//==============================================================================
// TODO : copy-pasted code from fixedAngle
class CircularFullyFixed final : public Base
{
    Radians mDeviationPerSource{};
    Radians mPrimarySourceFinalAngle{};
    Radians mRotation{};
    float mRadius{};
    std::array<int, MAX_NUMBER_OF_SOURCES> mOrdering{};
    //==============================================================================
    void initImpl(Sources const & finalStates, SourcesSnapshots const & initialStates) noexcept(IS_RELEASE) override;
    void applyImpl(Sources & finalStates, SourcesSnapshots const & initialStates, SourceIndex sourceIndex) const
        noexcept(IS_RELEASE) override;
    [[nodiscard]] SourceSnapshot deduceInitialStateImpl(Sources const & finalStates,
                                                        SourcesSnapshots const & initialStates,
                                                        SourceIndex sourceIndex) const noexcept(IS_RELEASE) override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(CircularFullyFixed)
};

//==============================================================================
class SymmetricX final : public Base
{
    juce::Point<float> mPrimarySourceFinalPosition;
    //==============================================================================
    void initImpl(Sources const & finalStates, SourcesSnapshots const & initialStates) noexcept(IS_RELEASE) override;
    void applyImpl(Sources & finalStates, SourcesSnapshots const & initialStates, SourceIndex sourceIndex) const
        noexcept(IS_RELEASE) override;
    [[nodiscard]] SourceSnapshot deduceInitialStateImpl(Sources const & finalStates,
                                                        SourcesSnapshots const & initialStates,
                                                        SourceIndex sourceIndex) const noexcept(IS_RELEASE) override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(SymmetricX)
};

//==============================================================================
class SymmetricY final : public Base
{
    juce::Point<float> mPrimarySourceFinalPosition;
    //==============================================================================
    void initImpl(Sources const & finalStates, SourcesSnapshots const & initialStates) noexcept(IS_RELEASE) override;
    void applyImpl(Sources & finalStates, SourcesSnapshots const & initialStates, SourceIndex sourceIndex) const
        noexcept(IS_RELEASE) override;
    [[nodiscard]] SourceSnapshot deduceInitialStateImpl(Sources const & finalStates,
                                                        SourcesSnapshots const & initialStates,
                                                        SourceIndex sourceIndex) const noexcept(IS_RELEASE) override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(SymmetricY)
};

//==============================================================================
class PositionDeltaLock final : public Base
{
    juce::Point<float> mDelta;
    //==============================================================================
    void initImpl(Sources const & finalStates, SourcesSnapshots const & initialStates) noexcept(IS_RELEASE) override;
    void applyImpl(Sources & finalStates, SourcesSnapshots const & initialStates, SourceIndex sourceIndex) const
        noexcept(IS_RELEASE) override;
    [[nodiscard]] SourceSnapshot deduceInitialStateImpl(Sources const & finalStates,
                                                        SourcesSnapshots const & initialStates,
                                                        SourceIndex sourceIndex) const noexcept(IS_RELEASE) override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(PositionDeltaLock)
};

//==============================================================================
// only usefuLl to recall saved positions
class ElevationIndependent final : public Base
{
    void initImpl(Sources const &, SourcesSnapshots const &) noexcept(IS_RELEASE) override {}
    void applyImpl(Sources & finalStates, SourcesSnapshots const & initialStates, SourceIndex sourceIndex) const
        noexcept(IS_RELEASE) override;
    [[nodiscard]] SourceSnapshot deduceInitialStateImpl(Sources const & finalStates,
                                                        SourcesSnapshots const & initialStates,
                                                        SourceIndex sourceIndex) const noexcept(IS_RELEASE) override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(ElevationIndependent)
};

//==============================================================================
class FixedElevation final : public Base
{
    Radians mElevation{};
    //==============================================================================
    void initImpl(Sources const & finalStates, SourcesSnapshots const & initialStates) noexcept(IS_RELEASE) override;
    void applyImpl(Sources & finalStates, SourcesSnapshots const & initialStates, SourceIndex sourceIndex) const
        noexcept(IS_RELEASE) override;
    [[nodiscard]] SourceSnapshot deduceInitialStateImpl(Sources const & finalStates,
                                                        SourcesSnapshots const & initialStates,
                                                        SourceIndex sourceIndex) const noexcept(IS_RELEASE) override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(FixedElevation)
};

//==============================================================================
class LinearMin final : public Base
{
    static constexpr Radians ELEVATION_DIFF{ -MAX_ELEVATION / 3.0f * 2.0f };
    Radians mBaseElevation{};
    Radians mElevationPerSource{};
    //==============================================================================
    void initImpl(Sources const & sources, SourcesSnapshots const & snapshots) noexcept(IS_RELEASE) override;
    void applyImpl(Sources & finalStates, SourcesSnapshots const & initialStates, SourceIndex sourceIndex) const
        noexcept(IS_RELEASE) override;
    [[nodiscard]] SourceSnapshot deduceInitialStateImpl(Sources const & finalStates,
                                                        SourcesSnapshots const & initialStates,
                                                        SourceIndex sourceIndex) const noexcept(IS_RELEASE) override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(LinearMin)
};

//==============================================================================
class LinearMax final : public Base
{
    static constexpr Radians ELEVATION_DIFF{ MAX_ELEVATION / 3.0f * 2.0f };
    Radians mBaseElevation{};
    Radians mElevationPerSource{};
    //==============================================================================
    void initImpl(Sources const & sources, SourcesSnapshots const & snapshots) noexcept(IS_RELEASE) override;
    void applyImpl(Sources & finalStates, SourcesSnapshots const & initialStates, SourceIndex sourceIndex) const
        noexcept(IS_RELEASE) override;
    [[nodiscard]] SourceSnapshot deduceInitialStateImpl(Sources const & finalStates,
                                                        SourcesSnapshots const & initialStates,
                                                        SourceIndex sourceIndex) const noexcept(IS_RELEASE) override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(LinearMax)
};

//==============================================================================
class ElevationDeltaLock final : public Base
{
    Radians mDelta{};
    //==============================================================================
    void initImpl(Sources const & sources, SourcesSnapshots const & snapshots) noexcept(IS_RELEASE) override;
    void applyImpl(Sources & finalStates, SourcesSnapshots const & initialStates, SourceIndex sourceIndex) const
        noexcept(IS_RELEASE) override;
    [[nodiscard]] SourceSnapshot deduceInitialStateImpl(Sources const & finalStates,
                                                        SourcesSnapshots const & initialStates,
                                                        SourceIndex sourceIndex) const noexcept(IS_RELEASE) override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(ElevationDeltaLock)
};

} // namespace source_link_strategies

} // namespace gris