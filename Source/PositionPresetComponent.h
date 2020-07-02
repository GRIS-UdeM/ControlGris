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

#include "ControlGrisConstants.h"
#include "GrisLookAndFeel.h"

//==============================================================================
class PresetButton final : public TextButton
{
public:
    //==============================================================================
    struct Listener {
        virtual ~Listener() {}

        virtual void buttonClicked(PresetButton * button) = 0;
        virtual void savingPresetClicked(PresetButton * button) = 0;
        virtual void deletingPresetClicked(PresetButton * button) = 0;
    };

private:
    //==============================================================================
    ListenerList<Listener> mListeners{};

    bool mSaved{ false };
    bool mLoaded{ false };

public:
    //==============================================================================
    PresetButton() = default;
    ~PresetButton() final = default;
    //==============================================================================
    void setSavedState(bool savedState);
    void setLoadedState(bool loadedState);
    void refresh();

    void clicked(ModifierKeys const & mods) final;
    void internalClickCallback(ModifierKeys const & mods) final;

    bool isSaved() const { return mSaved; }

    void addListener(Listener * l) { mListeners.add(l); }
    void removeListener(Listener * l) { mListeners.remove(l); }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetButton);
};

//================================================================================
class PositionPresetComponent final
    : public Component
    , private PresetButton::Listener
{
public:
    //==============================================================================
    struct Listener {
        virtual ~Listener() {}
        virtual void positionPresetChanged(int presetNumber) = 0;
        virtual void positionPresetSaved(int presetNumber) = 0;
        virtual void positionPresetDeleted(int presetNumber) = 0;
    };

private:
    //==============================================================================
    ListenerList<Listener> mListeners{};

    OwnedArray<PresetButton> mPresets{};

    int mCurrentSelection{ -1 };

    Label mActionLog{};
    Label mAppVersionLabel{};

public:
    //==============================================================================
    PositionPresetComponent();
    ~PositionPresetComponent() final { this->setLookAndFeel(nullptr); }
    //==============================================================================
    void paint(Graphics & g) final { g.fillAll(Colour::fromRGB(64, 64, 64)); }
    void resized() final;

    void buttonClicked(PresetButton * button) final;
    void savingPresetClicked(PresetButton * button) final;
    void deletingPresetClicked(PresetButton * button) final;

    void setPreset(int value, bool notify = false);
    void presetSaved(int presetNumber, bool isSaved);

    void addListener(Listener * l) { mListeners.add(l); }
    void removeListener(Listener * l) { mListeners.remove(l); }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PositionPresetComponent);
};
