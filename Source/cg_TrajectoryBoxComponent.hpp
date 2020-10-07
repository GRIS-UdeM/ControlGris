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

#include <JuceHeader.h>

#include "cg_ControlGrisLookAndFeel.hpp"
#include "cg_constants.hpp"

//==============================================================================
class TrajectoryBoxComponent final : public juce::Component
{
public:
    //==============================================================================
    class Listener
    {
    public:
        Listener() = default;
        virtual ~Listener() = default;

        Listener(Listener const &) = default;
        Listener(Listener &&) = default;

        Listener & operator=(Listener const &) = default;
        Listener & operator=(Listener &&) = default;
        //==============================================================================
        virtual void trajectoryBoxPositionSourceLinkChanged(PositionSourceLink sourceLink) = 0;
        virtual void trajectoryBoxElevationSourceLinkChanged(ElevationSourceLink sourceLink) = 0;
        virtual void trajectoryBoxPositionTrajectoryTypeChanged(PositionTrajectoryType trajectoryType) = 0;
        virtual void trajectoryBoxElevationTrajectoryTypeChanged(ElevationTrajectoryType trajectoryType) = 0;
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
    GrisLookAndFeel & mGrisLookAndFeel;
    juce::ListenerList<Listener> mListeners;
    SpatMode mSpatMode;

    juce::Label mSourceLinkLabel;
    juce::Label mTrajectoryTypeLabel;

    juce::ComboBox mPositionTrajectoryTypeCombo;
    juce::ComboBox mElevationTrajectoryTypeCombo;

    juce::ToggleButton mPositionBackAndForthToggle;
    juce::ToggleButton mElevationBackAndForthToggle;

    juce::Label mDampeningLabel;
    juce::TextEditor mPositionDampeningEditor;
    juce::TextEditor mElevationDampeningEditor;

    juce::Label mDeviationLabel;
    juce::TextEditor mDeviationEditor;

    juce::Label mDurationLabel;
    juce::TextEditor mDurationEditor;
    juce::ComboBox mDurationUnitCombo;

    juce::Label mCycleSpeedLabel;
    juce::Slider mCycleSpeedSlider;

    juce::TextButton mPositionActivateButton;
    juce::TextButton mElevationActivateButton;

    juce::ComboBox mPositionSourceLinkCombo;
    juce::ComboBox mElevationSourceLinkCombo;

public:
    //==============================================================================
    explicit TrajectoryBoxComponent(GrisLookAndFeel & grisLookAndFeel);
    ~TrajectoryBoxComponent() final = default;
    //==============================================================================
    void paint(juce::Graphics &) final;
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

    juce::ComboBox const & getPositionSourceLinkCombo() const { return mPositionSourceLinkCombo; }
    juce::ComboBox & getPositionSourceLinkCombo() { return mPositionSourceLinkCombo; }
    juce::ComboBox const & getElevationSourceLinkCombo() const { return mElevationSourceLinkCombo; }
    juce::ComboBox & getElevationSourceLinkCombo() { return mElevationSourceLinkCombo; }

    enum class SymmetricLinkComboState { enabled, disabled };
    void setSymmetricLinkComboState(bool allowed);

    void addListener(Listener * l) { mListeners.add(l); }
    void removeListener(Listener * l) { mListeners.remove(l); }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrajectoryBoxComponent)
};
