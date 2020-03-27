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

TrajectoryBoxComponent::TrajectoryBoxComponent() {
    m_spatMode = SPAT_MODE_VBAP;

    sourceLinkLabel.setText("Source Link:", NotificationType::dontSendNotification);
    addAndMakeVisible(&sourceLinkLabel);

    sourceLinkCombo.addItemList(SOURCE_LINK_TYPES, 1);
    sourceLinkCombo.setSelectedId(1);
    addAndMakeVisible(&sourceLinkCombo);
    sourceLinkCombo.onChange = [this] {
            listeners.call([&] (Listener& l) { l.trajectoryBoxSourceLinkChanged(sourceLinkCombo.getSelectedId()); }); };

    sourceLinkAltCombo.addItemList(SOURCE_LINK_ALT_TYPES, 1);
    sourceLinkAltCombo.setSelectedId(1);
    addChildComponent(&sourceLinkAltCombo);
    sourceLinkAltCombo.onChange = [this] { 
            listeners.call([&] (Listener& l) { l.trajectoryBoxSourceLinkAltChanged(sourceLinkAltCombo.getSelectedId()); }); };

    trajectoryTypeLabel.setText("Trajectory Type:", NotificationType::dontSendNotification);
    addAndMakeVisible(&trajectoryTypeLabel);

    trajectoryTypeCombo.addItemList(TRAJECTORY_TYPE_TYPES, 1);
    trajectoryTypeCombo.setSelectedId(1);
    addAndMakeVisible(&trajectoryTypeCombo);
    trajectoryTypeCombo.onChange = [this] { 
            listeners.call([&] (Listener& l) { l.trajectoryBoxTrajectoryTypeChanged(trajectoryTypeCombo.getSelectedId()); }); };

    trajectoryTypeAltCombo.addItemList(TRAJECTORY_TYPE_ALT_TYPES, 1);
    trajectoryTypeAltCombo.setSelectedId(1);
    addChildComponent(&trajectoryTypeAltCombo);
    trajectoryTypeAltCombo.onChange = [this] { 
            listeners.call([&] (Listener& l) { l.trajectoryBoxTrajectoryTypeAltChanged(trajectoryTypeAltCombo.getSelectedId()); }); };

    durationLabel.setText("Dur per cycle:", NotificationType::dontSendNotification);
    addAndMakeVisible(&durationLabel);

    addAndMakeVisible(&durationEditor);
    durationEditor.setTextToShowWhenEmpty ("1", Colours::white);
    durationEditor.setText("5", false);
    durationEditor.setInputRestrictions (10, "0123456789.");
    durationEditor.onReturnKey = [this] {
            durationUnitCombo.grabKeyboardFocus();
        };
    durationEditor.onFocusLost = [this] {
            listeners.call([&] (Listener& l) { l.trajectoryBoxCycleDurationChanged(durationEditor.getText().getDoubleValue(), durationUnitCombo.getSelectedId()); });
            durationUnitCombo.grabKeyboardFocus(); };

    addAndMakeVisible(&durationUnitCombo);
    durationUnitCombo.addItem("Sec(s)", 1);
    durationUnitCombo.addItem("Beat(s)", 2);
    durationUnitCombo.setSelectedId(1);
    durationUnitCombo.onChange = [this] { 
            listeners.call([&] (Listener& l) { l.trajectoryBoxDurationUnitChanged(durationEditor.getText().getDoubleValue(), durationUnitCombo.getSelectedId()); }); };

    cycleSpeedLabel.setText("Cycle Speed:", NotificationType::dontSendNotification);
    addAndMakeVisible(&cycleSpeedLabel);

    cycleSpeedSlider.setNormalisableRange(NormalisableRange<double>(-2.0f, 2.0f, 0.01f));
    cycleSpeedSlider.setValue(1.0, NotificationType::sendNotificationAsync);
    cycleSpeedSlider.setTextBoxStyle(Slider::TextBoxRight, false, 40, 20);
    cycleSpeedSlider.setColour(Slider:: textBoxOutlineColourId, Colours::transparentBlack);
    addAndMakeVisible(&cycleSpeedSlider);

    activateButton.addShortcut(KeyPress('a', 0, 0));
    addAndMakeVisible(&activateButton);
    activateButton.setButtonText("Activate");
    activateButton.setClickingTogglesState(true);
    activateButton.onClick = [this] { 
            listeners.call([&] (Listener& l) { l.trajectoryBoxActivateChanged(activateButton.getToggleState()); });
            durationUnitCombo.grabKeyboardFocus(); };

    backAndForthToggle.setButtonText("Back & Forth");
    backAndForthToggle.onClick = [this] {
            listeners.call([&] (Listener& l) { l.trajectoryBoxBackAndForthChanged(backAndForthToggle.getToggleState()); });
            setDampeningEditorEnabled(backAndForthToggle.getToggleState());
        };
    addAndMakeVisible(&backAndForthToggle);

    dampeningLabel.setText("Number of cycles \ndampening:", NotificationType::dontSendNotification);
    addAndMakeVisible(&dampeningLabel);

    addAndMakeVisible(&dampeningEditor);
    dampeningEditor.setTextToShowWhenEmpty ("0", Colours::white);
    dampeningEditor.setText("0", false);
    dampeningEditor.setInputRestrictions (10, "0123456789");
    dampeningEditor.onReturnKey = [this] {
            durationUnitCombo.grabKeyboardFocus();
        };
    dampeningEditor.onFocusLost = [this] {
            listeners.call([&] (Listener& l) { l.trajectoryBoxDampeningCyclesChanged(dampeningEditor.getText().getIntValue()); });
            durationUnitCombo.grabKeyboardFocus(); };

    deviationLabel.setText("Deviation degrees\nper cycle:", NotificationType::dontSendNotification);
    addAndMakeVisible(&deviationLabel);

    addAndMakeVisible(&deviationEditor);
    deviationEditor.setTextToShowWhenEmpty ("0", Colours::white);
    deviationEditor.setText("0", false);
    deviationEditor.setInputRestrictions (10, "-0123456789.");
    deviationEditor.onReturnKey = [this] {
            durationUnitCombo.grabKeyboardFocus();
        };
    deviationEditor.onFocusLost = [this] {
            listeners.call([&] (Listener& l) { l.trajectoryBoxDeviationPerCycleChanged(std::fmod(deviationEditor.getText().getFloatValue(), 360.0)); });
            deviationEditor.setText(String(std::fmod(deviationEditor.getText().getFloatValue(), 360.0)));
            durationUnitCombo.grabKeyboardFocus(); };

    activateAltButton.addShortcut(KeyPress('a', ModifierKeys::shiftModifier, 0));
    addChildComponent(&activateAltButton);
    activateAltButton.setButtonText("Activate");
    activateAltButton.setClickingTogglesState(true);
    activateAltButton.onClick = [this] { 
            listeners.call([&] (Listener& l) { l.trajectoryBoxActivateAltChanged(activateAltButton.getToggleState()); });
            durationUnitCombo.grabKeyboardFocus(); };

    backAndForthAltToggle.setButtonText("Back & Forth");
    backAndForthAltToggle.onClick = [this] {
            listeners.call([&] (Listener& l) { l.trajectoryBoxBackAndForthAltChanged(backAndForthAltToggle.getToggleState()); });
            setDampeningAltEditorEnabled(backAndForthAltToggle.getToggleState());
        };
    addAndMakeVisible(&backAndForthAltToggle);

    addAndMakeVisible(&dampeningAltEditor);
    dampeningAltEditor.setTextToShowWhenEmpty ("0", Colours::white);
    dampeningAltEditor.setText("0", false);
    dampeningAltEditor.setInputRestrictions (10, "0123456789");
    dampeningAltEditor.onReturnKey = [this] {
            durationUnitCombo.grabKeyboardFocus();
        };
    dampeningAltEditor.onFocusLost = [this] {
            listeners.call([&] (Listener& l) { l.trajectoryBoxDampeningCyclesAltChanged(dampeningAltEditor.getText().getIntValue()); });
            durationUnitCombo.grabKeyboardFocus(); };
}

