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
#include "TrajectoryBoxComponent.h"
#include "ControlGrisConstants.h"

TrajectoryBoxComponent::TrajectoryBoxComponent()
{
    m_spatMode = SpatMode::VBAP;

    sourceLinkLabel.setText("Source Link:", NotificationType::dontSendNotification);
    addAndMakeVisible(&sourceLinkLabel);

    mPositionSourceLinkCombo.addItemList(POSITION_SOURCE_LINK_TYPES, 1);
    mPositionSourceLinkCombo.setSelectedId(1);
    addAndMakeVisible(&mPositionSourceLinkCombo);
    mPositionSourceLinkCombo.onChange = [this] {
        listeners.call([&](Listener & l) {
            l.trajectoryBoxPositionSourceLinkChanged(
                static_cast<PositionSourceLink>(mPositionSourceLinkCombo.getSelectedId()));
        });
    };

    mElevationSourceLinkCombo.addItemList(ELEVATION_SOURCE_LINK_TYPES, 1);
    mElevationSourceLinkCombo.setSelectedId(1);
    addChildComponent(&mElevationSourceLinkCombo);
    mElevationSourceLinkCombo.onChange = [this] {
        listeners.call([&](Listener & l) {
            l.trajectoryBoxElevationSourceLinkChanged(
                static_cast<ElevationSourceLink>(mElevationSourceLinkCombo.getSelectedId()));
        });
    };

    trajectoryTypeLabel.setText("Trajectory Type:", NotificationType::dontSendNotification);
    addAndMakeVisible(&trajectoryTypeLabel);

    mPositionTrajectoryTypeCombo.addItemList(POSITION_TRAJECTORY_TYPE_TYPES, 1);
    mPositionTrajectoryTypeCombo.setSelectedId(1);
    addAndMakeVisible(&mPositionTrajectoryTypeCombo);
    mPositionTrajectoryTypeCombo.onChange = [this] {
        listeners.call([&](Listener & l) {
            l.trajectoryBoxPositionTrajectoryTypeChanged(
                static_cast<PositionTrajectoryType>(mPositionTrajectoryTypeCombo.getSelectedId()));
        });
    };

    mElevationTracjectoryTypeCombo.addItemList(ELEVATION_TRAJECTORY_TYPE_TYPES, 1);
    mElevationTracjectoryTypeCombo.setSelectedId(1);
    addChildComponent(&mElevationTracjectoryTypeCombo);
    mElevationTracjectoryTypeCombo.onChange = [this] {
        listeners.call([&](Listener & l) {
            l.trajectoryBoxElevationTrajectoryTypeChanged(
                static_cast<ElevationTrajectoryType>(mElevationTracjectoryTypeCombo.getSelectedId()));
        });
    };

    durationLabel.setText("Dur per cycle:", NotificationType::dontSendNotification);
    addAndMakeVisible(&durationLabel);

    addAndMakeVisible(&durationEditor);
    durationEditor.setTextToShowWhenEmpty("1", Colours::white);
    durationEditor.setText("5", false);
    durationEditor.setInputRestrictions(10, "0123456789.");
    durationEditor.onReturnKey = [this] { durationUnitCombo.grabKeyboardFocus(); };
    durationEditor.onFocusLost = [this] {
        listeners.call([&](Listener & l) {
            l.trajectoryBoxCycleDurationChanged(durationEditor.getText().getDoubleValue(),
                                                durationUnitCombo.getSelectedId());
        });
        durationUnitCombo.grabKeyboardFocus();
    };

    addAndMakeVisible(&durationUnitCombo);
    durationUnitCombo.addItem("Sec(s)", 1);
    durationUnitCombo.addItem("Beat(s)", 2);
    durationUnitCombo.setSelectedId(1);
    durationUnitCombo.onChange = [this] {
        listeners.call([&](Listener & l) {
            l.trajectoryBoxDurationUnitChanged(durationEditor.getText().getDoubleValue(),
                                               durationUnitCombo.getSelectedId());
        });
    };

    cycleSpeedLabel.setText("Cycle Speed:", NotificationType::dontSendNotification);
    addAndMakeVisible(&cycleSpeedLabel);

    cycleSpeedSlider.setNormalisableRange(NormalisableRange<double>(-2.0f, 2.0f, 0.01f));
    cycleSpeedSlider.setValue(1.0, NotificationType::sendNotificationAsync);
    cycleSpeedSlider.setTextBoxStyle(Slider::TextBoxRight, false, 40, 20);
    cycleSpeedSlider.setColour(Slider::textBoxOutlineColourId, Colours::transparentBlack);
    addAndMakeVisible(&cycleSpeedSlider);

    mPositionActivateButton.addShortcut(KeyPress('a', 0, 0));
    addAndMakeVisible(&mPositionActivateButton);
    mPositionActivateButton.setButtonText("Activate");
    mPositionActivateButton.setClickingTogglesState(true);
    mPositionActivateButton.onClick = [this] {
        listeners.call(
            [&](Listener & l) { l.trajectoryBoxPositionActivateChanged(mPositionActivateButton.getToggleState()); });
        durationUnitCombo.grabKeyboardFocus();
    };

    mPositionBackAndForthToggle.setButtonText("Back & Forth");
    mPositionBackAndForthToggle.onClick = [this] {
        listeners.call([&](Listener & l) {
            l.trajectoryBoxPositionBackAndForthChanged(mPositionBackAndForthToggle.getToggleState());
        });
        setPositionDampeningEnabled(mPositionBackAndForthToggle.getToggleState());
    };
    addAndMakeVisible(&mPositionBackAndForthToggle);

    dampeningLabel.setText("Number of cycles \ndampening:", NotificationType::dontSendNotification);
    addAndMakeVisible(&dampeningLabel);

    addAndMakeVisible(&mPositionDampeningEditor);
    mPositionDampeningEditor.setTextToShowWhenEmpty("0", Colours::white);
    mPositionDampeningEditor.setText("0", false);
    mPositionDampeningEditor.setInputRestrictions(10, "0123456789");
    mPositionDampeningEditor.onReturnKey = [this] { durationUnitCombo.grabKeyboardFocus(); };
    mPositionDampeningEditor.onFocusLost = [this] {
        listeners.call([&](Listener & l) {
            l.trajectoryBoxPositionDampeningCyclesChanged(mPositionDampeningEditor.getText().getIntValue());
        });
        durationUnitCombo.grabKeyboardFocus();
    };

    deviationLabel.setText("Deviation degrees\nper cycle:", NotificationType::dontSendNotification);
    addAndMakeVisible(&deviationLabel);

    addAndMakeVisible(&deviationEditor);
    deviationEditor.setTextToShowWhenEmpty("0", Colours::white);
    deviationEditor.setText("0", false);
    deviationEditor.setInputRestrictions(10, "-0123456789.");
    deviationEditor.onReturnKey = [this] { durationUnitCombo.grabKeyboardFocus(); };
    deviationEditor.onFocusLost = [this] {
        listeners.call([&](Listener & l) {
            l.trajectoryBoxDeviationPerCycleChanged(std::fmod(deviationEditor.getText().getFloatValue(), 360.0));
        });
        deviationEditor.setText(String(std::fmod(deviationEditor.getText().getFloatValue(), 360.0)));
        durationUnitCombo.grabKeyboardFocus();
    };

    mElevationActivateButton.addShortcut(KeyPress('a', ModifierKeys::shiftModifier, 0));
    addChildComponent(&mElevationActivateButton);
    mElevationActivateButton.setButtonText("Activate");
    mElevationActivateButton.setClickingTogglesState(true);
    mElevationActivateButton.onClick = [this] {
        listeners.call(
            [&](Listener & l) { l.trajectoryBoxElevationActivateChanged(mElevationActivateButton.getToggleState()); });
        durationUnitCombo.grabKeyboardFocus();
    };

    mElevationBackAndForthToggle.setButtonText("Back & Forth");
    mElevationBackAndForthToggle.onClick = [this] {
        listeners.call([&](Listener & l) {
            l.trajectoryBoxElevationBackAndForthChanged(mElevationBackAndForthToggle.getToggleState());
        });
        setElevationDampeningEnabled(mElevationBackAndForthToggle.getToggleState());
    };
    addAndMakeVisible(&mElevationBackAndForthToggle);

    addAndMakeVisible(&mElevationDampeningEditor);
    mElevationDampeningEditor.setTextToShowWhenEmpty("0", Colours::white);
    mElevationDampeningEditor.setText("0", false);
    mElevationDampeningEditor.setInputRestrictions(10, "0123456789");
    mElevationDampeningEditor.onReturnKey = [this] { durationUnitCombo.grabKeyboardFocus(); };
    mElevationDampeningEditor.onFocusLost = [this] {
        listeners.call([&](Listener & l) {
            l.trajectoryBoxElevationDampeningCyclesChanged(mElevationDampeningEditor.getText().getIntValue());
        });
        durationUnitCombo.grabKeyboardFocus();
    };
}

