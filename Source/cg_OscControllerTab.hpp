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

namespace gris
{
//==============================================================================
/** A tabbed component used to modify a plugin's OSC controller settings (for when someone wishes to control a
 * ControlGRIS plugin with an external controller). */
class OscControllerTab final
    : public juce::Component
    , public juce::TextEditor::Listener
{
public:
    //==============================================================================
    class Listener
    {
    public:
        Listener() = default;
        virtual ~Listener() = default;
        //==============================================================================
        Listener(Listener const &) = delete;
        Listener(Listener &&) = default;
        Listener & operator=(Listener const &) = delete;
        Listener & operator=(Listener &&) = default;
        //==============================================================================
        virtual void oscOutputPluginIdChangedCallback(int value) = 0;
        virtual void oscInputConnectionChangedCallback(bool state, int oscPort) = 0;
        virtual void oscOutputConnectionChangedCallback(bool state, juce::String oscAddress, int oscPort) = 0;

    private:
        //==============================================================================
        JUCE_LEAK_DETECTOR(Listener)
    };

private:
    //==============================================================================
    GrisLookAndFeel & mGrisLookAndFeel;

    juce::ListenerList<Listener> mListeners{};

    juce::ToggleButton mOscReceiveToggle{};
    juce::ToggleButton mOscSendToggle{};

    juce::Label mOscOutputPluginIdLabel{};
    juce::TextEditor mOscOutputPluginIdEditor{};

    juce::TextEditor mOscReceiveIpEditor{};
    juce::TextEditor mOscReceivePortEditor{};
    juce::TextEditor mOscSendIpEditor{};
    juce::TextEditor mOscSendPortEditor{};

    int mLastOscReceivePort{ 9000 };
    int mLastOscSendPort{ 8000 };
    juce::String mLastOscSendAddress{};

public:
    //==============================================================================
    explicit OscControllerTab(GrisLookAndFeel & grisLookAndFeel);
    OscControllerTab() = delete;
    ~OscControllerTab() override { setLookAndFeel(nullptr); } // TODO : is this necessary ?
    //==============================================================================
    OscControllerTab(OscControllerTab const &) = delete;
    OscControllerTab(OscControllerTab &&) = delete;
    OscControllerTab & operator=(OscControllerTab const &) = delete;
    OscControllerTab & operator=(OscControllerTab &&) = delete;
    //==============================================================================
    void setOscOutputPluginId(int const id) { mOscOutputPluginIdEditor.setText(juce::String(id)); }
    void setOscReceiveToggleState(bool state);
    void setOscReceiveInputPort(int port);
    void setOscSendToggleState(bool state);
    void setOscSendOutputAddress(juce::String const & address);
    void setOscSendOutputPort(int port);
    void addListener(Listener * l) { mListeners.add(l); }
    [[maybe_unused]] void removeListener(Listener * l) { mListeners.remove(l); }
    //==============================================================================
    void textEditorReturnKeyPressed([[maybe_unused]] juce::TextEditor & editor) override { unfocusAllComponents(); }
    void paint(juce::Graphics &) override;
    void resized() override;

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR(OscControllerTab)
};

} // namespace gris