TrajectoryBoxComponent::~TrajectoryBoxComponent() {
    setLookAndFeel(nullptr);
}

void TrajectoryBoxComponent::setSpatMode(SPAT_MODE_ENUM spatMode) {
    m_spatMode = spatMode;
    resized();
}

void TrajectoryBoxComponent::setNumberOfSources(int numOfSources) {
    if (numOfSources == 1) {
        sourceLinkCombo.setSelectedId(1);
        sourceLinkCombo.setEnabled(false);
        sourceLinkAltCombo.setSelectedId(1);
        sourceLinkAltCombo.setEnabled(false);
    } else {
        sourceLinkCombo.setEnabled(true);
        sourceLinkAltCombo.setEnabled(true);
    }

    if (numOfSources == 2) {
        sourceLinkCombo.setItemEnabled(SOURCE_LINK_SYMMETRIC_X, true);
        sourceLinkCombo.setItemEnabled(SOURCE_LINK_SYMMETRIC_Y, true);
    } else {
        sourceLinkCombo.setItemEnabled(SOURCE_LINK_SYMMETRIC_X, false);
        sourceLinkCombo.setItemEnabled(SOURCE_LINK_SYMMETRIC_Y, false);
    }
}

void TrajectoryBoxComponent::setTrajectoryType(int type) {
    trajectoryTypeCombo.setSelectedId(type);
}

void TrajectoryBoxComponent::setTrajectoryTypeAlt(int type) {
    trajectoryTypeAltCombo.setSelectedId(type);
}

void TrajectoryBoxComponent::setBackAndForth(bool state) {
    backAndForthToggle.setToggleState(state, NotificationType::sendNotificationAsync);
    setDampeningEditorEnabled(state);
}