TrajectoryBoxComponent::~TrajectoryBoxComponent()
{
    setLookAndFeel(nullptr);
}

void TrajectoryBoxComponent::setSpatMode(SpatMode spatMode)
{
    m_spatMode = spatMode;
    resized();
}

void TrajectoryBoxComponent::setNumberOfSources(int numOfSources)
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
        mPositionSourceLinkCombo.setItemEnabled(static_cast<int>(PositionSourceLink::linkSymmetricX), true);
        mPositionSourceLinkCombo.setItemEnabled(static_cast<int>(PositionSourceLink::linkSymmetricY), true);
    } else {
        mPositionSourceLinkCombo.setItemEnabled(static_cast<int>(PositionSourceLink::linkSymmetricX), false);
        mPositionSourceLinkCombo.setItemEnabled(static_cast<int>(PositionSourceLink::linkSymmetricY), false);
    }
}

void TrajectoryBoxComponent::setTrajectoryType(int type)
{
    mPositionTrajectoryTypeCombo.setSelectedId(type);
}

void TrajectoryBoxComponent::setElevationTrajectoryType(int type)
{
    mElevationTracjectoryTypeCombo.setSelectedId(type);
}

void TrajectoryBoxComponent::setPositionBackAndForth(bool state)
{
    mPositionBackAndForthToggle.setToggleState(state, NotificationType::sendNotificationAsync);
    setPositionDampeningEnabled(state);
}

