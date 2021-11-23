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

#include "cg_ControlGrisLookAndFeel.hpp"
#include "cg_constants.hpp"

namespace gris
{
//==============================================================================
/** A tabbed component used to modify general plugin settings. */
class GeneralSettingsTab final : public juce::Component
{
public:
    //==============================================================================
    struct Listener {
        Listener() = default;
        virtual ~Listener() = default;
        //==============================================================================
        Listener(Listener const &) = delete;
        Listener(Listener &&) = default;
        Listener & operator=(Listener const &) = delete;
        Listener & operator=(Listener &&) = default;
        //==============================================================================
        virtual void oscFormatChangedCallback(SpatMode mode) = 0;
        virtual void oscPortChangedCallback(int oscPort) = 0;
        virtual void oscAddressChangedCallback(juce::String const & address) = 0;
        virtual void oscStateChangedCallback(bool state) = 0;
        virtual void numberOfSourcesChangedCallback(int numOfSources) = 0;
        virtual void firstSourceIdChangedCallback(SourceId firstSourceId) = 0;

    private:
        //==============================================================================
        JUCE_LEAK_DETECTOR(Listener)
    };

private:
    //==============================================================================
    GrisLookAndFeel & mGrisLookAndFeel;

    juce::ListenerList<Listener> mListeners;

    juce::Label mOscFormatLabel;
    juce::ComboBox mOscFormatCombo;

    juce::Label mOscPortLabel;
    juce::TextEditor mOscPortEditor;

    juce::Label mOscAddressLabel;
    juce::TextEditor mOscAddressEditor;

    juce::Label mNumOfSourcesLabel;
    juce::TextEditor mNumOfSourcesEditor;

    juce::Label mFirstSourceIdLabel;
    juce::TextEditor mFirstSourceIdEditor;

    juce::ToggleButton mPositionActivateButton;

public:
    //==============================================================================
    explicit GeneralSettingsTab(GrisLookAndFeel & grisLookAndFeel);
    GeneralSettingsTab() = delete;
    ~GeneralSettingsTab() override = default;
    //==============================================================================
    GeneralSettingsTab(GeneralSettingsTab const &) = delete;
    GeneralSettingsTab(GeneralSettingsTab &&) = delete;
    GeneralSettingsTab & operator=(GeneralSettingsTab const &) = delete;
    GeneralSettingsTab & operator=(GeneralSettingsTab &&) = delete;
    //==============================================================================
    // These are only setters, they dont send notification.
    //-----------------------------------------------------
    void setNumberOfSources(int numOfSources);
    void setFirstSourceId(SourceId firstSourceId);
    void setOscFormat(SpatMode mode);
    void setOscPortNumber(int oscPortNumber);
    void setOscAddress(juce::String const & address);
    void setActivateButtonState(bool shouldBeOn);
    //==============================================================================
    void addListener(Listener * l) { mListeners.add(l); }
    void removeListener(Listener * l) { mListeners.remove(l); }
    //==============================================================================
    void paint(juce::Graphics &) override;
    void resized() override;

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR(GeneralSettingsTab)
};

} // namespace gris