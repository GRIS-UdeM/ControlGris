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

#include <JuceHeader.h>

class PresetsManager;

//==============================================================================
class PresetButton final : public TextButton
{
public:
    //==============================================================================
    class Listener
    {
    public:
        Listener() = default;
        virtual ~Listener() = default;

        Listener(Listener const &) = default;
        Listener(Listener &&) = default;

        Listener & operator=(Listener const &) = default;
        Listener & operator=(Listener &&) = default;
        //==============================================================================
        virtual void buttonClicked(PresetButton * button) = 0;
        virtual void savingPresetClicked(PresetButton * button) = 0;
        virtual void deletingPresetClicked(PresetButton * button) = 0;

    private:
        //==============================================================================
        JUCE_LEAK_DETECTOR(Listener)
    };

private:
    //==============================================================================
    ListenerList<Listener> mListeners{};
    bool mSaved{ false };
    bool mLoaded{ false };

public:
    //==============================================================================
    PresetButton() = default;
    ~PresetButton() override = default;

    PresetButton(PresetButton const &) = delete;
    PresetButton(PresetButton &&) = delete;

    PresetButton & operator=(PresetButton const &) = delete;
    PresetButton & operator=(PresetButton &&) = delete;
    //==============================================================================
    void setSavedState(bool savedState);
    void setLoadedState(bool loadedState);
    void refresh();

    void clicked(ModifierKeys const & mods) override;
    void internalClickCallback(ModifierKeys const & mods) override;

    [[nodiscard]] bool isSaved() const { return mSaved; }

    void addListener(Listener * l) { mListeners.add(l); }
    void removeListener(Listener * l) { mListeners.remove(l); }

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR(PresetButton)
}; // class PresetButton

//================================================================================
class PositionPresetComponent final
    : public Component
    , public ChangeListener
    , private PresetButton::Listener
{
public:
    //==============================================================================
    struct Listener {
        //==============================================================================
        Listener() = default;
        virtual ~Listener() = default;

        Listener(Listener const &) = default;
        Listener(Listener &&) = default;

        Listener & operator=(Listener const &) = default;
        Listener & operator=(Listener &&) = default;
        //==============================================================================
        virtual void positionPresetChanged(int presetNumber) = 0;
        virtual void positionPresetSaved(int presetNumber) = 0;
        virtual void positionPresetDeleted(int presetNumber) = 0;

    private:
        //==============================================================================
        JUCE_LEAK_DETECTOR(Listener)
    };

private:
    //==============================================================================
    ListenerList<Listener> mListeners{};
    OwnedArray<PresetButton> mPresets{};
    int mCurrentSelection{ -1 };
    Label mActionLog{};
    Label mAppVersionLabel{};
    PresetsManager & mPresetsManager;

public:
    //==============================================================================
    explicit PositionPresetComponent(PresetsManager & presetsManager);
    //==============================================================================
    PositionPresetComponent() = delete;
    ~PositionPresetComponent() override;

    PositionPresetComponent(PositionPresetComponent const &) = delete;
    PositionPresetComponent(PositionPresetComponent &&) = delete;

    PositionPresetComponent & operator=(PositionPresetComponent const &) = delete;
    PositionPresetComponent & operator=(PositionPresetComponent &&) = delete;
    //==============================================================================
    void paint(Graphics & g) override { g.fillAll(Colour::fromRGB(64, 64, 64)); }
    void resized() override;

    void buttonClicked(PresetButton * button) override;
    void savingPresetClicked(PresetButton * button) override;
    void deletingPresetClicked(PresetButton * button) override;

    void setPreset(int value, bool notify = false);
    void presetSaved(int presetNumber, bool isSaved);

    void addListener(Listener * l) { mListeners.add(l); }
    void removeListener(Listener * l) { mListeners.remove(l); }

private:
    //==============================================================================
    void changeListenerCallback(ChangeBroadcaster * changeBroadcaster) override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(PositionPresetComponent)
}; // class PositionPresetComponent