void TrajectoryBoxComponent::setElevationBackAndForth(bool state)
{
    mElevationBackAndForthToggle.setToggleState(state, NotificationType::sendNotificationAsync);
    setElevationDampeningEnabled(state);
}

void TrajectoryBoxComponent::setPositionDampeningEnabled(bool state)
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

void TrajectoryBoxComponent::setElevationDampeningEnabled(bool state)
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

void TrajectoryBoxComponent::setPositionDampeningCycles(int value)
{
    mPositionDampeningEditor.setText(String(value));
}

void TrajectoryBoxComponent::setElevationDampeningCycles(int value)
{
    mElevationDampeningEditor.setText(String(value));
}

void TrajectoryBoxComponent::setDeviationPerCycle(float value)
{
    deviationEditor.setText(String(value));
}

void TrajectoryBoxComponent::setPostionSourceLink(PositionSourceLink value)
{
    mPositionSourceLinkCombo.setSelectedId(static_cast<int>(value));
}

void TrajectoryBoxComponent::setElevationSourceLink(ElevationSourceLink value)
{
    mElevationSourceLinkCombo.setSelectedId(static_cast<int>(value));
}

void TrajectoryBoxComponent::setPositionActivateState(bool state)
{
    mPositionActivateButton.setToggleState(state, NotificationType::dontSendNotification);
}

void TrajectoryBoxComponent::setElevationActivateState(bool state)
{
    mElevationActivateButton.setToggleState(state, NotificationType::dontSendNotification);
}

void TrajectoryBoxComponent::setCycleDuration(double value)
{
    durationEditor.setText(String(value));
    listeners.call([&](Listener & l) {
        l.trajectoryBoxCycleDurationChanged(durationEditor.getText().getDoubleValue(),
                                            durationUnitCombo.getSelectedId());
    });
}

void TrajectoryBoxComponent::setDurationUnit(int value)
{
    durationUnitCombo.setSelectedId(value, NotificationType::sendNotificationSync);
    listeners.call([&](Listener & l) {
        l.trajectoryBoxDurationUnitChanged(durationEditor.getText().getDoubleValue(),
                                           durationUnitCombo.getSelectedId());
    });
}

void TrajectoryBoxComponent::paint(Graphics & g)
{
    GrisLookAndFeel * lookAndFeel;
    lookAndFeel = static_cast<GrisLookAndFeel *>(&getLookAndFeel());
    g.fillAll(lookAndFeel->findColour(ResizableWindow::backgroundColourId));
}

void TrajectoryBoxComponent::resized()
{
    sourceLinkLabel.setBounds(5, 10, 150, 20);
    mPositionSourceLinkCombo.setBounds(115, 10, 175, 20);
    trajectoryTypeLabel.setBounds(5, 40, 150, 20);
    mPositionTrajectoryTypeCombo.setBounds(115, 40, 175, 20);
    mPositionBackAndForthToggle.setBounds(196, 70, 94, 20);
    dampeningLabel.setBounds(5, 70, 150, 20);
    mPositionDampeningEditor.setBounds(115, 70, 75, 20);
    deviationLabel.setBounds(5, 100, 150, 20);
    deviationEditor.setBounds(115, 100, 75, 20);
    mPositionActivateButton.setBounds(114, 130, 176, 20);

    if (m_spatMode == SpatMode::LBAP) {
        mElevationSourceLinkCombo.setVisible(true);
        mElevationTracjectoryTypeCombo.setVisible(true);
        mElevationActivateButton.setVisible(true);
        mElevationBackAndForthToggle.setVisible(true);
        mElevationDampeningEditor.setVisible(true);
        mElevationSourceLinkCombo.setBounds(305, 10, 175, 20);
        mElevationTracjectoryTypeCombo.setBounds(305, 40, 175, 20);
        mElevationBackAndForthToggle.setBounds(386, 70, 94, 20);
        mElevationDampeningEditor.setBounds(305, 70, 75, 20);
        mElevationActivateButton.setBounds(304, 130, 176, 20);
    } else {
        mElevationSourceLinkCombo.setVisible(false);
        mElevationTracjectoryTypeCombo.setVisible(false);
        mElevationActivateButton.setVisible(false);
        mElevationBackAndForthToggle.setVisible(false);
        mElevationDampeningEditor.setVisible(false);
    }

    durationLabel.setBounds(490, 15, 90, 20);
    durationEditor.setBounds(495, 40, 90, 20);
    durationUnitCombo.setBounds(495, 70, 90, 20);

    // Hide Cycle Speed slider until we found the good way to handle it!
    cycleSpeedLabel.setBounds(5, 100, 150, 20);
    cycleSpeedSlider.setBounds(115, 100, 165, 20);
    cycleSpeedLabel.setVisible(false);
    cycleSpeedSlider.setVisible(false);
    //------------------------------------------------------------------
}
