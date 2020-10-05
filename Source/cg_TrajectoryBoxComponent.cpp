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

#include "cg_TrajectoryBoxComponent.hpp"
#include "cg_constants.hpp"

//==============================================================================
TrajectoryBoxComponent::TrajectoryBoxComponent(GrisLookAndFeel & grisLookAndFeel) : mGrisLookAndFeel(grisLookAndFeel)
{
    mSpatMode = SpatMode::dome;

    mSourceLinkLabel.setText("Source Link:", NotificationType::dontSendNotification);
    addAndMakeVisible(&mSourceLinkLabel);

    mPositionSourceLinkCombo.addItemList(POSITION_SOURCE_LINK_TYPES, 1);
    mPositionSourceLinkCombo.setSelectedId(1);
    addAndMakeVisible(&mPositionSourceLinkCombo);
    mPositionSourceLinkCombo.onChange = [this] {
        mListeners.call([&](Listener & l) {
            l.trajectoryBoxPositionSourceLinkChanged(
                static_cast<PositionSourceLink>(mPositionSourceLinkCombo.getSelectedId()));
        });
    };

    mElevationSourceLinkCombo.addItemList(ELEVATION_SOURCE_LINK_TYPES, 1);
    mElevationSourceLinkCombo.setSelectedId(1);
    addChildComponent(&mElevationSourceLinkCombo);
    mElevationSourceLinkCombo.onChange = [this] {
        mListeners.call([&](Listener & l) {
            l.trajectoryBoxElevationSourceLinkChanged(
                static_cast<ElevationSourceLink>(mElevationSourceLinkCombo.getSelectedId()));
        });
    };

    mTrajectoryTypeLabel.setText("Trajectory Type:", NotificationType::dontSendNotification);
    addAndMakeVisible(&mTrajectoryTypeLabel);

    mPositionTrajectoryTypeCombo.addItemList(POSITION_TRAJECTORY_TYPE_TYPES, 1);
    mPositionTrajectoryTypeCombo.setSelectedId(1);
    addAndMakeVisible(&mPositionTrajectoryTypeCombo);
    mPositionTrajectoryTypeCombo.onChange = [this] {
        mListeners.call([&](Listener & l) {
            l.trajectoryBoxPositionTrajectoryTypeChanged(
                static_cast<PositionTrajectoryType>(mPositionTrajectoryTypeCombo.getSelectedId()));
        });
    };

    mElevationTrajectoryTypeCombo.addItemList(ELEVATION_TRAJECTORY_TYPE_TYPES, 1);
    mElevationTrajectoryTypeCombo.setSelectedId(1);
    addChildComponent(&mElevationTrajectoryTypeCombo);
    mElevationTrajectoryTypeCombo.onChange = [this] {
        mListeners.call([&](Listener & l) {
            l.trajectoryBoxElevationTrajectoryTypeChanged(
                static_cast<ElevationTrajectoryType>(mElevationTrajectoryTypeCombo.getSelectedId()));
        });
    };

    mDurationLabel.setText("Dur per cycle:", NotificationType::dontSendNotification);
    addAndMakeVisible(&mDurationLabel);

    addAndMakeVisible(&mDurationEditor);
    mDurationEditor.setTextToShowWhenEmpty("1", Colours::white);
    mDurationEditor.setText("5", false);
    mDurationEditor.setInputRestrictions(10, "0123456789.");
    mDurationEditor.onReturnKey = [this] { mDurationUnitCombo.grabKeyboardFocus(); };
    mDurationEditor.onFocusLost = [this] {
        mListeners.call([&](Listener & l) {
            l.trajectoryBoxCycleDurationChanged(mDurationEditor.getText().getDoubleValue(),
                                                mDurationUnitCombo.getSelectedId());
        });
        mDurationUnitCombo.grabKeyboardFocus();
    };

    addAndMakeVisible(&mDurationUnitCombo);
    mDurationUnitCombo.addItem("Sec(s)", 1);
    mDurationUnitCombo.addItem("Beat(s)", 2);
    mDurationUnitCombo.setSelectedId(1);
    mDurationUnitCombo.onChange = [this] {
        mListeners.call([&](Listener & l) {
            l.trajectoryBoxDurationUnitChanged(mDurationEditor.getText().getDoubleValue(),
                                               mDurationUnitCombo.getSelectedId());
        });
    };

    mCycleSpeedLabel.setText("Cycle Speed:", NotificationType::dontSendNotification);
    addAndMakeVisible(&mCycleSpeedLabel);

    mCycleSpeedSlider.setNormalisableRange(NormalisableRange<double>(-2.0f, 2.0f, 0.01f));
    mCycleSpeedSlider.setValue(1.0, NotificationType::sendNotificationAsync);
    mCycleSpeedSlider.setTextBoxStyle(Slider::TextBoxRight, false, 40, 20);
    mCycleSpeedSlider.setColour(Slider::textBoxOutlineColourId, Colours::transparentBlack);
    addAndMakeVisible(&mCycleSpeedSlider);

    mPositionActivateButton.addShortcut(KeyPress('a', 0, 0));
    addAndMakeVisible(&mPositionActivateButton);
    mPositionActivateButton.setButtonText("Activate");
    mPositionActivateButton.setClickingTogglesState(true);
    mPositionActivateButton.onClick = [this] {
        mListeners.call(
            [&](Listener & l) { l.trajectoryBoxPositionActivateChanged(mPositionActivateButton.getToggleState()); });
        mDurationUnitCombo.grabKeyboardFocus();
    };

    mPositionBackAndForthToggle.setButtonText("Back & Forth");
    mPositionBackAndForthToggle.onClick = [this] {
        mListeners.call([&](Listener & l) {
            l.trajectoryBoxPositionBackAndForthChanged(mPositionBackAndForthToggle.getToggleState());
        });
        setPositionDampeningEnabled(mPositionBackAndForthToggle.getToggleState());
    };
    addAndMakeVisible(&mPositionBackAndForthToggle);

    mDampeningLabel.setText("Number of cycles \ndampening:", NotificationType::dontSendNotification);
    addAndMakeVisible(&mDampeningLabel);

    addAndMakeVisible(&mPositionDampeningEditor);
    mPositionDampeningEditor.setTextToShowWhenEmpty("0", Colours::white);
    mPositionDampeningEditor.setText("0", false);
    mPositionDampeningEditor.setInputRestrictions(10, "0123456789");
    mPositionDampeningEditor.onReturnKey = [this] { mDurationUnitCombo.grabKeyboardFocus(); };
    mPositionDampeningEditor.onFocusLost = [this] {
        mListeners.call([&](Listener & l) {
            l.trajectoryBoxPositionDampeningCyclesChanged(mPositionDampeningEditor.getText().getIntValue());
        });
        mDurationUnitCombo.grabKeyboardFocus();
    };

    mDeviationLabel.setText("Deviation degrees\nper cycle:", NotificationType::dontSendNotification);
    addAndMakeVisible(&mDeviationLabel);

    addAndMakeVisible(&mDeviationEditor);
    mDeviationEditor.setTextToShowWhenEmpty("0", Colours::white);
    mDeviationEditor.setText("0", false);
    mDeviationEditor.setInputRestrictions(10, "-0123456789.");
    mDeviationEditor.onReturnKey = [this] { mDurationUnitCombo.grabKeyboardFocus(); };
    mDeviationEditor.onFocusLost = [this] {
        mListeners.call([&](Listener & l) {
            l.trajectoryBoxDeviationPerCycleChanged(std::fmod(mDeviationEditor.getText().getFloatValue(), 360.0f));
        });
        mDeviationEditor.setText(String(std::fmod(mDeviationEditor.getText().getFloatValue(), 360.0)));
        mDurationUnitCombo.grabKeyboardFocus();
    };

    mElevationActivateButton.addShortcut(KeyPress('a', ModifierKeys::shiftModifier, 0));
    addChildComponent(&mElevationActivateButton);
    mElevationActivateButton.setButtonText("Activate");
    mElevationActivateButton.setClickingTogglesState(true);
    mElevationActivateButton.onClick = [this] {
        mListeners.call(
            [&](Listener & l) { l.trajectoryBoxElevationActivateChanged(mElevationActivateButton.getToggleState()); });
        mDurationUnitCombo.grabKeyboardFocus();
    };

    mElevationBackAndForthToggle.setButtonText("Back & Forth");
    mElevationBackAndForthToggle.onClick = [this] {
        mListeners.call([&](Listener & l) {
            l.trajectoryBoxElevationBackAndForthChanged(mElevationBackAndForthToggle.getToggleState());
        });
        setElevationDampeningEnabled(mElevationBackAndForthToggle.getToggleState());
    };
    addAndMakeVisible(&mElevationBackAndForthToggle);

    addAndMakeVisible(&mElevationDampeningEditor);
    mElevationDampeningEditor.setTextToShowWhenEmpty("0", Colours::white);
    mElevationDampeningEditor.setText("0", false);
    mElevationDampeningEditor.setInputRestrictions(10, "0123456789");
    mElevationDampeningEditor.onReturnKey = [this] { mDurationUnitCombo.grabKeyboardFocus(); };
    mElevationDampeningEditor.onFocusLost = [this] {
        mListeners.call([&](Listener & l) {
            l.trajectoryBoxElevationDampeningCyclesChanged(mElevationDampeningEditor.getText().getIntValue());
        });
        mDurationUnitCombo.grabKeyboardFocus();
    };
}

