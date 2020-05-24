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

#include "AutomationManager.h"
#include "GrisLookAndFeel.h"
#include "SettingsBoxComponent.h"
#include "Source.h"

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
class FieldComponent : public Component
{
public:
    FieldComponent();
    ~FieldComponent() override;

    void drawFieldBackground(Graphics &, bool isMainField, SpatMode spatMode = SpatMode::VBAP) const;

    Point<float> posToXy(Point<float> p, int p_iFieldWidth) const;
    Point<float> xyToPos(Point<float> p, int p_iFieldWidth) const;

    void setSources(Source * sources, int numberOfSources);
    void setSelectedSource(int selectedId);

    void setIsPlaying(bool state);

    struct Listener {
        virtual ~Listener() = default;

        virtual void fieldSourcePositionChanged(int sourceId, int whichField) = 0;
        virtual void fieldTrajectoryHandleClicked(int whichField) = 0;
    };

    void addListener(Listener * l) { listeners.add(l); }
    void removeListener(Listener * l) { listeners.remove(l); }

    ListenerList<Listener> listeners;

protected:
    Source * m_sources;

    bool m_isPlaying;
    int m_numberOfSources;
    int m_selectedSourceId;
    int m_oldSelectedSourceId;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FieldComponent)
};

//==============================================================================
class MainFieldComponent final : public FieldComponent
{
public:
    MainFieldComponent(AutomationManager & automan);
    ~MainFieldComponent() final;

    void createSpanPathVBAP(Graphics & g, int i) const;
    void createSpanPathLBAP(Graphics & g, int i) const;
    void drawTrajectoryHandle(Graphics &) const;
    void paint(Graphics &) final;

    bool isTrajectoryHandleClicked(const MouseEvent & event); // TODO: this should be const
    void mouseDown(const MouseEvent & event) final;
    void mouseDrag(const MouseEvent & event) final;
    void mouseMove(const MouseEvent & event) final;
    void mouseUp(const MouseEvent & event) final;

    void setSpatMode(SpatMode spatMode);

private:
    AutomationManager & automationManager;
    [[gnu::const]] static double degreeToRadian(float degree) { return (degree / 360.0 * 2.0 * M_PI); }
    Point<float> degreeToXy(Point<float> p, int p_iFieldWidth) const;
    Point<float> xyToDegree(Point<float> p, int p_iFieldWidth) const;

    Point<int> clipRecordingPosition(Point<int> pos);

    bool hasValidLineDrawingAnchor1() const { return lineDrawingAnchor1 != Point<float>(-1.0f, -1.0f); }
    bool hasValidLineDrawingAnchor2() const { return lineDrawingAnchor2 != Point<float>(-1.0f, -1.0f); }

    SpatMode m_spatMode;

    bool showCircularSourceSelectionWarning = false;

    Point<float> lineDrawingAnchor1;
    Point<float> lineDrawingAnchor2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainFieldComponent)
};

//==============================================================================
class ElevationFieldComponent final : public FieldComponent
{
public:
    ElevationFieldComponent(AutomationManager & automan);
    ~ElevationFieldComponent() final;

    void paint(Graphics &) final;

    void mouseDown(const MouseEvent & event) final;
    void mouseDrag(const MouseEvent & event) final;
    void mouseUp(const MouseEvent & event) final;

private:
    AutomationManager & automationManager;
    int currentRecordingPositionX;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ElevationFieldComponent)
};
