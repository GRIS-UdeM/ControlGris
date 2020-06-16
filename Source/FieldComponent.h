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

#include <type_traits>

#include "../JuceLibraryCode/JuceHeader.h"

#include "AutomationManager.h"
#include "ElevationSourceComponent.h"
#include "GrisLookAndFeel.h"
#include "PositionSourceComponent.h"
#include "SettingsBoxComponent.h"
#include "Source.h"
#include "TrajectoryHandleComponent.h"

//==============================================================================
// This file defines the classes that implement the 2D view (azimuth-elevation
// or azimuth-distance) and the elevation view.
//
// Classes:
//   FieldComponent : The base class. Implements the common attributes and
//                    methods of the two views.
//   PositionFieldComponent : The 2D view as a cartesian plane. It is used to show
//                        and control two parameters, azimuth-elevation for the
//                        VBAP algorithm and azimuth-distance for the LBAP
//                        algorithm.
//   ElevationFieldComponent : The 1D view used to show and control the elevation
//                             parameter for the LBAP algorithm.
//==============================================================================

//==============================================================================
class FieldComponent : public Component
{
public:
    //==============================================================================
    class Listener
    {
    public:
        Listener() noexcept = default;
        virtual ~Listener() noexcept = default;

        virtual void fieldSourcePositionChanged(int sourceId, int whichField) = 0;
        virtual void fieldTrajectoryHandleClicked(int whichField) = 0;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Listener);
    };
    //==============================================================================
    ListenerList<Listener> mListeners{};

protected:
    //==============================================================================
    static constexpr int TRAJECTORY_MARGINS = 10;
    //==============================================================================
    Sources * mSources;
    std::unique_ptr<TrajectoryHandleComponent> mTrajectoryHandleComponent{};

    bool mIsPlaying{ false };
    std::optional<int> mSelectedSourceId{};
    std::optional<int> mOldSelectedSourceId{};

public:
    //==============================================================================
    FieldComponent() noexcept;
    ~FieldComponent() noexcept override = default;
    //==============================================================================
    void setSources(Sources & sources);

    void setSelectedSource(int selectedId);

    std::optional<int> const & getSelectedSourceId() const { return mSelectedSourceId; }

    void setIsPlaying(bool const state) { mIsPlaying = state; }

    void addListener(Listener * l) { mListeners.add(l); }
    void removeListener(Listener * l) { mListeners.remove(l); }

protected:
    //==============================================================================
    void drawBackgroundGrid(Graphics & g) const;
    virtual void drawBackground(Graphics & g) const = 0;
    //==============================================================================
    virtual void notifySourcePositionChanged(int sourceId) = 0;
    virtual void rebuildSourceComponents(int numberOfSources) = 0;
    virtual void drawSpans(Graphics & g) const = 0;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FieldComponent)
};

//==============================================================================
class PositionFieldComponent final : public FieldComponent
{
    PositionAutomationManager & mAutomationManager;

    SpatMode mSpatMode{ SpatMode::dome };

    bool mShowCircularSourceSelectionWarning{ false };

    std::optional<Point<float>> mLineDrawingAnchor1{ std::nullopt };
    std::optional<Point<float>> mLineDrawingAnchor2{ std::nullopt };

    OwnedArray<PositionSourceComponent> mSourceComponents{};

public:
    PositionFieldComponent(PositionAutomationManager & positionAutomationManager) noexcept;
    ~PositionFieldComponent() noexcept final = default;
    //==============================================================================
    PositionAutomationManager const & getAutomationManager() const { return mAutomationManager; }
    PositionAutomationManager & getAutomationManager() { return mAutomationManager; }

    void rebuildSourceComponents(int numberOfSources) final;

    void drawSpans(Graphics & g) const final;
    void drawDomeSpans(Graphics & g) const;
    void drawCubeSpans(Graphics & g) const;

    void paint(Graphics & g) final;

    bool isTrajectoryHandleClicked(MouseEvent const & event); // TODO: this should be const

    void setCircularSourceSelectionWarning(bool showCircularSourceSelectionWarning);

    void setSpatMode(SpatMode spatMode);

    void notifySourcePositionChanged(int sourceId) final;

    Point<float> sourcePositionToComponentPosition(Point<float> const & sourcePosition) const;
    Point<float> componentPositionToSourcePosition(Point<float> const & componentPosition) const;

private:
    //==============================================================================
    Point<int> clipRecordingPosition(Point<int> const & pos);

    bool hasValidLineDrawingAnchor1() const { return mLineDrawingAnchor1.has_value(); }
    bool hasValidLineDrawingAnchor2() const { return mLineDrawingAnchor2.has_value(); }
    //==============================================================================
    void mouseDown(MouseEvent const & event) final;
    void mouseDrag(MouseEvent const & event) final;
    void mouseUp(MouseEvent const & event) final;

    void drawBackground(Graphics & g) const final;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PositionFieldComponent)
};

//==============================================================================
class ElevationFieldComponent final : public FieldComponent
{
    static constexpr auto xPadding{ 50.0f };
    static constexpr auto yTopPadding{ 5.0f };
    static constexpr auto yBottomPadding{ 35.0f };

    bool mCurrentlyDrawing{ false };
    ElevationAutomationManager & mAutomationManager;
    int mCurrentRecordingPositionX{};

    OwnedArray<ElevationSourceComponent> mSourceComponents{};

public:
    //==============================================================================
    ElevationFieldComponent(ElevationAutomationManager & mPositionAutomationManager) noexcept
        : mAutomationManager(mPositionAutomationManager)
    {
    }
    ~ElevationFieldComponent() noexcept final = default;
    //==============================================================================
    ElevationAutomationManager const & getAutomationManager() const { return mAutomationManager; }
    ElevationAutomationManager & getAutomationManager() { return mAutomationManager; }

    void paint(Graphics & g) final;

    void mouseDown(MouseEvent const & event) final;
    void mouseDrag(MouseEvent const & event) final;
    void mouseUp(MouseEvent const & event) final;

    void notifySourcePositionChanged(int sourceId) final;
    void rebuildSourceComponents(int numberOfSources) final;

    Point<float> sourceElevationToComponentPosition(Radians sourceElevation, int sourceId) const;
    Radians componentPositionToSourceElevation(Point<float> const & componentPosition) const;

private:
    void drawSpans(Graphics & g) const final;
    void drawBackground(Graphics & g) const final;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ElevationFieldComponent)
};