//==============================================================================
void TrajectoryBoxComponent::setSpatMode(SpatMode const spatMode)
{
    mSpatMode = spatMode;
    resized();
}

//==============================================================================
void TrajectoryBoxComponent::setNumberOfSources(int const numOfSources)
{
    if (numOfSources == 1) {
        mPositionSourceLinkCombo.setSelectedId(1);
        mPositionSourceLinkCombo.setEnabled(false);
        mElevationSourceLinkCombo.setSelectedId(1);
        mElevationSourceLinkCombo.setEnabled(false);
    } else {
        mPositionSourceLinkCombo.setEnabled(true);
        mElevationSourceLinkCombo.setEnabled(true);
    }

    if (numOfSources == 2) {
        mPositionSourceLinkCombo.setItemEnabled(static_cast<int>(PositionSourceLink::symmetricX), true);
        mPositionSourceLinkCombo.setItemEnabled(static_cast<int>(PositionSourceLink::symmetricY), true);
    } else {
        mPositionSourceLinkCombo.setItemEnabled(static_cast<int>(PositionSourceLink::symmetricX), false);
        mPositionSourceLinkCombo.setItemEnabled(static_cast<int>(PositionSourceLink::symmetricY), false);
    }
}

//==============================================================================
void TrajectoryBoxComponent::setTrajectoryType(int const type)
{
    mPositionTrajectoryTypeCombo.setSelectedId(type);
}

