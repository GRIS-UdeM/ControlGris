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

class TrajectoryBoxComponent : public Component
{
public:
    TrajectoryBoxComponent();
    ~TrajectoryBoxComponent();

    void paint(Graphics&) override;
    void resized() override;

    void setSpatMode(SPAT_MODE_ENUM spatMode);
    void setCycleDuration(double value);
    void setDurationUnit(int value);
    void setNumOfCycles(int value);

    struct Listener
    {
        virtual ~Listener() {}

        virtual void trajectoryBoxSourceLinkChanged(int value) = 0;
        virtual void trajectoryBoxSourceLinkAltChanged(int value) = 0;
        virtual void trajectoryBoxTrajectoryTypeChanged(int value) = 0;
        virtual void trajectoryBoxTrajectoryTypeAltChanged(int value) = 0;
        virtual void trajectoryBoxDurationChanged(double duration, int mode) = 0;
        virtual void trajectoryBoxNumOfCycleChanged(int value) = 0;
        virtual void trajectoryBoxActivateChanged(bool value) = 0;
        virtual void trajectoryBoxActivateAltChanged(bool value) = 0;
        virtual void trajectoryBoxEditFixedSourceButtonClicked() = 0;
        virtual void trajectoryBoxFixSourceButtonClicked() = 0;
        virtual void trajectoryBoxClearButtonClicked() = 0;
        virtual void trajectoryBoxClearAltButtonClicked() = 0;
    };

    void addListener(Listener* l) { listeners.add (l); }
    void removeListener(Listener* l) { listeners.remove (l); }

    ComboBox        sourceLinkCombo;
    ComboBox        sourceLinkAltCombo;

private:
    ListenerList<Listener> listeners;

    SPAT_MODE_ENUM  m_spatMode;

    Label           sourceLinkLabel;

    Label           trajectoryTypeLabel;
    ComboBox        trajectoryTypeCombo;
    ComboBox        trajectoryTypeAltCombo;

    Label           durationLabel;
    TextEditor      durationEditor;
    ComboBox        durationUnitCombo;

    Label           numOfCycleLabel;
    TextEditor      numOfCycleEditor;

    Label           cycleSpeedLabel;
    Slider          cycleSpeedSlider;

    TextButton      editFixedSourceButton;
    TextButton      fixSourceButton;
    TextButton      clearButton;
    TextButton      clearAltButton;
    TextButton      activateButton;
    TextButton      activateAltButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrajectoryBoxComponent)
};
