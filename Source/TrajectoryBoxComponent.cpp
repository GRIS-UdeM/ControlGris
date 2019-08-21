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
    durationEditor.onFocusLost = [this] {
            listeners.call([&] (Listener& l) { l.trajectoryBoxDurationChanged(durationEditor.getText().getDoubleValue(), durationUnitCombo.getSelectedId()); });
            durationUnitCombo.grabKeyboardFocus(); };

    addAndMakeVisible(&durationUnitCombo);
    durationUnitCombo.addItem("Sec(s)", 1);
    durationUnitCombo.addItem("Beat(s)", 2);
    durationUnitCombo.setSelectedId(1);
    durationUnitCombo.onChange = [this] { 
            listeners.call([&] (Listener& l) { l.trajectoryBoxDurationChanged(durationEditor.getText().getDoubleValue(), durationUnitCombo.getSelectedId()); }); };

    numOfCycleLabel.setText("Number of Cycles:", NotificationType::dontSendNotification);
    addAndMakeVisible(&numOfCycleLabel);

    addAndMakeVisible(&numOfCycleEditor);
    numOfCycleEditor.setTextToShowWhenEmpty ("1", Colours::white);
    numOfCycleEditor.setText("1", false);
    numOfCycleEditor.setInputRestrictions (6, "0123456789");
    numOfCycleEditor.onFocusLost = [this] { 
            listeners.call([&] (Listener& l) { l.trajectoryBoxNumOfCycleChanged(numOfCycleEditor.getText().getIntValue()); });
            durationUnitCombo.grabKeyboardFocus(); };

    cycleSpeedLabel.setText("Cycle Speed:", NotificationType::dontSendNotification);
    addAndMakeVisible(&cycleSpeedLabel);

    cycleSpeedSlider.setNormalisableRange(NormalisableRange<double>(-2.0f, 2.0f, 0.01f));
    cycleSpeedSlider.setValue(1.0, NotificationType::sendNotificationAsync);
    cycleSpeedSlider.setTextBoxStyle(Slider::TextBoxRight, false, 40, 20);
    cycleSpeedSlider.setColour(Slider:: textBoxOutlineColourId, Colours::transparentBlack);
    addAndMakeVisible(&cycleSpeedSlider);

    addAndMakeVisible(&editFixedSourceButton);
    editFixedSourceButton.setButtonText("Edit Fixed Sources");
    editFixedSourceButton.onClick = [this] { 
            listeners.call([&] (Listener& l) { l.trajectoryBoxEditFixedSourceButtonClicked(); }); };

    addAndMakeVisible(&fixSourceButton);
    fixSourceButton.setButtonText("Fix Sources");
    fixSourceButton.onClick = [this] { 
            listeners.call([&] (Listener& l) { l.trajectoryBoxFixSourceButtonClicked(); });
            durationUnitCombo.grabKeyboardFocus(); };

    addAndMakeVisible(&clearButton);
    clearButton.setButtonText("Clear");
    clearButton.onClick = [this] { 
            listeners.call([&] (Listener& l) { l.trajectoryBoxClearButtonClicked(); });
            durationUnitCombo.grabKeyboardFocus(); };

    addChildComponent(&clearAltButton);
    clearAltButton.setButtonText("Clear");
    clearAltButton.onClick = [this] { 
            listeners.call([&] (Listener& l) { l.trajectoryBoxClearAltButtonClicked(); });
            durationUnitCombo.grabKeyboardFocus(); };

    addAndMakeVisible(&activateButton);
    activateButton.setButtonText("Activate");
    activateButton.setClickingTogglesState(true);
    activateButton.onClick = [this] { 
            listeners.call([&] (Listener& l) { l.trajectoryBoxActivateChanged(activateButton.getToggleState()); });
            durationUnitCombo.grabKeyboardFocus(); };

    addChildComponent(&activateAltButton);
    activateAltButton.setButtonText("Activate");
    activateAltButton.setClickingTogglesState(true);
    activateAltButton.onClick = [this] { 
            listeners.call([&] (Listener& l) { l.trajectoryBoxActivateAltChanged(activateAltButton.getToggleState()); });
            durationUnitCombo.grabKeyboardFocus(); };
}