//==============================================================================
void TrajectoryBoxComponent::setElevationTrajectoryType(int const type)
{
    mElevationTrajectoryTypeCombo.setSelectedId(type);
}

//==============================================================================
void TrajectoryBoxComponent::setPositionBackAndForth(bool const state)
{
    mPositionBackAndForthToggle.setToggleState(state, NotificationType::sendNotification);
    setPositionDampeningEnabled(state);
}

//==============================================================================
void TrajectoryBoxComponent::setElevationBackAndForth(bool const state)
{
    mElevationBackAndForthToggle.setToggleState(state, NotificationType::sendNotification);
    setElevationDampeningEnabled(state);
}

//==============================================================================
void TrajectoryBoxComponent::setPositionDampeningEnabled(bool const state)
{
    mPositionDampeningEditor.setEnabled(state);
    String text = mPositionDampeningEditor.getText();
    mPositionDampeningEditor.clear();
    if (state)
        mPositionDampeningEditor.setColour(TextEditor::textColourId, Colour::fromRGB(235, 245, 250));
    else
        mPositionDampeningEditor.setColour(TextEditor::textColourId, Colour::fromRGB(172, 172, 172));
    mPositionDampeningEditor.setText(text);
}

//==============================================================================
void TrajectoryBoxComponent::setElevationDampeningEnabled(bool const state)
{
    mElevationDampeningEditor.setEnabled(state);
    String text = mElevationDampeningEditor.getText();
    mElevationDampeningEditor.clear();
    if (state)
        mElevationDampeningEditor.setColour(TextEditor::textColourId, Colour::fromRGB(235, 245, 250));
    else
        mElevationDampeningEditor.setColour(TextEditor::textColourId, Colour::fromRGB(172, 172, 172));
    mElevationDampeningEditor.setText(text);
}

//==============================================================================
void TrajectoryBoxComponent::setPositionDampeningCycles(int const value)
{
    mPositionDampeningEditor.setText(String(value));
}

//==============================================================================
void TrajectoryBoxComponent::setElevationDampeningCycles(int const value)
{
    mElevationDampeningEditor.setText(String(value));
}

//==============================================================================
void TrajectoryBoxComponent::setDeviationPerCycle(float const value)
{
    mDeviationEditor.setText(String(value));
}

