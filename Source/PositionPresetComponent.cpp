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

void PresetButton::setSavedState(bool isSaved) {
    saved = isSaved;
    refresh();
}

void PresetButton::setLoadedState(bool isLoaded) {
    loaded = isLoaded;
    setToggleState(isLoaded, NotificationType::dontSendNotification);
    refresh();
}

bool PresetButton::isSaved() {
    return saved;
}

void PresetButton::refresh() {
    if (loaded) {
        setColour(TextButton::buttonColourId, Colour::fromRGB(255, 165, 25));
    } else {
        if (saved) {
            setColour(TextButton::buttonColourId, Colour::fromRGB(255, 255, 255));
        } else {
            setColour(TextButton::buttonColourId, Colour::fromRGB(172, 172, 172));
        }
    }
}

void PresetButton::clicked(const ModifierKeys &mods) {
    listeners.call([&] (Listener& l) { l.buttonClicked(this); });
    if (! mods.isShiftDown()) {
        loaded = getToggleState();
    }
    refresh();
};

void PresetButton::internalClickCallback (const ModifierKeys &mods) {
    if (mods.isShiftDown()) {
        listeners.call([&] (Listener& l) { l.savingPresetClicked(this); });
        saved = true;
    } else if (saved && mods.isAltDown()) {
        listeners.call([&] (Listener& l) { l.deletingPresetClicked(this); });
        setToggleState(false, NotificationType::dontSendNotification);
        loaded = saved = false;
    } else if (saved) {
        TextButton::internalClickCallback(mods);
    }
    refresh();
};

//===============================================================================
PositionPresetComponent::PositionPresetComponent() {
    int groupId = 1;

    currentSelection = -1;

    for (int i = 0; i < NUMBER_OF_POSITION_PRESETS; i++) {
        auto* button = new PresetButton();
        presets.add (button);

        button->setRadioGroupId(groupId);
        button->setClickingTogglesState(true);
        button->setButtonText(String(i+1));
        button->addListener(this);

        addAndMakeVisible (button);
    }

    actionLog.setColour(Label::backgroundColourId, Colour::fromRGB(120, 120, 120));
    actionLog.setColour(Label::outlineColourId, Colour::fromRGB(0, 0, 0));
    actionLog.setColour(Label::textColourId, Colour::fromRGB(0, 0, 0));
    addAndMakeVisible(&actionLog);

    appVersionLabel.setText(String("v. ") + JucePlugin_VersionString, NotificationType::dontSendNotification);
    addAndMakeVisible(&appVersionLabel);
}

PositionPresetComponent::~PositionPresetComponent() {
    setLookAndFeel(nullptr);
}

void PositionPresetComponent::setPreset(int value, bool notify) {
    if (currentSelection >= 0) {
        presets[currentSelection]->setLoadedState(false);
    }

    if (value == 0) {
        return;
    }

    if (presets[value - 1]->isSaved()) {
        presets[value - 1]->setToggleState(true, NotificationType::dontSendNotification);
        currentSelection = value - 1;
        if (notify) {
            buttonClicked(presets[value - 1]);
        }
    }
}

void PositionPresetComponent::presetSaved(int presetNumber, bool isSaved) {
    if (presets[presetNumber - 1] != nullptr) {
        presets[presetNumber - 1]->setSavedState(isSaved);
    }
}

void PositionPresetComponent::buttonClicked(PresetButton *button) {
    if (button->getToggleState()) {
        currentSelection = button->getButtonText().getIntValue() - 1;
        actionLog.setText(String("Load ") + button->getButtonText(), NotificationType::dontSendNotification);
        listeners.call([&] (Listener& l) { l.positionPresetChanged(button->getButtonText().getIntValue()); });
    }
}

void PositionPresetComponent::savingPresetClicked(PresetButton *button) {
    actionLog.setText(String("Save ") + button->getButtonText(), NotificationType::dontSendNotification);
    listeners.call([&] (Listener& l) { l.positionPresetSaved(button->getButtonText().getIntValue()); });
}

void PositionPresetComponent:: deletingPresetClicked(PresetButton *button) {
    actionLog.setText(String("Del ") + button->getButtonText(), NotificationType::dontSendNotification);
    listeners.call([&] (Listener& l) { l.positionPresetDeleted(button->getButtonText().getIntValue()); });
}

void PositionPresetComponent::paint(Graphics& g) {
    g.fillAll (Colour::fromRGB(64, 64, 64));
}

void PositionPresetComponent::resized() {
    int width = getWidth();

    for (int i = 0; i < NUMBER_OF_POSITION_PRESETS; i++) {
        int x = i % 2 == 0 ? 1 : 25;
        int y = (i / 2) * 24 + 1;
        presets[i]->setBounds(x, y, 23, 23);
    }

    actionLog.setBounds(1, 25 * 24 + 2, width - 3, 20);
    appVersionLabel.setBounds(3, 25 * 24 + 25, width - 3, 20);
}
