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

#include "cg_SectionAbstractSpatialization.hpp"

#include "cg_constants.hpp"

namespace gris
{
//==============================================================================
SectionAbstractSpatialization::SectionAbstractSpatialization(GrisLookAndFeel & grisLookAndFeel) : mGrisLookAndFeel(grisLookAndFeel)
{
    setName("SectionAbstractSpatialization");

    mSpatMode = SpatMode::dome;

    mTrajectoryTypeLabel.setText("Trajectory Type:", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(&mTrajectoryTypeLabel);

    mPositionTrajectoryTypeCombo.addItemList(POSITION_TRAJECTORY_TYPE_TYPES, 1);
    mPositionTrajectoryTypeCombo.setSelectedId(1);
    addAndMakeVisible(&mPositionTrajectoryTypeCombo);
    mPositionTrajectoryTypeCombo.onChange = [this] {
        mListeners.call([&](Listener & l) {
            l.positionTrajectoryTypeChangedCallback(
                static_cast<PositionTrajectoryType>(mPositionTrajectoryTypeCombo.getSelectedId()));
        });
    };

    mElevationTrajectoryTypeCombo.addItemList(ELEVATION_TRAJECTORY_TYPE_TYPES, 1);
    mElevationTrajectoryTypeCombo.setSelectedId(1);
    addChildComponent(&mElevationTrajectoryTypeCombo);
    mElevationTrajectoryTypeCombo.onChange = [this] {
        mListeners.call([&](Listener & l) {
            l.elevationTrajectoryTypeChangedCallback(
                static_cast<ElevationTrajectoryType>(mElevationTrajectoryTypeCombo.getSelectedId()));
        });
    };

    mDurationLabel.setText("Dur per cycle:", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(&mDurationLabel);

    addAndMakeVisible(&mDurationEditor);
    mDurationEditor.setTextToShowWhenEmpty("1", juce::Colours::white);
    mDurationEditor.setText("5", false);
    mDurationEditor.setInputRestrictions(10, "0123456789.");
    mDurationEditor.onReturnKey = [this] { mDurationUnitCombo.grabKeyboardFocus(); };
    mDurationEditor.onFocusLost = [this] {
        mListeners.call([&](Listener & l) {
            l.trajectoryCycleDurationChangedCallback(mDurationEditor.getText().getDoubleValue(),
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
            l.trajectoryDurationUnitChangedCallback(mDurationEditor.getText().getDoubleValue(),
                                                    mDurationUnitCombo.getSelectedId());
        });
    };

    mCycleSpeedLabel.setText("Cycle Speed:", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(&mCycleSpeedLabel);

    mCycleSpeedSlider.setNormalisableRange(juce::NormalisableRange<double>(-2.0f, 2.0f, 0.01f));
    mCycleSpeedSlider.setValue(1.0, juce::NotificationType::sendNotificationAsync);
    mCycleSpeedSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);
    mCycleSpeedSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(&mCycleSpeedSlider);

    // Removed because this interacted with DAWs
    // mPositionActivateButton.addShortcut(juce::KeyPress('a', 0, 0));

    addAndMakeVisible(&mPositionActivateButton);
    mPositionActivateButton.setButtonText("Activate");
    mPositionActivateButton.setClickingTogglesState(true);
    mPositionActivateButton.onClick = [this] {
        mListeners.call(
            [&](Listener & l) { l.positionTrajectoryStateChangedCallback(mPositionActivateButton.getToggleState()); });
        mDurationUnitCombo.grabKeyboardFocus();
    };

    mPositionBackAndForthToggle.setButtonText("Back & Forth");
    mPositionBackAndForthToggle.onClick = [this] {
        mListeners.call([&](Listener & l) {
            l.positionTrajectoryBackAndForthChangedCallback(mPositionBackAndForthToggle.getToggleState());
        });
        setPositionDampeningEnabled(mPositionBackAndForthToggle.getToggleState());
    };
    addAndMakeVisible(&mPositionBackAndForthToggle);

    mDampeningLabel.setText("Number of cycles \ndampening:", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(&mDampeningLabel);

    addAndMakeVisible(&mPositionDampeningEditor);
    mPositionDampeningEditor.setTextToShowWhenEmpty("0", juce::Colours::white);
    mPositionDampeningEditor.setText("0", false);
    mPositionDampeningEditor.setInputRestrictions(10, "0123456789");
    mPositionDampeningEditor.onReturnKey = [this] { mDurationUnitCombo.grabKeyboardFocus(); };
    mPositionDampeningEditor.onFocusLost = [this] {
        mListeners.call([&](Listener & l) {
            l.positionTrajectoryDampeningCyclesChangedCallback(mPositionDampeningEditor.getText().getIntValue());
        });
        mDurationUnitCombo.grabKeyboardFocus();
    };

    mDeviationLabel.setText("Deviation degrees\nper cycle:", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(&mDeviationLabel);

    addAndMakeVisible(&mDeviationEditor);
    mDeviationEditor.setTextToShowWhenEmpty("0", juce::Colours::white);
    mDeviationEditor.setText("0", false);
    mDeviationEditor.setInputRestrictions(10, "-0123456789.");
    mDeviationEditor.onReturnKey = [this] { mDurationUnitCombo.grabKeyboardFocus(); };
    mDeviationEditor.onFocusLost = [this] {
        mListeners.call([&](Listener & l) {
            l.trajectoryDeviationPerCycleChangedCallback(std::fmod(mDeviationEditor.getText().getFloatValue(), 360.0f));
        });
        mDeviationEditor.setText(juce::String(std::fmod(mDeviationEditor.getText().getFloatValue(), 360.0)));
        mDurationUnitCombo.grabKeyboardFocus();
    };

    mElevationActivateButton.addShortcut(juce::KeyPress('a', juce::ModifierKeys::shiftModifier, 0));
    addChildComponent(&mElevationActivateButton);
    mElevationActivateButton.setButtonText("Activate");
    mElevationActivateButton.setClickingTogglesState(true);
    mElevationActivateButton.onClick = [this] {
        mListeners.call([&](Listener & l) {
            l.elevationTrajectoryStateChangedCallback(mElevationActivateButton.getToggleState());
        });
        mDurationUnitCombo.grabKeyboardFocus();
    };

    mElevationBackAndForthToggle.setButtonText("Back & Forth");
    mElevationBackAndForthToggle.onClick = [this] {
        mListeners.call([&](Listener & l) {
            l.elevationTrajectoryBackAndForthChangedCallback(mElevationBackAndForthToggle.getToggleState());
        });
        setElevationDampeningEnabled(mElevationBackAndForthToggle.getToggleState());
    };
    addAndMakeVisible(&mElevationBackAndForthToggle);

    addAndMakeVisible(&mElevationDampeningEditor);
    mElevationDampeningEditor.setTextToShowWhenEmpty("0", juce::Colours::white);
    mElevationDampeningEditor.setText("0", false);
    mElevationDampeningEditor.setInputRestrictions(10, "0123456789");
    mElevationDampeningEditor.onReturnKey = [this] { mDurationUnitCombo.grabKeyboardFocus(); };
    mElevationDampeningEditor.onFocusLost = [this] {
        mListeners.call([&](Listener & l) {
            l.elevationTrajectoryDampeningCyclesChangedCallback(mElevationDampeningEditor.getText().getIntValue());
        });
        mDurationUnitCombo.grabKeyboardFocus();
    };
}

//==============================================================================
void SectionAbstractSpatialization::setSpatMode(SpatMode const spatMode)
{
    mSpatMode = spatMode;
    resized();
}

//==============================================================================
void SectionAbstractSpatialization::setTrajectoryType(int const type)
{
    mPositionTrajectoryTypeCombo.setSelectedId(type);
}

//==============================================================================
void SectionAbstractSpatialization::setElevationTrajectoryType(int const type)
{
    mElevationTrajectoryTypeCombo.setSelectedId(type);
}

//==============================================================================
void SectionAbstractSpatialization::setPositionBackAndForth(bool const state)
{
    mPositionBackAndForthToggle.setToggleState(state, juce::NotificationType::sendNotification);
    setPositionDampeningEnabled(state);
}

//==============================================================================
void SectionAbstractSpatialization::setElevationBackAndForth(bool const state)
{
    mElevationBackAndForthToggle.setToggleState(state, juce::NotificationType::sendNotification);
    setElevationDampeningEnabled(state);
}

//==============================================================================
void SectionAbstractSpatialization::setPositionDampeningEnabled(bool const state)
{
    mPositionDampeningEditor.setEnabled(state);
    juce::String text = mPositionDampeningEditor.getText();
    mPositionDampeningEditor.clear();
    if (state)
        mPositionDampeningEditor.setColour(juce::TextEditor::textColourId, juce::Colour::fromRGB(235, 245, 250));
    else
        mPositionDampeningEditor.setColour(juce::TextEditor::textColourId, juce::Colour::fromRGB(172, 172, 172));
    mPositionDampeningEditor.setText(text);
}

//==============================================================================
void SectionAbstractSpatialization::setElevationDampeningEnabled(bool const state)
{
    mElevationDampeningEditor.setEnabled(state);
    juce::String text = mElevationDampeningEditor.getText();
    mElevationDampeningEditor.clear();
    if (state)
        mElevationDampeningEditor.setColour(juce::TextEditor::textColourId, juce::Colour::fromRGB(235, 245, 250));
    else
        mElevationDampeningEditor.setColour(juce::TextEditor::textColourId, juce::Colour::fromRGB(172, 172, 172));
    mElevationDampeningEditor.setText(text);
}

//==============================================================================
void SectionAbstractSpatialization::setPositionDampeningCycles(int const value)
{
    mPositionDampeningEditor.setText(juce::String(value));
}

//==============================================================================
void SectionAbstractSpatialization::setElevationDampeningCycles(int const value)
{
    mElevationDampeningEditor.setText(juce::String(value));
}

//==============================================================================
void SectionAbstractSpatialization::setDeviationPerCycle(float const value)
{
    mDeviationEditor.setText(juce::String(value));
}

//==============================================================================
void SectionAbstractSpatialization::setPositionActivateState(bool const state)
{
    mPositionActivateButton.setToggleState(state, juce::NotificationType::dontSendNotification);
}

//==============================================================================
void SectionAbstractSpatialization::setElevationActivateState(bool const state)
{
    mElevationActivateButton.setToggleState(state, juce::NotificationType::dontSendNotification);
}

//==============================================================================
void SectionAbstractSpatialization::setCycleDuration(double const value)
{
    mDurationEditor.setText(juce::String(value));
    mListeners.call([&](Listener & l) {
        l.trajectoryCycleDurationChangedCallback(mDurationEditor.getText().getDoubleValue(),
                                                 mDurationUnitCombo.getSelectedId());
    });
}

//==============================================================================
void SectionAbstractSpatialization::setDurationUnit(int const value)
{
    mDurationUnitCombo.setSelectedId(value, juce::NotificationType::sendNotificationSync);
    mListeners.call([&](Listener & l) {
        l.trajectoryDurationUnitChangedCallback(mDurationEditor.getText().getDoubleValue(),
                                                mDurationUnitCombo.getSelectedId());
    });
}

//==============================================================================
void SectionAbstractSpatialization::paint(juce::Graphics & g)
{
    g.fillAll(mGrisLookAndFeel.findColour(juce::ResizableWindow::backgroundColourId));
}

//==============================================================================
void SectionAbstractSpatialization::resized()
{
    mTrajectoryTypeLabel.setBounds(5, 5, 150, 20);
    mPositionTrajectoryTypeCombo.setBounds(115, 5, 175, 20);
    mPositionBackAndForthToggle.setBounds(196, 35, 94, 20);
    mDampeningLabel.setBounds(5, 35, 150, 20);
    mPositionDampeningEditor.setBounds(115, 35, 75, 20);
    mDeviationLabel.setBounds(5, 65, 150, 20);
    mDeviationEditor.setBounds(115, 65, 75, 20);
    mPositionActivateButton.setBounds(114, 95, 176, 20);

    if (mSpatMode == SpatMode::cube) {
        mElevationTrajectoryTypeCombo.setVisible(true);
        mElevationActivateButton.setVisible(true);
        mElevationBackAndForthToggle.setVisible(true);
        mElevationDampeningEditor.setVisible(true);
        mElevationTrajectoryTypeCombo.setBounds(305, 5, 175, 20);
        mElevationBackAndForthToggle.setBounds(386, 35, 94, 20);
        mElevationDampeningEditor.setBounds(305, 35, 75, 20);
        mElevationActivateButton.setBounds(304, 95, 176, 20);
    } else {
        mElevationTrajectoryTypeCombo.setVisible(false);
        mElevationActivateButton.setVisible(false);
        mElevationBackAndForthToggle.setVisible(false);
        mElevationDampeningEditor.setVisible(false);
    }

    mDurationLabel.setBounds(490, 5, 90, 20);
    mDurationEditor.setBounds(495, 30, 90, 20);
    mDurationUnitCombo.setBounds(495, 60, 90, 20);

    // Hide Cycle Speed slider until we found the good way to handle it!
    mCycleSpeedLabel.setBounds(5, 100, 150, 20);
    mCycleSpeedSlider.setBounds(115, 100, 165, 20);
    mCycleSpeedLabel.setVisible(false);
    mCycleSpeedSlider.setVisible(false);
}

} // namespace gris