//==============================================================================
void TrajectoryBoxComponent::setPositionSourceLink(PositionSourceLink const value)
{
    mPositionSourceLinkCombo.setSelectedId(static_cast<int>(value));
}

//==============================================================================
void TrajectoryBoxComponent::setElevationSourceLink(ElevationSourceLink const value)
{
    mElevationSourceLinkCombo.setSelectedId(static_cast<int>(value));
}

//==============================================================================
void TrajectoryBoxComponent::setPositionActivateState(bool const state)
{
    mPositionActivateButton.setToggleState(state, NotificationType::dontSendNotification);
}

//==============================================================================
void TrajectoryBoxComponent::setElevationActivateState(bool const state)
{
    mElevationActivateButton.setToggleState(state, NotificationType::dontSendNotification);
}

//==============================================================================
void TrajectoryBoxComponent::setSymmetricLinkComboState(bool const allowed)
{
    mPositionSourceLinkCombo.setItemEnabled(static_cast<int>(PositionSourceLink::symmetricX), allowed);
    mPositionSourceLinkCombo.setItemEnabled(static_cast<int>(PositionSourceLink::symmetricY), allowed);
}

//==============================================================================
void TrajectoryBoxComponent::setCycleDuration(double const value)
{
    mDurationEditor.setText(String(value));
    mListeners.call([&](Listener & l) {
        l.trajectoryBoxCycleDurationChanged(mDurationEditor.getText().getDoubleValue(),
                                            mDurationUnitCombo.getSelectedId());
    });
}

//==============================================================================
void TrajectoryBoxComponent::setDurationUnit(int const value)
{
    mDurationUnitCombo.setSelectedId(value, NotificationType::sendNotificationSync);
    mListeners.call([&](Listener & l) {
        l.trajectoryBoxDurationUnitChanged(mDurationEditor.getText().getDoubleValue(),
                                           mDurationUnitCombo.getSelectedId());
    });
}

//==============================================================================
void TrajectoryBoxComponent::paint(Graphics & g)
{
    g.fillAll(mGrisLookAndFeel.findColour(ResizableWindow::backgroundColourId));
}

//==============================================================================
void TrajectoryBoxComponent::resized()
{
    mSourceLinkLabel.setBounds(5, 10, 150, 20);
    mPositionSourceLinkCombo.setBounds(115, 10, 175, 20);
    mTrajectoryTypeLabel.setBounds(5, 40, 150, 20);
    mPositionTrajectoryTypeCombo.setBounds(115, 40, 175, 20);
    mPositionBackAndForthToggle.setBounds(196, 70, 94, 20);
    mDampeningLabel.setBounds(5, 70, 150, 20);
    mPositionDampeningEditor.setBounds(115, 70, 75, 20);
    mDeviationLabel.setBounds(5, 100, 150, 20);
    mDeviationEditor.setBounds(115, 100, 75, 20);
    mPositionActivateButton.setBounds(114, 130, 176, 20);

    if (mSpatMode == SpatMode::cube) {
        mElevationSourceLinkCombo.setVisible(true);
        mElevationTrajectoryTypeCombo.setVisible(true);
        mElevationActivateButton.setVisible(true);
        mElevationBackAndForthToggle.setVisible(true);
        mElevationDampeningEditor.setVisible(true);
        mElevationSourceLinkCombo.setBounds(305, 10, 175, 20);
        mElevationTrajectoryTypeCombo.setBounds(305, 40, 175, 20);
        mElevationBackAndForthToggle.setBounds(386, 70, 94, 20);
        mElevationDampeningEditor.setBounds(305, 70, 75, 20);
        mElevationActivateButton.setBounds(304, 130, 176, 20);
    } else {
        mElevationSourceLinkCombo.setVisible(false);
        mElevationTrajectoryTypeCombo.setVisible(false);
        mElevationActivateButton.setVisible(false);
        mElevationBackAndForthToggle.setVisible(false);
        mElevationDampeningEditor.setVisible(false);
    }

    mDurationLabel.setBounds(490, 15, 90, 20);
    mDurationEditor.setBounds(495, 40, 90, 20);
    mDurationUnitCombo.setBounds(495, 70, 90, 20);

    // Hide Cycle Speed slider until we found the good way to handle it!
    mCycleSpeedLabel.setBounds(5, 100, 150, 20);
    mCycleSpeedSlider.setBounds(115, 100, 165, 20);
    mCycleSpeedLabel.setVisible(false);
    mCycleSpeedSlider.setVisible(false);
}
