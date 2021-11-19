/**************************************************************************
 * Copyright 2021 UdeM - GRIS - Samuel Béland & Olivier Belanger          *
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

namespace gris
{
class PresetsManager;

//==============================================================================
class PresetButton final : public juce::TextButton
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
        virtual void presetButtonClickedCallback(PresetButton * button) = 0;
        virtual void savingPresetClickedCallback(PresetButton * button) = 0;
        virtual void deletingPresetClickedCallback(PresetButton * button) = 0;

    private:
        //==============================================================================
        JUCE_LEAK_DETECTOR(Listener)
    };

private:
    //==============================================================================
    juce::ListenerList<Listener> mListeners{};
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

    void clicked(juce::ModifierKeys const & mods) override;
    void internalClickCallback(juce::ModifierKeys const & mods) override;

    [[nodiscard]] bool isSaved() const { return mSaved; }

    void addListener(Listener * l) { mListeners.add(l); }
    void removeListener(Listener * l) { mListeners.remove(l); }

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR(PresetButton)
}; // class PresetButton

//================================================================================
class PositionPresetComponent final
    : public juce::Component
    , public juce::ChangeListener
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
        virtual void positionPresetChangedCallback(int presetNumber) = 0;
        virtual void positionPresetSavedCallback(int presetNumber) = 0;
        virtual void positionPresetDeletedCallback(int presetNumber) = 0;

    private:
        //==============================================================================
        JUCE_LEAK_DETECTOR(Listener)
    };

private:
    //==============================================================================
    juce::ListenerList<Listener> mListeners{};
    juce::OwnedArray<PresetButton> mPresets{};
    int mCurrentSelection{ -1 };
    juce::Label mActionLog{};
    juce::Label mAppVersionLabel{};
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
    void paint(juce::Graphics & g) override { g.fillAll(juce::Colour::fromRGB(64, 64, 64)); }
    void resized() override;

    void presetButtonClickedCallback(PresetButton * button) override;
    void savingPresetClickedCallback(PresetButton * button) override;
    void deletingPresetClickedCallback(PresetButton * button) override;

    void setPreset(int value, bool notify = false);
    void presetSaved(int presetNumber, bool isSaved) const;

    void addListener(Listener * l) { mListeners.add(l); }
    void removeListener(Listener * l) { mListeners.remove(l); }

private:
    //==============================================================================
    void changeListenerCallback(juce::ChangeBroadcaster * changeBroadcaster) override;
    //==============================================================================
    JUCE_LEAK_DETECTOR(PositionPresetComponent)
}; // class PositionPresetComponent

} // namespace gris