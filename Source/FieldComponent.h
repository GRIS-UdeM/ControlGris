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
#include "GrisLookAndFeel.h"
#include "SettingsBoxComponent.h"
#include "Source.h"

//==============================================================================
// This file defines the classes that implement the 2D view (azimuth-elevation
// or azimuth-distance) and the elevation view.
//
// Classes:
//   FieldComponent : The base class. Implements the common attributes and
//                    methods of the two views.
//   MainFieldComponent : The 2D view as a cartesian plane. It is used to show
//                        and control two parameters, azimuth-elevation for the
//                        VBAP algorithm and azimuth-distance for the LBAP
//                        algorithm.
//   ElevationFieldComponent : The 1D view used to show and control the elevation
//                             parameter for the LBAP algorithm.
//==============================================================================

//==============================================================================
template<typename Float>
struct AngleVector {
    static_assert(std::is_floating_point_v<Float>);

    AngleVector(Float const angle, Float const distance) : angle(angle), distance(distance) {}

    Float angle;
    Float distance;
};

//==============================================================================
class FieldComponent : public Component
{
public:
    //==============================================================================
    struct Listener {
        virtual ~Listener() = default;

        virtual void fieldSourcePositionChanged(int sourceId, int whichField) = 0;
        virtual void fieldTrajectoryHandleClicked(int whichField) = 0;
    };
    //==============================================================================
    ListenerList<Listener> listeners{};

protected:
    //==============================================================================
    static constexpr Point<float> INVALID_POINT{ -1.0f, -1.0f };
    static constexpr int TRAJECTORY_HANDLE_SOURCE_ID = -1;
    static constexpr int NO_SELECTION_SOURCE_ID = -2;

    Source * mSources{};

    bool mIsPlaying{ false };
    int mNumberOfSources{};
    int mSelectedSourceId{};
    int mOldSelectedSourceId{};

public:
    //==============================================================================
    FieldComponent() = default;
    ~FieldComponent() override { setLookAndFeel(nullptr); }
    //==============================================================================
    void drawFieldBackground(Graphics & g, bool isMainField, SpatMode spatMode = SpatMode::VBAP) const;
    virtual void drawSources(Graphics & g) const = 0;

    Point<float> posToXy(Point<float> const & p, int p_iFieldWidth) const;
    Point<float> xyToPos(Point<float> const & p, int p_iFieldWidth) const;

    void setSources(Source * sources, int numberOfSources);
    void setSelectedSource(int selectedId);

    void setIsPlaying(bool const state) { mIsPlaying = state; }

    void addListener(Listener * l) { listeners.add(l); }
    void removeListener(Listener * l) { listeners.remove(l); }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FieldComponent)
};

//==============================================================================
class MainFieldComponent final : public FieldComponent
{
    PositionAutomationManager & mAutomationManager;

    SpatMode mSpatMode;

    bool mShowCircularSourceSelectionWarning{ false };

    Point<float> mLineDrawingAnchor1;
    Point<float> mLineDrawingAnchor2;

public:
    MainFieldComponent(PositionAutomationManager & positionAutomationManager);
    ~MainFieldComponent() final = default;
    //==============================================================================
    void drawSources(Graphics & g) const final;
    void drawSpans(Graphics & g) const;
    void drawDomeSpans(Graphics & g) const;
    void drawCubeSpans(Graphics & g) const;
    void drawTrajectoryHandle(Graphics &) const;
    void paint(Graphics & g) final;

    bool isTrajectoryHandleClicked(MouseEvent const & event); // TODO: this should be const
    void mouseDown(MouseEvent const & event) final;
    void mouseDrag(MouseEvent const & event) final;
    void mouseMove(MouseEvent const & event) final;
    void mouseUp(MouseEvent const & event) final;

    void setSpatMode(SpatMode spatMode);

private:
    //==============================================================================
    Point<float> degreeToXy(AngleVector<float> const & p, int p_iFieldWidth) const;
    AngleVector<float> xyToDegree(Point<float> const & p, int p_iFieldWidth) const;

    Point<int> clipRecordingPosition(Point<int> const & pos);

    bool hasValidLineDrawingAnchor1() const { return mLineDrawingAnchor1 != INVALID_POINT; }
    bool hasValidLineDrawingAnchor2() const { return mLineDrawingAnchor2 != INVALID_POINT; }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainFieldComponent)
};

//==============================================================================
class ElevationFieldComponent final : public FieldComponent
{
    ElevationAutomationManager & mAutomationManager;
    int mCurrentRecordingPositionX{};

public:
    //==============================================================================
    ElevationFieldComponent(ElevationAutomationManager & mPositionAutomationManager)
        : mAutomationManager(mPositionAutomationManager)
    {
    }
    ~ElevationFieldComponent() final = default;
    //==============================================================================
    void paint(Graphics & g) final;
    void drawSources(Graphics & g) const final;

    void mouseDown(MouseEvent const & event) final;
    void mouseDrag(MouseEvent const & event) final;
    void mouseUp(MouseEvent const & event) final;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ElevationFieldComponent)
};
