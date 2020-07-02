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

#include "ControlGrisConstants.h"
#include "GrisLookAndFeel.h"

//==============================================================================
class TrajectoryBoxComponent final : public Component
{
public:
    //==============================================================================
    struct Listener {
        virtual ~Listener() {}

        virtual void trajectoryBoxPositionSourceLinkChanged(PositionSourceLink value) = 0;
        virtual void trajectoryBoxElevationSourceLinkChanged(ElevationSourceLink value) = 0;
        virtual void trajectoryBoxPositionTrajectoryTypeChanged(PositionTrajectoryType value) = 0;
        virtual void trajectoryBoxElevationTrajectoryTypeChanged(ElevationTrajectoryType value) = 0;
        virtual void trajectoryBoxPositionBackAndForthChanged(bool value) = 0;
        virtual void trajectoryBoxElevationBackAndForthChanged(bool value) = 0;
        virtual void trajectoryBoxPositionDampeningCyclesChanged(int value) = 0;
        virtual void trajectoryBoxElevationDampeningCyclesChanged(int value) = 0;
        virtual void trajectoryBoxDeviationPerCycleChanged(float value) = 0;
        virtual void trajectoryBoxCycleDurationChanged(double duration, int mode) = 0;
        virtual void trajectoryBoxDurationUnitChanged(double duration, int mode) = 0;
        virtual void trajectoryBoxPositionActivateChanged(bool value) = 0;
        virtual void trajectoryBoxElevationActivateChanged(bool value) = 0;
    };

private:
    //==============================================================================
    ListenerList<Listener> mListeners;

    SpatMode mSpatMode;

    Label mSourceLinkLabel;

    Label mTrajectoryTypeLabel;
    ComboBox mPositionTrajectoryTypeCombo;
    ComboBox mElevationTracjectoryTypeCombo;

    ToggleButton mPositionBackAndForthToggle;
    ToggleButton mElevationBackAndForthToggle;

    Label mDampeningLabel;
    TextEditor mPositionDampeningEditor;
    TextEditor mElevationDampeningEditor;

    Label mDeviationLabel;
    TextEditor mDeviationEditor;

    Label mDurationLabel;
    TextEditor mDurationEditor;
    ComboBox mDurationUnitCombo;

    Label mCycleSpeedLabel;
    Slider mCycleSpeedSlider;

    TextButton mPositionActivateButton;
    TextButton mElevationActivateButton;

public:
    //==============================================================================
    ComboBox mPositionSourceLinkCombo;
    ComboBox mElevationSourceLinkCombo;
    //==============================================================================
    TrajectoryBoxComponent();
    ~TrajectoryBoxComponent() final;
    //==============================================================================
    void paint(Graphics &) final;
    void resized() final;

    void setNumberOfSources(int numOfSources);
    void setSpatMode(SpatMode spatMode);
    void setTrajectoryType(int type);
    void setElevationTrajectoryType(int type);
    void setPositionBackAndForth(bool state);
    void setElevationBackAndForth(bool state);
    void setPositionDampeningEnabled(bool state);
    void setElevationDampeningEnabled(bool state);
    void setPositionDampeningCycles(int value);
    void setElevationDampeningCycles(int value);
    void setPositionSourceLink(PositionSourceLink value);
    void setElevationSourceLink(ElevationSourceLink value);
    void setCycleDuration(double value);
    void setDurationUnit(int value);
    void setDeviationPerCycle(float value);

    bool getPositionActivateState() const { return mPositionActivateButton.getToggleState(); }
    bool getElevationActivateState() const { return mElevationActivateButton.getToggleState(); }
    void setPositionActivateState(bool state);
    void setElevationActivateState(bool state);

    void addListener(Listener * l) { mListeners.add(l); }
    void removeListener(Listener * l) { mListeners.remove(l); }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrajectoryBoxComponent)
};
