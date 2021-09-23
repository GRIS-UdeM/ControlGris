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

#include "cg_SectionTrajectory.hpp"

#include "BinaryData.h"
#include "cg_constants.hpp"

namespace gris
{
//==============================================================================
SectionTrajectory::SectionTrajectory(GrisLookAndFeel & grisLookAndFeel)
    : mGrisLookAndFeel(grisLookAndFeel)
    , mLockedImage{ juce::PNGImageFormat::loadFrom(BinaryData::lock_closed_png, BinaryData::lock_closed_pngSize) }
    , mUnlockedImage{ juce::PNGImageFormat::loadFrom(BinaryData::lock_open_png, BinaryData::lock_open_pngSize) }
{
    auto const initLockButton = [&](ToggleImage & button, bool const isPosition) {
        addAndMakeVisible(button);
        if (isPosition) {
            auto const callback
                = [&](Listener & listener) { listener.positionActivateLockChangedCallback(button.getToggleState()); };
            button.onClick = [this, callback]() { mListeners.call(callback); };
        } else {
            auto const callback
                = [&](Listener & listener) { listener.elevationActivateLockChangedCallback(button.getToggleState()); };
            button.onClick = [this, callback]() { mListeners.call(callback); };
        }
    };

    initLockButton(mPositionActivateLockButton, true);
    initLockButton(mElevationActivateLockButton, false);

    mSpatMode = SpatMode::dome;

    mSourceLinkLabel.setText("Sources Link:", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(&mSourceLinkLabel);

    mPositionSourceLinkCombo.addItemList(POSITION_SOURCE_LINK_TYPES, 1);
    mPositionSourceLinkCombo.setSelectedId(1);
    addAndMakeVisible(&mPositionSourceLinkCombo);
    mPositionSourceLinkCombo.onChange = [this] {
        mListeners.call([&](Listener & l) {
            l.positionSourceLinkChangedCallback(
                static_cast<PositionSourceLink>(mPositionSourceLinkCombo.getSelectedId()));
        });
    };

    mElevationSourceLinkCombo.addItemList(ELEVATION_SOURCE_LINK_TYPES, 1);
    mElevationSourceLinkCombo.setSelectedId(1);
    addChildComponent(&mElevationSourceLinkCombo);
    mElevationSourceLinkCombo.onChange = [this] {
        mListeners.call([&](Listener & l) {
            l.elevationSourceLinkChangedCallback(
                static_cast<ElevationSourceLink>(mElevationSourceLinkCombo.getSelectedId()));
        });
    };

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
void SectionTrajectory::setSpatMode(SpatMode const spatMode)
{
    mSpatMode = spatMode;
    resized();
}

//==============================================================================
void SectionTrajectory::setNumberOfSources(int const numOfSources)
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
void SectionTrajectory::setTrajectoryType(int const type)
{
    mPositionTrajectoryTypeCombo.setSelectedId(type);
}

//==============================================================================
void SectionTrajectory::setElevationTrajectoryType(int const type)
{
    mElevationTrajectoryTypeCombo.setSelectedId(type);
}

//==============================================================================
void SectionTrajectory::setPositionBackAndForth(bool const state)
{
    mPositionBackAndForthToggle.setToggleState(state, juce::NotificationType::sendNotification);
    setPositionDampeningEnabled(state);
}

//==============================================================================
void SectionTrajectory::setElevationBackAndForth(bool const state)
{
    mElevationBackAndForthToggle.setToggleState(state, juce::NotificationType::sendNotification);
    setElevationDampeningEnabled(state);
}

//==============================================================================
void SectionTrajectory::setPositionDampeningEnabled(bool const state)
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
void SectionTrajectory::setElevationDampeningEnabled(bool const state)
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
void SectionTrajectory::setPositionDampeningCycles(int const value)
{
    mPositionDampeningEditor.setText(juce::String(value));
}

//==============================================================================
void SectionTrajectory::setElevationDampeningCycles(int const value)
{
    mElevationDampeningEditor.setText(juce::String(value));
}

//==============================================================================
void SectionTrajectory::setDeviationPerCycle(float const value)
{
    mDeviationEditor.setText(juce::String(value));
}

//==============================================================================
void SectionTrajectory::setPositionSourceLink(PositionSourceLink const value)
{
    mPositionSourceLinkCombo.setSelectedId(static_cast<int>(value));
}

//==============================================================================
void SectionTrajectory::setElevationSourceLink(ElevationSourceLink const value)
{
    mElevationSourceLinkCombo.setSelectedId(static_cast<int>(value));
}

//==============================================================================
void SectionTrajectory::setPositionActivateState(bool const state)
{
    mPositionActivateButton.setToggleState(state, juce::NotificationType::dontSendNotification);
}

//==============================================================================
void SectionTrajectory::setElevationActivateState(bool const state)
{
    mElevationActivateButton.setToggleState(state, juce::NotificationType::dontSendNotification);
}

//==============================================================================
void SectionTrajectory::setSymmetricLinkComboState(bool const allowed)
{
    mPositionSourceLinkCombo.setItemEnabled(static_cast<int>(PositionSourceLink::symmetricX), allowed);
    mPositionSourceLinkCombo.setItemEnabled(static_cast<int>(PositionSourceLink::symmetricY), allowed);
}

//==============================================================================
void SectionTrajectory::setCycleDuration(double const value)
{
    mDurationEditor.setText(juce::String(value));
    mListeners.call([&](Listener & l) {
        l.trajectoryCycleDurationChangedCallback(mDurationEditor.getText().getDoubleValue(),
                                                 mDurationUnitCombo.getSelectedId());
    });
}

//==============================================================================
void SectionTrajectory::setDurationUnit(int const value)
{
    mDurationUnitCombo.setSelectedId(value, juce::NotificationType::sendNotificationSync);
    mListeners.call([&](Listener & l) {
        l.trajectoryDurationUnitChangedCallback(mDurationEditor.getText().getDoubleValue(),
                                                mDurationUnitCombo.getSelectedId());
    });
}

//==============================================================================
void SectionTrajectory::paint(juce::Graphics & g)
{
    g.fillAll(mGrisLookAndFeel.findColour(juce::ResizableWindow::backgroundColourId));
}

//==============================================================================
void SectionTrajectory::resized()
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
    mPositionActivateButton.setBounds(114, 130, 151, 20);
    mPositionActivateLockButton.setBounds(270, 130, 20, 20);

    if (mSpatMode == SpatMode::cube) {
        mElevationSourceLinkCombo.setVisible(true);
        mElevationTrajectoryTypeCombo.setVisible(true);
        mElevationActivateButton.setVisible(true);
        mElevationActivateLockButton.setVisible(true);
        mElevationBackAndForthToggle.setVisible(true);
        mElevationDampeningEditor.setVisible(true);
        mElevationSourceLinkCombo.setBounds(305, 10, 175, 20);
        mElevationTrajectoryTypeCombo.setBounds(305, 40, 175, 20);
        mElevationBackAndForthToggle.setBounds(386, 70, 94, 20);
        mElevationDampeningEditor.setBounds(305, 70, 75, 20);
        mElevationActivateButton.setBounds(304, 130, 151, 20);
        mElevationActivateLockButton.setBounds(460, 130, 20, 20);
    } else {
        mElevationSourceLinkCombo.setVisible(false);
        mElevationTrajectoryTypeCombo.setVisible(false);
        mElevationActivateButton.setVisible(false);
        mElevationActivateLockButton.setVisible(false);
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

} // namespace gris