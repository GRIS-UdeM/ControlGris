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

#include "PositionPresetComponent.h"

#include "PresetsManager.h"

//==============================================================================
void PresetButton::setSavedState(bool const savedState)
{
    mSaved = savedState;
    refresh();
}

//==============================================================================
void PresetButton::setLoadedState(bool const loadedState)
{
    mLoaded = loadedState;
    setToggleState(loadedState, NotificationType::dontSendNotification);
    refresh();
}

//==============================================================================
void PresetButton::refresh()
{
    if (mLoaded) {
        setColour(TextButton::buttonColourId, Colour::fromRGB(255, 165, 25));
    } else {
        if (mSaved) {
            setColour(TextButton::buttonColourId, Colour::fromRGB(255, 255, 255));
        } else {
            setColour(TextButton::buttonColourId, Colour::fromRGB(172, 172, 172));
        }
    }
}

//==============================================================================
void PresetButton::clicked(ModifierKeys const & mods)
{
    mListeners.call([&](Listener & l) { l.buttonClicked(this); });
    if (!mods.isShiftDown()) {
        mLoaded = getToggleState();
    }
    refresh();
};

//==============================================================================
void PresetButton::internalClickCallback(ModifierKeys const & mods)
{
    if (mods.isShiftDown()) {
        mListeners.call([&](Listener & l) { l.savingPresetClicked(this); });
        mSaved = true;
    } else if (mSaved && mods.isAltDown()) {
        mListeners.call([&](Listener & l) { l.deletingPresetClicked(this); });
        setToggleState(false, NotificationType::dontSendNotification);
        mLoaded = mSaved = false;
    } else if (mSaved) {
        TextButton::internalClickCallback(mods);
    }
    refresh();
};

//===============================================================================
PositionPresetComponent::PositionPresetComponent(PresetsManager & presetsManager) : mPresetsManager(presetsManager)
{
    constexpr int groupId = 1;

    for (int i{}; i < NUMBER_OF_POSITION_PRESETS; ++i) {
        auto button{ std::make_unique<PresetButton>() };

        button->setRadioGroupId(groupId);
        button->setClickingTogglesState(true);
        button->setButtonText(String(i + 1));
        button->addListener(this);

        addAndMakeVisible(button.get());

        mPresets.add(button.release());
    }

    mActionLog.setColour(Label::backgroundColourId, Colour::fromRGB(120, 120, 120));
    mActionLog.setColour(Label::outlineColourId, Colour::fromRGB(0, 0, 0));
    mActionLog.setColour(Label::textColourId, Colour::fromRGB(0, 0, 0));
    addAndMakeVisible(&mActionLog);

    mAppVersionLabel.setText(String("v. ") + JucePlugin_VersionString, NotificationType::dontSendNotification);
    addAndMakeVisible(&mAppVersionLabel);

    mPresetsManager.addChangeListener(this);
}

//==============================================================================
PositionPresetComponent::~PositionPresetComponent()
{
    mPresetsManager.removeChangeListener(this);
}

//==============================================================================
void PositionPresetComponent::setPreset(int const value, bool const notify)
{
    if (mCurrentSelection >= 0) {
        mPresets[mCurrentSelection]->setLoadedState(false);
    }

    if (value == 0) {
        return;
    }

    if (mPresets[value - 1]->isSaved()) {
        mPresets[value - 1]->setToggleState(true, NotificationType::dontSendNotification);
        mCurrentSelection = value - 1;
        if (notify) {
            buttonClicked(mPresets[value - 1]);
        }
    }
}

//==============================================================================
void PositionPresetComponent::presetSaved(int const presetNumber, bool const isSaved)
{
    if (mPresets[presetNumber - 1] != nullptr) {
        mPresets[presetNumber - 1]->setSavedState(isSaved);
    }
}

//==============================================================================
void PositionPresetComponent::changeListenerCallback(ChangeBroadcaster * changeBroadcaster)
{
    auto const currentPreset{ mPresetsManager.getCurrentPreset() };
    setPreset(currentPreset, false);
}

//==============================================================================
void PositionPresetComponent::buttonClicked(PresetButton * button)
{
    if (button->getToggleState()) {
        mCurrentSelection = button->getButtonText().getIntValue() - 1;
        mActionLog.setText(String("Load ") + button->getButtonText(), NotificationType::dontSendNotification);
        mListeners.call([&](Listener & l) { l.positionPresetChanged(button->getButtonText().getIntValue()); });
    }
}

//==============================================================================
void PositionPresetComponent::savingPresetClicked(PresetButton * button)
{
    mActionLog.setText(String("Save ") + button->getButtonText(), NotificationType::dontSendNotification);
    mListeners.call([&](Listener & l) { l.positionPresetSaved(button->getButtonText().getIntValue()); });
}

//==============================================================================
void PositionPresetComponent::deletingPresetClicked(PresetButton * button)
{
    mActionLog.setText(String("Del ") + button->getButtonText(), NotificationType::dontSendNotification);
    mListeners.call([&](Listener & l) { l.positionPresetDeleted(button->getButtonText().getIntValue()); });
}

//==============================================================================
void PositionPresetComponent::resized()
{
    auto const width{ getWidth() };

    for (int i{}; i < NUMBER_OF_POSITION_PRESETS; ++i) {
        auto const x{ i % 2 == 0 ? 1 : 25 };
        auto const y{ (i / 2) * 24 + 1 };
        mPresets[i]->setBounds(x, y, 23, 23);
    }

    mActionLog.setBounds(1, 25 * 24 + 2, width - 3, 20);
    mAppVersionLabel.setBounds(3, 25 * 24 + 35, width - 3, 20);
}