void TrajectoryBoxComponent::setBackAndForthAlt(bool state) {
    backAndForthAltToggle.setToggleState(state, NotificationType::sendNotificationAsync);
    setDampeningAltEditorEnabled(state);
}

void TrajectoryBoxComponent::setDampeningEditorEnabled(bool state) {
    dampeningEditor.setEnabled(state);
    String text = dampeningEditor.getText();
    dampeningEditor.clear();
    if (state)
        dampeningEditor.setColour(TextEditor::textColourId, Colour::fromRGB(235, 245, 250));
    else
        dampeningEditor.setColour(TextEditor::textColourId, Colour::fromRGB(172, 172, 172));
    dampeningEditor.setText(text);
}

void TrajectoryBoxComponent::setDampeningAltEditorEnabled(bool state) {
    dampeningAltEditor.setEnabled(state);
    String text = dampeningAltEditor.getText();
    dampeningAltEditor.clear();
    if (state)
        dampeningAltEditor.setColour(TextEditor::textColourId, Colour::fromRGB(235, 245, 250));
    else
        dampeningAltEditor.setColour(TextEditor::textColourId, Colour::fromRGB(172, 172, 172));
    dampeningAltEditor.setText(text);
}

void TrajectoryBoxComponent::setDampeningCycles(int value) {
    dampeningEditor.setText(String(value));
}

void TrajectoryBoxComponent::setDampeningCyclesAlt(int value) {
    dampeningAltEditor.setText(String(value));
}

void TrajectoryBoxComponent::setDeviationPerCycle(float value) {
    deviationEditor.setText(String(value));
}

void TrajectoryBoxComponent::setSourceLink(int value) {
    sourceLinkCombo.setSelectedId(value);
}

void TrajectoryBoxComponent::setSourceLinkAlt(int value) {
    sourceLinkAltCombo.setSelectedId(value);
}

bool TrajectoryBoxComponent::getActivateState() {
    return activateButton.getToggleState();
}

void TrajectoryBoxComponent::setActivateState(bool state) {
    activateButton.setToggleState(state, NotificationType::dontSendNotification);
}

bool TrajectoryBoxComponent::getActivateAltState() {
    return activateAltButton.getToggleState();
}

void TrajectoryBoxComponent::setActivateAltState(bool state) {
    activateAltButton.setToggleState(state, NotificationType::dontSendNotification);
}

void TrajectoryBoxComponent::setCycleDuration(double value) {
    durationEditor.setText(String(value));
    listeners.call([&] (Listener& l) { l.trajectoryBoxCycleDurationChanged(durationEditor.getText().getDoubleValue(), durationUnitCombo.getSelectedId()); });
}

void TrajectoryBoxComponent::setDurationUnit(int value) {
    durationUnitCombo.setSelectedId(value, NotificationType::sendNotificationSync);
    listeners.call([&] (Listener& l) { l.trajectoryBoxDurationUnitChanged(durationEditor.getText().getDoubleValue(), durationUnitCombo.getSelectedId()); });
}

void TrajectoryBoxComponent::paint(Graphics& g) {
    GrisLookAndFeel *lookAndFeel;
    lookAndFeel = static_cast<GrisLookAndFeel *> (&getLookAndFeel());
    g.fillAll (lookAndFeel->findColour (ResizableWindow::backgroundColourId));
}

void TrajectoryBoxComponent::resized() {
    sourceLinkLabel.setBounds(5, 10, 150, 20);
    sourceLinkCombo.setBounds(115, 10, 175, 20);
    trajectoryTypeLabel.setBounds(5, 40, 150, 20);
    trajectoryTypeCombo.setBounds(115, 40, 175, 20);
    backAndForthToggle.setBounds(196, 70, 94, 20);
    dampeningLabel.setBounds(5, 70, 150, 20);
    dampeningEditor.setBounds(115, 70, 75, 20);
    deviationLabel.setBounds(5, 100, 150, 20);
    deviationEditor.setBounds(115, 100, 75, 20);
    activateButton.setBounds(114, 130, 176, 20);

    if (m_spatMode == SPAT_MODE_LBAP) {
        sourceLinkAltCombo.setVisible(true);
        trajectoryTypeAltCombo.setVisible(true);
        activateAltButton.setVisible(true);
        backAndForthAltToggle.setVisible(true);
        dampeningAltEditor.setVisible(true);
        sourceLinkAltCombo.setBounds(305, 10, 175, 20);
        trajectoryTypeAltCombo.setBounds(305, 40, 175, 20);
        backAndForthAltToggle.setBounds(386, 70, 94, 20);
        dampeningAltEditor.setBounds(305, 70, 75, 20);
        activateAltButton.setBounds(304, 130, 176, 20);
    } else {
        sourceLinkAltCombo.setVisible(false);
        trajectoryTypeAltCombo.setVisible(false);
        activateAltButton.setVisible(false);
        backAndForthAltToggle.setVisible(false);
        dampeningAltEditor.setVisible(false);
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
