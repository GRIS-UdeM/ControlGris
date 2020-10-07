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

#include "cg_SettingsBoxComponent.hpp"

namespace gris
{
//==============================================================================
SettingsBoxComponent::SettingsBoxComponent(GrisLookAndFeel & grisLookAndFeel) : mGrisLookAndFeel(grisLookAndFeel)
{
    mOscFormatLabel.setText("OSC Format:", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(&mOscFormatLabel);

    mOscFormatCombo.addItem("DOME - SpatGris", 1);
    mOscFormatCombo.addItem("CUBE - SpatGris", 2);
    mOscFormatCombo.onChange = [this] {
        mListeners.call(
            [&](Listener & l) { l.settingsBoxOscFormatChanged((SpatMode)(mOscFormatCombo.getSelectedId() - 1)); });
    };
    mOscFormatCombo.setSelectedId(1);
    addAndMakeVisible(&mOscFormatCombo);

    mOscPortLabel.setText("OSC Port:", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(&mOscPortLabel);

    juce::String defaultPort("18032");
    mOscPortEditor.setExplicitFocusOrder(4);
    mOscPortEditor.setText(defaultPort);
    mOscPortEditor.setInputRestrictions(5, "0123456789");
    mOscPortEditor.onReturnKey = [this] { mOscFormatCombo.grabKeyboardFocus(); };
    mOscPortEditor.onFocusLost = [this, defaultPort] {
        if (!mOscPortEditor.isEmpty()) {
            mListeners.call(
                [&](Listener & l) { l.settingsBoxOscPortNumberChanged(mOscPortEditor.getText().getIntValue()); });
        } else {
            mListeners.call([&](Listener & l) {
                l.settingsBoxOscPortNumberChanged(defaultPort.getIntValue());
                mOscPortEditor.setText(defaultPort);
            });
        }
    };
    addAndMakeVisible(&mOscPortEditor);

    mNumOfSourcesLabel.setText("Number of Sources:", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(&mNumOfSourcesLabel);

    mNumOfSourcesEditor.setExplicitFocusOrder(2);
    mNumOfSourcesEditor.setText("2");
    mNumOfSourcesEditor.setInputRestrictions(1, "12345678");
    mNumOfSourcesEditor.onReturnKey = [this] { mOscFormatCombo.grabKeyboardFocus(); };
    mNumOfSourcesEditor.onFocusLost = [this] {
        if (!mNumOfSourcesEditor.isEmpty()) {
            mListeners.call([&](Listener & l) {
                l.settingsBoxNumberOfSourcesChanged(mNumOfSourcesEditor.getText().getIntValue());
            });
        } else {
            mListeners.call([&](Listener & l) {
                l.settingsBoxNumberOfSourcesChanged(1);
                mNumOfSourcesEditor.setText("1");
            });
        }
    };
    addAndMakeVisible(&mNumOfSourcesEditor);

    mFirstSourceIdLabel.setText("First Source ID:", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(&mFirstSourceIdLabel);

    mFirstSourceIdEditor.setExplicitFocusOrder(3);
    mFirstSourceIdEditor.setText("1");
    mFirstSourceIdEditor.setInputRestrictions(3, "0123456789");
    mFirstSourceIdEditor.onReturnKey = [this] { mOscFormatCombo.grabKeyboardFocus(); };
    mFirstSourceIdEditor.onFocusLost = [this] {
        if (!mFirstSourceIdEditor.isEmpty()) {
            mListeners.call([&](Listener & l) {
                l.settingsBoxFirstSourceIdChanged(SourceId{ mFirstSourceIdEditor.getText().getIntValue() });
            });
        } else {
            mListeners.call([&](Listener & l) {
                l.settingsBoxFirstSourceIdChanged(SourceId{ 1 });
                mFirstSourceIdEditor.setText("1");
            });
        }
    };
    addAndMakeVisible(&mFirstSourceIdEditor);

    mPositionActivateButton.setExplicitFocusOrder(1);
    mPositionActivateButton.setButtonText("Activate OSC");
    mPositionActivateButton.onClick = [this] {
        mListeners.call([&](Listener & l) { l.settingsBoxOscActivated(mPositionActivateButton.getToggleState()); });
    };
    addAndMakeVisible(&mPositionActivateButton);
}

//==============================================================================
void SettingsBoxComponent::setOscFormat(SpatMode mode)
{
    mOscFormatCombo.setSelectedId(static_cast<int>(mode) + 1, juce::NotificationType::dontSendNotification);
}

//==============================================================================
void SettingsBoxComponent::setOscPortNumber(int oscPortNumber)
{
    mOscPortEditor.setText(juce::String(oscPortNumber));
}

//==============================================================================
void SettingsBoxComponent::setNumberOfSources(int numOfSources)
{
    mNumOfSourcesEditor.setText(juce::String(numOfSources));
}

//==============================================================================
void SettingsBoxComponent::setFirstSourceId(SourceId const firstSourceId)
{
    mFirstSourceIdEditor.setText(firstSourceId.toString());
}

//==============================================================================
void SettingsBoxComponent::setActivateButtonState(bool shouldBeOn)
{
    mPositionActivateButton.setToggleState(shouldBeOn, juce::NotificationType::dontSendNotification);
}

//==============================================================================
void SettingsBoxComponent::paint(juce::Graphics & g)
{
    g.fillAll(mGrisLookAndFeel.findColour(juce::ResizableWindow::backgroundColourId));
}

//==============================================================================
void SettingsBoxComponent::resized()
{
    mOscFormatLabel.setBounds(5, 10, 90, 15);
    mOscFormatCombo.setBounds(95, 10, 150, 20);

    mOscPortLabel.setBounds(5, 40, 90, 15);
    mOscPortEditor.setBounds(95, 40, 150, 20);

    mNumOfSourcesLabel.setBounds(265, 10, 130, 15);
    mNumOfSourcesEditor.setBounds(395, 10, 40, 15);

    mFirstSourceIdLabel.setBounds(265, 40, 130, 15);
    mFirstSourceIdEditor.setBounds(395, 40, 40, 15);

    mPositionActivateButton.setBounds(5, 70, 150, 20);
}

} // namespace gris