TrajectoryBoxComponent::~TrajectoryBoxComponent() {
    setLookAndFeel(nullptr);
}

void TrajectoryBoxComponent::setSpatMode(SPAT_MODE_ENUM spatMode) {
    m_spatMode = spatMode;
    resized();
}

void TrajectoryBoxComponent::setSourceLink(int value) {
    sourceLinkCombo.setSelectedId(value);
}

void TrajectoryBoxComponent::setSourceLinkAlt(int value) {
    sourceLinkAltCombo.setSelectedId(value);
}

void TrajectoryBoxComponent::setCycleDuration(double value) {
    durationEditor.setText(String(value));
    listeners.call([&] (Listener& l) { l.trajectoryBoxDurationChanged(durationEditor.getText().getDoubleValue(), durationUnitCombo.getSelectedId()); });
}

void TrajectoryBoxComponent::setDurationUnit(int value) {
    durationUnitCombo.setSelectedId(value, NotificationType::sendNotificationSync);
    listeners.call([&] (Listener& l) { l.trajectoryBoxDurationChanged(durationEditor.getText().getDoubleValue(), durationUnitCombo.getSelectedId()); });
}

void TrajectoryBoxComponent::setNumOfCycles(int value) {
    numOfCycleEditor.setText(String(value));
    listeners.call([&] (Listener& l) { l.trajectoryBoxNumOfCycleChanged(numOfCycleEditor.getText().getIntValue()); });
}

void TrajectoryBoxComponent::paint(Graphics& g) {
    GrisLookAndFeel *lookAndFeel;
    lookAndFeel = static_cast<GrisLookAndFeel *> (&getLookAndFeel());
    g.fillAll (lookAndFeel->findColour (ResizableWindow::backgroundColourId));
}

void TrajectoryBoxComponent::resized() {
    sourceLinkLabel.setBounds(5, 10, 150, 20);
    sourceLinkCombo.setBounds(120, 10, 160, 20);

    trajectoryTypeLabel.setBounds(5, 40, 150, 20);
    trajectoryTypeCombo.setBounds(120, 40, 160, 20);

    durationLabel.setBounds(5, 70, 150, 20);
    durationEditor.setBounds(120, 70, 90, 20);
    durationUnitCombo.setBounds(215, 70, 65, 20);

    cycleSpeedLabel.setBounds(5, 100, 150, 20);
    cycleSpeedSlider.setBounds(115, 100, 165, 20);

    activateButton.setBounds(10, 100, 100, 20);
    clearButton.setBounds(120, 100, 50, 20);

    // Hide Cycle Speed slider until we found the good way to handle it!
    cycleSpeedLabel.setVisible(false);
    cycleSpeedSlider.setVisible(false);

    numOfCycleLabel.setBounds(300, 70, 100, 20);
    numOfCycleEditor.setBounds(410, 70, 50, 20);

    editFixedSourceButton.setBounds(getWidth() - 120, 10, 100, 20);
    fixSourceButton.setBounds(getWidth() - 120, 40, 100, 20);

    if (m_spatMode == SPAT_MODE_LBAP) {
        sourceLinkAltCombo.setVisible(true);
        trajectoryTypeAltCombo.setVisible(true);
        clearAltButton.setVisible(true);
        activateAltButton.setVisible(true);
        sourceLinkAltCombo.setBounds(300, 10, 160, 20);
        trajectoryTypeAltCombo.setBounds(300, 40, 160, 20);
        activateAltButton.setBounds(300, 100, 100, 20);
        clearAltButton.setBounds(410, 100, 50, 20);
    } else {
        sourceLinkAltCombo.setVisible(false);
        trajectoryTypeAltCombo.setVisible(false);
        clearAltButton.setVisible(false);
        activateAltButton.setVisible(false);
    }

}
