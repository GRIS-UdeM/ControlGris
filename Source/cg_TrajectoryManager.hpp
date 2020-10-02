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
#pragma once

#include <optional>

#include <JuceHeader.h>

#include "cg_Source.hpp"
#include "cg_Trajectory.hpp"
#include "cg_constants.hpp"

class ControlGrisAudioProcessor;

//==============================================================================
class AutomationManager
{
public:
    //==============================================================================
    class Listener
    {
    public:
        Listener() noexcept = default;
        virtual ~Listener() noexcept = default;

        Listener(Listener const &) = delete;
        Listener(Listener &&) = delete;

        Listener & operator=(Listener const &) = delete;
        Listener & operator=(Listener &&) = delete;
        //==============================================================================
        virtual void trajectoryPositionChanged(AutomationManager * manager, Point<float> position, Radians elevation)
            = 0;

    private:
        //==============================================================================
        JUCE_LEAK_DETECTOR(Listener)

    }; // class AutomationManager::Listener

    //==============================================================================
    enum class Direction { forward, backward };

protected:
    //==============================================================================
    ControlGrisAudioProcessor & mProcessor;

    ListenerList<Listener> mListeners;

    bool mIsBackAndForth{ false };
    Direction mBackAndForthDirection{ Direction::forward };

    int mDampeningCycles{};
    int mDampeningCycleCount{};
    double mDampeningLastDelta{};

    bool mActivateState{ false };
    double mPlaybackDuration{ 5.0 };
    double mCurrentPlaybackDuration{ 5.0 };

    double mTrajectoryDeltaTime{};
    double mLastTrajectoryDeltaTime{};
    std::optional<Trajectory> mTrajectory{};
    Point<float> mCurrentTrajectoryPoint{};
    Point<float> mLastRecordingPoint{};

    Degrees mDegreeOfDeviationPerCycle{};
    Degrees mCurrentDegreeOfDeviation{};
    int mDeviationCycleCount{};

    Source & mPrimarySource;

public:
    //==============================================================================
    AutomationManager(ControlGrisAudioProcessor & processor, Source & principalSource) noexcept;
    virtual ~AutomationManager() = default;

    AutomationManager(AutomationManager const &) = delete;
    AutomationManager(AutomationManager &&) = delete;

    AutomationManager & operator=(AutomationManager const &) = delete;
    AutomationManager & operator=(AutomationManager &&) = delete;
    //==============================================================================
    [[nodiscard]] ControlGrisAudioProcessor & getProcessor() const { return mProcessor; }

    void setPositionActivateState(bool state);
    [[nodiscard]] bool getPositionActivateState() const { return mActivateState; }

    void setPlaybackDuration(double const value) { mPlaybackDuration = value; }

    void resetRecordingTrajectory(Point<float> currentPosition);
    void addRecordingPoint(Point<float> const & pos);
    [[nodiscard]] Point<float> getCurrentTrajectoryPoint() const;

    void setTrajectoryDeltaTime(double relativeTimeFromPlay);
    [[nodiscard]] std::optional<Trajectory> const & getTrajectory() const { return mTrajectory; }

    void setPositionBackAndForth(bool const newState) { mIsBackAndForth = newState; }

    void setPositionDampeningCycles(int const value) { this->mDampeningCycles = value; }
    void setDeviationPerCycle(Degrees const value) { this->mDegreeOfDeviationPerCycle = value; }
    void setPrimarySourcePosition(Point<float> const & pos) const;
    void addListener(Listener * l) { mListeners.add(l); }

    void sourceMoved(Source & source);
    //==============================================================================
    virtual void sendTrajectoryPositionChangedEvent() = 0;
    virtual void recomputeTrajectory() = 0;
    virtual void applyCurrentTrajectoryPointToPrimarySource() = 0;

private:
    //==============================================================================
    void invertBackAndForthDirection();
    void computeCurrentTrajectoryPoint();
    [[nodiscard]] Point<float> smoothRecordingPosition(Point<float> const & pos);
    //==============================================================================
    JUCE_LEAK_DETECTOR(AutomationManager)

}; // AutomationManager

//==============================================================================
class PositionAutomationManager final : public AutomationManager
{
    PositionTrajectoryType mTrajectoryType{ PositionTrajectoryType::drawing };
    PositionSourceLink mSourceLink{ PositionSourceLink::independent };

public:
    //==============================================================================
    PositionAutomationManager() = delete;
    ~PositionAutomationManager() noexcept override = default;

    PositionAutomationManager(PositionAutomationManager const &) = delete;
    PositionAutomationManager(PositionAutomationManager &&) = delete;

    PositionAutomationManager & operator=(PositionAutomationManager const &) = delete;
    PositionAutomationManager & operator=(PositionAutomationManager &&) = delete;
    //==============================================================================
    PositionAutomationManager(ControlGrisAudioProcessor & processor, Source & principalSource) noexcept
        : AutomationManager(processor, principalSource)
    {
    }
    //==============================================================================
    [[nodiscard]] PositionTrajectoryType getTrajectoryType() const { return mTrajectoryType; }
    void setTrajectoryType(PositionTrajectoryType type, Point<float> const & startPos);

    void setSourceLink(PositionSourceLink const sourceLink) { mSourceLink = sourceLink; }
    [[nodiscard]] PositionSourceLink getSourceLink() const { return mSourceLink; }
    //==============================================================================
    void applyCurrentTrajectoryPointToPrimarySource() override;
    void sendTrajectoryPositionChangedEvent() override;
    void recomputeTrajectory() override;

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR(PositionAutomationManager)

}; // PositionAutomationManager

//==============================================================================
class ElevationAutomationManager final : public AutomationManager
{
    ElevationTrajectoryType mTrajectoryType{ ElevationTrajectoryType::drawing };
    ElevationSourceLink mSourceLink{ ElevationSourceLink::independent };

public:
    //==============================================================================
    ElevationAutomationManager() = delete;
    ~ElevationAutomationManager() noexcept override = default;

    ElevationAutomationManager(ElevationAutomationManager const &) = delete;
    ElevationAutomationManager(ElevationAutomationManager &&) = delete;

    ElevationAutomationManager & operator=(ElevationAutomationManager const &) = delete;
    ElevationAutomationManager & operator=(ElevationAutomationManager &&) = delete;
    //==============================================================================
    ElevationAutomationManager(ControlGrisAudioProcessor & processor, Source & principalSource) noexcept
        : AutomationManager(processor, principalSource)
    {
    }
    //==============================================================================
    void setTrajectoryType(ElevationTrajectoryType type);
    [[nodiscard]] ElevationTrajectoryType getTrajectoryType() const { return mTrajectoryType; }

    void setSourceLink(ElevationSourceLink const sourceLink) { mSourceLink = sourceLink; }
    [[nodiscard]] ElevationSourceLink getSourceLink() const { return mSourceLink; }

    void sendTrajectoryPositionChangedEvent() override;
    void applyCurrentTrajectoryPointToPrimarySource() override;
    void recomputeTrajectory() override;

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR(ElevationAutomationManager)

}; // ElevationAutomationManager