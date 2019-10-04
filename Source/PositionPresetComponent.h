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

#include "../JuceLibraryCode/JuceHeader.h"
#include "GrisLookAndFeel.h"
#include "ControlGrisConstants.h"

class PresetButton : public TextButton
{
public:
    PresetButton() { saved = loaded = false; };
    ~PresetButton() {};

    void setSavedState(bool isSaved);
    void refresh();

    void clicked (const ModifierKeys &mods) override;
    void internalClickCallback (const ModifierKeys &mods) override;

    struct Listener
    {
        virtual ~Listener() {}

        virtual void buttonClicked(PresetButton *button) = 0;
        virtual void savingPresetClicked(PresetButton *button) = 0;
        virtual void deletingPresetClicked(PresetButton *button) = 0;
    };

    void addListener(Listener* l) { listeners.add (l); }
    void removeListener(Listener* l) { listeners.remove (l); }

private:
    ListenerList<Listener> listeners;

    bool saved;
    bool loaded;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetButton)
};

//================================================================================
class PositionPresetComponent : public Component,
                                PresetButton::Listener
{
public:
    PositionPresetComponent();
    ~PositionPresetComponent();

    void paint(Graphics&) override;
    void resized() override;

    void buttonClicked(PresetButton *button) override;
    void savingPresetClicked(PresetButton *button) override;
    void deletingPresetClicked(PresetButton *button) override;

    void setPreset(int value);
    void presetSaved(int presetNumber, bool isSaved);

    struct Listener
    {
        virtual ~Listener() {}
        virtual void positionPresetChanged(int presetNumber) = 0;
        virtual void positionPresetSaved(int presetNumber) = 0;
        virtual void positionPresetDeleted(int presetNumber) = 0;
    };

    void addListener(Listener* l) { listeners.add (l); }
    void removeListener(Listener* l) { listeners.remove (l); }

private:
    ListenerList<Listener> listeners;

    OwnedArray<PresetButton> presets;

    Label actionLog;
    Label appVersionLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PositionPresetComponent)
};
