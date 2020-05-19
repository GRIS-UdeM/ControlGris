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

#include "../JuceLibraryCode/JuceHeader.h"
#include "GrisLookAndFeel.h"
#include "ControlGrisConstants.h"

class TrajectoryBoxComponent final : public Component
{
public:
    TrajectoryBoxComponent();
    ~TrajectoryBoxComponent() final;

    void paint(Graphics&) final;
    void resized() final;

    void setNumberOfSources(int numOfSources);
    void setSpatMode(SpatMode spatMode);
    void setTrajectoryType(int type);
    void setTrajectoryTypeAlt(int type);
    void setBackAndForth(bool state);
    void setBackAndForthAlt(bool state);
    void setDampeningEditorEnabled(bool state);
    void setDampeningAltEditorEnabled(bool state);
    void setDampeningCycles(int value);
    void setDampeningCyclesAlt(int value);
    void setSourceLink(SourceLink value);
    void setSourceLinkAlt(SourceLinkAlt value);
    void setCycleDuration(double value);
    void setDurationUnit(int value);
    void setDeviationPerCycle(float value);

    bool getActivateState() const { return activateButton.getToggleState(); }
    bool getActivateAltState() const { return activateAltButton.getToggleState(); }
    void setActivateState(bool state);
    void setActivateAltState(bool state);

    struct Listener
    {
        virtual ~Listener() {}

        virtual void trajectoryBoxSourceLinkChanged(SourceLink value) = 0;
        virtual void trajectoryBoxSourceLinkAltChanged(SourceLinkAlt value) = 0;
        virtual void trajectoryBoxTrajectoryTypeChanged(TrajectoryType value) = 0;
        virtual void trajectoryBoxTrajectoryTypeAltChanged(TrajectoryTypeAlt value) = 0;
        virtual void trajectoryBoxBackAndForthChanged(bool value) = 0;
        virtual void trajectoryBoxBackAndForthAltChanged(bool value) = 0;
        virtual void trajectoryBoxDampeningCyclesChanged(int value) = 0;
        virtual void trajectoryBoxDampeningCyclesAltChanged(int value) = 0;
        virtual void trajectoryBoxDeviationPerCycleChanged(float value) = 0;
        virtual void trajectoryBoxCycleDurationChanged(double duration, int mode) = 0;
        virtual void trajectoryBoxDurationUnitChanged(double duration, int mode) = 0;
        virtual void trajectoryBoxActivateChanged(bool value) = 0;
        virtual void trajectoryBoxActivateAltChanged(bool value) = 0;
    };

    void addListener(Listener* l) { listeners.add (l); }
    void removeListener(Listener* l) { listeners.remove (l); }

    ComboBox        sourceLinkCombo;
    ComboBox        sourceLinkAltCombo;

private:
    ListenerList<Listener> listeners;

    SpatMode  m_spatMode;

    Label           sourceLinkLabel;

    Label           trajectoryTypeLabel;
    ComboBox        trajectoryTypeCombo;
    ComboBox        trajectoryTypeAltCombo;

    ToggleButton    backAndForthToggle;
    ToggleButton    backAndForthAltToggle;

    Label           dampeningLabel;
    TextEditor      dampeningEditor;
    TextEditor      dampeningAltEditor;

    Label           deviationLabel;
    TextEditor      deviationEditor;

    Label           durationLabel;
    TextEditor      durationEditor;
    ComboBox        durationUnitCombo;

    Label           cycleSpeedLabel;
    Slider          cycleSpeedSlider;

    TextButton      activateButton;
    TextButton      activateAltButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrajectoryBoxComponent)
};
